#ifndef SETTINGSHANDLERBASE_H
#define SETTINGSHANDLERBASE_H

// INCLUDES
#include <QVariant>
#include "xqsettingsmanager.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class XQSettingsKey;

class MSettingsHandlerObserver
{
public:
    virtual void valueChanged(const XQSettingsKey& key, XQSettingsManager::Type type) = 0;
    virtual void itemDeleted(const XQSettingsKey& key) = 0;
};

class SettingsHandlerBase
{
public:
    virtual ~SettingsHandlerBase() {};
    virtual void setObserver(MSettingsHandlerObserver* observer) = 0;
    QVariant handleReadItemValue(const XQSettingsKey& key, XQSettingsManager::Type type, TInt& error);
    bool handleWriteItemValue(const XQSettingsKey& key, const QVariant& value, TInt& error);
    virtual bool handleStartMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type, MSettingsHandlerObserver& observer, TInt& error) = 0;
    virtual bool handleStopMonitoring(const XQSettingsKey& key, TInt& error) = 0;

protected:
    virtual TInt getValue(unsigned long key, TInt& value) = 0;
    virtual TInt getValue(unsigned long key, TReal& value) = 0;
    virtual void getValueL(unsigned long key, RBuf8& value) = 0;
    virtual void getValueL(unsigned long key, RBuf16& value) = 0;

    virtual TInt setValue(unsigned long key, const TInt& value) = 0;
    virtual TInt setValue(unsigned long key, const TReal& value) = 0;
    virtual TInt setValue(unsigned long key, const TDesC8& value) = 0;
    virtual TInt setValue(unsigned long key, const TDesC16& value) = 0;
};

#endif //SETTINGSHANDLERBASE_H
