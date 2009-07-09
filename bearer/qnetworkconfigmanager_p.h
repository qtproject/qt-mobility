/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QNETWORKCONFIGURATIONMANAGERPRIVATE_H
#define QNETWORKCONFIGURATIONMANAGERPRIVATE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qnetworkconfigmanager.h>
#include <qnetworkconfiguration_p.h>

#include <QHash>

QT_BEGIN_NAMESPACE

#ifdef BEARER_ENGINE
class QNetworkSessionEngine;
class QGenericEngine;
class QNlaEngine;
class QNativeWifiEngine;
class QIoctlWifiEngine;
#endif

#include <QStringList>

#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC) && !defined(BEARER_ENGINE)
#include <qnetworkmanagerservice_p.h>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QObject>
#include <QDBusObjectPath>
#endif

class QNetworkConfigurationManagerPrivate : public QObject
{
    Q_OBJECT
public:
    QNetworkConfigurationManagerPrivate()
    :   QObject(0), capFlags(0), firstUpdate(true)
    {
        registerPlatformCapabilities();
        updateConfigurations();
    }

    virtual ~QNetworkConfigurationManagerPrivate() 
    {
        QList<QString> configIdents = snapConfigurations.keys();
        foreach(const QString oldIface, configIdents) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = snapConfigurations.take(oldIface);
            priv->isValid = false;
            priv->id.clear();
        }

        configIdents = accessPointConfigurations.keys();
        foreach(const QString oldIface, configIdents) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(oldIface);
            priv->isValid = false;
            priv->id.clear();
        }
    }

    QNetworkConfiguration defaultConfiguration();

    QNetworkConfigurationManager::CapabilityFlags capFlags;
    void registerPlatformCapabilities();

    void performAsyncConfigurationUpdate();

    //this table contains an up to date list of all configs at any time.
    //it must be updated if configurations change, are added/removed or
    //the members of ServiceNetworks change
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > accessPointConfigurations;
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > snapConfigurations;
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > userChoiceConfigurations;
    QHash<QString, QNetworkSessionEngine *> configurationEngine;
    bool firstUpdate;

public slots:
    void updateConfigurations();

Q_SIGNALS:
    void configurationAdded(const QNetworkConfiguration& config);
    void configurationRemoved(const QNetworkConfiguration& config);
    void configurationUpdateComplete();
    void configurationChanged(const QNetworkConfiguration& config);
    void onlineStateChanged(bool isOnline);

private:
#ifdef BEARER_ENGINE
    void updateInternetServiceConfiguration();

    void abort();
#endif

#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC) && !defined(BEARER_ENGINE)
//    QNetworkManagerInterface *iface;

    QStringList knownSsids;
    bool updating;
    QString currentActiveAP;
    QString defaultConnectionPath;
    QStringList getKnownSsids();

    void updateEthConfigurations(QNetworkManagerInterfaceDevice *devIface);
    void updateWifiConfigurations(QNetworkManagerInterfaceDevice *devIface);
    void updateServiceNetworks(QNetworkManagerInterfaceDevice *devIface);

    void updateServiceNetworkState(bool isWifi);
    void updateState(const QString &ident, quint32 state);

    QString getNameForConfiguration(QNetworkManagerInterfaceDevice *devIface);

    QStringList getActiveConnectionsPaths(QDBusInterface &iface);
    QNetworkConfiguration::StateFlags getAPState(qint32 vState, bool isKnown);

     QNetworkManagerInterfaceDeviceWireless *devWirelessIface;
     QNetworkManagerInterfaceDevice *devIface;
     QNetworkManagerInterface *iface;

    //    QStringList getActiveDevicesPaths(QDBusInterface &iface);
#endif

#ifdef BEARER_ENGINE
    QGenericEngine *generic;
#ifdef Q_OS_WIN
    QNlaEngine *nla;
#ifndef Q_OS_WINCE
    QNativeWifiEngine *nativeWifi;
    QIoctlWifiEngine *ioctlWifi;
#endif
#endif

    uint onlineConfigurations;

    enum EngineUpdate {
        NotUpdating = 0x00,
        Updating = 0x01,
        GenericUpdating = 0x02,
        NlaUpdating = 0x04,
        NativeWifiUpdating = 0x08,
        IoctlWifiUpdating = 0x10,
    };
    Q_DECLARE_FLAGS(EngineUpdateState, EngineUpdate)

    EngineUpdateState updateState;
#endif

private Q_SLOTS:
#ifdef BEARER_ENGINE
    void configurationAdded(QNetworkConfigurationPrivate *cpPriv, QNetworkSessionEngine *engine);
    void configurationRemoved(const QString &id);
    void configurationChanged(QNetworkConfigurationPrivate *cpPriv);
#endif
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC) && !defined(BEARER_ENGINE)
    void cmpPropertiesChanged(const QString &, QMap<QString,QVariant> map);
    void accessPointAdded(const QString &, QDBusObjectPath );
    void accessPointRemoved( const QString &, QDBusObjectPath );
    void updateDeviceInterfaceState(const QString &, quint32);
    void updateAccessPointState(const QString &, quint32);
#endif
};

QT_END_NAMESPACE

#endif //QNETWORKCONFIGURATIONMANAGERPRIVATE_H

