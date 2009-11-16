#ifndef XQCENTRALREPOSITORYUTILS_P_H
#define XQCENTRALREPOSITORYUTILS_P_H

#include "xqsettingsmanager.h"
#include "xqsettingskey.h"
#include "xqcentralrepositoryutils.h"

class XQSettingsManagerPrivate;
class XQCentralRepositorySearchCriteria;

class XQCentralRepositoryUtilsPrivate
{
public:
    XQCentralRepositoryUtilsPrivate(XQSettingsManager& settingsManager);
    ~XQCentralRepositoryUtilsPrivate();
    bool createKey(const XQCentralRepositorySettingsKey& key, const QVariant& value);
    bool deleteKey(const XQCentralRepositorySettingsKey& key);
    bool resetKey(const XQCentralRepositorySettingsKey& key);
    bool resetRepository(long int repositoryUid);
    QList<XQCentralRepositorySettingsKey> findKeys(const XQCentralRepositorySearchCriteria& criteria);

    bool startTransaction(long int repositoryUid, XQCentralRepositoryUtils::TransactionMode transactionMode);
    bool commitTransaction(long int repositoryUid);
    void cancelTransaction(long int repositoryUid);
    void failTransaction(long int repositoryUid);
    XQCentralRepositoryUtils::TransactionState transactionState(long int repositoryUid) const;

private:
    XQSettingsManagerPrivate& m_settingsManagerPrivate;
};

#endif //XQCENTRALREPOSITORYUTILS_P_H
