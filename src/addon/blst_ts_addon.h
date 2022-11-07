#pragma once

#include <napi.h>

class BlstTsAddon : public Napi::ObjectWrap<BlstTsAddon>
{
public:
    BlstTsAddon(const Napi::CallbackInfo&);
    Napi::Value Greet(const Napi::CallbackInfo&);

    static Napi::Function GetClass(Napi::Env);

private:
    std::string _greeterName;
};
