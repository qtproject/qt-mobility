#ifndef CENTRALREPOSITORYHANDLER_H
#define CENTRALREPOSITORYHANDLER_H

#include <e32base.h>
#include <centralrepository.h>

#include "settingshandlerbase.h"
#include "cenrepmonitor.h"

class XQSettingsKey;
class CRepository;

class CCentralRepositoryHandler : public CBase, public SettingsHandlerBase
{
public:
    static CCentralRepositoryHandler* NewL(TUid aUid);
    ~CCentralRepositoryHandler();

    TInt createKey(unsigned long key, const TInt& value);
    TInt createKey(unsigned long key, const TReal& value);
    TInt createKey(unsigned long key, const TDesC8& value);
    TInt createKey(unsigned long key, const TDesC16& value);

    TInt deleteKey(unsigned long key);

    TInt resetKey(unsigned long key);
    TInt resetRepository();

    TInt findKeyL(unsigned long partialKey, TUint32 mask, RArray<TUint32>& aFoundKeys);
    TInt findKeyL(unsigned long partialKey, TUint32 mask, const TInt& value, bool negative, RArray<TUint32>& aFoundKeys);
    TInt findKeyL(unsigned long partialKey, TUint32 mask, const TReal& value, bool negative, RArray<TUint32>& aFoundKeys);
    TInt findKeyL(unsigned long partialKey, TUint32 mask, const TDesC8& value, bool negative, RArray<TUint32>& aFoundKeys);
    TInt findKeyL(unsigned long partialKey, TUint32 mask, const TDesC16& value, bool negative, RArray<TUint32>& aFoundKeys);

    TInt startTransaction(CRepository::TTransactionMode transactionMode);
    TInt commitTransaction();
    void cancelTransaction();
    void failTransaction();
    TInt transactionState() const;

private:
    CCentralRepositoryHandler(TUid aUid);
    void ConstructL();

protected:  //From SettingsHandlerBase
    void setObserver(MSettingsHandlerObserver* observer);
    bool handleStartMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type, MSettingsHandlerObserver& observer, TInt& error);
    bool handleStopMonitoring(const XQSettingsKey& key, TInt& error);

    TInt getValue(unsigned long key, TInt& value);
    TInt getValue(unsigned long key, TReal& value);
    void getValueL(unsigned long key, RBuf8& value);
    void getValueL(unsigned long key, RBuf16& value);

    TInt setValue(unsigned long key, const TInt& value);
    TInt setValue(unsigned long key, const TReal& value);
    TInt setValue(unsigned long key, const TDesC8& value);
    TInt setValue(unsigned long key, const TDesC16& value);

private:
    TUid m_uid;
    CRepository* iRepository;
    MSettingsHandlerObserver* m_observer;
    QHash<unsigned long int, CCenRepMonitor*> m_monitors;
};

#endif //CENTRALREPOSITORYHANDLER_H
