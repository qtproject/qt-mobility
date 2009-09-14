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

#include <QHash>

#include "qnetworksession_p.h"
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

#include <maemo_icd.h>
#include <iapconf.h>

QT_BEGIN_NAMESPACE

static QHash<QString, QVariant> properties;

class IcdListener;
static IcdListener &icdListener();
static DBusConnection *dbus_connection;
static DBusHandlerResult signal_handler(DBusConnection *connection,
					DBusMessage *message,
					void *user_data);

#define ICD_DBUS_MATCH		"type='signal'," \
				"interface='" ICD_DBUS_INTERFACE "'," \
				"path='" ICD_DBUS_PATH "'"


static inline DBusConnection *get_dbus_conn(DBusError *error)
{
    DBusConnection *conn = dbus_bus_get(DBUS_BUS_SYSTEM, error);
    qDebug() << "Listening to bus" << dbus_bus_get_unique_name(conn);

    return conn;
}


/* Helper class that monitors the Icd status messages and
 * can change the IAP status accordingly. This is a singleton.
 */
class IcdListener : public QObject
{
    Q_OBJECT

public:
    friend IcdListener &icdListener();
    friend DBusHandlerResult signal_handler(DBusConnection *connection,
					    DBusMessage *message,
					    void *user_data);
    void setup(QNetworkSessionPrivate *d);
    void cleanup();
    void cleanupSession(QNetworkSessionPrivate *ptr);

    enum IapConnectionStatus {
	/* The IAP was connected */
	CONNECTED = 0,
	/* The IAP was disconnected */
	DISCONNECTED,
	/* The IAP is disconnecting */
	DISCONNECTING,
	/* The IAP has a network address, but is not yet fully connected */
	NETWORK_UP
    };

private:
    void icdSignalReceived(QString&, QString&, QString&);
    bool first_call;
    IcdListener() : first_call(true) { }
    QHash<QString, QNetworkSessionPrivate* > sessions;
};

 
static IcdListener &icdListener()
{ 
    static IcdListener icd_listener;
    return icd_listener;
}


static DBusHandlerResult signal_handler(DBusConnection *,
					DBusMessage *message,
					void *user_data)
{
    if (dbus_message_is_signal(message,
				ICD_DBUS_INTERFACE,
				ICD_STATUS_CHANGED_SIG)) {

	IcdListener *icd = (IcdListener *)user_data;
	DBusError error;
	dbus_error_init(&error);

	char *iap_id = 0;
	char *network_type = 0;
	char *state = 0;

	if (dbus_message_get_args(message, &error,
				    DBUS_TYPE_STRING, &iap_id,
				    DBUS_TYPE_STRING, &network_type,
				    DBUS_TYPE_STRING, &state,
				    DBUS_TYPE_INVALID) == FALSE) {


	    qWarning() << QString("Failed to parse icd status signal: %1").arg(error.message);

        } else {
	    QString _iap_id(iap_id);
	    QString _network_type(network_type);
	    QString _state(state);

	    icd->icdSignalReceived(_iap_id, _network_type, _state);
	}

	dbus_error_free(&error);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


void IcdListener::setup(QNetworkSessionPrivate *d)
{
    if (first_call) {
	// We use the old Icd dbus interface like in ConIC
	DBusError error;
	dbus_error_init(&error);

	dbus_connection = get_dbus_conn(&error);
	if (dbus_error_is_set(&error)) {
	    qWarning() << "Cannot get dbus connection.";
	    dbus_error_free(&error);
	    return;
	}

	static struct DBusObjectPathVTable icd_vtable;
	icd_vtable.message_function = signal_handler;

	dbus_bus_add_match(dbus_connection, ICD_DBUS_MATCH, &error);
	if (dbus_error_is_set(&error)) {
	    qWarning() << "Cannot add match" << ICD_DBUS_MATCH;
	    dbus_error_free(&error);
	    return;
	}

	if (dbus_connection_register_object_path(dbus_connection,
						    ICD_DBUS_PATH,
						    &icd_vtable,
						    (void*)this) == FALSE) {
	    qWarning() << "Cannot register dbus signal handler, interface"<< ICD_DBUS_INTERFACE << "path" << ICD_DBUS_PATH;
	    dbus_error_free(&error);
	    return;
	}

	qDebug() << "Listening" << ICD_STATUS_CHANGED_SIG << "signal from" << ICD_DBUS_SERVICE;
	first_call = false;
	dbus_error_free(&error);
    }

    QString id = d->activeConfig.identifier();
    if (!sessions.contains(id)) {
	QNetworkSessionPrivate *ptr = d;
	sessions.insert(id, ptr);
    }
}


void IcdListener::icdSignalReceived(QString& iap_id,
				    QString& network_type,
				    QString& state)
{
    if (iap_id == OSSO_IAP_SCAN) // icd sends scan status signals which we will ignore
	return;

    qDebug() << "Status received:" << iap_id << "type" << network_type << "state" << state;

    if (!sessions.contains(iap_id)) {
	qDebug() << "No session for IAP" << iap_id;
	return;
    }

    QNetworkSessionPrivate *session = sessions.value(iap_id);
    QNetworkConfiguration ap_conf = session->manager.configurationFromIdentifier(iap_id);
    if (!ap_conf.isValid()) {
	qDebug() << "Unknown IAP" << iap_id;
	return;
    }

    IapConnectionStatus status;

    if (state == "IDLE") {
	status = DISCONNECTED;
    } else if (state == "CONNECTED") {
	status = CONNECTED;
    } else if (state == "NETWORKUP") {
	status = NETWORK_UP;
    } else {
	//qDebug() << "Unknown state" << state;
	return;
    }

    if (status == DISCONNECTED) {
	if (ap_conf.state() == QNetworkConfiguration::Active) {
	    /* The IAP was just disconnected by Icd */
	    session->updateState(QNetworkSession::Disconnected);
	} else {
	    qDebug() << "Got a network disconnect when in state" << ap_conf.state();
	}
    } else if (status == CONNECTED) {
	/* The IAP was just connected by Icd */
	session->updateState(QNetworkSession::Connected);

	/* Check that we connected to [ANY] so change the IAP name to real one */
	if (session->publicConfig.identifier() == OSSO_IAP_ANY) {
	    // TODO: the returned IAP id is not the ssid and it cannot be found when doing updateConfigurations(), fix me
	    session->updateIdentifier(iap_id);
	    // TODO: what about updating other fields in publicConfig
	    qDebug() << "IAP" << iap_id << "connected when connecting to" << OSSO_IAP_ANY;
	} else {
	    qDebug() << "IAP" << iap_id << "connected";
	}
    }

    return;
}


void IcdListener::cleanup()
{
    if (!first_call) {
	dbus_bus_remove_match(dbus_connection, ICD_DBUS_MATCH, NULL);
	dbus_connection_unref(dbus_connection);
    }
}


void IcdListener::cleanupSession(QNetworkSessionPrivate *ptr)
{
    (void)sessions.take(ptr->activeConfig.identifier());
}


void QNetworkSessionPrivate::cleanupSession(void)
{
    icdListener().cleanupSession(this);
}


void QNetworkSessionPrivate::updateState(QNetworkSession::State newState)
{
    if( newState != state) {
        state = newState;

	if (state == QNetworkSession::Disconnected)
	    isActive = false;
	else if (state == QNetworkSession::Connected)
	    isActive = true;

	if (publicConfig.d &&
	    publicConfig.d->type == QNetworkConfiguration::UserChoice)
	    publicConfig.d->type = QNetworkConfiguration::InternetAccessPoint;

	emit q->stateChanged(state);
    }
}


QString QNetworkSessionPrivate::updateIdentifier(QString &newId)
{
    return (publicConfig.d->id = newId);
}


/* The function does not update the tx_data and rx_data class vars because
 * the this update func is called from const function.
 */
quint64 QNetworkSessionPrivate::getStatistics(bool sent) const
{
    /* This could be also implemented by using the Maemo::Icd::statistics()
     * that gets the statistics data for a specific IAP. Change if
     * necessary.
     */
    Maemo::Icd icd;
    QList<Maemo::IcdStatisticsResult> stats_results;
    quint64 counter_rx = 0, counter_tx = 0;

    if (!icd.statistics(stats_results)) {
	return 0;
    }

    foreach (Maemo::IcdStatisticsResult res, stats_results) {
	if (res.params.network_attrs & ICD_NW_ATTR_IAPNAME) {
	    /* network_id is the IAP UUID */
	    if (QString(res.params.network_id.data()) == activeConfig.identifier()) {
		counter_tx = res.bytes_sent;
		counter_rx = res.bytes_received;
	    }
	} else {
	    /* We probably will never get to this branch */
	    QNetworkConfigurationPrivate *d = activeConfig.d.data();
	    if (res.params.network_id == d->network_id) {
		counter_tx = res.bytes_sent;
		counter_rx = res.bytes_received;
	    }
	}
    }

    if (sent)
	return counter_tx;
    else
	return counter_rx;
}


quint64 QNetworkSessionPrivate::sentData() const
{
    return getStatistics(true);
}

quint64 QNetworkSessionPrivate::receivedData() const
{
    return getStatistics(false);
}

quint64 QNetworkSessionPrivate::activeTime() const
{
    if (startTime.isNull()) {
        return 0;
    }
    return startTime.secsTo(QDateTime::currentDateTime());
}


/* This is called by QNetworkSession constructor */
void QNetworkSessionPrivate::syncStateWithInterface()
{
    /* Start to listen Icd status messages. */
    icdListener().setup(this);

    if (publicConfig.isValid())
	state = QNetworkSession::NotAvailable;
    else
	state = QNetworkSession::Invalid;
}


void QNetworkSessionPrivate::open()
{
    icd_connection_flags flags = ICD_CONNECTION_FLAG_USER_EVENT;
    bool st;
    QString result;
    QString iap = publicConfig.identifier();
    QString bearer_name;

    if (state == QNetworkSession::Connected) {
	qDebug() << "Already connected to" << publicConfig.identifier();
        emit q->stateChanged(QNetworkSession::Connected);
	return;
    }

    updateState(QNetworkSession::Connecting);

    Maemo::IcdConnectResult connect_result;
    Maemo::Icd icd(ICD_LONG_CONNECT_TIMEOUT);

    if (iap == OSSO_IAP_ANY) {
	qDebug() << "connecting to default IAP" << iap;
	st = icd.connect(flags, connect_result);
    } else {
	QList<Maemo::ConnectParams> params;
	Maemo::ConnectParams param;
	param.connect.network_type = bearerName();
	param.connect.network_attrs = 0;
	param.connect.network_id = QByteArray(iap.toLatin1());
	params.append(param);
	qDebug() << "connecting to" << param.connect.network_id.data() << "type" << param.connect.network_type;
	st = icd.connect(flags, params, connect_result);
    }

    if (st) {
	result = connect_result.connect.network_id.data();
	QString connected_iap = result;

	if (connected_iap.isEmpty()) {
	    qDebug() << "connect to"<< iap << "failed, result is empty";
	    emit q->error(QNetworkSession::InvalidConfigurationError);
	    return;
	}

	// TODO: the returned IAP id is not the ssid and it cannot be found when doing updateConfigurations(), fix me
	updateIdentifier(connected_iap);

	/* TODO:
	 * - listen IAP gconf deletions and remove configurations if necessary
	 * - when starting use state flag to get current online/offline status
	 */

	bearer_name = connect_result.connect.network_type;
	if (bearer_name == "WLAN_INFRA" ||
	    bearer_name == "WLAN_ADHOC")
	    currentBearerName = "WLAN";
	else if (bearer_name == "GPRS")
	    currentBearerName = "HSPA";
	else
	    currentBearerName = bearer_name;

	publicConfig.d->isValid = true;

	Maemo::IAPConf iap_name(publicConfig.d->id);
	publicConfig.d->name = iap_name.value("name").toString();
	publicConfig.d->state = QNetworkConfiguration::Active;
	publicConfig.d->type = QNetworkConfiguration::InternetAccessPoint;

	startTime = QDateTime::currentDateTime();
	updateState(QNetworkSession::Connected);

	qDebug() << "connected to" << result << publicConfig.d->name;

	activeConfig = publicConfig;
	emit q->sessionOpened();

    } else {
	qDebug() << "connect to"<< iap << "failed";
	emit q->error(QNetworkSession::UnknownSessionError);
    }
}


void QNetworkSessionPrivate::close()
{
    // this one simulates the application disconnect (same as libconic i.e., the connection is not really closed)
    Maemo::Icd icd;
    qDebug() << "closing session" << publicConfig.identifier();
    icd.disconnect(ICD_CONNECTION_FLAG_APPLICATION_EVENT);
    startTime = QDateTime();
    updateState(QNetworkSession::Disconnected);
    emit q->sessionClosed();
}


void QNetworkSessionPrivate::stop()
{
    // this one simulates the UI originated disconnect (the connection is closed for real)
    Maemo::Icd icd;
    qDebug() << "stopping session" << publicConfig.identifier();
    icd.disconnect(ICD_CONNECTION_FLAG_UI_EVENT);
    startTime = QDateTime();
    updateState(QNetworkSession::Disconnected);
    emit q->sessionClosed();
}


void QNetworkSessionPrivate::migrate()
{
    //TODO
}


void QNetworkSessionPrivate::accept()
{
    //TODO
}


void QNetworkSessionPrivate::ignore()
{
    //TODO
}


void QNetworkSessionPrivate::reject()
{
    //TODO
}


QNetworkInterface QNetworkSessionPrivate::currentInterface() const
{
    //TODO: set the currentNetworkInterface so some sane value somewhere
    return QNetworkInterface::interfaceFromName(currentNetworkInterface);
}


QVariant QNetworkSessionPrivate::property(const QString& key)
{
    if (!publicConfig.isValid())
        return QVariant();

    if (key == "ActiveConfigurationIdentifier") {
        if (!isActive) {
            return QString();
        } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork){
            return activeConfig.identifier();
        } else { 
            return publicConfig.identifier();
        }
    }

    return properties.value(key);
}


QString QNetworkSessionPrivate::bearerName() const
{
    return currentBearerName;
}


QString QNetworkSessionPrivate::errorString() const
{
    QString errorStr;
    switch(q->error()) {
    case QNetworkSession::RoamingError:
        errorStr = QObject::tr("Roaming error");
        break;
    case QNetworkSession::SessionAbortedError:
        errorStr = QObject::tr("Session aborted by user or system");
        break;
    default:
    case QNetworkSession::UnknownSessionError:
        errorStr = QObject::tr("Unidentified Error");
        break;
    }
    return errorStr;
}


QNetworkSession::SessionError QNetworkSessionPrivate::error() const
{
    return QNetworkSession::UnknownSessionError;
}

#include "qnetworksession_maemo.moc"

QT_END_NAMESPACE
