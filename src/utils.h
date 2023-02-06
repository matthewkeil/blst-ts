#ifndef BLST_TS_UTILS_H__
#define BLST_TS_UTILS_H__

#include "napi.h"
#include "addon.h"

class BlstAsyncWorker : public Napi::AsyncWorker
{
public:

    BlstAsyncWorker(const Napi::CallbackInfo &info) : Napi::AsyncWorker{info.Env()},
                                                      _module{info.Env().GetInstanceData<BlstTsAddon>()},
                                                      _info{info},
                                                      _env{Env()},
                                                      _deferred{_env},
                                                      _use_deferred{false},
                                                      _threw_error{false} {};
    virtual void Setup() = 0;
    virtual Napi::Value GetReturnValue() = 0;

    Napi::Value RunSync()
    {
        Napi::HandleScope scope(_env);
        Setup();
        OnExecute(_env);
        SuppressDestruct();
        OnWorkComplete(_env, napi_ok);
        Napi::Value return_val = _threw_error ? _env.Undefined() : GetReturnValue();
        return return_val;
    }
    Napi::Value Run()
    {
        _use_deferred = true;
        Setup();
        Napi::AsyncWorker::Queue();
        return GetPromise();
    };

protected:
    BlstTsAddon *_module;
    const Napi::CallbackInfo &_info;
    const Napi::Env _env;

    void OnOK() override
    {
        if (_use_deferred)
        {
            _deferred.Resolve(GetReturnValue());
        }
    }
    void OnError(Napi::Error const &err) override
    {
        if (_use_deferred)
        {
            _deferred.Reject(err.Value());
        }
        else
        {
            _threw_error = true;
            err.ThrowAsJavaScriptException();
        }
    }
    Napi::Promise GetPromise()
    {
        return _deferred.Promise();
    }

private:
    Napi::Promise::Deferred _deferred;
    bool _use_deferred;
    bool _threw_error;
};

#endif /* BLST_TS_UTILS_H__ */
