// Windows.h strict mode
#define STRICT
#define UNICODE

#include <napi.h>
#include "uv.h"

#include <assert.h>

#include <cstdio>
#include <memory>
#include <iostream>

#import <UserNotifications/UserNotifications.h>

#import "GHDesktopNotificationsManager.h"
#import "Utils.h"

using namespace Napi;

GHDesktopNotificationsManager *desktopNotificationsManager = nil;
Napi::ThreadSafeFunction notificationsCallback;

namespace
{
  Napi::ThreadSafeFunction notificationCallback;

  Napi::Value initializeNotifications(const Napi::CallbackInfo &info)
  {
    const Napi::Env &env = info.Env();

    if (desktopNotificationsManager)
    {
      return env.Undefined();
    }

    // There is a second argument, which is an object with options, but it is
    // not used on macOS.
    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[0].IsFunction())
    {
      Napi::TypeError::New(env, "Callback must be a function.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto callback = info[0].As<Napi::Function>();

    notificationsCallback = Napi::ThreadSafeFunction::New(callback.Env(), callback, "Notification Callback", 0, 1);

    desktopNotificationsManager = [GHDesktopNotificationsManager new];
    desktopNotificationsManager.completionHandler = ^(NSString *event, NSString *identifier, NSDictionary *userInfo, dispatch_block_t completionHandler) {
      auto cb = [event, identifier, userInfo, completionHandler](Napi::Env env, Napi::Function jsCallback)
      {
        jsCallback.Call({
          Napi::String::New(env, event.UTF8String),
          Napi::String::New(env, identifier.UTF8String),
          getNapiValueFromObject(env, userInfo),
        });

        // Invoke the OS completion handler
        completionHandler();
      };

      notificationsCallback.BlockingCall(cb);
    };

    return info.Env().Undefined();
  }

  Napi::Value showNotification(const Napi::CallbackInfo &info)
  {
    const Napi::Env &env = info.Env();

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

    NSString *notificationID = [NSString stringWithCString:info[0].As<Napi::String>().Utf8Value().c_str()
                                                  encoding:[NSString defaultCStringEncoding]];
    auto titleUTF16 = info[1].As<Napi::String>().Utf16Value();
    NSString *title = [NSString stringWithCharacters:(const unichar *)titleUTF16.c_str()
                                              length:titleUTF16.size()];
    auto bodyUTF16 = info[2].As<Napi::String>().Utf16Value();
    NSString *body = [NSString stringWithCharacters:(const unichar *)bodyUTF16.c_str()
                                             length:bodyUTF16.size()];
    NSLog(@"the title is '%@' and the body is '%@'", title, body);

    UNMutableNotificationContent* content = [[UNMutableNotificationContent alloc] init];
    content.title = [NSString localizedUserNotificationStringForKey:title arguments:nil];
    content.body = [NSString localizedUserNotificationStringForKey:body arguments:nil];

    if (info.Length() > 3 && info[3].IsObject())
    {
      auto userInfo = info[3].As<Napi::Object>();
      content.userInfo = getObjectFromNapiValue(env, userInfo);
    }

    // Create the request object.
    UNNotificationRequest* request = [UNNotificationRequest
          requestWithIdentifier:notificationID content:content trigger:nil];

    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];

    [center
     requestAuthorizationWithOptions:UNAuthorizationOptionBadge | UNAuthorizationOptionSound | UNAuthorizationOptionAlert
     completionHandler:^(BOOL granted, NSError *error) {

        if (error != nil) {
            NSLog(@"Error requesting permission %@", error);
            return;
        }

        [center addNotificationRequest:request withCompletionHandler:^(NSError * _Nullable error) {
          if (error != nil) {
              NSLog(@"Error posting notification: %@", error.localizedDescription);
          }
        }];
    }];

    return env.Undefined();
  }

  Napi::Value closeNotification(const Napi::CallbackInfo &info)
  {
    const Napi::Env &env = info.Env();

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

    NSString *notificationID = [NSString stringWithCString:info[0].As<Napi::String>().Utf8Value().c_str()
                                                  encoding:[NSString defaultCStringEncoding]];

    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
    [center removePendingNotificationRequestsWithIdentifiers:@[notificationID]];

    return env.Undefined();
  }

  Napi::Value getNotificationsPermission(const Napi::CallbackInfo &info)
  {
    const Napi::Env &env = info.Env();

    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    __block std::string permission = "default";

    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
    [center getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings *settings) {
      dispatch_semaphore_signal(semaphore);

      if (settings.authorizationStatus == UNAuthorizationStatusNotDetermined)
      {
        permission = "default";
      }
      else if (settings.authorizationStatus == UNAuthorizationStatusDenied)
      {
        permission = "denied";
      }
      else
      {
        permission = "granted";
      }
    }];

    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);

    return Napi::String::New(env, permission);
  }

  Napi::Value requestNotificationsPermission(const Napi::CallbackInfo &info)
  {
    const Napi::Env &env = info.Env();

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];

    [center
     requestAuthorizationWithOptions:UNAuthorizationOptionBadge | UNAuthorizationOptionSound | UNAuthorizationOptionAlert
     completionHandler:^(BOOL granted, NSError *error) {

      if (error != nil) {
          NSLog(@"Error requesting permission %@", error);
          return;
      }

      deferred.Resolve(Napi::Boolean::New(env, granted));
    }];

    return deferred.Promise();
  }

  Napi::Object Init(Napi::Env env, Napi::Object exports)
  {
    exports.Set(Napi::String::New(env, "initializeNotifications"), Napi::Function::New(env, initializeNotifications));
    exports.Set(Napi::String::New(env, "showNotification"), Napi::Function::New(env, showNotification));
    exports.Set(Napi::String::New(env, "closeNotification"), Napi::Function::New(env, closeNotification));
    exports.Set(Napi::String::New(env, "requestNotificationsPermission"), Napi::Function::New(env, requestNotificationsPermission));
    exports.Set(Napi::String::New(env, "getNotificationsPermission"), Napi::Function::New(env, getNotificationsPermission));

    return exports;
  }
}

#if NODE_MAJOR_VERSION >= 10
NAN_MODULE_WORKER_ENABLED(desktopNotificationsNativeModule, Init)
#else
NODE_API_MODULE(desktopNotificationsNativeModule, Init);
#endif
