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

using namespace Napi;

namespace
{
  std::shared_ptr<DesktopNotificationsManager> desktopNotificationsManager;

  LPWSTR utf8ToWideChar(std::string utf8)
  {
    int wide_char_length = MultiByteToWideChar(CP_UTF8,
                                               0,
                                               utf8.c_str(),
                                               -1,
                                               nullptr,
                                               0);
    if (wide_char_length == 0)
    {
      return nullptr;
    }

    LPWSTR result = new WCHAR[wide_char_length];
    if (MultiByteToWideChar(CP_UTF8,
                            0,
                            utf8.c_str(),
                            -1,
                            result,
                            wide_char_length) == 0)
    {
      delete[] result;
      return nullptr;
    }

    return result;
  }

  Napi::Value initializeNotifications(const Napi::CallbackInfo &info)
  {
    Napi::Env &env = info.Env();

    if (desktopNotificationsManager)
    {
      //Napi::TypeError::New(env, "Notifications already initialized.").ThrowAsJavaScriptException();
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

    auto toastActivatorClsid = utf8ToWideChar(info[0].As<Napi::String>());
    desktopNotificationsManager = std::make_shared<DesktopNotificationsManager>(toastActivatorClsid);

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
      Napi::TypeError::New(env, "Notifications not initialized. Call `initializeNotifications`.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() < 4)
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

    if (!info[3].IsFunction())
    {
      Napi::TypeError::New(env, "Callback must be a function.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto id = utf8ToWideChar(info[0].As<Napi::String>());
    auto title = utf8ToWideChar(info[1].As<Napi::String>());
    auto body = utf8ToWideChar(info[2].As<Napi::String>());
    auto callback = info[3].As<Napi::Function>();

    desktopNotificationsManager->displayToast(id, title, body, callback);

    return env.Undefined();
  }

  Napi::Value closeNotification(const Napi::CallbackInfo &info)
  {
    Napi::Env &env = info.Env();

    if (!desktopNotificationsManager)
    {
      Napi::TypeError::New(env, "Notifications not initialized. Call `initializeNotifications`.").ThrowAsJavaScriptException();
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

    auto id = utf8ToWideChar(info[0].As<Napi::String>());

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
