#ifndef XQSETTINGSMANAGER_H
#define XQSETTINGSMANAGER_H

// INCLUDES
#include <QObject>
#include <QVariant>
#include "xqsettingskey.h"
#include "settingsmanager_global.h"

// FORWARD DECLARATIONS
class XQSettingsManagerPrivate;

// CLASS DECLARATION
class XQSETTINGSMANAGER_EXPORT XQSettingsManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(Error)
    Q_ENUMS(Type)

public:

    enum Type
    {
        TypeVariant = 0,
        TypeInt,
        TypeDouble,
        TypeString,
        TypeByteArray
    };

    enum Error 
    {
        NoError = 0,
        OutOfMemoryError,
        NotFoundError,
        AlreadyExistsError,
        PermissionDeniedError,
        BadTypeError,
        NotSupportedError,
        UnknownError = -1
    };

    XQSettingsManager(QObject* parent = 0);
    ~XQSettingsManager();

    QVariant readItemValue(const XQSettingsKey& key, XQSettingsManager::Type type = XQSettingsManager::TypeVariant);
    bool writeItemValue(const XQSettingsKey& key, const QVariant& value);
    bool startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type = XQSettingsManager::TypeVariant);
    bool stopMonitoring(const XQSettingsKey& key);
    
    XQSettingsManager::Error error() const;

Q_SIGNALS:
    void valueChanged(const XQSettingsKey& key, const QVariant& value);
    void itemDeleted(const XQSettingsKey& key);

private:
    friend class XQSettingsManagerPrivate;
    friend class XQCentralRepositoryUtilsPrivate;
    friend class XQPublishAndSubscribeUtilsPrivate;
    XQSettingsManagerPrivate* d;
};

#endif // XQSETTINGSMANAGER_H

// End of file
