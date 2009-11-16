#ifndef XQSETTINGSMANAGER_P_H
#define XQSETTINGSMANAGER_P_H

// INCLUDES
#include "xqsettingsmanager.h"
#include <QHash>

#include "xqsettingskey.h"
#include "settingshandlerbase.h"

// FORWARD DECLARATIONS
class QVariant;
class CCentralRepositoryHandler;
class CPublishAndSubscribeHandler;

// CLASS DECLARATION
class XQSettingsManagerPrivate
    : public CBase, public MSettingsHandlerObserver
{
public:
    XQSettingsManagerPrivate(XQSettingsManager* settingsmanager);
    ~XQSettingsManagerPrivate();

    QVariant readItemValue(const XQSettingsKey& key, XQSettingsManager::Type type);
    bool writeItemValue(const XQSettingsKey& key, const QVariant& value);
    bool startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type);
    bool stopMonitoring(const XQSettingsKey& key);

    XQSettingsManager::Error error() const;

    CCentralRepositoryHandler* cenRepHandlerInstance(long int repositoryUid);
    CPublishAndSubscribeHandler* pubSubHandlerInstance(long int categoryUid);

private:
    SettingsHandlerBase* handlerInstance(const XQSettingsKey& key);

protected:  //MSettingsHandlerObserver
    void valueChanged(const XQSettingsKey& key, XQSettingsManager::Type type);
    void itemDeleted(const XQSettingsKey& key);

private:
    friend class XQCentralRepositoryUtilsPrivate;
    friend class XQPublishAndSubscribeUtilsPrivate;
    XQSettingsManager* q;
    mutable int iError;
    QHash<long int, CCentralRepositoryHandler*> m_centralRepositoryHandlers;
    QHash<long int, CPublishAndSubscribeHandler*> m_publishAndSubscribeHandlers;
};

#endif /*XQSETTINGSMANAGER_P_H*/

// End of file
