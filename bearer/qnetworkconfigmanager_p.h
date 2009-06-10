/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QNETWORKCONFIGURATIONMANAGERPRIVATE_H
#define QNETWORKCONFIGURATIONMANAGERPRIVATE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <qnetworkconfigmanager.h>
#include <qnetworkconfiguration_p.h>

#include <QHash>

QT_BEGIN_NAMESPACE

#ifdef Q_OS_WIN
class QNlaEngine;
class QNativeWifiEngine;
class QIoctlWifiEngine;
#endif
#include <QStringList>
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
#include <QDBusConnection>
#include <QDBusMessage>
#include <QObject>
#include <QDBusObjectPath>
class QNmDBusHelper;
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
    bool firstUpdate;

#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    QNmDBusHelper *testobj;
#endif

public slots:
    void updateConfigurations();
Q_SIGNALS:
    void configurationAdded(const QNetworkConfiguration& config);
    void configurationRemoved(const QNetworkConfiguration& config);
    void configurationUpdateComplete();
    void configurationChanged(const QNetworkConfiguration& config);
    void onlineStateChanged(bool isOnline);

private:
#ifdef Q_OS_WIN
    void updateAccessPointConfiguration(QNetworkConfigurationPrivate *cpPriv, QList<QString> &knownConfigs);
    void updateNlaConfigurations(QList<QString> &knownConfigs);
#ifndef Q_OS_WINCE
    bool updateWlanNativeConfigurations(QList<QString> &knownConfigs);
    void updateWlanIoctlConfigurations(QList<QString> &knownConfigs);
#endif
    void updateInternetServiceConfiguration(QList<QString> &knownConfigs);
#ifndef Q_OS_WINCE
    void updateWlanConfigurations(QList<QString> &knownConfigs);
#endif

    void abort();
#endif
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    QStringList knownSsids;
    bool updating;
    QString currentActiveAP;
    QString defaultConnectionPath;
    QStringList getKnownSsids();
    void updateEthConfigurations(QDBusInterface &devIface);
    void updateWifiConfigurations(QDBusInterface &devIface);
    void updateServiceNetworks(QDBusInterface &devIface);
    void updateServiceNetworkState(bool isWifi);
    void updateState(const QString &ident, quint32 state);
    QString getNameForConfiguration(QDBusInterface &devIface);

    QStringList getActiveConnectionsPaths(QDBusInterface &iface);
    QNetworkConfiguration::StateFlags getAPState(qint32 vState, bool isKnown);
//    QStringList getActiveDevicesPaths(QDBusInterface &iface);
#endif
#ifdef Q_OS_WIN
    QNlaEngine *nla;
    QNativeWifiEngine *nativeWifi;
    QIoctlWifiEngine *ioctlWifi;

    uint onlineConfigurations;

    enum EngineUpdate {
        NotUpdating = 0x00,
        Updating = 0x01,
        NlaUpdating = 0x02,
        NativeWifiUpdating = 0x04,
        IoctlWifiUpdating = 0x08,
    };
    Q_DECLARE_FLAGS(EngineUpdateState, EngineUpdate)

    EngineUpdateState updateState;
#endif
private slots:
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    void cmpPropertiesChanged(const QString &, QMap<QString,QVariant> map);
    void accessPointAdded(const QString &, QDBusObjectPath );
    void accessPointRemoved( const QString &, QDBusObjectPath );
    void updateDeviceInterfaceState(const QString &, quint32);
    void updateAccessPointState(const QString &, quint32);
#endif


};



QT_END_NAMESPACE

#endif //QNETWORKCONFIGURATIONMANAGERPRIVATE_H
