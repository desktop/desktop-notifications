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

using namespace Napi;

namespace
{
  Napi::ThreadSafeFunction notificationCallback;

  Napi::Value initializeNotifications(const Napi::CallbackInfo &info)
  {
    const Napi::Env &env = info.Env();

    std::cout << "initializeNotifications - start" << std::endl;

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // The first parameter is the toast activator CLSID for Windows, which could be ignored.

    if (!info[1].IsFunction())
    {
      Napi::TypeError::New(env, "Callback must be a function.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    //auto callback = info[1].As<Napi::Function>();
    //notificationCallback = Napi::ThreadSafeFunction::New(callback.Env(), callback, "Notification Callback", 0, 1);
    std::cout << "initializeNotifications - end" << std::endl;

    return env.Undefined();
  }

  Napi::Value terminateNotifications(const Napi::CallbackInfo &info)
  {
    const Napi::Env &env = info.Env();

    // Do nothing

    return env.Undefined();
  }

  Napi::Value showNotification(const Napi::CallbackInfo &info)
  {
    const Napi::Env &env = info.Env();
    std::cout << "showNotification - start" << std::endl;

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
    NSString *title = [NSString stringWithCString:info[1].As<Napi::String>().Utf8Value().c_str()
                                         encoding:[NSString defaultCStringEncoding]];
    NSString *body = [NSString stringWithCString:info[2].As<Napi::String>().Utf8Value().c_str()
                                        encoding:[NSString defaultCStringEncoding]];

    std::cout << "showNotification - 1 " << std::endl;
    NSLog(@"FROM NSLOG: %@ %@ %@", notificationID, title, body);

    UNMutableNotificationContent* content = [[UNMutableNotificationContent alloc] init];
    content.title = [NSString localizedUserNotificationStringForKey:title arguments:nil];
    content.body = [NSString localizedUserNotificationStringForKey:body arguments:nil];
    std::cout << "showNotification - 2 " << std::endl;

    // Create the request object.
    UNNotificationRequest* request = [UNNotificationRequest
          requestWithIdentifier:notificationID content:content trigger:nil];

    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];

    [center
     requestAuthorizationWithOptions:UNAuthorizationOptionBadge | UNAuthorizationOptionSound | UNAuthorizationOptionAlert
     completionHandler:^(BOOL granted, NSError *error) {
    std::cout << "showNotification - 4 " << std::endl;

        if (error != nil) {
            // Handle the error here.
            NSLog(@"error requesting permission %@", error);
            return;
        }
    std::cout << "showNotification - 5 " << std::endl;

        // Enable or disable features based on the authorization.
        [center addNotificationRequest:request withCompletionHandler:^(NSError * _Nullable error) {
    std::cout << "showNotification - 7 " << std::endl;
          if (error != nil) {
              NSLog(@"%@", error.localizedDescription);
          }
        }];
    std::cout << "showNotification - 6 " << std::endl;
    }];
    std::cout << "showNotification - 3 " << std::endl;

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

  Napi::Object Init(Napi::Env env, Napi::Object exports)
  {
    exports.Set(Napi::String::New(env, "initializeNotifications"), Napi::Function::New(env, initializeNotifications));
    exports.Set(Napi::String::New(env, "terminateNotifications"), Napi::Function::New(env, terminateNotifications));
    exports.Set(Napi::String::New(env, "showNotification"), Napi::Function::New(env, showNotification));
    exports.Set(Napi::String::New(env, "closeNotification"), Napi::Function::New(env, closeNotification));
    exports.Set(Napi::String::New(env, "getNotificationsPermission"), Napi::Function::New(env, getNotificationsPermission));

    return exports;
  }
}

#if NODE_MAJOR_VERSION >= 10
NAN_MODULE_WORKER_ENABLED(desktopNotificationsNativeModule, Init)
#else
NODE_API_MODULE(desktopNotificationsNativeModule, Init);
#endif
