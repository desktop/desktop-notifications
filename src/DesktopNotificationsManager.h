#pragma once

#include <sdkddkver.h>

// Windows Header Files:
#include <windows.h>
#include <sal.h>
#include <psapi.h>
#include <strsafe.h>
#include <objbase.h>
#include <shobjidl.h>
#include <functiondiscoverykeys.h>
#include <guiddef.h>
#include <shlguid.h>

#include <wrl/client.h>
#include <wrl/implements.h>
#include <windows.ui.notifications.h>

#include <string>
#include <vector>

#include <napi.h>

#include "DesktopNotification.h"

using namespace Microsoft::WRL;
using namespace ABI::Windows::Data::Xml::Dom;

class DesktopNotificationsManager
{
    friend class DesktopNotification;

public:
    DesktopNotificationsManager(const std::wstring &toastActivatorClsid);
    ~DesktopNotificationsManager();

    HRESULT displayToast(const std::wstring &id,
                         const std::wstring &title, const std::wstring &body,
                         Napi::Function &callback);
    bool closeToast(const std::wstring &id);

private:
    Microsoft::WRL::ComPtr<ABI::Windows::UI::Notifications::IToastNotificationHistory> getHistory();

    Microsoft::WRL::ComPtr<ABI::Windows::UI::Notifications::IToastNotificationManagerStatics> m_toastManager;

    std::vector<Microsoft::WRL::ComPtr<DesktopNotification>> m_desktopNotifications;

    void SignalObjectCountZero();
    HRESULT RegisterClassObjects(const std::wstring &toastActivatorClsid);
    HRESULT UnregisterClassObjects();
    bool closeNotification(Microsoft::WRL::ComPtr<DesktopNotification> d);

    // Identifiers of registered class objects. Used for unregistration.
    DWORD m_comCookies[1] = {0};
    const std::wstring m_toastActivatorClsid;
};
