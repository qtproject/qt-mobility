/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qnetworksession_p.h"
#include "qnetworksession.h"
#include "qnetworksessionengine_win_p.h"
#include "qnlaengine_win_p.h"

#ifndef Q_OS_WINCE
#include "qnativewifiengine_win_p.h"
#include "qioctlwifiengine_win_p.h"
#endif

#include <QtCore/qstringlist.h>
#include <QtCore/qdebug.h>
#include <QtCore/qmutex.h>

#include <QtNetwork/qnetworkinterface.h>

QT_BEGIN_NAMESPACE

static QNetworkSessionEngine *getEngineFromId(const QString &id)
{
    QNlaEngine *nla = QNlaEngine::instance();
    if (nla && nla->hasIdentifier(id))
        return nla;

#ifndef Q_OS_WINCE
    QNativeWifiEngine *nativeWifi = QNativeWifiEngine::instance();
    if (nativeWifi && nativeWifi->hasIdentifier(id))
        return nativeWifi;

    QIoctlWifiEngine *ioctlWifi = QIoctlWifiEngine::instance();
    if (ioctlWifi && ioctlWifi->hasIdentifier(id))
        return ioctlWifi;
#endif

    return 0;
}

class QNetworkSessionManagerPrivate : public QObject
{
    Q_OBJECT

public:
    QNetworkSessionManagerPrivate(QObject *parent = 0);
    ~QNetworkSessionManagerPrivate();

    void increment(const QNetworkConfiguration &config);
    void decrement(const QNetworkConfiguration &config);
    void clear(const QNetworkConfiguration &config);

    unsigned int referenceCount(const QNetworkConfiguration &config);

private:
    void startConfiguration(const QNetworkConfiguration &config);
    void stopConfiguration(const QNetworkConfiguration &config);

Q_SIGNALS:
    void forcedSessionClose(const QNetworkConfiguration &config);

private:
    QMutex lock;
    QMap<QString, unsigned int> refCount;
};

#include "qnetworksession_win.moc"

Q_GLOBAL_STATIC(QNetworkSessionManagerPrivate, sessionManager);

QNetworkSessionManagerPrivate::QNetworkSessionManagerPrivate(QObject *parent)
:   QObject(parent)
{
}

QNetworkSessionManagerPrivate::~QNetworkSessionManagerPrivate()
{
}

void QNetworkSessionManagerPrivate::increment(const QNetworkConfiguration &config)
{
    QMutexLocker locker(&lock);

    ++refCount[config.identifier()];

    if ((config.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active &&
        (config.state() & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
        startConfiguration(config);
    }
}

void QNetworkSessionManagerPrivate::decrement(const QNetworkConfiguration &config)
{
    QMutexLocker locker(&lock);

    if (refCount[config.identifier()] > 0) {
        --refCount[config.identifier()];

        if (refCount[config.identifier()] == 0 &&
            (config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            stopConfiguration(config);
        }
    }
}

void QNetworkSessionManagerPrivate::clear(const QNetworkConfiguration &config)
{
    QMutexLocker locker(&lock);

    if (refCount[config.identifier()] != 0) {
        refCount[config.identifier()] = 0;

        if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active)
            stopConfiguration(config);

        emit forcedSessionClose(config);
    }
}

void QNetworkSessionManagerPrivate::startConfiguration(const QNetworkConfiguration &config)
{
    QNetworkSessionEngine *engine = getEngineFromId(config.identifier());

    if (!engine) {
        qDebug() << "cannot start configuration (no engine)" << config.name();
        return;
    }

    engine->connectToId(config.identifier());
}

void QNetworkSessionManagerPrivate::stopConfiguration(const QNetworkConfiguration &config)
{
    QNetworkSessionEngine *engine = getEngineFromId(config.identifier());

    if (!engine) {
        qDebug() << "cannot stop configuration (no engine)" << config.name();
        return;
    }

    engine->disconnectFromId(config.identifier());
}

unsigned int QNetworkSessionManagerPrivate::referenceCount(const QNetworkConfiguration &config)
{
    QMutexLocker locker(&lock);

    return refCount[config.identifier()];
}

void QNetworkSessionPrivate::syncStateWithInterface()
{
    connect(&manager, SIGNAL(updateCompleted()), this, SLOT(networkConfigurationsChanged()));
    connect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
            this, SLOT(configurationChanged(QNetworkConfiguration)));
    connect(sessionManager(), SIGNAL(forcedSessionClose(QNetworkConfiguration)),
            this, SLOT(forcedSessionClose(QNetworkConfiguration)));

    opened = false;
    state = QNetworkSession::Invalid;
    lastError = QNetworkSession::UnknownSessionError;

    qRegisterMetaType<QNetworkSessionEngine::ConnectionError>
        ("QNetworkSessionEngine::ConnectionError");

    if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
        engine = getEngineFromId(publicConfig.identifier());
        connect(engine, SIGNAL(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                this, SLOT(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                Qt::QueuedConnection);
    } else {
        engine = 0;
    }

    networkConfigurationsChanged();
}

void QNetworkSessionPrivate::open()
{
    QNetworkConfigurationManager manager;

    if (manager.capabilities() & QNetworkConfigurationManager::SystemSessionSupport) {
        qFatal("System Session supported not available on Microsoft Windows.");
    } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit q->error(lastError);
    } else if (!isActive) {
        if ((publicConfig.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            lastError =QNetworkSession::InvalidConfigurationError;
            emit q->error(lastError);
            return;
        }
        // increment session count
        opened = true;
        sessionManager()->increment(publicConfig);
        isActive = (publicConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active;
        if (isActive)
            emit quitPendingWaitsForOpened();
    }
}

void QNetworkSessionPrivate::close()
{
    QNetworkConfigurationManager manager;

    if (manager.capabilities() & QNetworkConfigurationManager::SystemSessionSupport) {
        qFatal("System Session supported not available on Microsoft Windows.");
    } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit q->error(lastError);
    } else if (isActive) {
        // decrement session count
        sessionManager()->decrement(publicConfig);

        opened = false;
        isActive = false;
        emit q->sessionClosed();
    }
}

void QNetworkSessionPrivate::stop()
{
    QNetworkConfigurationManager manager;

    if (manager.capabilities() & QNetworkConfigurationManager::SystemSessionSupport) {
        qFatal("System Session supported not available on Microsoft Windows.");
    } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit q->error(lastError);
    } else {
        // force the session reference count to 0
        sessionManager()->clear(publicConfig);

        opened = false;
        isActive = false;
        emit q->sessionClosed();
    }
}

void QNetworkSessionPrivate::migrate()
{
    qFatal("Function not implemented at %s.", __FUNCTION__);
}

void QNetworkSessionPrivate::accept()
{
    qFatal("Function not implemented at %s.", __FUNCTION__);
}

void QNetworkSessionPrivate::ignore()
{
    qFatal("Function not implemented at %s.", __FUNCTION__);
}

void QNetworkSessionPrivate::reject()
{
    qFatal("Function not implemented at %s.", __FUNCTION__);
}

QNetworkInterface QNetworkSessionPrivate::currentInterface() const
{
    if (!publicConfig.isValid() || !engine)
        return QNetworkInterface();

    if (state != QNetworkSession::Connected)
        return QNetworkInterface();

    QString interface;

    if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork)
        interface = engine->getInterfaceFromId(actualConfig.identifier());
    else
        interface = engine->getInterfaceFromId(publicConfig.identifier());

    if (interface.isEmpty())
        return QNetworkInterface();

    return QNetworkInterface::interfaceFromName(interface);
}

QVariant QNetworkSessionPrivate::property(const QString& key)
{
    if (!publicConfig.isValid())
        return QVariant();

    if (key == "ActiveConfigurationIdentifier") {
        if (!isActive) {
            return QString();
        } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork){
            return actualConfig.identifier();
        } else {
            return publicConfig.identifier();
        }
    } else if (key == "SessionReferenceCount") {
        return QVariant::fromValue(sessionManager()->referenceCount(publicConfig));
    }

    return QVariant();
}

QString QNetworkSessionPrivate::bearerName() const
{
    if (!publicConfig.isValid() || !engine)
        return QString();

    if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork)
        return engine->bearerName(actualConfig.identifier());
    else
        return engine->bearerName(publicConfig.identifier());
}

QString QNetworkSessionPrivate::errorString() const
{
    switch (lastError) {
    case QNetworkSession::UnknownSessionError:
        return tr("Unknown session error.");
    case QNetworkSession::SessionAbortedError:
        return tr("The session was aborted by the user or system.");
    case QNetworkSession::OperationNotSupportedError:
        return tr("The requested operation is not supported by the system.");
    case QNetworkSession::InvalidConfigurationError:
        return tr("The specified configuration cannot be used.");
    }

    return QString();
}

QNetworkSession::SessionError QNetworkSessionPrivate::error() const
{
    return lastError;
}

quint64 QNetworkSessionPrivate::sentData() const
{
    return tx_data;
}

quint64 QNetworkSessionPrivate::receivedData() const
{
    return rx_data;
}

quint64 QNetworkSessionPrivate::activeTime() const
{
    return m_activeTime;
}

void QNetworkSessionPrivate::updateStateFromServiceNetwork()
{
    QNetworkSession::State oldState = state;

    foreach (const QNetworkConfiguration &config, publicConfig.children()) {
        if ((config.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active)
            continue;

        if (actualConfig != config) {
            if (engine) {
                disconnect(engine, SIGNAL(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                           this, SLOT(connectionError(QString,QNetworkSessionEngine::ConnectionError)));
            }

            actualConfig = config;
            engine = getEngineFromId(actualConfig.identifier());
            connect(engine, SIGNAL(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                    this, SLOT(connectionError(QString,QNetworkSessionEngine::ConnectionError)),
                    Qt::QueuedConnection);
            emit q->newConfigurationActivated();
        }

        state = QNetworkSession::Connected;
        if (state != oldState)
            emit q->stateChanged(state);

        return;
    }

    // No active configurations found, must be disconnected.
    state = QNetworkSession::Disconnected;
    if (state != oldState)
        emit q->stateChanged(state);
}

void QNetworkSessionPrivate::updateStateFromPublicConfig()
{
    QNetworkSession::State oldState = state;

    bool newActive = false;

    if (!publicConfig.isValid()) {
        state = QNetworkSession::Invalid;
    } else if ((publicConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
        state = QNetworkSession::Connected;
        newActive = opened;
    } else if ((publicConfig.state() & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
        state = QNetworkSession::Disconnected;
    } else if ((publicConfig.state() & QNetworkConfiguration::Defined) == QNetworkConfiguration::Defined) {
        state = QNetworkSession::NotAvailable;
    } else if ((publicConfig.state() & QNetworkConfiguration::Undefined) == QNetworkConfiguration::Undefined) {
        state = QNetworkSession::NotAvailable;
    }

    bool oldActive = isActive;
    isActive = newActive;

    if (!oldActive && isActive)
        emit quitPendingWaitsForOpened();
    if (oldActive && !isActive)
        emit q->sessionClosed();

    if (oldState != state)
        emit q->stateChanged(state);
}

void QNetworkSessionPrivate::networkConfigurationsChanged()
{
    if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork)
        updateStateFromServiceNetwork();
    else
        updateStateFromPublicConfig();
}

void QNetworkSessionPrivate::configurationChanged(const QNetworkConfiguration &config)
{
    if (config == publicConfig && publicConfig.type() == QNetworkConfiguration::ServiceNetwork ||
        config == actualConfig) {
        updateStateFromServiceNetwork();
    } else {
        updateStateFromPublicConfig();
    }
}

void QNetworkSessionPrivate::forcedSessionClose(const QNetworkConfiguration &config)
{
    if (publicConfig == config) {
        opened = false;
        isActive = false;

        emit q->sessionClosed();

        lastError = QNetworkSession::SessionAbortedError;
        emit q->error(lastError);
    }
}

void QNetworkSessionPrivate::connectionError(const QString &id, QNetworkSessionEngine::ConnectionError error)
{
    if (publicConfig.identifier() == id ||
        publicConfig.type() == QNetworkConfiguration::ServiceNetwork &&
        actualConfig.identifier() == id) {
        switch (error) {
        case QNetworkSessionEngine::OperationNotSupported:
            lastError = QNetworkSession::OperationNotSupportedError;
            opened = false;
            if (publicConfig.identifier() == id)
                sessionManager()->decrement(publicConfig);
            else
                sessionManager()->decrement(actualConfig);
            break;
        case QNetworkSessionEngine::InterfaceLookupError:
        case QNetworkSessionEngine::ConnectError:
        case QNetworkSessionEngine::DisconnectionError:
        default:
            lastError = QNetworkSession::UnknownSessionError;
        }

        emit q->error(lastError);
    }
}

QT_END_NAMESPACE

