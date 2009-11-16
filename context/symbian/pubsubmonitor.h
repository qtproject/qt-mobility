#ifndef PUBSUBMONITOR_H
#define PUBSUBMONITOR_H

#include <e32base.h>
#include <e32property.h>

#include "xqsettingskey.h"
#include "xqsettingsmanager.h"

class MSettingsHandlerObserver;

class CPubSubMonitor : public CActive
    { 
    public:
        CPubSubMonitor(const XQSettingsKey& aKey, XQSettingsManager::Type type, MSettingsHandlerObserver& aObserver);
        ~CPubSubMonitor();
        TInt StartMonitoring();
        
    private:
        void RunL();
        void DoCancel();
              
    private:
        RProperty iProperty;
        XQSettingsKey iKey;
        XQSettingsManager::Type m_type;
        MSettingsHandlerObserver& iObserver;
    };

#endif //PUBSUBMONITOR_H
