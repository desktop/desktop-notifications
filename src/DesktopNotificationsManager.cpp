#include "DesktopNotificationsManager.h"
#include "DesktopNotification.h"
#include "Utils.h"
#include "DesktopNotificationsActionCenterActivator.h"

#include <wrl\wrappers\corewrappers.h>
#include <sstream>
#include <iostream>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Details;
using namespace ABI::Windows::UI;
using namespace ABI::Windows::UI::Notifications;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace Windows::Foundation;
using namespace Wrappers;

ComPtr<IToastNotificationHistory> DesktopNotificationsManager::getHistory()
{
    ComPtr<IToastNotificationManagerStatics2> toastStatics2;
    if (DN_CHECK_RESULT(m_toastManager.As(&toastStatics2)))
    {
        ComPtr<IToastNotificationHistory> nativeHistory;
        DN_CHECK_RESULT(toastStatics2->get_History(&nativeHistory));
        return nativeHistory;
    }
    return {};
}

DesktopNotificationsManager::DesktopNotificationsManager(const std::wstring &toastActivatorClsid) : m_toastActivatorClsid(toastActivatorClsid)
{
    {
        HRESULT hr = Windows::Foundation::Initialize(RO_INIT_MULTITHREADED);
        if (!SUCCEEDED(hr))
        {
            std::wcerr << L"Failed to initialize with RO_INIT_MULTITHREADED: " << hr << std::endl;
        }
    }
    {
        HRESULT hr = GetActivationFactory(
            HStringReference(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager)
                .Get(),
            &m_toastManager);
        if (!SUCCEEDED(hr))
        {
            std::wcerr << L"Failed to register com Factory, please make sure you "
                          L"correctly initialized with RO_INIT_MULTITHREADED: "
                       << hr
                       << std::endl;
        }
    }
    {
        // Delete later: this will be set by the host app
        HRESULT hr = SetCurrentProcessExplicitAppUserModelID(L"com.squirrel.GitHubDesktop.GitHubDesktop");
        if (!SUCCEEDED(hr))
        {
            std::wcerr << L"DesktopNotificationsManager: Failed to set AUMID"
                       << std::endl;
            return;
        }
    }

    RegisterClassObjects(m_toastActivatorClsid);
}

void DesktopNotificationsManager::SignalObjectCountZero()
{
    // Do nothing
}

HRESULT DesktopNotificationsManager::RegisterClassObjects(const std::wstring &toastActivatorClsid)
{
    // Create an out-of-proc COM module with caching disabled. The supplied
    // method is invoked when the last instance object of the module is released.
    auto &module = Module<OutOfProcDisableCaching>::Create(
        this, &DesktopNotificationsManager::SignalObjectCountZero);

    // Usually COM module classes statically define their CLSID at compile time
    // through the use of various macros, and WRL::Module internals takes care of
    // creating the class objects and registering them. However, we need to
    // register the same object with different CLSIDs depending on a runtime
    // setting, so we handle that logic here.

    ComPtr<IUnknown> factory;
    unsigned int flags = ModuleType::OutOfProcDisableCaching;

    HRESULT hr = CreateClassFactory<
        SimpleClassFactory<DesktopNotificationsActionCenterActivator>>(
        &flags, nullptr, __uuidof(IClassFactory), &factory);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create Factory for Action Center activator; hr: " << hr << std::endl;
        return hr;
    }

    ComPtr<IClassFactory> class_factory;
    hr = factory.As(&class_factory);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create IClassFactory for Action Center activator; hr: " << hr << std::endl;
        return hr;
    }

    CLSID activatorClsid;
    CLSIDFromString(toastActivatorClsid.c_str(), &activatorClsid);

    // All pointers in this array are unowned. Do not release them.
    IClassFactory *class_factories[] = {class_factory.Get()};
    IID class_ids[] = {activatorClsid};

    hr = module.RegisterCOMObject(nullptr, class_ids, class_factories, m_comCookies,
                                  std::extent<decltype(m_comCookies)>());
    if (FAILED(hr))
    {
        std::cerr << "Failed to register Action Center activator; hr: " << hr << std::endl;
    }
    else
    {
        module.IncrementObjectCount();
    }

    return hr;
}

DesktopNotificationsManager::~DesktopNotificationsManager()
{
    for (auto n : m_desktopNotifications)
    {
        closeNotification(n);
    }
    m_desktopNotifications.clear();

    UnregisterClassObjects();
}

HRESULT DesktopNotificationsManager::UnregisterClassObjects()
{
    auto &module = Module<OutOfProcDisableCaching>::GetModule();

    module.DecrementObjectCount();

    HRESULT hr = module.UnregisterCOMObject(nullptr, m_comCookies,
                                            std::extent<decltype(m_comCookies)>());
    if (FAILED(hr))
    {
        std::cerr << "Failed to unregister Action Center activator; hr: " << hr << std::endl;
    }

    return hr;
}

HRESULT DesktopNotificationsManager::displayToast(const std::wstring &id,
                                                  const std::wstring &title, const std::wstring &body,
                                                  Napi::Function &callback)
{
    ComPtr<DesktopNotification> d(new DesktopNotification(id, title, body, callback));
    m_desktopNotifications.push_back(d);
    return d->createToast(m_toastManager);
}

bool DesktopNotificationsManager::closeToast(const std::wstring &id)
{
    // Iterate through m_desktopNotifications looking for a notification with
    // the given id, close that notification and remove it from the list.
    for (auto it = m_desktopNotifications.begin(); it != m_desktopNotifications.end(); ++it)
    {
        auto notification = *it;
        if (notification->getID() == id)
        {
            m_desktopNotifications.erase(it);
            return closeNotification(notification);
        }
    }

    return false;
}

bool DesktopNotificationsManager::closeNotification(ComPtr<DesktopNotification> d)
{
    d->stopListeningEvents();

    if (auto history = getHistory())
    {
        if (DN_CHECK_RESULT(history->RemoveGroupedTagWithId(
                HStringReference(d->getID().c_str()).Get(), HStringReference(DN_GROUP_NAME).Get(),
                HStringReference(d->getAppID().c_str()).Get())))
        {
            return true;
        }
    }
    std::wcout << "Notification " << d->getID() << " does not exist" << std::endl;
    return false;
}
