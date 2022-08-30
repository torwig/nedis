#pragma once

#include <napi.h>

#include "../deps/hiredis/async.h"

#include <map>

class Nedis : public Napi::ObjectWrap<Nedis>
{
public:
    Nedis(const Napi::CallbackInfo&);
    Napi::Value Connect(const Napi::CallbackInfo&);
    Napi::Value Disconnect(const Napi::CallbackInfo&);
    Napi::Value SendCommand(const Napi::CallbackInfo&);

    static Napi::Function GetClass(Napi::Env);

private:
    redisAsyncContext* redis_ctx = nullptr;
    bool is_connected = false;
    uint64_t callback_id = 1;
    Napi::Function connect_cb;
    Napi::Function disconnect_cb;
    std::map<uint64_t, Napi::Function> js_callbacks;

    static void ConnectCallback(redisAsyncContext *c, int status);
	static void DisconnectCallback(const redisAsyncContext *c, int status);
	static void OnRedisResponse(redisAsyncContext *c, void *r, void *privdata);
};
