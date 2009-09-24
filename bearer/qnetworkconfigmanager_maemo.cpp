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

#include <string.h>

#include "qnetworkconfigmanager_maemo_p.h"

#include <QDebug>
#include <QtDBus>
#include <QRegExp>
#include <QHash>

#include <wlancond.h>
#include <libicd-network-wlan-dev.h>
#include <maemo_icd.h>
#include <iapconf.h>
#include <iapmonitor.h>

QT_BEGIN_NAMESPACE

class IapMonitor;
static IapMonitor &iapMonitor();

#define IAP "/system/osso/connectivity/IAP"
static int iap_prefix_len;
static void notify_iap(GConfClient *client, guint id,
		    GConfEntry *entry, gpointer user_data);


/* The IapAddTimer is a helper class that makes sure we update
 * the configuration only after all gconf additions to certain
 * iap are finished (after a certain timeout)
 */
class _IapAddTimer : public QObject
{
    Q_OBJECT

public:
    _IapAddTimer() {}
    ~_IapAddTimer()
    {
	if (timer.isActive()) {
	    QObject::disconnect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
	    timer.stop();
	}
    }

    void add(QString& iap_id, QNetworkConfigurationManagerPrivate *d);

    QString iap_id;
    QTimer timer;
    QNetworkConfigurationManagerPrivate *d;

public Q_SLOTS:
    void timeout();
};


void _IapAddTimer::add(QString& id, QNetworkConfigurationManagerPrivate *d_ptr)
{
    iap_id = id;
    d = d_ptr;

    if (timer.isActive()) {
	QObject::disconnect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
	timer.stop();
    }
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer.start(1500);
}


void _IapAddTimer::timeout()
{
    d->addConfiguration(iap_id);
}


class IapAddTimer {
    QHash<QString, _IapAddTimer* > timers;

public:
    IapAddTimer() {}
    ~IapAddTimer() {}

    void add(QString& iap_id, QNetworkConfigurationManagerPrivate *d);
    void del(QString& iap_id);
    void removeAll();
};


void IapAddTimer::removeAll()
{
    QHashIterator<QString, _IapAddTimer* > i(timers);
    while (i.hasNext()) {
	i.next();
	_IapAddTimer *t = i.value();
	delete t;
    }
    timers.clear();
}

void IapAddTimer::add(QString& iap_id, QNetworkConfigurationManagerPrivate *d)
{
    if (timers.contains(iap_id)) {
	_IapAddTimer *iap = timers.value(iap_id);
	iap->add(iap_id, d);
    } else {
	_IapAddTimer *iap = new _IapAddTimer;
	iap->add(iap_id, d);
	timers.insert(iap_id, iap);
    }
}

void IapAddTimer::del(QString& iap_id)
{
    if (timers.contains(iap_id)) {
	_IapAddTimer *iap = timers.take(iap_id);
	delete iap;
    }
}


class IapMonitor
{
public:
    friend void notify_iap(GConfClient *, guint,
			GConfEntry *entry, gpointer user_data);

    friend IapMonitor &iapMonitor();
    void setup(QNetworkConfigurationManagerPrivate *d);
    void cleanup();

private:
    bool first_call;
    IapMonitor() : first_call(true) { }

    void iapAdded(const char *key, GConfEntry *entry);
    void iapDeleted(const char *key, GConfEntry *entry);

    Maemo::IAPMonitor *iap;
    QNetworkConfigurationManagerPrivate *d;
    IapAddTimer timers;
};


static IapMonitor &iapMonitor()
{ 
    static IapMonitor iap_monitor;
    return iap_monitor;
}


/* Notify func that is called when IAP is added or deleted */
static void notify_iap(GConfClient *, guint,
		    GConfEntry *entry, gpointer user_data)
{
    const char *key = gconf_entry_get_key(entry);
    if (key && g_str_has_prefix(key, IAP)) {
	IapMonitor *ptr = (IapMonitor *)user_data;
	if (gconf_entry_get_value(entry)) {
	    ptr->iapAdded(key, entry);
	} else {
	    ptr->iapDeleted(key, entry);
	}
    }
}


void IapMonitor::setup(QNetworkConfigurationManagerPrivate *d_ptr)
{
    if (first_call) {
	d = d_ptr;
	iap_prefix_len = strlen(IAP);	
	iap = new Maemo::IAPMonitor(notify_iap, (gpointer)this);
	first_call = false;
    }
}


void IapMonitor::cleanup()
{
    if (!first_call) {
	delete iap;
	timers.removeAll();
	first_call = true;
    }
}


void IapMonitor::iapAdded(const char *key, GConfEntry * /*entry*/)
{
    //qDebug("Notify called for added element: %s=%s", gconf_entry_get_key(entry), gconf_value_to_string(gconf_entry_get_value(entry)));

    /* We cannot know when the IAP is fully added to gconf, so a timer is
     * installed instead. When the timer expires we hope that IAP is added ok.
     */
    QString iap_id = QString(key + iap_prefix_len + 1).section('/',0,0);
    timers.add(iap_id, d);
}


void IapMonitor::iapDeleted(const char *key, GConfEntry * /*entry*/)
{
    //qDebug("Notify called for deleted element: %s", gconf_entry_get_key(entry));

    /* We are only interested in IAP deletions so we skip the config entries
     */
    if (strstr(key + iap_prefix_len + 1, "/")) {	
	//qDebug("Deleting IAP config %s", key+iap_prefix_len);
	return;
    }

    QString iap_id = key + iap_prefix_len + 1;
    d->deleteConfiguration(iap_id);
}



void QNetworkConfigurationManagerPrivate::registerPlatformCapabilities()
{
    capFlags |= QNetworkConfigurationManager::BearerManagement;
    capFlags |= QNetworkConfigurationManager::DataStatistics;
    capFlags |= QNetworkConfigurationManager::SystemSessionSupport;
    capFlags |= QNetworkConfigurationManager::ForcedRoaming;
}


static inline QString network_attrs_to_security(uint network_attrs)
{
    uint cap = 0;
    nwattr2cap(network_attrs, &cap); /* from libicd-network-wlan-dev.h */
    if (cap & WLANCOND_OPEN)
	return "NONE";
    else if (cap & WLANCOND_WEP)
	return "WEP";
    else if (cap & WLANCOND_WPA_PSK)
	return "WPA_PSK";
    else if (cap & WLANCOND_WPA_EAP)
	return "WPA_EAP";
    return "";
}


struct SSIDInfo {
    QString iap_id;
    QString wlan_security;
};


void QNetworkConfigurationManagerPrivate::deleteConfiguration(QString& iap_id)
{
    /* Called when IAPs are deleted in gconf, in this case we do not scan
     * or read all the IAPs from gconf because it might take too much power
     * (multiple applications would need to scan and read all IAPs from gconf)
     */
    if (accessPointConfigurations.contains(iap_id)) {
	QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(iap_id);
	if (priv) {
	    priv->isValid = false;
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "IAP" << iap_id << "was removed from storage.";
#endif

	    QNetworkConfiguration item;
	    item.d = priv;
	    emit configurationRemoved(item);
	} else
	    qWarning("Configuration not found for IAP %s", iap_id.toAscii().data());
    } else {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug("IAP: %s, already missing from the known list", iap_id.toAscii().data());
#endif
    }
}


void QNetworkConfigurationManagerPrivate::addConfiguration(QString& iap_id)
{
    if (!accessPointConfigurations.contains(iap_id)) {
	Maemo::IAPConf saved_iap(iap_id);
	QString iap_type = saved_iap.value("type").toString();
	if (!iap_type.isEmpty()) {
	    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
	    cpPriv->name = saved_iap.value("name").toString();
	    if (cpPriv->name.isEmpty())
		cpPriv->name = iap_id;
	    cpPriv->isValid = true;
	    cpPriv->id = iap_id;
	    cpPriv->iap_type = iap_type;
	    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
	    cpPriv->state = QNetworkConfiguration::Defined;

	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
	    accessPointConfigurations.insert(iap_id, ptr);

#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug("IAP: %s, name: %s, added to known list", iap_id.toAscii().data(), cpPriv->name.toAscii().data());
#endif

	    QNetworkConfiguration item;
	    item.d = ptr;
	    emit configurationAdded(item);
	} else {
	    qWarning("IAP %s does not have \"type\" field defined, skipping this IAP.", iap_id.toAscii().data());
	}
    } else {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "IAP" << iap_id << "already in gconf.";
#endif
    }
}


void QNetworkConfigurationManagerPrivate::updateConfigurations()
{
    /* Contains known network id (like ssid) from storage */
    QMultiHash<QByteArray, SSIDInfo* > knownConfigs;

    /* All the scanned access points */
    QList<Maemo::IcdScanResult> scanned;

    const QRegExp wlan = QRegExp("WLAN.*");

    /* Turn on IAP monitoring */
    iapMonitor().setup(this);

    /* We return currently configured IAPs in the first run and do the WLAN
     * scan in subsequent runs.
     */
    QList<QString> all_iaps;
    Maemo::IAPConf::getAll(all_iaps);

    foreach (QString iap_id, all_iaps) {
	QByteArray ssid;
	Maemo::IAPConf saved_ap(iap_id);
	bool is_temporary = saved_ap.value("temporary").toBool();
	if (is_temporary) {
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "IAP" << iap_id << "is temporary, skipping it.";
#endif
	    continue;
	}

	QString iap_type = saved_ap.value("type").toString();
	if (iap_type.contains(wlan)) {
	    ssid = saved_ap.value("wlan_ssid").toByteArray();
	    if (ssid.isEmpty()) {
		qWarning() << "Cannot get ssid for" << iap_id;
		continue;
	    }

	    QString security_method = saved_ap.value("wlan_security").toString();
	    SSIDInfo *info = new SSIDInfo;
	    info->iap_id = iap_id;
	    info->wlan_security = security_method;
	    knownConfigs.insert(ssid, info);
	} else if (iap_type.isEmpty()) {
	    qWarning() << "IAP" << iap_id << "network type is not set! Skipping it";
	    continue;
	} else {
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "IAP" << iap_id << "network type is" << iap_type;
#endif
	    ssid.clear();
	}

	if (!accessPointConfigurations.contains(iap_id)) {
	    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
	    //cpPriv->name = iap_info.value().toString();
	    cpPriv->name = saved_ap.value("name").toString();
	    if (cpPriv->name.isEmpty())
		if (ssid.size() > 0)
		    cpPriv->name = ssid.data();
		else
		    cpPriv->name = iap_id;
	    cpPriv->isValid = true;
	    cpPriv->id = iap_id;
	    cpPriv->network_id = ssid;
	    cpPriv->iap_type = iap_type;
	    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
	    cpPriv->state = QNetworkConfiguration::Defined;

	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
	    accessPointConfigurations.insert(iap_id, ptr);

#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug("IAP: %s, name: %s, ssid: %s, added to known list", iap_id.toAscii().data(), cpPriv->name.toAscii().data(), ssid.size() ? ssid.data() : "-");
#endif
	} else {
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug("IAP: %s, ssid: %s, already exists in the known list", iap_id.toAscii().data(), ssid.size() ? ssid.data() : "-");
#endif
	}
    }

    if (!firstUpdate) {
	QStringList scannedNetworkTypes;
	QStringList networkTypesToScan;
	QString error;
	Maemo::Icd icd(ICD_SHORT_SCAN_TIMEOUT);

	scannedNetworkTypes = icd.scan(ICD_SCAN_REQUEST_ACTIVE,
				    networkTypesToScan,
				    scanned,
				    error);
	if (!error.isEmpty()) {
	    qWarning() << "Network scanning failed.";
	} else {
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "Scan returned" << scanned.size() << "networks";
#endif
	}
    }


    /* This is skipped in the first update as scanned size is zero */
    for (int i=0; i<scanned.size(); ++i) {
	const Maemo::IcdScanResult ap = scanned.at(i); 

	if (ap.scan.network_attrs & ICD_NW_ATTR_IAPNAME) {
	    /* The network_id is IAP id, so the IAP is a known one */
	    QString iapid = ap.scan.network_id.data();
	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(iapid);
	    if (priv) {
		priv->state = QNetworkConfiguration::Discovered; /* Defined is set automagically */
		accessPointConfigurations.insert(iapid, priv);
#ifdef BEARER_MANAGEMENT_DEBUG
		qDebug("IAP: %s, ssid: %s, discovered", iapid.toAscii().data(), priv->network_id.data());
#endif

		if (!ap.scan.network_type.contains(wlan))
		    continue; // not a wlan AP

		/* Remove scanned AP from known configurations so that we can
		 * emit configurationRemoved signal later
		 */
		QList<SSIDInfo* > known_iaps = knownConfigs.values(priv->network_id);
	    rescan_list:
		for (int k=0; k<known_iaps.size(); ++k) {
		    SSIDInfo *iap = known_iaps.at(k);
		    if (iap->wlan_security == 
			network_attrs_to_security(ap.scan.network_attrs)) {
			/* Remove IAP from the list */
			knownConfigs.remove(priv->network_id, iap);
#ifdef BEARER_MANAGEMENT_DEBUG
			qDebug() << "Removed IAP" << iap->iap_id << "from unknown config";
#endif
			known_iaps.removeAt(k);
			delete iap;
			goto rescan_list;
		    }
		}
	    } else {
		qWarning() << "IAP" << iapid << "is missing!";
	    }

	} else {
	    /* Non saved access point data */
	    QByteArray scanned_ssid = ap.scan.network_id;
	    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
	    QString hrs = scanned_ssid.data();
	    cpPriv->name = ap.network_name.isEmpty() ? hrs : ap.network_name;
	    cpPriv->isValid = true;
	    cpPriv->id = scanned_ssid.data();  // Note: id is now ssid, it should be set to IAP id if the IAP is saved
	    cpPriv->network_id = scanned_ssid;
	    cpPriv->iap_type = ap.scan.network_type;
	    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
	    cpPriv->state = QNetworkConfiguration::Undefined;

	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
	    accessPointConfigurations.insert(cpPriv->id, ptr);

#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug() << "IAP with network id" << cpPriv->id << "was found in the scan.";
#endif

	    QNetworkConfiguration item;
	    item.d = ptr;
	    emit configurationAdded(item);
	}
    }


    /* Remove non existing iaps since last update */
    if (!firstUpdate) {
	QHashIterator<QByteArray, SSIDInfo* > i(knownConfigs);
	while (i.hasNext()) {
	    i.next();
	    SSIDInfo *iap = i.value();
	    QString iap_id = iap->iap_id;
	    //qDebug() << i.key() << ": " << iap_id;

	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(iap_id);
	    if (priv) {
		priv->isValid = false;
#ifdef BEARER_MANAGEMENT_DEBUG
		qDebug() << "IAP" << iap_id << "was removed as it was not found in scan.";
#endif

		QNetworkConfiguration item;
		item.d = priv;
		emit configurationRemoved(item);

		//if we would have SNAP support we would have to remove the references
		//from existing ServiceNetworks to the removed access point configuration
	    }
	}
    }


    QMutableHashIterator<QByteArray, SSIDInfo* > i(knownConfigs);
    while (i.hasNext()) {
	i.next();
	SSIDInfo *iap = i.value();
	delete iap;
	i.remove();
    }

    if (firstUpdate)
        firstUpdate = false;

    QTimer::singleShot(0, this, SIGNAL(configurationUpdateComplete()));
}


QNetworkConfiguration QNetworkConfigurationManagerPrivate::defaultConfiguration()
{
    /* Here we just create a [ANY] request to icd and let the icd decide which
     * IAP to connect.
     */
    QNetworkConfigurationPrivate *d = new QNetworkConfigurationPrivate;
    if (d) {
	d->name = OSSO_IAP_ANY;
	d->id = OSSO_IAP_ANY;
	d->state = QNetworkConfiguration::Defined;
	d->type = QNetworkConfiguration::UserChoice;
	d->roamingSupported = false;
	d->isValid = true;
    }

    QNetworkConfiguration item;
    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(d);
    item.d = ptr;
    return item;
}


void QNetworkConfigurationManagerPrivate::performAsyncConfigurationUpdate()
{
    QTimer::singleShot(0, this, SLOT(updateConfigurations()));
}


void QNetworkConfigurationManagerPrivate::cleanup()
{
    iapMonitor().cleanup();
}

#include "qnetworkconfigmanager_maemo.moc"

QT_END_NAMESPACE
