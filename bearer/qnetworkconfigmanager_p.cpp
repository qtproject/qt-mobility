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

#include "qnetworkconfigmanager_p.h"
#include "qgenericengine_p.h"
#include "qnlaengine_win_p.h"

#ifndef Q_OS_WINCE
#include "qnativewifiengine_win_p.h"
#include "qioctlwifiengine_win_p.h"
#endif

#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

void QNetworkConfigurationManagerPrivate::registerPlatformCapabilities()
{
    capFlags = QNetworkConfigurationManager::ForcedRoaming;
}

void QNetworkConfigurationManagerPrivate::updateAccessPointConfiguration(QNetworkConfigurationPrivate *cpPriv, QList<QString> &knownConfigs)
{
    if (accessPointConfigurations.contains(cpPriv->id)) {
        knownConfigs.removeOne(cpPriv->id);

        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr =
            accessPointConfigurations.value(cpPriv->id);

        if (ptr.data()->isValid != cpPriv->isValid ||
            ptr.data()->name != cpPriv->name ||
            ptr.data()->id != cpPriv->id ||
            ptr.data()->state != cpPriv->state ||
            ptr.data()->type != cpPriv->type ||
            ptr.data()->roamingSupported != cpPriv->roamingSupported ||
            ptr.data()->purpose != cpPriv->purpose ||
            ptr.data()->internet != cpPriv->internet) {

            const QNetworkConfiguration::StateFlags oldState = ptr.data()->state;

            ptr.data()->isValid = cpPriv->isValid;
            ptr.data()->name = cpPriv->name;
            ptr.data()->id = cpPriv->id;
            ptr.data()->state = cpPriv->state;
            ptr.data()->type = cpPriv->type;
            ptr.data()->roamingSupported = cpPriv->roamingSupported;
            ptr.data()->purpose = cpPriv->purpose;
            ptr.data()->internet = cpPriv->internet;

            if (!firstUpdate) {
                QNetworkConfiguration item;
                item.d = ptr;
                emit configurationChanged(item);

                if (ptr.data()->state == QNetworkConfiguration::Active &&
                    oldState != ptr.data()->state) {
                    // configuration went online
                    ++onlineConfigurations;
                    if (onlineConfigurations == 1)
                        emit onlineStateChanged(true);
                } else if (oldState == QNetworkConfiguration::Active) {
                    // configuration went offline
                    --onlineConfigurations;
                    if (onlineConfigurations == 0)
                        emit onlineStateChanged(false);
                }
            }
        }

        delete cpPriv;
    } else {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
        accessPointConfigurations.insert(cpPriv->id, ptr);
        if (!firstUpdate) {
            QNetworkConfiguration item;
            item.d = ptr;
            emit configurationAdded(item);

            if (ptr.data()->state == QNetworkConfiguration::Active) {
                ++onlineConfigurations;
                if (onlineConfigurations == 1)
                    emit onlineStateChanged(true);
            }
        }
    }
}

void QNetworkConfigurationManagerPrivate::updateGenericConfigurations(QList<QString> &knownConfigs)
{
    QList<QNetworkConfigurationPrivate *> foundConfigurations = generic->getConfigurations();

    while (!foundConfigurations.isEmpty()) {
        QNetworkConfigurationPrivate *cpPriv = foundConfigurations.takeFirst();

        updateAccessPointConfiguration(cpPriv, knownConfigs);
    }
}

#ifdef Q_OS_WIN
void QNetworkConfigurationManagerPrivate::updateNlaConfigurations(QList<QString> &knownConfigs)
{
    QList<QNetworkConfigurationPrivate *> foundConfigurations = nla->getConfigurations();

    while (!foundConfigurations.isEmpty()) {
        QNetworkConfigurationPrivate *cpPriv = foundConfigurations.takeFirst();

        updateAccessPointConfiguration(cpPriv, knownConfigs);
    }
}
#endif

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
bool QNetworkConfigurationManagerPrivate::updateWlanNativeConfigurations(QList<QString> &knownConfigs)
{
    bool ok;

    QList<QNetworkConfigurationPrivate *> foundConfigurations = nativeWifi->getConfigurations(&ok);

    if (!ok)
        return false;

    while (!foundConfigurations.isEmpty()) {
        QNetworkConfigurationPrivate *cpPriv = foundConfigurations.takeFirst();

        updateAccessPointConfiguration(cpPriv, knownConfigs);
    }

    return true;
}

void QNetworkConfigurationManagerPrivate::updateWlanIoctlConfigurations(QList<QString> &knownConfigs)
{
    QList<QNetworkConfigurationPrivate *> foundConfigurations = ioctlWifi->getConfigurations();

    while (!foundConfigurations.isEmpty()) {
        QNetworkConfigurationPrivate *cpPriv = foundConfigurations.takeFirst();

        updateAccessPointConfiguration(cpPriv, knownConfigs);
    }

    /*
        TODO

        Work around limitation in ioctl method, cannot determine the state of WLAN configurations
        correctly.

        Mark all now out of range WLANs as Defined.
    */
    QMutableListIterator<QString> i(knownConfigs);
    while (i.hasNext()) {
        const QString identifier = i.next();

        if (ioctlWifi->hasIdentifier(identifier)) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr =
                accessPointConfigurations.value(identifier);

            ptr.data()->state = QNetworkConfiguration::Defined;

            i.remove();
        }
    }
}
#endif

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
void QNetworkConfigurationManagerPrivate::updateWlanConfigurations(QList<QString> &knownConfigs)
{
    // try to use Native WiFi, otherwise fallback to ioctl method
    if (!(nativeWifi && updateWlanNativeConfigurations(knownConfigs)) && ioctlWifi)
        updateWlanIoctlConfigurations(knownConfigs);
}
#endif

void QNetworkConfigurationManagerPrivate::updateInternetServiceConfiguration(QList<QString> &knownConfigs)
{
    if (!snapConfigurations.contains(QLatin1String("Internet Service Network"))) {
        QNetworkConfigurationPrivate *serviceNetwork = new QNetworkConfigurationPrivate;
        serviceNetwork->name = tr("Internet");
        serviceNetwork->isValid = true;
        serviceNetwork->id = QLatin1String("Internet Service Network");
        serviceNetwork->state = QNetworkConfiguration::Defined;
        serviceNetwork->type = QNetworkConfiguration::ServiceNetwork;

        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(serviceNetwork);

        snapConfigurations.insert(serviceNetwork->id, ptr);

        if (!firstUpdate) {
            QNetworkConfiguration item;
            item.d = ptr;
            emit configurationAdded(item);
        }
    }

    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr =
        snapConfigurations.value(QLatin1String("Internet Service Network"));

    QList<QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > serviceNetworkMembers;

    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> >::const_iterator i =
        accessPointConfigurations.constBegin();

    QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Defined;
    while (i != accessPointConfigurations.constEnd()) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> child = i.value();

        if (child.data()->internet && !knownConfigs.contains(child.data()->id)) {
            serviceNetworkMembers.append(child);

            state |= child.data()->state;
        }

        ++i;
    }

    ptr.data()->state = state;

    if (ptr.data()->serviceNetworkMembers != serviceNetworkMembers) {
        ptr.data()->serviceNetworkMembers = serviceNetworkMembers;

        QNetworkConfiguration item;
        item.d = ptr;
        emit configurationChanged(item);
    }
}

void QNetworkConfigurationManagerPrivate::updateConfigurations()
{
    if (firstUpdate) {
        updateState = NotUpdating;
        onlineConfigurations = 0;

        generic = QGenericEngine::instance();
        if (generic) {
            connect(generic, SIGNAL(configurationsChanged()),
                    this, SLOT(updateConfigurations()));
        }

#ifdef Q_OS_WIN
        nla = QNlaEngine::instance();
        if (nla) {
            connect(nla, SIGNAL(configurationsChanged()),
                    this, SLOT(updateConfigurations()));
        }
#endif

#ifdef Q_OS_WIN
#ifndef Q_OS_WINCE
        nativeWifi = QNativeWifiEngine::instance();
        if (nativeWifi) {
            connect(nativeWifi, SIGNAL(configurationsChanged()),
                    this, SLOT(updateConfigurations()));

            capFlags |= QNetworkConfigurationManager::BearerManagement;
        }

        ioctlWifi = QIoctlWifiEngine::instance();
        if (ioctlWifi) {
            connect(ioctlWifi, SIGNAL(configurationsChanged()),
                    this, SLOT(updateConfigurations()));
        }
#endif
#endif
    }

    if (updateState & Updating) {
        QNetworkSessionEngine *engine = qobject_cast<QNetworkSessionEngine *>(sender());
        if (engine) {
            if (engine == generic)
                updateState &= ~GenericUpdating;
#ifdef Q_OS_WIN
            else if (engine == nla)
                updateState &= ~NlaUpdating;
#ifndef Q_OS_WINCE
            else if (engine == nativeWifi)
                updateState &= ~NativeWifiUpdating;
            else if (engine == ioctlWifi)
                updateState &= ~IoctlWifiUpdating;
#endif
#endif
        }
    }

    QList<QString> knownConfigs = accessPointConfigurations.keys();

    updateGenericConfigurations(knownConfigs);
#ifdef Q_OS_WIN
    updateNlaConfigurations(knownConfigs);
#endif
#ifdef Q_OS_WIN
#ifndef Q_OS_WINCE
    updateWlanConfigurations(knownConfigs);
#endif
#endif
    updateInternetServiceConfiguration(knownConfigs);

    QListIterator<QString> i(knownConfigs);
    while (i.hasNext()) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv =
            accessPointConfigurations.take(i.next());

        priv->isValid = false;

        if (!firstUpdate) {
            QNetworkConfiguration item;
            item.d = priv;
            emit configurationRemoved(item);

            if (priv.data()->state == QNetworkConfiguration::Active) {
                --onlineConfigurations;
                if (onlineConfigurations == 0)
                    emit onlineStateChanged(false);
            }
        }
    }

    if (updateState == Updating) {
        updateState = NotUpdating;
        emit configurationUpdateComplete();
    }

    if (firstUpdate)
        firstUpdate = false;
}

/*!
    Returns the first active configuration found, if one exists; otherwise returns the first
    discovered configuration found, if one exists; otherwise returns an empty configuration.

    \internal
*/
QNetworkConfiguration QNetworkConfigurationManagerPrivate::defaultConfiguration()
{
    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> firstActive;
    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> firstDiscovered;

    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> >::const_iterator i =
        accessPointConfigurations.constBegin();
    while (i != accessPointConfigurations.constEnd()) {
        QNetworkConfigurationPrivate *priv = i.value().data();

        if (!firstActive && priv->isValid &&
            (priv->state & QNetworkConfiguration::Active) == QNetworkConfiguration::Active)
            firstActive = i.value();
        if (!firstDiscovered && priv->isValid &&
            (priv->state & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered)
            firstDiscovered = i.value();

        ++i;
    }

    QNetworkConfiguration item;

    if (firstActive)
        item.d = firstActive;
    else if (firstDiscovered)
        item.d = firstDiscovered;

    return item;
}

void QNetworkConfigurationManagerPrivate::performAsyncConfigurationUpdate()
{
    updateState = Updating;

    if (generic) {
        updateState |= GenericUpdating;
        generic->requestUpdate();
    }

#ifdef Q_OS_WIN
    if (nla) {
        updateState |= NlaUpdating;
        nla->requestUpdate();
    }
#endif

#ifdef Q_OS_WIN
#ifndef Q_OS_WINCE
    if (nativeWifi) {
        updateState |= NativeWifiUpdating;
        nativeWifi->requestUpdate();
    }

    if (ioctlWifi) {
        updateState |= IoctlWifiUpdating;
        ioctlWifi->requestUpdate();
    }
#endif
#endif
}

QT_END_NAMESPACE

