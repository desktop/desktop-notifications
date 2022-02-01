#pragma once
#include "DesktopNotificationsManager.h"
#include <iostream>

class DesktopNotificationsManager;

class DesktopNotification
{

public:
    explicit DesktopNotification::DesktopNotification(const std::wstring &id,
                                                      const std::wstring &appID,
                                                      const std::wstring &title,
                                                      const std::wstring &body);

    std::wstring getID()
    {
        return m_id;
    }

    // Create and display the toast
    HRESULT createToast(Microsoft::WRL::ComPtr<ABI::Windows::UI::Notifications::IToastNotificationManagerStatics> toastManager,
                        DesktopNotificationsManager *desktopNotificationsManager);

    void stopListeningEvents();

private:
    std::wstring m_appID;

    std::wstring m_title;
    std::wstring m_body;
    std::wstring m_id;
    EventRegistrationToken m_activatedToken, m_dismissedToken, m_failedToken;

    Microsoft::WRL::ComPtr<ABI::Windows::Data::Xml::Dom::IXmlDocument> m_toastXml;
    Microsoft::WRL::ComPtr<ABI::Windows::UI::Notifications::IToastNotifier> m_notifier;
    Microsoft::WRL::ComPtr<ABI::Windows::UI::Notifications::IToastNotification> m_notification;

    // Set the values of each of the text nodes
    HRESULT setTextValues();
    HRESULT startListeningEvents(DesktopNotificationsManager *desktopNotificationsManager);
    HRESULT setNodeValueString(const HSTRING &inputString, ABI::Windows::Data::Xml::Dom::IXmlNode *node);
    HRESULT addAttribute(const std::wstring &name, ABI::Windows::Data::Xml::Dom::IXmlNamedNodeMap *attributeMap);
    HRESULT addAttribute(const std::wstring &name, ABI::Windows::Data::Xml::Dom::IXmlNamedNodeMap *attributeMap,
                         const std::wstring &value);
    void printXML();
    std::wstring formatAction(
        const std::wstring &action,
        const std::vector<std::pair<std::wstring, std::wstring>> &extraData) const;
};
