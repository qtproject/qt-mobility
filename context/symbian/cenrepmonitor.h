#ifndef CENREPMONITOR_H
#define CENREPMONITOR_H

#include <e32base.h>

#include "xqsettingskey.h"
#include "xqsettingsmanager.h"

class MSettingsHandlerObserver;

class CRepository;

class CCenRepMonitor : public CActive
    { 
    public:
        CCenRepMonitor(CRepository& aRepository, const XQSettingsKey& aKey, XQSettingsManager::Type type, MSettingsHandlerObserver& aObserver);
        ~CCenRepMonitor();
        TInt StartMonitoring();
        
    private:
        void RunL();
        void DoCancel();
              
    private:
        CRepository& iRepository;
        XQSettingsKey iKey;
        XQSettingsManager::Type m_type;
        MSettingsHandlerObserver& iObserver;
    };

#endif //CENREPMONITOR_H
