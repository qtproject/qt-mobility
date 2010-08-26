#define IN_TELEPATHY_QT4_HEADER
#include "maemo/interfaces/cli-account.h"

namespace Tp
{
namespace Client
{

AccountInterface::AccountInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

AccountInterface::AccountInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

AccountInterface::AccountInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

void AccountInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(Removed()), NULL, NULL);
    disconnect(this, SIGNAL(AccountPropertyChanged(const QVariantMap&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

AccountInterfaceAvatarInterface::AccountInterfaceAvatarInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

AccountInterfaceAvatarInterface::AccountInterfaceAvatarInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

AccountInterfaceAvatarInterface::AccountInterfaceAvatarInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

AccountInterfaceAvatarInterface::AccountInterfaceAvatarInterface(const Tp::Client::AccountInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

AccountInterfaceAvatarInterface::AccountInterfaceAvatarInterface(const Tp::Client::AccountInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void AccountInterfaceAvatarInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(AvatarChanged()), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}
}
}
