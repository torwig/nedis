#pragma once

#include <napi.h>

class Nedis : public Napi::ObjectWrap<Nedis>
{
public:
    Nedis(const Napi::CallbackInfo&);
    Napi::Value Greet(const Napi::CallbackInfo&);

    static Napi::Function GetClass(Napi::Env);

private:
    std::string _greeterName;
};
