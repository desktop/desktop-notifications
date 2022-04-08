#import "Utils.h"

Napi::Object getNapiObjectFromDictionary(const Napi::Env &env, NSDictionary *d)
{
  Napi::Object obj = Napi::Object::New(env);
  for (NSString *key in d) {
    id value = d[key];
    obj.Set(Napi::String::New(env, key.UTF8String), getNapiValueFromObject(env, value));
  }
  return obj;
}

Napi::Array getNapiArrayFromArray(const Napi::Env &env, NSArray *a)
{
  Napi::Array result = Napi::Array::New(env);
  for (NSUInteger i = 0; i < a.count; i++) {
    id value = a[i];
    result.Set(i, getNapiValueFromObject(env, value));
  }
  return result;
}

Napi::Value getNapiValueFromObject(const Napi::Env &env, id value)
{
  if (value == nil)
  {
    return env.Undefined();
  }

  if ([value isKindOfClass:[NSString class]])
  {
    NSString *string = (NSString *)value;
    return Napi::String::New(env, string.UTF8String);
  }
  else if ([value isKindOfClass:[NSNumber class]])
  {
    NSNumber *number = value;

    // Booleans wrapped in NSNumber require special treatment
    CFTypeID boolID = CFBooleanGetTypeID(); // the type ID of CFBoolean
    CFTypeID numID = CFGetTypeID((__bridge CFTypeRef)(number)); // the type ID of number
    if (numID == boolID)
    {
      return Napi::Boolean::New(env, [number boolValue]);
    }

    CFNumberType numberType = CFNumberGetType((CFNumberRef)value);
    if (numberType == kCFNumberFloatType || numberType == kCFNumberDoubleType)
    {
      return Napi::Number::New(env, number.doubleValue);
    }
    else if (numberType == kCFNumberSInt64Type || numberType == kCFNumberLongLongType)
    {
      return Napi::Number::New(env, number.longLongValue);
    }
    else if (numberType == kCFNumberSInt32Type || numberType == kCFNumberIntType)
    {
      return Napi::Number::New(env, number.intValue);
    }
    else if (numberType == kCFNumberSInt16Type || numberType == kCFNumberShortType)
    {
      return Napi::Number::New(env, number.shortValue);
    }
    else if (numberType == kCFNumberSInt8Type || numberType == kCFNumberCharType)
    {
      return Napi::Number::New(env, number.charValue);
    }
    else if (numberType == kCFNumberCFIndexType || numberType == kCFNumberNSIntegerType)
    {
      return Napi::Number::New(env, number.integerValue);
    }
    else
    {
      return Napi::Number::New(env, number.longValue);
    }
  }
  else if ([value isKindOfClass:[NSDictionary class]])
  {
    return getNapiObjectFromDictionary(env, value);
  }
  else if ([value isKindOfClass:[NSArray class]])
  {
    return getNapiArrayFromArray(env, value);
  }

  return env.Undefined();
}
