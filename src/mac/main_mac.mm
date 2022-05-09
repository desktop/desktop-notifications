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

// Dummy value to pass into function parameter for ThreadSafeFunction.
Napi::Value NoOp(const Napi::CallbackInfo &info) {
  return info.Env().Undefined();
}

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
          getNapiValueFromNSObject(env, userInfo),
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
      Napi::TypeError::New(env, "A string was expected for the third argument, but wasn't received.").ThrowAsJavaScriptException();
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

    UNMutableNotificationContent* content = [[UNMutableNotificationContent alloc] init];
    content.title = [NSString localizedUserNotificationStringForKey:title arguments:nil];
    content.body = [NSString localizedUserNotificationStringForKey:body arguments:nil];
    content.sound = [UNNotificationSound defaultSound];

    if (info.Length() > 3 && info[3].IsObject())
    {
      auto userInfo = info[3].As<Napi::Object>();
      content.userInfo = getNSObjectFromNapiValue(env, userInfo);
    }

    // Create the request object.
    UNNotificationRequest* request = [UNNotificationRequest
          requestWithIdentifier:notificationID content:content trigger:nil];

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::ThreadSafeFunction ts_fn = Napi::ThreadSafeFunction::New(
      env, Napi::Function::New(env, NoOp), "showNotificationCallback", 0, 1);

    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
    __block Napi::ThreadSafeFunction tsfn = ts_fn;

    [center
     requestAuthorizationWithOptions:UNAuthorizationOptionBadge | UNAuthorizationOptionSound | UNAuthorizationOptionAlert
     completionHandler:^(BOOL granted, NSError *error) {

        if (error != nil || !granted) {
            NSLog(@"Permission to display notification wasn't granted: %@", error);

            auto callback = [deferred](Napi::Env env, Napi::Function js_cb) {
              deferred.Reject(env.Undefined());
            };
            tsfn.BlockingCall(callback);
            tsfn.Release();
            return;
        }

        [center addNotificationRequest:request withCompletionHandler:^(NSError * _Nullable error) {
          if (error != nil) {
              NSLog(@"Error posting notification: %@", error.localizedDescription);

              auto callback = [deferred](Napi::Env env, Napi::Function js_cb) {
                deferred.Reject(env.Undefined());
              };
              tsfn.BlockingCall(callback);
              tsfn.Release();
              return;
          }

            auto callback = [deferred](Napi::Env env, Napi::Function js_cb) {
              deferred.Resolve(env.Undefined());
            };
            tsfn.BlockingCall(callback);
            tsfn.Release();
        }];
    }];

    return deferred.Promise();
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

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::ThreadSafeFunction ts_fn = Napi::ThreadSafeFunction::New(
      env, Napi::Function::New(env, NoOp), "getNotificationsPermissionCallback", 0, 1);

    __block Napi::ThreadSafeFunction tsfn = ts_fn;
    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
    [center getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings *settings) {

      std::string permission = "default";

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

      auto callback = [permission, deferred](Napi::Env env, Napi::Function js_cb) {
        deferred.Resolve(Napi::String::New(env, permission));
      };
      tsfn.BlockingCall(callback);
      tsfn.Release();
    }];

    return deferred.Promise();
  }

  Napi::Value requestNotificationsPermission(const Napi::CallbackInfo &info)
  {
    const Napi::Env &env = info.Env();

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    Napi::ThreadSafeFunction ts_fn = Napi::ThreadSafeFunction::New(
      env, Napi::Function::New(env, NoOp), "requestNotificationsPermissionCallback", 0, 1);

    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];

    __block Napi::ThreadSafeFunction tsfn = ts_fn;
    [center
     requestAuthorizationWithOptions:UNAuthorizationOptionBadge | UNAuthorizationOptionSound | UNAuthorizationOptionAlert
     completionHandler:^(BOOL granted, NSError *error) {

      if (error != nil) {
          NSLog(@"Error requesting permission %@", error);
      }

      auto callback = [granted, deferred](Napi::Env env, Napi::Function js_cb) {
        deferred.Resolve(Napi::Boolean::New(env, granted));
      };
      tsfn.BlockingCall(callback);
      tsfn.Release();
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
    exports.Set(Napi::String::New(env, "requestNotificationsPermission"), Napi::Function::New(env, requestNotificationsPermission));

    return exports;
  }
}

NODE_API_MODULE(desktopNotificationsNativeModule, Init);
