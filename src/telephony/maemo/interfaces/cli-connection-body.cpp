#include <QtCore/QDebug>
#include "maemo/interfaces/cli-connection.h"

namespace Tp
{
namespace Client
{

ConnectionInterface::ConnectionInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
    qDebug() << "ConnectionInterface::ConnectionInterface(...)-1";
}

ConnectionInterface::ConnectionInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
    qDebug() << "ConnectionInterface::ConnectionInterface(...)-2";
}

ConnectionInterface::ConnectionInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
    qDebug() << "ConnectionInterface::ConnectionInterface(...)-3";
}

ConnectionInterface::~ConnectionInterface()
{
    qDebug() << "ConnectionInterface::~ConnectionInterface()";
}


void ConnectionInterface::invalidate(Tp::DBusProxy *proxy, const QString &error, const QString &message)
{
    qDebug() << "ConnectionInterface::~invalidate(...)";

    disconnect(this, SIGNAL(SelfHandleChanged(uint)), NULL, NULL);
    disconnect(this, SIGNAL(NewChannel(const QDBusObjectPath&, const QString&, uint, uint, bool)), NULL, NULL);
    disconnect(this, SIGNAL(ConnectionError(const QString&, const QVariantMap&)), NULL, NULL);
    disconnect(this, SIGNAL(StatusChanged(uint, uint)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ConnectionInterfaceAliasingInterface::ConnectionInterfaceAliasingInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ConnectionInterfaceAliasingInterface::ConnectionInterfaceAliasingInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ConnectionInterfaceAliasingInterface::ConnectionInterfaceAliasingInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ConnectionInterfaceAliasingInterface::ConnectionInterfaceAliasingInterface(const Tp::Client::ConnectionInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ConnectionInterfaceAliasingInterface::ConnectionInterfaceAliasingInterface(const Tp::Client::ConnectionInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ConnectionInterfaceAliasingInterface::invalidate(Tp::DBusProxy *proxy, const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(AliasesChanged(const Tp::AliasPairList&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ConnectionInterfaceAvatarsInterface::ConnectionInterfaceAvatarsInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ConnectionInterfaceAvatarsInterface::ConnectionInterfaceAvatarsInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ConnectionInterfaceAvatarsInterface::ConnectionInterfaceAvatarsInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ConnectionInterfaceAvatarsInterface::ConnectionInterfaceAvatarsInterface(const Tp::Client::ConnectionInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ConnectionInterfaceAvatarsInterface::ConnectionInterfaceAvatarsInterface(const Tp::Client::ConnectionInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ConnectionInterfaceAvatarsInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(AvatarUpdated(uint, const QString&)), NULL, NULL);
    disconnect(this, SIGNAL(AvatarRetrieved(uint, const QString&, const QByteArray&, const QString&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ConnectionInterfaceBalanceInterface::ConnectionInterfaceBalanceInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ConnectionInterfaceBalanceInterface::ConnectionInterfaceBalanceInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ConnectionInterfaceBalanceInterface::ConnectionInterfaceBalanceInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ConnectionInterfaceBalanceInterface::ConnectionInterfaceBalanceInterface(const Tp::Client::ConnectionInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ConnectionInterfaceBalanceInterface::ConnectionInterfaceBalanceInterface(const Tp::Client::ConnectionInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ConnectionInterfaceBalanceInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(BalanceChanged(const Tp::CurrencyAmount&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ConnectionInterfaceCapabilitiesInterface::ConnectionInterfaceCapabilitiesInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ConnectionInterfaceCapabilitiesInterface::ConnectionInterfaceCapabilitiesInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ConnectionInterfaceCapabilitiesInterface::ConnectionInterfaceCapabilitiesInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ConnectionInterfaceCapabilitiesInterface::ConnectionInterfaceCapabilitiesInterface(const Tp::Client::ConnectionInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ConnectionInterfaceCapabilitiesInterface::ConnectionInterfaceCapabilitiesInterface(const Tp::Client::ConnectionInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ConnectionInterfaceCapabilitiesInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(CapabilitiesChanged(const Tp::CapabilityChangeList&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ConnectionInterfaceContactCapabilitiesInterface::ConnectionInterfaceContactCapabilitiesInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ConnectionInterfaceContactCapabilitiesInterface::ConnectionInterfaceContactCapabilitiesInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ConnectionInterfaceContactCapabilitiesInterface::ConnectionInterfaceContactCapabilitiesInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ConnectionInterfaceContactCapabilitiesInterface::ConnectionInterfaceContactCapabilitiesInterface(const Tp::Client::ConnectionInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ConnectionInterfaceContactCapabilitiesInterface::ConnectionInterfaceContactCapabilitiesInterface(const Tp::Client::ConnectionInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ConnectionInterfaceContactCapabilitiesInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(ContactCapabilitiesChanged(const Tp::ContactCapabilitiesMap&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ConnectionInterfaceContactInfoInterface::ConnectionInterfaceContactInfoInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ConnectionInterfaceContactInfoInterface::ConnectionInterfaceContactInfoInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ConnectionInterfaceContactInfoInterface::ConnectionInterfaceContactInfoInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ConnectionInterfaceContactInfoInterface::ConnectionInterfaceContactInfoInterface(const Tp::Client::ConnectionInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ConnectionInterfaceContactInfoInterface::ConnectionInterfaceContactInfoInterface(const Tp::Client::ConnectionInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ConnectionInterfaceContactInfoInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(ContactInfoChanged(uint, const Tp::ContactInfoFieldList&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ConnectionInterfaceContactsInterface::ConnectionInterfaceContactsInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ConnectionInterfaceContactsInterface::ConnectionInterfaceContactsInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ConnectionInterfaceContactsInterface::ConnectionInterfaceContactsInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ConnectionInterfaceContactsInterface::ConnectionInterfaceContactsInterface(const Tp::Client::ConnectionInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ConnectionInterfaceContactsInterface::ConnectionInterfaceContactsInterface(const Tp::Client::ConnectionInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ConnectionInterfaceContactsInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ConnectionInterfaceLocationInterface::ConnectionInterfaceLocationInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ConnectionInterfaceLocationInterface::ConnectionInterfaceLocationInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ConnectionInterfaceLocationInterface::ConnectionInterfaceLocationInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ConnectionInterfaceLocationInterface::ConnectionInterfaceLocationInterface(const Tp::Client::ConnectionInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ConnectionInterfaceLocationInterface::ConnectionInterfaceLocationInterface(const Tp::Client::ConnectionInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ConnectionInterfaceLocationInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(LocationUpdated(uint, const QVariantMap&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ConnectionInterfacePresenceInterface::ConnectionInterfacePresenceInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ConnectionInterfacePresenceInterface::ConnectionInterfacePresenceInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ConnectionInterfacePresenceInterface::ConnectionInterfacePresenceInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ConnectionInterfacePresenceInterface::ConnectionInterfacePresenceInterface(const Tp::Client::ConnectionInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ConnectionInterfacePresenceInterface::ConnectionInterfacePresenceInterface(const Tp::Client::ConnectionInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ConnectionInterfacePresenceInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(PresenceUpdate(const Tp::ContactPresences&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ConnectionInterfaceRequestsInterface::ConnectionInterfaceRequestsInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
    qDebug() << "ConnectionInterfaceRequestsInterface::ConnectionInterfaceRequestsInterface(...)-1";
}

ConnectionInterfaceRequestsInterface::ConnectionInterfaceRequestsInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
    qDebug() << "ConnectionInterfaceRequestsInterface::ConnectionInterfaceRequestsInterface(...)-2";
}

ConnectionInterfaceRequestsInterface::ConnectionInterfaceRequestsInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
    qDebug() << "ConnectionInterfaceRequestsInterface::ConnectionInterfaceRequestsInterface(...)-3";
}

ConnectionInterfaceRequestsInterface::ConnectionInterfaceRequestsInterface(const Tp::Client::ConnectionInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
    qDebug() << "ConnectionInterfaceRequestsInterface::ConnectionInterfaceRequestsInterface(...)-4";
}

ConnectionInterfaceRequestsInterface::ConnectionInterfaceRequestsInterface(const Tp::Client::ConnectionInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
    qDebug() << "ConnectionInterfaceRequestsInterface::ConnectionInterfaceRequestsInterface(...)-5";
}

ConnectionInterfaceRequestsInterface::~ConnectionInterfaceRequestsInterface()
{
    qDebug() << "ConnectionInterfaceRequestsInterface::~ConnectionInterfaceRequestsInterface(...)";
}

void ConnectionInterfaceRequestsInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(NewChannels(const Tp::ChannelDetailsList&)), NULL, NULL);
    disconnect(this, SIGNAL(ChannelClosed(const QDBusObjectPath&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}

ConnectionInterfaceSimplePresenceInterface::ConnectionInterfaceSimplePresenceInterface(const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), QDBusConnection::sessionBus(), parent)
{
}

ConnectionInterfaceSimplePresenceInterface::ConnectionInterfaceSimplePresenceInterface(const QDBusConnection& connection, const QString& busName, const QString& objectPath, QObject *parent)
    : Tp::AbstractInterface(busName, objectPath, staticInterfaceName(), connection, parent)
{
}

ConnectionInterfaceSimplePresenceInterface::ConnectionInterfaceSimplePresenceInterface(Tp::DBusProxy *proxy)
    : Tp::AbstractInterface(proxy, staticInterfaceName())
{
}

ConnectionInterfaceSimplePresenceInterface::ConnectionInterfaceSimplePresenceInterface(const Tp::Client::ConnectionInterface& mainInterface)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), mainInterface.parent())
{
}

ConnectionInterfaceSimplePresenceInterface::ConnectionInterfaceSimplePresenceInterface(const Tp::Client::ConnectionInterface& mainInterface, QObject *parent)
    : Tp::AbstractInterface(mainInterface.service(), mainInterface.path(), staticInterfaceName(), mainInterface.connection(), parent)
{
}

void ConnectionInterfaceSimplePresenceInterface::invalidate(Tp::DBusProxy *proxy,
        const QString &error, const QString &message)
{
    disconnect(this, SIGNAL(PresencesChanged(const Tp::SimpleContactPresences&)), NULL, NULL);

    Tp::AbstractInterface::invalidate(proxy, error, message);
}
}
}
