#ifndef XQSETTINGSITEM_H
#define XQSETTINGSITEM_H

// INCLUDES
#include "settingsmanager_global.h"

// CLASS DECLARATION
class XQSETTINGSMANAGER_EXPORT XQSettingsKey
{
public:
    enum Target
    {
        TargetCentralRepository = 0,
        TargetPublishAndSubscribe
    };

    XQSettingsKey(XQSettingsKey::Target target, long int uid, unsigned long int key);
    ~XQSettingsKey();

    XQSettingsKey::Target target() const;
    long int uid() const;
    unsigned long int key() const;

protected:
    XQSettingsKey::Target m_target;
    long int m_uid;
    unsigned long int m_key;
};

class XQSETTINGSMANAGER_EXPORT XQCentralRepositorySettingsKey : public XQSettingsKey
{
public:
    XQCentralRepositorySettingsKey(long int repositoryUid, unsigned long int key);
    ~XQCentralRepositorySettingsKey();
};

class XQSETTINGSMANAGER_EXPORT XQPublishAndSubscribeSettingsKey : public XQSettingsKey
{
public:
    XQPublishAndSubscribeSettingsKey(long int categoryUid, unsigned long int key);
    ~XQPublishAndSubscribeSettingsKey();
};

#endif //XQSETTINGSITEM_H
