#include "utils.h"

Napi::Value BlstAsyncWorker::RunSync()
{
    Napi::HandleScope scope(_env);
    Setup();
    if (true)
    {
        /**
         *
         *
         *
         *
         *
         *
         *
         *
         *
         */
    }
    OnExecute(_env);
    SuppressDestruct();
    OnWorkComplete(_env, napi_ok);
    Napi::Value return_val = _threw_error ? _env.Undefined() : GetReturnValue();
    return return_val;
}
Napi::Value BlstAsyncWorker::Run()
{
    _use_deferred = true;
    Setup();
    Queue();
    return GetPromise();
};

void BlstAsyncWorker::SetError(const std::string &err)
{
    _threw_error = true;
    Napi::AsyncWorker::SetError(err);
}

void BlstAsyncWorker::OnOK()
{
    if (_use_deferred)
    {
        _deferred.Resolve(GetReturnValue());
    }
}
void BlstAsyncWorker::OnError(Napi::Error const &err)
{
    if (_use_deferred)
    {
        _deferred.Reject(err.Value());
    }
    else
    {
        err.ThrowAsJavaScriptException();
    }
}
Napi::Promise BlstAsyncWorker::GetPromise()
{
    return _deferred.Promise();
}

Uint8ArrayArg::Uint8ArrayArg(
    const Napi::Env &env,
    const Napi::Value &val,
    const std::string &err_prefix,
    size_t length1,
    size_t length2) : Uint8ArrayArg{env}
{
    if (val.IsTypedArray())
    {
        Napi::TypedArray untyped = val.As<Napi::TypedArray>();
        if (untyped.TypedArrayType() == napi_uint8_array)
        {
            auto a = untyped.As<Napi::TypedArrayOf<u_int8_t>>();
            _ref = Napi::Persistent(a);
            _data = _ref.Value().Data();
            _byte_length = _ref.Value().ByteLength();
            ValidateLength(err_prefix, length1, length2);
            return;
        }
    }
    SetError(err_prefix + " must be of type BlstBuffer");
};

const uint8_t *Uint8ArrayArg::Data()
{
    if (HasError())
    {
        return nullptr;
    }
    return _data;
};

size_t Uint8ArrayArg::ByteLength()
{
    if (HasError())
    {
        return 0;
    }
    return _byte_length;
};

// void Uint8ArrayArg::ThrowCppExceptionIfError()
// {
//     if (HasError())
//     {
//         throw Napi::Error::New(_env, _error);
//     }
// }

// void Uint8ArrayArg::ThrowJsExceptionIfError()
// {
//     if (HasError())
//     {
//         Napi::Error::New(_env, _error).ThrowAsJavaScriptException();
//     }
// }

bool Uint8ArrayArg::ValidateLength(const std::string &_error_prefix, size_t length1, size_t length2)
{
    bool is_valid = true;
    std::ostringstream msg;
    msg << _error_prefix << " is " << ByteLength() << " bytes, but must be ";
    if (length1 != 0)
    {
        msg << length1;
        if (ByteLength() != length1)
        {
            is_valid = false;
        }
    };
    if (length2 != 0 && ByteLength() != length1)
    {
        if (length1 != 0)
        {
            msg << " or ";
        }
        msg << length2;
        if (ByteLength() != length1)
        {
            is_valid = false;
        }
    };
    msg << " bytes long";
    if (!is_valid)
    {
        SetError(msg.str());
    }
    return is_valid;
};

Uint8ArrayArgArray::Uint8ArrayArgArray(
    const Napi::Env &env,
    const Napi::Value &arr_val,
    const std::string &err_prefix_singular,
    const std::string &err_prefix_plural,
    size_t length1,
    size_t length2)
    : Uint8ArrayArgArray{}
{
    if (!arr_val.IsArray())
    {
        throw Napi::TypeError::New(env, err_prefix_plural + " must be of type BlstBuffer[]");
    }
    Napi::Array arr = arr_val.As<Napi::Array>();
    for (size_t i = 0; i < arr.Length(); i++)
    {
        (*this)[i] = Uint8ArrayArg{env, arr[i], err_prefix_singular, length1, length2};
    }
}
