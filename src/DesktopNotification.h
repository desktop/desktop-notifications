#pragma once
#include "DesktopNotificationsManager.h"
#include <iostream>

typedef ABI::Windows::Foundation::ITypedEventHandler<
    ABI::Windows::UI::Notifications::ToastNotification *, ::IInspectable *>
    DesktopToastActivatedEventHandler;
typedef ABI::Windows::Foundation::ITypedEventHandler<
    ABI::Windows::UI::Notifications::ToastNotification *,
    ABI::Windows::UI::Notifications::ToastDismissedEventArgs *>
    DesktopToastDismissedEventHandler;
typedef ABI::Windows::Foundation::ITypedEventHandler<
    ABI::Windows::UI::Notifications::ToastNotification *,
    ABI::Windows::UI::Notifications::ToastFailedEventArgs *>
    DesktopToastFailedEventHandler;

class DesktopNotification : public Microsoft::WRL::Implements<DesktopToastActivatedEventHandler,
                                                              DesktopToastDismissedEventHandler,
                                                              DesktopToastFailedEventHandler>
{

public:
    explicit DesktopNotification::DesktopNotification(const std::wstring &id,
                                                      const std::wstring &title, const std::wstring &body,
                                                      const Napi::Function &callback);
    ~DesktopNotification();

    // DesktopToastActivatedEventHandler
    IFACEMETHODIMP Invoke(_In_ ABI::Windows::UI::Notifications::IToastNotification *sender,
                          _In_ IInspectable *args);

    // DesktopToastDismissedEventHandler
    IFACEMETHODIMP Invoke(_In_ ABI::Windows::UI::Notifications::IToastNotification *sender,
                          _In_ ABI::Windows::UI::Notifications::IToastDismissedEventArgs *e);

    // DesktopToastFailedEventHandler
    IFACEMETHODIMP Invoke(_In_ ABI::Windows::UI::Notifications::IToastNotification *sender,
                          _In_ ABI::Windows::UI::Notifications::IToastFailedEventArgs *e);

    // IUnknown
    IFACEMETHODIMP_(ULONG)
    AddRef()
    {
        return InterlockedIncrement(&m_ref);
    }

    IFACEMETHODIMP_(ULONG)
    Release()
    {
        ULONG l = InterlockedDecrement(&m_ref);
        if (l == 0)
        {
            delete this;
        }
        return l;
    }

    IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _COM_Outptr_ void **ppv)
    {
        if (IsEqualIID(riid, IID_IUnknown))
        {
            *ppv = static_cast<IUnknown *>(static_cast<DesktopToastActivatedEventHandler *>(this));
        }
        else if (IsEqualIID(riid, __uuidof(DesktopToastActivatedEventHandler)))
        {
            *ppv = static_cast<DesktopToastActivatedEventHandler *>(this);
        }
        else if (IsEqualIID(riid, __uuidof(DesktopToastDismissedEventHandler)))
        {
            *ppv = static_cast<DesktopToastDismissedEventHandler *>(this);
        }
        else if (IsEqualIID(riid, __uuidof(DesktopToastFailedEventHandler)))
        {
            *ppv = static_cast<DesktopToastFailedEventHandler *>(this);
        }
        else
        {
            *ppv = nullptr;
        }

        if (*ppv)
        {
            reinterpret_cast<IUnknown *>(*ppv)->AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    std::wstring getID()
    {
        return m_id;
    }

    std::wstring getAppID()
    {
        return m_appID;
    }

    // Create and display the toast
    HRESULT createToast(Microsoft::WRL::ComPtr<ABI::Windows::UI::Notifications::IToastNotificationManagerStatics> toastManager);

    void stopListeningEvents();

private:
    ULONG m_ref;
    Napi::ThreadSafeFunction m_callback;
    std::wstring m_appID;

    std::wstring m_title;
    std::wstring m_body;
    std::wstring m_id;
    EventRegistrationToken m_activatedToken, m_dismissedToken, m_failedToken;

    Microsoft::WRL::ComPtr<ABI::Windows::Data::Xml::Dom::IXmlDocument> m_toastXml;
    Microsoft::WRL::ComPtr<ABI::Windows::UI::Notifications::IToastNotifier> m_notifier;
    Microsoft::WRL::ComPtr<ABI::Windows::UI::Notifications::IToastNotification> m_notification;

    void invokeJSCallback(std::string eventName);

    // Set the values of each of the text nodes
    HRESULT setTextValues();
    HRESULT startListeningEvents();
    HRESULT setNodeValueString(const HSTRING &inputString, ABI::Windows::Data::Xml::Dom::IXmlNode *node);
    HRESULT addAttribute(const std::wstring &name, ABI::Windows::Data::Xml::Dom::IXmlNamedNodeMap *attributeMap);
    HRESULT addAttribute(const std::wstring &name, ABI::Windows::Data::Xml::Dom::IXmlNamedNodeMap *attributeMap,
                         const std::wstring &value);
    void printXML();
    std::wstring formatAction(
        const std::wstring &action,
        const std::vector<std::pair<std::wstring, std::wstring>> &extraData) const;
};
