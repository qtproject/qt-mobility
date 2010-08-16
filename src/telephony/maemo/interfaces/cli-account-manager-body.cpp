#define IN_TELEPATHY_QT4_HEADER
#include "maemo/interfaces/cli-account-manager.h"

namespace Tp
{
namespace Client
{

AccountManagerInterface::AccountManagerInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

AccountManagerInterface::AccountManagerInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

AccountManagerInterface::AccountManagerInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

void AccountManagerInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(AccountRemoved(const QDBusObjectPath&)), NULL, NULL);
    disconnect(this, SIGNAL(AccountValidityChanged(const QDBusObjectPath&, bool)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}
}
}
