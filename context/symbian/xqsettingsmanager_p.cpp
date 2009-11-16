#include "xqsettingsmanager_p.h"
#include <QVariant>
#include "settingshandlerbase.h"
#include "ccentralrepositoryhandler.h"
#include "cpublishandsubscribehandler.h"

XQSettingsManagerPrivate::XQSettingsManagerPrivate(XQSettingsManager* settingsManager) 
    : q(settingsManager)
{   
}

XQSettingsManagerPrivate::~XQSettingsManagerPrivate()
{
    foreach (SettingsHandlerBase* handler, m_centralRepositoryHandlers)
        delete handler;
    foreach (SettingsHandlerBase* handler, m_publishAndSubscribeHandlers)
        delete handler;
}

QVariant XQSettingsManagerPrivate::readItemValue(const XQSettingsKey& key, XQSettingsManager::Type type)
{
    SettingsHandlerBase* handler = handlerInstance(key);
    if (handler)
    {
        return handler->handleReadItemValue(key, type, iError);
    }
    return QVariant();
}

bool XQSettingsManagerPrivate::writeItemValue(const XQSettingsKey& key, const QVariant& value)
{
    SettingsHandlerBase* handler = handlerInstance(key);
    if (handler)
    {
        return handler->handleWriteItemValue(key, value, iError);
    }
    return false;
}

bool XQSettingsManagerPrivate::startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type)
{
    SettingsHandlerBase* handler = handlerInstance(key);
    if (handler)
    {
        return handler->handleStartMonitoring(key, type, *this, iError);
    }
    return false;
}

bool XQSettingsManagerPrivate::stopMonitoring(const XQSettingsKey& key)
{
    SettingsHandlerBase* handler = NULL;
    const long int uid = key.uid();
    switch (key.target())
    {
        case XQSettingsKey::TargetCentralRepository:
            if (m_centralRepositoryHandlers.contains(uid))
            {
                handler = m_centralRepositoryHandlers.value(uid);
            }
            break;
        case XQSettingsKey::TargetPublishAndSubscribe:
            if (m_publishAndSubscribeHandlers.contains(uid))
            {
                handler = m_publishAndSubscribeHandlers.value(uid);
            }
            break;
        default:
            break;
    }
    if (handler)
    {
        return handler->handleStopMonitoring(key, iError);
    }
    iError = KErrNotFound;
    return false;
}

XQSettingsManager::Error XQSettingsManagerPrivate::error() const
{
    switch (iError) 
    {
        case KErrNone:
            return XQSettingsManager::NoError;
        case KErrNoMemory:
            return XQSettingsManager::OutOfMemoryError;
        case KErrNotFound:
            return XQSettingsManager::NotFoundError;
        case KErrAlreadyExists:
            return XQSettingsManager::AlreadyExistsError;
        case KErrPermissionDenied:
            return XQSettingsManager::PermissionDeniedError;
        case KErrArgument:
            return XQSettingsManager::BadTypeError;
        case KErrNotSupported:
            return XQSettingsManager::NotSupportedError;
        default:
            qDebug("XQSettingsManagerPrivate::error() iError = %d", iError);
            return XQSettingsManager::UnknownError;
    }    
}

CCentralRepositoryHandler* XQSettingsManagerPrivate::cenRepHandlerInstance(long int repositoryUid)
{
    CCentralRepositoryHandler* handler = NULL;
    if (m_centralRepositoryHandlers.contains(repositoryUid))
    {
        handler = m_centralRepositoryHandlers.value(repositoryUid);
    }
    else
    {
        TUid uid;
        uid.iUid = repositoryUid;
        TRAP(iError, handler = CCentralRepositoryHandler::NewL(uid);)
        if (handler)
        {
            static_cast<SettingsHandlerBase*>(handler)->setObserver(this);
            m_centralRepositoryHandlers[repositoryUid] = handler;
        }
    }
    return handler;
}

CPublishAndSubscribeHandler* XQSettingsManagerPrivate::pubSubHandlerInstance(long int categoryUid)
{
    CPublishAndSubscribeHandler* handler = NULL;
    if (m_publishAndSubscribeHandlers.contains(categoryUid))
    {
        handler = m_publishAndSubscribeHandlers.value(categoryUid);
    }
    else
    {
        TUid uid;
        uid.iUid = categoryUid;
        TRAP(iError, handler = CPublishAndSubscribeHandler::NewL(uid);)
        if (handler)
        {
            static_cast<SettingsHandlerBase*>(handler)->setObserver(this);
            m_publishAndSubscribeHandlers[categoryUid] = handler;
        }
    }
    return handler;
}

SettingsHandlerBase* XQSettingsManagerPrivate::handlerInstance(const XQSettingsKey& key)
{
    SettingsHandlerBase* handler = NULL;
    const XQSettingsKey::Target target = key.target();
    switch (target)
    {
        case XQSettingsKey::TargetCentralRepository:
            handler = cenRepHandlerInstance(key.uid());
            break;
        case XQSettingsKey::TargetPublishAndSubscribe:
            handler = pubSubHandlerInstance(key.uid());
            break;
        default:
            break;
    }
    return handler;
}

void XQSettingsManagerPrivate::valueChanged(const XQSettingsKey& key, XQSettingsManager::Type type)
{
    emit q->valueChanged(key, readItemValue(key, type));
}

void XQSettingsManagerPrivate::itemDeleted(const XQSettingsKey& key)
{
    XQSettingsKey deletedKey(key);
    stopMonitoring(deletedKey);
    emit q->itemDeleted(deletedKey);
}
// End of file
