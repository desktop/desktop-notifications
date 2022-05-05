#pragma once

#include <algorithm>
#include <ntverp.h>
#include <sstream>
#include <wrl.h>

#include "Utils.h"

#define DN_WSTRINGIFY(X) L##X
#define DN_STRINGIFY(X) DN_WSTRINGIFY(X)

typedef struct NOTIFICATION_USER_INPUT_DATA
{
    LPCWSTR Key;
    LPCWSTR Value;
} NOTIFICATION_USER_INPUT_DATA;

MIDL_INTERFACE("53E31837-6600-4A81-9395-75CFFE746F94")
INotificationActivationCallback : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE Activate(
        __RPC__in_string LPCWSTR appUserModelId, __RPC__in_opt_string LPCWSTR invokedArgs,
        __RPC__in_ecount_full_opt(count) const NOTIFICATION_USER_INPUT_DATA *data,
        ULONG count) = 0;
};

// The COM server which implements the callback notifcation from Action Center
class DesktopNotificationsActionCenterActivator
    : public Microsoft::WRL::RuntimeClass<
          Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
          INotificationActivationCallback>
{
public:
    DesktopNotificationsActionCenterActivator() {}
    virtual HRESULT STDMETHODCALLTYPE Activate(__RPC__in_string LPCWSTR appUserModelId,
                                               __RPC__in_opt_string LPCWSTR invokedArgs,
                                               __RPC__in_ecount_full_opt(count)
                                                   const NOTIFICATION_USER_INPUT_DATA *data,
                                               ULONG count) override
    {
        // Do nothing
        printf("Hello from the COM server! Count: %d\n", count);
        std::wstringstream msg;
        for (ULONG i = 0; i < count; ++i)
        {
            std::wstring tmp = data[i].Value;
            DN_LOG_ERROR(tmp);
            // printing \r to stdcout is kind of problematic :D
            std::replace(tmp.begin(), tmp.end(), L'\r', L'\n');
            msg << tmp;
        }

        DN_LOG_ERROR(msg.str());
        DN_LOG_ERROR(invokedArgs);

        return S_OK;
    }
};
