#ifndef XQCENTRALREPOSITORYUTILS_P_H
#define XQCENTRALREPOSITORYUTILS_P_H

#include "xqsettingsmanager.h"

class XQSettingsManagerPrivate;
class XQPublishAndSubscribeSecurityPolicy;

class XQPublishAndSubscribeUtilsPrivate
{
public:
    XQPublishAndSubscribeUtilsPrivate(XQSettingsManager& settingsManager);
    ~XQPublishAndSubscribeUtilsPrivate();
    bool defineProperty(const XQPublishAndSubscribeSettingsKey& key, XQSettingsManager::Type type);
    bool defineProperty(const XQPublishAndSubscribeSettingsKey& key, XQSettingsManager::Type type,
        const XQPublishAndSubscribeSecurityPolicy& readPolicy, const XQPublishAndSubscribeSecurityPolicy& writePolicy);
    bool deleteProperty(const XQPublishAndSubscribeSettingsKey& key);

private:
    XQSettingsManagerPrivate& m_settingsManagerPrivate;
};

#endif //XQCENTRALREPOSITORYUTILS_P_H
