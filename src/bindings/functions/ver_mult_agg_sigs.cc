#include "ver_mult_agg_sigs.h"

const size_t kRandomBitesLength = 8;

std::string stringify_error_ptr(std::exception_ptr p_error)
{
    try
    {
        if (p_error)
        {
            std::rethrow_exception(p_error);
        }
    }
    catch (const std::exception &e)
    {
        std::ostringstream errorMsg;
        errorMsg << "Caught exception \"" << e.what() << "\"\n";
        return errorMsg.str();
    }
    return "Unknown Error";
}

Napi::Object ArrayIndexToObject(const Napi::Env &env,
                                const Napi::Array &arr,
                                const size_t index,
                                const std::string &obj_name)
{
    Napi::Value val = arr[index];
    if (!val.IsObject())
    {
        throw Napi::Error::New(env, obj_name + " must be an object");
    }
    return val.As<Napi::Object>();
}

Napi::Array InfoIndexToArray(const Napi::Env &env,
                             const Napi::CallbackInfo &info,
                             const size_t index,
                             const std::string &obj_name)
{
    Napi::Value val = info[index];
    if (!val.IsArray())
    {
        throw Napi::Error::New(env, obj_name + " must be an array");
    }
    return val.As<Napi::Array>();
}

Napi::Value GetNapiValueAtKey(
    const Napi::Env &env,
    const Napi::Object &obj,
    const std::string &obj_name,
    const std::string &key_name)
{
    if (!obj.Has(key_name))
    {
        throw Napi::Error::New(env, obj_name + " must have a '" + key_name + "' property");
    }
    return obj.Get(key_name);
}

ByteArray TryAsStringOrUint8Array(
    const Napi::Env &env,
    const Napi::Value &value,
    const std::string &obj_name,
    const std::string &key_name,
    const bool should_throw = false)
{
    if (value.IsString())
    {
        return ByteArray{value.As<Napi::String>().Utf8Value()};
    }
    else if (value.IsTypedArray())
    {
        return ByteArray{value.As<Napi::Uint8Array>()};
    }
    else if (should_throw)
    {
        throw Napi::Error::New(env, obj_name + "." + key_name + " is not a string or Uint8Array");
    }
    return ByteArray{};
}

/**
 *
 *
 *
 *
 *
 *
 *
 */
void GetMessage(const Napi::Env &env,
                size_t insert_index,
                Napi::Object &set,
                std::string &obj_name,
                std::vector<ByteArray> &msgs)
{
    std::string key_name = "msg";
    Napi::Value msg = GetNapiValueAtKey(env, set, obj_name, key_name);
    ByteArray bytes = TryAsStringOrUint8Array(env, msg, obj_name, key_name, true);
    if (bytes.ByteLength() > 0)
    {
        msgs[insert_index] = std::move(bytes);
        return;
    }
    throw Napi::TypeError::New(env, obj_name + ".msg must be a string or Uint8Array");
}

void GetPublicKey(const Napi::Env &env,
                  size_t insert_index,
                  Napi::Object &set,
                  std::string &obj_name,
                  std::vector<ByteArray> &public_keys,
                  std::map<size_t, PublicKey *> &public_key_map)
{
    std::string key_name = "publicKey";
    Napi::Value pk_value = GetNapiValueAtKey(env, set, obj_name, key_name);
    ByteArray bytes = TryAsStringOrUint8Array(env, pk_value, obj_name, key_name, false);
    if (bytes.ByteLength() > 0)
    {
        public_keys[insert_index] = std::move(bytes);
        return;
    }
    Napi::Object pk_obj = pk_value.As<Napi::Object>();
    if (pk_obj.Has("__type") && (pk_obj.Get("__type").As<Napi::String>().Utf8Value().compare(PublicKey::kType_) == 0))
    {
        PublicKey *pk = PublicKey::Unwrap(pk_obj);
        if (pk != nullptr)
        {
            public_key_map[insert_index] = pk;
            return;
        }
    }
    throw Napi::TypeError::New(env, obj_name + ".publicKey must be a string, Uint8Array or PublicKey");
}

void GetSignature(const Napi::Env &env,
                  size_t insert_index,
                  Napi::Object &set,
                  std::string &obj_name,
                  std::vector<ByteArray> &signatures,
                  std::map<size_t, Signature *> &signature_map)
{
    std::string key_name = "signature";
    Napi::Value sig_value = GetNapiValueAtKey(env, set, obj_name, key_name);
    ByteArray bytes = TryAsStringOrUint8Array(env, sig_value, obj_name, key_name, false);
    if (bytes.ByteLength() > 0)
    {
        signatures[insert_index] = std::move(bytes);
        return;
    }
    Napi::Object sig_obj = sig_value.As<Napi::Object>();
    if (sig_obj.Has("__type") && (sig_obj.Get("__type").As<Napi::String>().Utf8Value().compare(Signature::kType_) == 0))
    {
        Signature *sig = Signature::Unwrap(sig_obj);
        if (sig != nullptr)
        {
            signature_map[insert_index] = sig;
            return;
        }
    }
    throw Napi::TypeError::New(env, obj_name + ".signature must be a string, Uint8Array or Signature");
}

void ParseSignatureSets(const Napi::Env &env,
                        const Napi::Array &signature_sets,
                        size_t set_length,
                        std::vector<ByteArray> &msgs,
                        std::vector<ByteArray> &public_keys,
                        std::vector<ByteArray> &signatures,
                        std::map<size_t, PublicKey *> &public_key_map,
                        std::map<size_t, Signature *> &signature_map)
{
    for (size_t i = 0; i < set_length; i++)
    {
        std::string obj_name{"SignatureSet"};
        Napi::Object set = ArrayIndexToObject(env, signature_sets, i, obj_name);
        GetMessage(env, i, set, obj_name, msgs);
        GetPublicKey(env, i, set, obj_name, public_keys, public_key_map);
        GetSignature(env, i, set, obj_name, signatures, signature_map);
    }
}

void AggregateThread(
    size_t index,
    std::unique_ptr<blst::Pairing> &ctx,
    std::mutex &ctx_mutex,
    std::vector<ByteArray> &msgs,
    std::vector<ByteArray> &public_keys,
    std::vector<ByteArray> &signatures,
    std::map<size_t, PublicKey *> &public_key_map,
    std::map<size_t, Signature *> &signature_map)
{
    blst::P1_Affine pk;
    auto pk_iter = public_key_map.find(index);
    if (pk_iter != public_key_map.end())
    {
        pk = pk_iter->second->AsAffine();
    }
    else
    {
        pk = blst::P1_Affine{public_keys[index].Data(), public_keys[index].ByteLength()};
    }
    blst::P2_Affine sig;
    auto sig_iter = signature_map.find(index);
    if (sig_iter != signature_map.end())
    {
        sig = sig_iter->second->AsAffine();
    }
    else
    {
        sig = blst::P2_Affine{signatures[index].Data(), signatures[index].ByteLength()};
    }

    blst::Pairing *thread_ctx = new blst::Pairing(true, DST);
    blst::BLST_ERROR err = thread_ctx->mul_n_aggregate(&pk,
                                                       &sig,
                                                       ByteArray::RandomBytes(kRandomBitesLength).Data(),
                                                       kRandomBitesLength,
                                                       msgs[index].Data(),
                                                       msgs[index].ByteLength());
    if (err != blst::BLST_ERROR::BLST_SUCCESS)
    {
        throw err;
    }
    thread_ctx->commit();
    std::lock_guard<std::mutex> guard{ctx_mutex};
    ctx->merge(thread_ctx);
}

void RunAggregation(
    size_t set_length,
    std::unique_ptr<blst::Pairing> &ctx,
    std::mutex &ctx_mutex,
    std::vector<ByteArray> &msgs,
    std::vector<ByteArray> &public_keys,
    std::vector<ByteArray> &signatures,
    std::map<size_t, PublicKey *> &public_key_map,
    std::map<size_t, Signature *> &signature_map)
{
    ThreadPool pool{10};
    std::future<void> results[set_length];
    for (size_t i = 0; i < set_length; i++)
    {
        results[i] = pool.submit(std::bind(&AggregateThread,
                                           i,
                                           std::ref(ctx),
                                           std::ref(ctx_mutex),
                                           std::ref(msgs),
                                           std::ref(public_keys),
                                           std::ref(signatures),
                                           std::ref(public_key_map),
                                           std::ref(signature_map)));
    }
    for (size_t i = 0; i < set_length; i++)
    {
        results[i].get();
    }
}

/**
 *
 *
 *
 *
 *
 *
 *
 */
Napi::Value VerifyMultipleAggregateSignatures(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    try
    {
        Napi::Array signature_sets = InfoIndexToArray(env, info, 0, "signatureSets");
        size_t set_length{signature_sets.Length()};
        std::vector<ByteArray> msgs(set_length);
        std::vector<ByteArray> public_keys(set_length);
        std::vector<ByteArray> signatures(set_length);
        std::map<size_t, PublicKey *> public_key_map;
        std::map<size_t, Signature *> signature_map;
        std::unique_ptr<blst::Pairing> ctx = std::unique_ptr<blst::Pairing>(new blst::Pairing(true, DST));
        std::mutex ctx_mutex;

        ParseSignatureSets(env, signature_sets, set_length, msgs, public_keys, signatures, public_key_map, signature_map);
        RunAggregation(
            set_length,
            ctx,
            ctx_mutex,
            msgs,
            public_keys,
            signatures,
            public_key_map,
            signature_map);

        return Napi::Boolean::New(env, ctx->finalverify());
    }
    catch (const Napi::Error &err)
    {
        err.ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }
    catch (...)
    {
        std::exception_ptr err = std::current_exception();
        Napi::Error::New(env, stringify_error_ptr(err)).ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }
}
/**
 *
 *
 *
 *
 *
 *
 *
 */
class VerifyMultipleAggregateSignaturesWorker : public Napi::AsyncWorker
{
public:
    VerifyMultipleAggregateSignaturesWorker(const Napi::Env &env);
    void Setup(const Napi::Env &env, const Napi::Array &signature_sets);
    void Execute() override;
    void OnOK() override;
    void OnError(const Napi::Error &err) override;
    Napi::Promise GetPromise();

private:
    Napi::Promise::Deferred deferred;
    std::unique_ptr<blst::Pairing> ctx;
    std::mutex ctx_mutex;
    size_t set_length;
    std::vector<ByteArray> msgs;
    std::vector<ByteArray> public_keys;
    std::vector<ByteArray> signatures;
    std::map<size_t, PublicKey *> public_key_map;
    std::map<size_t, Signature *> signature_map;
    bool result;
    void Aggregate(size_t i);
};

VerifyMultipleAggregateSignaturesWorker::VerifyMultipleAggregateSignaturesWorker(const Napi::Env &env)
    : Napi::AsyncWorker{env},
      deferred{env},
      ctx{new blst::Pairing(true, DST)},
      ctx_mutex{},
      set_length{0},
      msgs{},
      public_keys{},
      signatures{},
      public_key_map{},
      signature_map{},
      result{false}
{
}

void VerifyMultipleAggregateSignaturesWorker::Setup(const Napi::Env &env, const Napi::Array &signature_sets)
{
    set_length = signature_sets.Length();
    msgs.resize(set_length);
    public_keys.resize(set_length);
    signatures.resize(set_length);
    ParseSignatureSets(env, signature_sets, set_length, msgs, public_keys, signatures, public_key_map, signature_map);
    Queue();
}

void VerifyMultipleAggregateSignaturesWorker::Execute()
{
    try
    {
        RunAggregation(
            set_length,
            ctx,
            ctx_mutex,
            msgs,
            public_keys,
            signatures,
            public_key_map,
            signature_map);
        result = ctx->finalverify();
    }
    catch (blst::BLST_ERROR err)
    {
        std::ostringstream errorMsg;
        errorMsg << "BLST_ERROR:: " << get_blst_error_string(err) << " in verifyMultipleAggregateSignatures";
        SetError(errorMsg.str());
        return;
    }
}

void VerifyMultipleAggregateSignaturesWorker::OnOK()
{
    deferred.Resolve(Napi::Boolean::New(Env(), result));
}

void VerifyMultipleAggregateSignaturesWorker::OnError(Napi::Error const &err)
{
    deferred.Reject(err.Value());
}

Napi::Promise VerifyMultipleAggregateSignaturesWorker::GetPromise()
{
    return deferred.Promise();
}

Napi::Value VerifyMultipleAggregateSignaturesAsync(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    try
    {
        Napi::Array signature_sets = InfoIndexToArray(env, info, 0, "signatureSets");
        VerifyMultipleAggregateSignaturesWorker *worker = new VerifyMultipleAggregateSignaturesWorker{env};
        worker->Setup(env, signature_sets);
        return worker->GetPromise();
    }
    catch (const Napi::Error &err)
    {
        err.ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }
    catch (...)
    {
        std::exception_ptr err = std::current_exception();
        Napi::Error::New(env, stringify_error_ptr(err)).ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }
}