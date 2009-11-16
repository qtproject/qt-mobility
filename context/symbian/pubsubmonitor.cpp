#include "pubsubmonitor.h"
#include "settingshandlerbase.h"

CPubSubMonitor::CPubSubMonitor(const XQSettingsKey& aKey, XQSettingsManager::Type type, MSettingsHandlerObserver& aObserver) 
    : CActive(EPriorityStandard), iKey(aKey), m_type(type), iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

CPubSubMonitor::~CPubSubMonitor()
    {
    Cancel();
    }

TInt CPubSubMonitor::StartMonitoring()
    {
    TUid uid;
    uid.iUid = iKey.uid();
    TInt err = iProperty.Attach(uid, iKey.key());
    if (err == KErrNone)
        {
        iProperty.Subscribe(iStatus);
        SetActive();
        }
    return err;
    }

void CPubSubMonitor::DoCancel()
    {
    iProperty.Cancel();
    }

void CPubSubMonitor::RunL()
    {
    if (iStatus == KErrNone)
        {
        iProperty.Subscribe(iStatus);
        SetActive();
        iObserver.valueChanged(iKey, m_type);
        }
    else if (iStatus == KErrNotFound)
        {
        iObserver.itemDeleted(iKey);
        }
    }
