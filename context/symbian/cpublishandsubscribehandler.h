#ifndef PUBLISHANDSUBSCRIBEHANDLER_H
#define PUBLISHANDSUBSCRIBEHANDLER_H

#include <e32base.h>

#include "settingshandlerbase.h"
#include "pubsubmonitor.h"
#include "xqpublishandsubscribesecuritypolicy.h"

class XQSettingsKey;

class CPublishAndSubscribeHandler : public CBase, public SettingsHandlerBase
{
public:
    static CPublishAndSubscribeHandler* NewL(TUid aUid);
    ~CPublishAndSubscribeHandler();

    TInt defineProperty(unsigned long key, XQSettingsManager::Type type);
    TInt defineProperty(unsigned long key, XQSettingsManager::Type type,
        const XQPublishAndSubscribeSecurityPolicy& readPolicy, const XQPublishAndSubscribeSecurityPolicy& writePolicy);
    TInt deleteProperty(unsigned long key);

private:
    CPublishAndSubscribeHandler(TUid aUid);
    void ConstructL();

protected:
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
    TSecurityPolicy symbianPolicy(const XQPublishAndSubscribeSecurityPolicy& policy);
    TCapability symbianCapability(const XQPublishAndSubscribeSecurityPolicy::Capability& capability);
private:
    MSettingsHandlerObserver* m_observer;
    TUid m_uid;
    QHash<unsigned long int, CPubSubMonitor*> m_monitors;
};

#endif //PUBLISHANDSUBSCRIBEHANDLER_H
