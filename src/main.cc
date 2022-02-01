// Windows.h strict mode
#define STRICT
#define UNICODE

#include <napi.h>
#include "uv.h"

#include <assert.h>
#include <windows.h>

#include <cstdio>
#include <memory>
#include <iostream>

#include "DesktopNotificationsManager.h"
#include "Utils.h"

using namespace Napi;

namespace
{
  std::shared_ptr<DesktopNotificationsManager> desktopNotificationsManager;

  Napi::Value initializeNotifications(const Napi::CallbackInfo &info)
  {
    Napi::Env &env = info.Env();

    if (desktopNotificationsManager)
    {
      return env.Undefined();
    }

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[0].IsString())
    {
      Napi::TypeError::New(env, "A string was expected for the first argument, but wasn't received.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[1].IsFunction())
    {
      Napi::TypeError::New(env, "Callback must be a function.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto toastActivatorClsid = Utils::utf8ToWideChar(info[0].As<Napi::String>());
    auto callback = info[1].As<Napi::Function>();

    desktopNotificationsManager = std::make_shared<DesktopNotificationsManager>(toastActivatorClsid, callback);

    return info.Env().Undefined();
  }

  Napi::Value terminateNotifications(const Napi::CallbackInfo &info)
  {
    desktopNotificationsManager = nullptr;
    return info.Env().Undefined();
  }

  Napi::Value showNotification(const Napi::CallbackInfo &info)
  {
    Napi::Env &env = info.Env();

    if (!desktopNotificationsManager)
    {
      DN_LOG_ERROR("Cannot show notification: notifications not initialized.");
      return env.Undefined();
    }

    if (info.Length() < 3)
    {
      Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[0].IsString())
    {
      Napi::TypeError::New(env, "A string was expected for the first argument, but wasn't received.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[1].IsString())
    {
      Napi::TypeError::New(env, "A string was expected for the second argument, but wasn't received.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[2].IsString())
    {
      Napi::TypeError::New(env, "A string was expected for the second argument, but wasn't received.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto id = Utils::utf8ToWideChar(info[0].As<Napi::String>());
    auto title = Utils::utf8ToWideChar(info[1].As<Napi::String>());
    auto body = Utils::utf8ToWideChar(info[2].As<Napi::String>());

    desktopNotificationsManager->displayToast(id, title, body);

    return env.Undefined();
  }

  Napi::Value closeNotification(const Napi::CallbackInfo &info)
  {
    Napi::Env &env = info.Env();

    if (!desktopNotificationsManager)
    {
      DN_LOG_ERROR("Cannot close notification: notifications not initialized.");
      return env.Undefined();
    }

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[0].IsString())
    {
      Napi::TypeError::New(env, "A string was expected for the first argument, but wasn't received.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto id = Utils::utf8ToWideChar(info[0].As<Napi::String>());

    desktopNotificationsManager->closeToast(id);

    return env.Undefined();
  }

  Napi::Object Init(Napi::Env env, Napi::Object exports)
  {
    exports.Set(Napi::String::New(env, "initializeNotifications"), Napi::Function::New(env, initializeNotifications));
    exports.Set(Napi::String::New(env, "terminateNotifications"), Napi::Function::New(env, terminateNotifications));
    exports.Set(Napi::String::New(env, "showNotification"), Napi::Function::New(env, showNotification));
    exports.Set(Napi::String::New(env, "closeNotification"), Napi::Function::New(env, closeNotification));

    return exports;
  }
}

#if NODE_MAJOR_VERSION >= 10
NAN_MODULE_WORKER_ENABLED(desktopNotificationsNativeModule, Init)
#else
NODE_API_MODULE(desktopNotificationsNativeModule, Init);
#endif
