#include "cenrepmonitor.h"
#include <centralrepository.h>

#include "xqsettingskey.h"
#include "settingshandlerbase.h"

CCenRepMonitor::CCenRepMonitor(CRepository& aRepository, const XQSettingsKey& aKey, XQSettingsManager::Type type, MSettingsHandlerObserver& aObserver)
    : CActive(EPriorityStandard), iRepository(aRepository), iKey(aKey), m_type(type), iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

CCenRepMonitor::~CCenRepMonitor()
    {
    Cancel();
    }

TInt CCenRepMonitor::StartMonitoring()
    {
    TInt ret = iRepository.NotifyRequest(iKey.key(), iStatus);
    SetActive();
    return ret;
    }

void CCenRepMonitor::DoCancel()
    {
    iRepository.NotifyCancel(iKey.key());
    }

void CCenRepMonitor::RunL()
    {
    TUint32 meta;
    //This trick is used to check whether the key no longer exists.
    TInt ret = iRepository.GetMeta(iKey.key(), meta);
    if (ret == KErrNone)
        {
        iRepository.NotifyRequest(iKey.key(), iStatus);
        SetActive();
        iObserver.valueChanged(iKey, m_type);
        }
    else if (ret == KErrNotFound)
        {
        iObserver.itemDeleted(iKey);
        }
    }
