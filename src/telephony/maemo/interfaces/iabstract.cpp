#include "maemo/interfaces/iabstract.h"
#include "maemo/dbusproxy.h"

namespace DBus
{
    IAbstract::IAbstract(const QString &busName,
            const QString &path, const char *interface,
            const QDBusConnection &dbusConnection, QObject *parent)
        : QDBusAbstractInterface(busName, path, interface, dbusConnection, parent)
    {
    }

    IAbstract::IAbstract(DBusProxy *parent, const char *interface)
        : QDBusAbstractInterface(parent->busName(), parent->objectPath(),
                interface, parent->dbusConnection(), parent)
    {
        connect(parent, SIGNAL(invalidated(DBus::DBusProxy *, const QString &, const QString &)),
                this, SLOT(invalidate(DBus::DBusProxy *, const QString &, const QString &)));
    }

    IAbstract::~IAbstract()
    {
    }

    bool IAbstract::isValid() const
    {
        return QDBusAbstractInterface::isValid() && mError.isEmpty();
    }

    QString IAbstract::invalidationReason() const
    {
        return mError;
    }

    QString IAbstract::invalidationMessage() const
    {
        return mMessage;
    }

    void IAbstract::invalidate(DBusProxy *proxy,
            const QString &error, const QString &message)
    {
        Q_ASSERT(!error.isEmpty());

        if (mError.isEmpty()) {
            mError = error;
            mMessage = message;
        }
    }
} // DBus
