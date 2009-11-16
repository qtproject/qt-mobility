#ifndef XQCENTRALREPOSITORYUTILS_H
#define XQCENTRALREPOSITORYUTILS_H

#include <QObject>

#include "xqsettingsmanager.h"
#include "settingsmanager_global.h"

class XQPublishAndSubscribeUtilsPrivate;
class XQPublishAndSubscribeSettingsKey;
class XQPublishAndSubscribeSecurityPolicy;

class XQSETTINGSMANAGER_EXPORT XQPublishAndSubscribeUtils : public QObject
{
public:
    XQPublishAndSubscribeUtils(XQSettingsManager& settingsManager, QObject* parent = 0);
    ~XQPublishAndSubscribeUtils();
    bool defineProperty(const XQPublishAndSubscribeSettingsKey& key, XQSettingsManager::Type type);
    bool defineProperty(const XQPublishAndSubscribeSettingsKey& key, XQSettingsManager::Type type,
        const XQPublishAndSubscribeSecurityPolicy& readPolicy, const XQPublishAndSubscribeSecurityPolicy& writePolicy);
    bool deleteProperty(const XQPublishAndSubscribeSettingsKey& key);

private:
    XQPublishAndSubscribeUtilsPrivate* d;
};

#endif //XQCENTRALREPOSITORYUTILS_H
