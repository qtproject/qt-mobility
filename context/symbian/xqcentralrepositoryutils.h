#ifndef XQCENTRALREPOSITORYUTILS_H
#define XQCENTRALREPOSITORYUTILS_H

#include <QObject>
#include "xqsettingskey.h"
#include "settingsmanager_global.h"

class XQSettingsManager;
class XQCentralRepositoryUtilsPrivate;
class XQCentralRepositorySearchCriteria;

class XQSETTINGSMANAGER_EXPORT XQCentralRepositoryUtils : public QObject
{
public:

    enum TransactionMode
    {
        ConcurrentReadWriteTransaction,
        ReadTransaction,
        ReadWriteTransaction
    };

    enum TransactionState
    {
        NotInTransaction,
        ActiveTransaction,
        FailedTransaction,
        UnknownTransaction
    };
    
    XQCentralRepositoryUtils(XQSettingsManager& settingsManager, QObject* parent = 0);
    ~XQCentralRepositoryUtils();
    bool createKey(const XQCentralRepositorySettingsKey& key, const QVariant& value);
    bool deleteKey(const XQCentralRepositorySettingsKey& key);
    bool resetKey(const XQCentralRepositorySettingsKey& key);
    bool resetRepository(long int repositoryUid);
    QList<XQCentralRepositorySettingsKey> findKeys(const XQCentralRepositorySearchCriteria& criteria);

    bool startTransaction(long int repositoryUid, TransactionMode transactionMode);
    bool commitTransaction(long int repositoryUid);
    void cancelTransaction(long int repositoryUid);
    void failTransaction(long int repositoryUid);
    TransactionState transactionState(long int repositoryUid) const;

private:
    XQCentralRepositoryUtilsPrivate* d;
};

#endif //XQCENTRALREPOSITORYUTILS_H
