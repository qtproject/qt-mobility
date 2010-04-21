/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
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
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qnetworkconfigmanager.h"
#include "qnetworkconfiguration_p.h"

#include <QHash>
#include <QStringList>

QTM_BEGIN_NAMESPACE

#ifdef BEARER_ENGINE
class QNetworkSessionEngine;
class QGenericEngine;
class QNlaEngine;
class QNativeWifiEngine;
class QNmWifiEngine;
class QCoreWlanEngine;
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

    QNetworkConfigurationManager::Capabilities capFlags;
    void registerPlatformCapabilities();

    void performAsyncConfigurationUpdate();

    //this table contains an up to date list of all configs at any time.
    //it must be updated if configurations change, are added/removed or
    //the members of ServiceNetworks change
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > accessPointConfigurations;
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > snapConfigurations;
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > userChoiceConfigurations;
#ifdef BEARER_ENGINE
    QHash<QString, QNetworkSessionEngine *> configurationEngine;
#endif
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

#ifdef BEARER_ENGINE
    QGenericEngine *generic;
#ifdef Q_OS_WIN
    QNlaEngine *nla;
#ifndef Q_OS_WINCE
    QNativeWifiEngine *nativeWifi;
#endif
#endif
#ifdef BACKEND_NM
    QNmWifiEngine *nmWifi;
#endif
#ifdef Q_OS_DARWIN
    QCoreWlanEngine *coreWifi;
#endif

    uint onlineConfigurations;

    enum EngineUpdate {
        NotUpdating = 0x00,
        Updating = 0x01,
        GenericUpdating = 0x02,
        NlaUpdating = 0x04,
        NativeWifiUpdating = 0x08,
        NmUpdating = 0x20,
        CoreWifiUpdating = 0x40,
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
};

QTM_END_NAMESPACE

#endif //QNETWORKCONFIGURATIONMANAGERPRIVATE_H
