#include "xqpublishandsubscribeutils_p.h"
#include "xqsettingsmanager.h"
#include "xqsettingsmanager_p.h"
#include "cpublishandsubscribehandler.h"
#include "xqpublishandsubscribesecuritypolicy.h"

XQPublishAndSubscribeUtilsPrivate::XQPublishAndSubscribeUtilsPrivate(XQSettingsManager& settingsManager)
    : m_settingsManagerPrivate(*settingsManager.d)
{
}

XQPublishAndSubscribeUtilsPrivate::~XQPublishAndSubscribeUtilsPrivate()
{
}

bool XQPublishAndSubscribeUtilsPrivate::defineProperty(const XQPublishAndSubscribeSettingsKey& key, XQSettingsManager::Type type)
{
    CPublishAndSubscribeHandler* handler = m_settingsManagerPrivate.pubSubHandlerInstance(key.uid());
    
    if (handler)
    {
        m_settingsManagerPrivate.iError = handler->defineProperty(key.key(), type);
    }
    else
    {
        m_settingsManagerPrivate.iError = XQSettingsManager::UnknownError;
    }
    return m_settingsManagerPrivate.iError == KErrNone;
}


bool XQPublishAndSubscribeUtilsPrivate::defineProperty(const XQPublishAndSubscribeSettingsKey& key, XQSettingsManager::Type type,
    const XQPublishAndSubscribeSecurityPolicy& readPolicy, const XQPublishAndSubscribeSecurityPolicy& writePolicy)
{
    CPublishAndSubscribeHandler* handler = m_settingsManagerPrivate.pubSubHandlerInstance(key.uid());
    
    if (handler)
    {
        m_settingsManagerPrivate.iError = handler->defineProperty(key.key(), type, readPolicy, writePolicy);
    }
    else
    {
        m_settingsManagerPrivate.iError = XQSettingsManager::UnknownError;
    }
    return m_settingsManagerPrivate.iError == KErrNone;    
}

bool XQPublishAndSubscribeUtilsPrivate::deleteProperty(const XQPublishAndSubscribeSettingsKey& key)
{
    CPublishAndSubscribeHandler* handler = m_settingsManagerPrivate.pubSubHandlerInstance(key.uid());
    
    if (handler)
    {
        m_settingsManagerPrivate.iError = handler->deleteProperty(key.key());
    }
    else
    {
        m_settingsManagerPrivate.iError = XQSettingsManager::UnknownError;
    }
    return m_settingsManagerPrivate.iError == KErrNone;
}
