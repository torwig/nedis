#include "nedis.h"

#include "../deps/hiredis/adapters/libuv.h"

#include <iostream>

using namespace Napi;

Nedis::Nedis(const Napi::CallbackInfo& info) : ObjectWrap(info) {

}

Napi::Value Nedis::Connect(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 4) {
        Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Host should be a string value").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "Port should be a number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[2].IsFunction()) {
        Napi::TypeError::New(env, "Connect callback should be a function").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[3].IsFunction()) {
        Napi::TypeError::New(env, "Disconnect callback should be a function").ThrowAsJavaScriptException();
        return env.Null();
    }

    connect_cb = info[2].As<Napi::Function>();
    disconnect_cb = info[3].As<Napi::Function>();

    auto host = info[0].As<Napi::String>().Utf8Value();
    if (host[0] == '/') {
        redis_ctx = redisAsyncConnectUnix(host.c_str());
    } else {
        redis_ctx = redisAsyncConnect(host.c_str(), info[1].As<Napi::Number>());
    }

    if (redis_ctx->err) {
        is_connected = false;
        connect_cb.Call(env.Global(), {Napi::String::New(env, redis_ctx->errstr)});
        return env.Undefined();
    }

    uv_loop_t* loop = uv_default_loop();
	redis_ctx->data = (void*)(this);
	redisLibuvAttach(redis_ctx, loop);
	redisAsyncSetConnectCallback(redis_ctx, ConnectCallback);
	redisAsyncSetDisconnectCallback(redis_ctx, DisconnectCallback);
	return env.Undefined();
}

Napi::Value Nedis::Disconnect(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
    return env.Undefined();
}

Napi::Value Nedis::SendCommand(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
    return env.Undefined();
}

void Nedis::ConnectCallback(redisAsyncContext *c, int status) {
    std::cout << "inside connect callback, status: " << status << std::endl;
	Nedis* self = (Nedis*)c->data;
	if (status != REDIS_OK) {
		self->is_connected = false;
		self->connect_cb.Call({Napi::String::New(, c->errstr)});
		return;
	}

	self->is_connected = true;
	self->connect_cb.Call({Napi::Env::Null()});
}

void Nedis::DisconnectCallback(const redisAsyncContext *c, int status) {
    std::cout << "inside disconnect callback, status: " << status << std::endl; 
}

/*Napi::Value ParseResponse(redisReply *reply, size_t* size) {
	Napi::EscapableHandleScope scope;
	Napi::Value resp;
	Napi::Array arr = Napi::New<Array>();
	
	switch (reply->type) {
	case REDIS_REPLY_NIL:
		resp = Napi::Null();
		*size += sizeof(NULL);
		break;
	case REDIS_REPLY_INTEGER:
		resp = Napi::New<Number>(reply->integer);
		*size += sizeof(int);
		break;
	case REDIS_REPLY_STATUS:
	case REDIS_REPLY_STRING:
		resp = Napi::New<String>(reply->str, reply->len).ToLocalChecked();
		*size += reply->len;
		break;
	case REDIS_REPLY_ARRAY:
		for (size_t i = 0; i < reply->elements; ++i ) {
			Napi::Set(arr, Napi::New<Number>(i), ParseResponse(reply->element[i], size));
		}
		resp = arr;
		break;
	default:
		printf("Redis protocol error, unknown type %d\n", reply->type);
		Napi::ThrowError("Protocol error, unknown type");
		return Napi::Undefined();
	}
	
	return scope.Escape(resp);
}

void Nedis::OnRedisResponse(redisAsyncContext *c, void *r, void *privdata) {
	Napi::HandleScope scope;
	size_t total_size = 0;

	redisReply *reply = (redisReply*)r;
    if (!reply) {
        return;
    }

	uint64_t callback_id = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(privdata));

	Nedis *self = (Nedis*)c->data;
	Napi::Function js_cb = Napi::New(self->js_callbacks[callback_id]);

	if (!(c->c.flags & REDIS_SUBSCRIBED || c->c.flags & REDIS_MONITORING)) {
		self->js_callbacks[callback_id].Reset();
		self->js_callbacks.erase(callback_id);
	}
	
	if (reply->type == REDIS_REPLY_ERROR) {
		total_size += reply->len;
        js_cb.Call(Napi::GetCurrentContext()->Global(), {Napi::String::New(reply->str), Napi::Number::New(total_size)});
		return;
	}

	Napi::Value resp = ParseResponse(reply, &total_size);
	if (resp->IsUndefined()) {
        js_cb.Call(Napi::GetCurrentContext()->Global(), {Napi::String::New("Protocol error, can not parse answer from redis"), Napi::Number::New(total_size)});
		return;
	}
	
    js_cb.Call(Napi::GetCurrentContext()->Global(), {Napi::Value, resp, Napi::Number::New(total_size)});
}
*/

Napi::Function Nedis::GetClass(Napi::Env env) {
    return DefineClass(env, "Nedis", {
        Nedis::InstanceMethod("Connect", &Nedis::Connect),
		Nedis::InstanceMethod("Disconnect", &Nedis::Disconnect),
		Nedis::InstanceMethod("SendCommand", &Nedis::SendCommand),
    });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "Nedis"), Nedis::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
