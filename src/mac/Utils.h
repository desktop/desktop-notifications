#include <napi.h>
#import <Foundation/Foundation.h>

Napi::Value getNapiValueFromObject(const Napi::Env &env, id value);
id getObjectFromNapiValue(const Napi::Env &env, const Napi::Value &value);
