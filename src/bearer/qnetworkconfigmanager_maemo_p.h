/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
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

#include <QHash>
#include <QStringList>
#include <QDBusInterface>
#include <maemo_icd.h>

#include "qnetworkconfigmanager.h"
#include "qnetworkconfiguration_maemo_p.h"
#include "qnetworksession_maemo_p.h"

QTM_BEGIN_NAMESPACE

class QNetworkConfigurationPrivate;

class QNetworkConfigurationManagerPrivate : public QObject
{
    Q_OBJECT
public:
    QNetworkConfigurationManagerPrivate()
    :   QObject(0), capFlags(0), firstUpdate(true), onlineConfigurations(0), m_scanGoingOn(false)
    {
        registerPlatformCapabilities();
        updateConfigurations();
        init();
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

        configIdents = userChoiceConfigurations.keys();
        foreach(const QString oldIface, configIdents) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = userChoiceConfigurations.take(oldIface);
            priv->isValid = false;
            priv->id.clear();
        }

	cleanup();
    }

    QNetworkConfiguration defaultConfiguration();

    void init();

    QNetworkConfigurationManager::Capabilities capFlags;
    void registerPlatformCapabilities();

    void performAsyncConfigurationUpdate();
    void doUpdateConfigurations(QList<Maemo::IcdScanResult> scanned = QList<Maemo::IcdScanResult>());
    void startListeningStateSignalsForAllConnections();

    //this table contains an up to date list of all configs at any time.
    //it must be updated if configurations change, are added/removed or
    //the members of ServiceNetworks change
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > accessPointConfigurations;
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > snapConfigurations;
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > userChoiceConfigurations;
    bool firstUpdate;
    int onlineConfigurations;
    friend class IapMonitor;
    void cleanup();
    void deleteConfiguration(QString &iap_id);
    void addConfiguration(QString &iap_id);
    void configurationChanged(QNetworkConfigurationPrivate *ptr);
    uint32_t getNetworkAttrs(bool is_iap_id, const QString& iap_id,
                             const QString& iap_type, QString security_method);

    QDBusInterface *m_dbusInterface;
    QTimer m_scanTimer;
    bool m_gettingInitialConnectionState;
    bool m_scanGoingOn;
    QStringList m_typesToBeScanned;
    QList<Maemo::IcdScanResult> m_scanResult;
    QString m_onlineIapId;

    friend class QNetworkSessionPrivate;

public slots:
    void updateConfigurations();

private slots:
    void cancelAsyncConfigurationUpdate();
    void finishAsyncConfigurationUpdate();
    void asyncUpdateConfigurationsSlot(QDBusMessage msg);
    void connectionStateSignalsSlot(QDBusMessage msg);

Q_SIGNALS:
    void configurationAdded(const QNetworkConfiguration& config);
    void configurationRemoved(const QNetworkConfiguration& config);
    void configurationUpdateComplete();
    void configurationChanged(const QNetworkConfiguration& config);
    void onlineStateChanged(bool isOnline);

    void iapStateChanged(const QString& iapid, uint icd_connection_state);
};

QTM_END_NAMESPACE

#endif //QNETWORKCONFIGURATIONMANAGERPRIVATE_H
