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

#ifndef QNETWORKSESSIONPRIVATE_H
#define QNETWORKSESSIONPRIVATE_H

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
#include "qnetworkconfigmanager_maemo_p.h"
#include "qnetworksession.h"

#include <qnetworksession.h>
#include <QNetworkInterface>
#include <QDateTime>
#include <QTimer>

#include <icd/dbus_api.h>

QTM_BEGIN_NAMESPACE

struct ICd2DetailsDBusStruct
{
    QString serviceType;
    uint serviceAttributes;
    QString setviceId;
    QString networkType;
    uint networkAttributes;
    QByteArray networkId;
};

typedef QList<ICd2DetailsDBusStruct> ICd2DetailsList;

class QNetworkSessionPrivate : public QObject
{
    Q_OBJECT
public:
    QNetworkSessionPrivate() : 
        tx_data(0), rx_data(0), m_activeTime(0), isOpen(false),
        connectFlags(ICD_CONNECTION_FLAG_USER_EVENT),
        currentState(QNetworkSession::Invalid),
        m_asynchCallActive(false)
    {
        m_stopTimer.setSingleShot(true);
        connect(&m_stopTimer, SIGNAL(timeout()), this, SLOT(finishStopBySendingClosedSignal()));

        QDBusConnection systemBus = QDBusConnection::systemBus();

        m_dbusInterface = new QDBusInterface(ICD_DBUS_API_INTERFACE,
                                         ICD_DBUS_API_PATH,
                                         ICD_DBUS_API_INTERFACE,
                                         systemBus,
                                         this);

        systemBus.connect(ICD_DBUS_API_INTERFACE,
                        ICD_DBUS_API_PATH,
                        ICD_DBUS_API_INTERFACE,
                        ICD_DBUS_API_CONNECT_SIG,
                        this,
                        SLOT(stateChange(const QDBusMessage&)));

        qDBusRegisterMetaType<ICd2DetailsDBusStruct>();
        qDBusRegisterMetaType<ICd2DetailsList>();

        m_connectRequestTimer.setSingleShot(true);
        connect(&m_connectRequestTimer, SIGNAL(timeout()), this, SLOT(connectTimeout()));
    }

    ~QNetworkSessionPrivate()
    {
	cleanupSession();
    }

    //called by QNetworkSession constructor and ensures
    //that the state is immediately updated (w/o actually opening
    //a session). Also this function should take care of 
    //notification hooks to discover future state changes.
    void syncStateWithInterface();

    QNetworkInterface currentInterface() const;
    QVariant sessionProperty(const QString& key) const;
    void setSessionProperty(const QString& key, const QVariant& value);

    void open();
    void close();
    void stop();
    void migrate();
    void accept();
    void ignore();
    void reject();

    QString errorString() const; //must return translated string
    QNetworkSession::SessionError error() const;

    quint64 bytesWritten() const;
    quint64 bytesReceived() const;
    quint64 activeTime() const;

private:
    void updateStateFromServiceNetwork();
    void updateStateFromActiveConfig();

Q_SIGNALS:
    //releases any pending waitForOpened() calls
    void quitPendingWaitsForOpened();

private Q_SLOTS:
    void do_open();
    void networkConfigurationsChanged();
    void iapStateChanged(const QString& iapid, uint icd_connection_state);
    void updateProxies(QNetworkSession::State newState);
    void finishStopBySendingClosedSignal();
    void stateChange(const QDBusMessage& rep);
    void connectTimeout();

private:
    QNetworkConfigurationManager manager;

    quint64 tx_data;
    quint64 rx_data;
    quint64 m_activeTime;

    // The config set on QNetworkSession.
    QNetworkConfiguration publicConfig;
    QNetworkConfiguration config;

    // If publicConfig is a ServiceNetwork this is a copy of publicConfig.
    // If publicConfig is an UserChoice that is resolved to a ServiceNetwork this is the actual
    // ServiceNetwork configuration.
    QNetworkConfiguration serviceConfig;

    // This is the actual active configuration currently in use by the session.
    // Either a copy of publicConfig or one of serviceConfig.children().
    QNetworkConfiguration activeConfig;

    QNetworkConfiguration& copyConfig(QNetworkConfiguration &fromConfig, QNetworkConfiguration &toConfig, bool deepCopy = true);
    void clearConfiguration(QNetworkConfiguration &config);

    QNetworkSession::State state;
    bool isOpen;
    bool opened;
    icd_connection_flags connectFlags;

    QNetworkSession::SessionError lastError;

    QNetworkSession* q;
    friend class QNetworkSession;

    QDateTime startTime;
    QString currentNetworkInterface;
    friend class IcdListener;
    void updateState(QNetworkSession::State);
    void updateIdentifier(const QString &newId);
    quint64 getStatistics(bool sent) const;
    void cleanupSession(void);

    void updateProxyInformation();
    void clearProxyInformation();
    QNetworkSession::State currentState;

    QDBusInterface *m_dbusInterface;

    QTimer m_stopTimer;

    bool m_asynchCallActive;
    QTimer m_connectRequestTimer;
};

QTM_END_NAMESPACE

// Marshall the ICd2DetailsDBusStruct data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const QtMobility::ICd2DetailsDBusStruct &icd2);

// Retrieve the ICd2DetailsDBusStruct data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, QtMobility::ICd2DetailsDBusStruct &icd2);

Q_DECLARE_METATYPE(QtMobility::ICd2DetailsDBusStruct);
Q_DECLARE_METATYPE(QtMobility::ICd2DetailsList);

#endif //QNETWORKSESSIONPRIVATE_H

