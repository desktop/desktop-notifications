#include "DesktopNotificationsManager.h"
#include "DesktopNotification.h"
#include "Utils.h"

#include <sstream>
#include <iostream>
#include <wchar.h>
#include <algorithm>
#include <assert.h>
#include <wrl\wrappers\corewrappers.h>

using namespace ABI::Windows::UI::Notifications;
using namespace Windows::Foundation;
using namespace Wrappers;

DesktopNotification::DesktopNotification(const std::wstring &id,
                                         const std::wstring &title, const std::wstring &body,
                                         const Napi::Function &callback)
    : m_ref(0),
      m_title(title),
      m_body(body),
      m_callback(Napi::ThreadSafeFunction::New(callback.Env(), callback, "Notification Callback", 0, 1)),
      m_id(id)
{
    PWSTR appID;
    HRESULT hr = GetCurrentProcessExplicitAppUserModelID(&appID);
    if (!SUCCEEDED(hr))
    {
        DN_LOG_ERROR(L"Couldn't retrieve AUMID");
        return;
    }
    else
    {
        m_appID = std::wstring(appID);
        CoTaskMemFree(appID);
    }
}

DesktopNotification::~DesktopNotification()
{
    m_callback.Release();
}

// DesktopToastActivatedEventHandler
IFACEMETHODIMP DesktopNotification::Invoke(_In_ IToastNotification * /*sender*/,
                                           _In_ IInspectable *args)
{
    IToastActivatedEventArgs *buttonReply = nullptr;
    args->QueryInterface(&buttonReply);
    if (buttonReply == nullptr)
    {
        DN_LOG_ERROR(L"args is not a IToastActivatedEventArgs");
        return S_OK;
    }

    invokeJSCallback("click");

    return S_OK;
}

// DesktopToastDismissedEventHandler
IFACEMETHODIMP DesktopNotification::Invoke(_In_ IToastNotification * /* sender */,
                                           _In_ IToastDismissedEventArgs *e)
{
    ToastDismissalReason tdr;
    HRESULT hr = e->get_Reason(&tdr);
    if (SUCCEEDED(hr))
    {
        switch (tdr)
        {
        case ToastDismissalReason_ApplicationHidden:
            invokeJSCallback("hidden");
            break;
        case ToastDismissalReason_UserCanceled:
            invokeJSCallback("dismissed");
            break;
        case ToastDismissalReason_TimedOut:
            invokeJSCallback("timedout");
            break;
        }
    }
    return S_OK;
}

// DesktopToastFailedEventHandler
IFACEMETHODIMP DesktopNotification::Invoke(_In_ IToastNotification * /* sender */,
                                           _In_ IToastFailedEventArgs * /* e */)
{
    DN_LOG_ERROR(L"The toast encountered an error.");
    invokeJSCallback("error");
    return S_OK;
}

void DesktopNotification::invokeJSCallback(std::string eventName)
{
    auto cb = [eventName](Napi::Env env, Napi::Function jsCallback)
    {
        jsCallback.Call({Napi::String::New(env, eventName)});
    };

    m_callback.BlockingCall(cb);
}

// Set the values of each of the text nodes
HRESULT DesktopNotification::setTextValues()
{
    ComPtr<IXmlNodeList> nodeList;
    DN_RETURN_ON_ERROR(
        m_toastXml->GetElementsByTagName(HStringReference(L"text").Get(), &nodeList));
    // create the title
    ComPtr<IXmlNode> textNode;
    DN_RETURN_ON_ERROR(nodeList->Item(0, &textNode));
    DN_RETURN_ON_ERROR(
        setNodeValueString(HStringReference(m_title.c_str()).Get(), textNode.Get()));
    DN_RETURN_ON_ERROR(nodeList->Item(1, &textNode));
    return setNodeValueString(HStringReference(m_body.c_str()).Get(), textNode.Get());
}

HRESULT DesktopNotification::startListeningEvents()
{
    ComPtr<IToastNotification> toast = m_notification;

    // Register the event handlers
    DN_RETURN_ON_ERROR(toast->add_Activated(this, &m_activatedToken));
    DN_RETURN_ON_ERROR(toast->add_Dismissed(this, &m_dismissedToken));
    DN_RETURN_ON_ERROR(toast->add_Failed(this, &m_failedToken));

    return S_OK;
}

void DesktopNotification::stopListeningEvents()
{
    if (m_notification)
    {
        ComPtr<IToastNotification> toast = m_notification;
        toast->remove_Activated(m_activatedToken);
        toast->remove_Dismissed(m_dismissedToken);
        toast->remove_Failed(m_failedToken);
    }
}

HRESULT DesktopNotification::setNodeValueString(const HSTRING &inputString, IXmlNode *node)
{
    ComPtr<IXmlText> inputText;
    DN_RETURN_ON_ERROR(m_toastXml->CreateTextNode(inputString, &inputText));

    ComPtr<IXmlNode> inputTextNode;
    DN_RETURN_ON_ERROR(inputText.As(&inputTextNode));

    ComPtr<IXmlNode> pAppendedChild;
    return node->AppendChild(inputTextNode.Get(), &pAppendedChild);
}

HRESULT DesktopNotification::addAttribute(const std::wstring &name, IXmlNamedNodeMap *attributeMap)
{
    ComPtr<ABI::Windows::Data::Xml::Dom::IXmlAttribute> srcAttribute;
    HRESULT hr =
        m_toastXml->CreateAttribute(HStringReference(name.c_str()).Get(), &srcAttribute);

    if (SUCCEEDED(hr))
    {
        ComPtr<IXmlNode> node;
        hr = srcAttribute.As(&node);
        if (SUCCEEDED(hr))
        {
            ComPtr<IXmlNode> pNode;
            hr = attributeMap->SetNamedItem(node.Get(), &pNode);
        }
    }
    return hr;
}

HRESULT DesktopNotification::addAttribute(const std::wstring &name, IXmlNamedNodeMap *attributeMap,
                                          const std::wstring &value)
{
    ComPtr<ABI::Windows::Data::Xml::Dom::IXmlAttribute> srcAttribute;
    DN_RETURN_ON_ERROR(
        m_toastXml->CreateAttribute(HStringReference(name.c_str()).Get(), &srcAttribute));

    ComPtr<IXmlNode> node;
    DN_RETURN_ON_ERROR(srcAttribute.As(&node));

    ComPtr<IXmlNode> pNode;
    DN_RETURN_ON_ERROR(attributeMap->SetNamedItem(node.Get(), &pNode));
    return setNodeValueString(HStringReference(value.c_str()).Get(), node.Get());
}

void DesktopNotification::printXML()
{
    ComPtr<ABI::Windows::Data::Xml::Dom::IXmlNodeSerializer> s;
    ComPtr<ABI::Windows::Data::Xml::Dom::IXmlDocument> ss(m_toastXml);
    ss.As(&s);
    HSTRING string;
    s->GetXml(&string);
    PCWSTR str = WindowsGetStringRawBuffer(string, nullptr);
    DN_LOG_DEBUG(L"------------------------\n\t\t\t" << str << L"\n\t\t" << L"------------------------");
}

std::wstring DesktopNotification::formatAction(
    const std::wstring &action,
    const std::vector<std::pair<std::wstring, std::wstring>> &extraData = {}) const
{
    std::vector<std::pair<std::wstring, std::wstring>> data = {
        {L"action", action},
        {L"notificationId", m_id}};
    data.insert(data.end(), extraData.cbegin(), extraData.cend());
    return Utils::formatData(data);
}

// Create and display the toast
HRESULT DesktopNotification::createToast(ComPtr<IToastNotificationManagerStatics> toastManager)
{
    DN_RETURN_ON_ERROR(toastManager->GetTemplateContent(
        ToastTemplateType_ToastImageAndText02, &m_toastXml));
    ComPtr<ABI::Windows::Data::Xml::Dom::IXmlNodeList> rootList;
    DN_RETURN_ON_ERROR(
        m_toastXml->GetElementsByTagName(HStringReference(L"toast").Get(), &rootList));

    ComPtr<IXmlNode> root;
    DN_RETURN_ON_ERROR(rootList->Item(0, &root));
    ComPtr<IXmlNamedNodeMap> rootAttributes;
    DN_RETURN_ON_ERROR(root->get_Attributes(&rootAttributes));

    const auto data = formatAction(L"clicked");
    DN_RETURN_ON_ERROR(addAttribute(L"launch", rootAttributes.Get(), data));
    DN_RETURN_ON_ERROR(setTextValues());

    // printXML();

    DN_RETURN_ON_ERROR(toastManager->CreateToastNotifierWithId(
        HStringReference(m_appID.c_str()).Get(), &m_notifier));

    ComPtr<IToastNotificationFactory> factory;
    DN_RETURN_ON_ERROR(GetActivationFactory(
        HStringReference(RuntimeClass_Windows_UI_Notifications_ToastNotification).Get(),
        &factory));
    DN_RETURN_ON_ERROR(factory->CreateToastNotification(m_toastXml.Get(), &m_notification));

    ComPtr<IToastNotification2> toastV2;
    if (SUCCEEDED(m_notification.As(&toastV2)))
    {
        DN_RETURN_ON_ERROR(toastV2->put_Tag(HStringReference(m_id.c_str()).Get()));
        DN_RETURN_ON_ERROR(toastV2->put_Group(HStringReference(DN_GROUP_NAME).Get()));
    }

    std::wstring error;
    NotificationSetting setting = NotificationSetting_Enabled;
    if (!DN_CHECK_RESULT(m_notifier->get_Setting(&setting)))
    {
        DN_LOG_ERROR("Failed to retreive NotificationSettings ensure your appId is registered");
    }
    switch (setting)
    {
    case NotificationSetting_Enabled:
        DN_RETURN_ON_ERROR(startListeningEvents());
        break;
    case NotificationSetting_DisabledForApplication:
        error = L"DisabledForApplication";
        break;
    case NotificationSetting_DisabledForUser:
        error = L"DisabledForUser";
        break;
    case NotificationSetting_DisabledByGroupPolicy:
        error = L"DisabledByGroupPolicy";
        break;
    case NotificationSetting_DisabledByManifest:
        error = L"DisabledByManifest";
        break;
    }
    if (!error.empty())
    {
        std::wstringstream err;
        err << L"Notifications are disabled\n"
            << L"Reason: " << error << L" Please make sure that the app id is set correctly.\n"
            << L"Command Line: " << GetCommandLineW();
        DN_LOG_ERROR(err.str());
    }
    return m_notifier->Show(m_notification.Get());
}
