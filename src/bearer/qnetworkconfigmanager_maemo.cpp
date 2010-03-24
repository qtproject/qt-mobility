/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <string.h>

#include "qnetworkconfigmanager_maemo_p.h"

#include <QDebug>
#include <QHash>

#include <wlancond.h>
#include <libicd-network-wlan-dev.h>
#include <maemo_icd.h>
#include <iapconf.h>
#include <iapmonitor.h>

QTM_BEGIN_NAMESPACE

/* The IapAddTimer is a helper class that makes sure we update
 * the configuration only after all db additions to certain
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


class IapMonitor : public Maemo::IAPMonitor
{
public:
    IapMonitor() : first_call(true) { }

    void setup(QNetworkConfigurationManagerPrivate *d);
    void cleanup();

protected:
    void iapAdded(const QString &iapId);
    void iapRemoved(const QString &iapId);

private:
    bool first_call;

    QNetworkConfigurationManagerPrivate *d;
    IapAddTimer timers;
};

Q_GLOBAL_STATIC(IapMonitor, iapMonitor);


void IapMonitor::setup(QNetworkConfigurationManagerPrivate *d_ptr)
{
    if (first_call) {
	d = d_ptr;
	first_call = false;
    }
}


void IapMonitor::cleanup()
{
    if (!first_call) {
	timers.removeAll();
	first_call = true;
    }
}


void IapMonitor::iapAdded(const QString &iap_id)
{
    /* We cannot know when the IAP is fully added to db, so a timer is
     * installed instead. When the timer expires we hope that IAP is added ok.
     */
    QString id = iap_id;
    timers.add(id, d);
}


void IapMonitor::iapRemoved(const QString &iap_id)
{
    QString id = iap_id;
    d->deleteConfiguration(id);
}

void QNetworkConfigurationManagerPrivate::registerPlatformCapabilities()
{
    capFlags |= QNetworkConfigurationManager::CanStartAndStopInterfaces;
    capFlags |= QNetworkConfigurationManager::DataStatistics;
    capFlags |= QNetworkConfigurationManager::ForcedRoaming;
    capFlags |= QNetworkConfigurationManager::NetworkSessionRequired;
}

void QNetworkConfigurationManagerPrivate::init()
{
    // Setup DBus Interface for ICD
    m_dbusInterface = new QDBusInterface(ICD_DBUS_API_INTERFACE,
                                         ICD_DBUS_API_PATH,
                                         ICD_DBUS_API_INTERFACE,
                                         QDBusConnection::systemBus(),
                                         this);
    connect(&m_scanTimer, SIGNAL(timeout()), this, SLOT(finishAsyncConfigurationUpdate()));
    m_scanTimer.setSingleShot(true);

    /* Turn on IAP state monitoring */
    startListeningStateSignalsForAllConnections();

    /* Turn on IAP add/remove monitoring */
    iapMonitor()->setup(this);

    /* We create a default configuration which is a pseudo config */
    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
    cpPriv->name = "UserChoice";
    cpPriv->state = QNetworkConfiguration::Discovered;
    cpPriv->isValid = true;
    cpPriv->id = OSSO_IAP_ANY;
    cpPriv->type = QNetworkConfiguration::UserChoice;
    cpPriv->purpose = QNetworkConfiguration::UnknownPurpose;
    cpPriv->roamingSupported = false;
    cpPriv->manager = this;
    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
    userChoiceConfigurations.insert(cpPriv->id, ptr);
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


void QNetworkConfigurationManagerPrivate::configurationChanged(QNetworkConfigurationPrivate *ptr)
{
    QNetworkConfiguration item;
    item.d = ptr;
    emit configurationChanged(item);
}

void QNetworkConfigurationManagerPrivate::deleteConfiguration(QString& iap_id)
{
    /* Called when IAPs are deleted in db, in this case we do not scan
     * or read all the IAPs from db because it might take too much power
     * (multiple applications would need to scan and read all IAPs from db)
     */
    if (accessPointConfigurations.contains(iap_id)) {
	QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(iap_id);
	if (priv.data()) {
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


uint32_t QNetworkConfigurationManagerPrivate::getNetworkAttrs(bool is_iap_id,
                                const QString& iap_id,
							    QString& iap_type,
							    QString security_method)
{
    guint network_attr = 0;
    dbus_uint32_t cap = 0;

    if (iap_type == "WLAN_INFRA")
	cap |= WLANCOND_INFRA;
    else if (iap_type == "WLAN_ADHOC")
	cap |= WLANCOND_ADHOC;

    if (security_method.isEmpty() && (cap & (WLANCOND_INFRA | WLANCOND_ADHOC))) {
	Maemo::IAPConf saved_ap(iap_id);
	security_method = saved_ap.value("wlan_security").toString();
    }

    if (!security_method.isEmpty()) {
	if (security_method == "WEP")
	    cap |= WLANCOND_WEP;
	else if (security_method == "WPA_PSK")
	    cap |= WLANCOND_WPA_PSK;
	else if (security_method == "WPA_EAP")
	    cap |= WLANCOND_WPA_EAP;
	else if (security_method == "NONE")
	    cap |= WLANCOND_OPEN;

	if (cap & (WLANCOND_WPA_PSK | WLANCOND_WPA_EAP)) {
	    Maemo::IAPConf saved_iap(iap_id);
	    bool wpa2_only = saved_iap.value("EAP_wpa2_only_mode").toBool();
	    if (wpa2_only) {
		cap |= WLANCOND_WPA2;
	    }
	}
    }

    cap2nwattr(cap, &network_attr);
    if (is_iap_id)
	network_attr |= ICD_NW_ATTR_IAPNAME;

    return (uint32_t)network_attr;
}


void QNetworkConfigurationManagerPrivate::addConfiguration(QString& iap_id)
{
    // Note: When new IAP is created, this function gets called multiple times
    //       in a row.
    //       For example: Empty type & name for WLAN was stored into newly
    //                    created IAP data in gconf when this function gets
    //                    called for the first time.
    //                    WLAN type & name are updated into IAP data in gconf
    //                    as soon as WLAN connection is up and running.
    //                    => And this function gets called again.

    if (!accessPointConfigurations.contains(iap_id)) {
	Maemo::IAPConf saved_iap(iap_id);
        QString iap_type = saved_iap.value("type").toString();
        QString iap_name = saved_iap.value("name").toString();
        QByteArray ssid = saved_iap.value("wlan_ssid").toByteArray();
        if (!iap_type.isEmpty() && !iap_name.isEmpty()) {
            // Check if new IAP is actually Undefined WLAN configuration
            // Note: SSID is used as an iap id for Undefined WLAN configurations
            //       => configuration must be searched using SSID
            if (!ssid.isEmpty() && accessPointConfigurations.contains(ssid)) {
                QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr = accessPointConfigurations.take(ssid);
                if (ptr.data()) {
                    QString iap_type = saved_iap.value("type").toString();
                    ptr->id = iap_id;
                    ptr->iap_type = iap_type;
                    ptr->network_attrs = getNetworkAttrs(true, iap_id, iap_type, QString());
                    ptr->network_id = ssid;
                    ptr->service_id = saved_iap.value("service_id").toString();
                    ptr->service_type = saved_iap.value("service_type").toString();
                    if (m_onlineIapId == iap_id) {
                        ptr->state = QNetworkConfiguration::Active;
                    } else {
                        ptr->state = QNetworkConfiguration::Defined;
                    }
                    accessPointConfigurations.insert(iap_id, ptr);
                    configurationChanged(ptr.data());
                }
            } else {
                QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
                cpPriv->name = saved_iap.value("name").toString();
                if (cpPriv->name.isEmpty())
                    cpPriv->name = iap_id;
                cpPriv->isValid = true;
                cpPriv->id = iap_id;
                cpPriv->iap_type = iap_type;
                cpPriv->network_attrs = getNetworkAttrs(true, iap_id, iap_type, QString());
                cpPriv->service_id = saved_iap.value("service_id").toString();
                cpPriv->service_type = saved_iap.value("service_type").toString();
                if (iap_type.startsWith("WLAN")) {
                    QByteArray ssid = saved_iap.value("wlan_ssid").toByteArray();
                    if (ssid.isEmpty()) {
                        qWarning() << "Cannot get ssid for" << iap_id;
                    }
                    cpPriv->network_id = ssid;
                }
                cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
                if (m_onlineIapId == iap_id) {
                    cpPriv->state = QNetworkConfiguration::Active;
                } else {
                    cpPriv->state = QNetworkConfiguration::Defined;
                }
                cpPriv->manager = this;

                QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
                accessPointConfigurations.insert(iap_id, ptr);

#ifdef BEARER_MANAGEMENT_DEBUG
                qDebug("IAP: %s, name: %s, added to known list", iap_id.toAscii().data(), cpPriv->name.toAscii().data());
#endif
                QNetworkConfiguration item;
                item.d = ptr;
                emit configurationAdded(item);
            }
        } else {
            qWarning("IAP %s does not have \"type\" or \"name\" fields defined, skipping this IAP.", iap_id.toAscii().data());
        }
    } else {
#ifdef BEARER_MANAGEMENT_DEBUG
	qDebug() << "IAP" << iap_id << "already in db.";
#endif

	/* Check if the data in db changed and update configuration accordingly
	 */
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr = accessPointConfigurations.value(iap_id);
	if (ptr.data()) {
	    Maemo::IAPConf changed_iap(iap_id);
	    QString iap_type = changed_iap.value("type").toString();
	    bool update_needed = false; /* if IAP type or ssid changed, we need to change the state */

	    ptr->network_attrs = getNetworkAttrs(true, iap_id, iap_type, QString());
	    ptr->service_id = changed_iap.value("service_id").toString();
	    ptr->service_type = changed_iap.value("service_type").toString();

	    if (!iap_type.isEmpty()) {
		ptr->name = changed_iap.value("name").toString();
		if (ptr->name.isEmpty())
		    ptr->name = iap_id;
		ptr->isValid = true;
		if (ptr->iap_type != iap_type) {
		    ptr->iap_type = iap_type;
		    update_needed = true;
		}
        if (iap_type.startsWith(QLatin1String("WLAN"))) {
		    QByteArray ssid = changed_iap.value("wlan_ssid").toByteArray();
		    if (ssid.isEmpty()) {
			qWarning() << "Cannot get ssid for" << iap_id;
		    }
		    if (ptr->network_id != ssid) {
                        ptr->network_id = ssid;
                        update_needed = true;
		    }
		}
	    }
	    if (update_needed) {
                ptr->type = QNetworkConfiguration::InternetAccessPoint;
                if (m_onlineIapId == iap_id) {
                    if (ptr->state < QNetworkConfiguration::Active) {
                        ptr->state = QNetworkConfiguration::Active;
                        configurationChanged(ptr.data());
                    }
                } else if (ptr->state < QNetworkConfiguration::Defined) {
		    ptr->state = QNetworkConfiguration::Defined;
                    configurationChanged(ptr.data());
                }
	    }
	} else {
	    qWarning("Cannot find IAP %s from current configuration although it should be there.", iap_id.toAscii().data());
	}
    }
}

void QNetworkConfigurationManagerPrivate::updateConfigurations()
{
    doUpdateConfigurations();
}

void QNetworkConfigurationManagerPrivate::doUpdateConfigurations(QList<Maemo::IcdScanResult> scanned)
{
    /* Contains all known iap_ids from storage */
    QList<QString> knownConfigs = accessPointConfigurations.keys();

    /* Contains all known WLAN network ids (like ssid) from storage */
    QMultiHash<QByteArray, SSIDInfo* > notDiscoveredWLANConfigs;

    QList<QString> all_iaps;
    Maemo::IAPConf::getAll(all_iaps);

    foreach (const QString &iap_id, all_iaps) {
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
    if (iap_type.startsWith(QLatin1String("WLAN"))) {
	    ssid = saved_ap.value("wlan_ssid").toByteArray();
	    if (ssid.isEmpty()) {
		qWarning() << "Cannot get ssid for" << iap_id;
		continue;
	    }

	    QString security_method = saved_ap.value("wlan_security").toString();
	    SSIDInfo *info = new SSIDInfo;
	    info->iap_id = iap_id;
	    info->wlan_security = security_method;
            notDiscoveredWLANConfigs.insert(ssid, info);
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
	    cpPriv->name = saved_ap.value("name").toString();
	    if (cpPriv->name.isEmpty()) {
		if (!ssid.isEmpty() && ssid.size() > 0)
		    cpPriv->name = ssid.data();
		else
		    cpPriv->name = iap_id;
	    }
            cpPriv->isValid = true;
	    cpPriv->id = iap_id;
	    cpPriv->network_id = ssid;
	    cpPriv->network_attrs = getNetworkAttrs(true, iap_id, iap_type, QString());
	    cpPriv->iap_type = iap_type;
	    cpPriv->service_id = saved_ap.value("service_id").toString();
	    cpPriv->service_type = saved_ap.value("service_type").toString();
	    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
	    cpPriv->state = QNetworkConfiguration::Defined;
	    cpPriv->manager = this;

	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
	    accessPointConfigurations.insert(iap_id, ptr);

#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug("IAP: %s, name: %s, ssid: %s, added to known list", iap_id.toAscii().data(), cpPriv->name.toAscii().data(), !ssid.isEmpty() ? ssid.data() : "-");
#endif
	} else {
            knownConfigs.removeOne(iap_id);
#ifdef BEARER_MANAGEMENT_DEBUG
	    qDebug("IAP: %s, ssid: %s, already exists in the known list", iap_id.toAscii().data(), !ssid.isEmpty() ? ssid.data() : "-");
#endif
	}
    }

    /* This is skipped in the first update as scanned size is zero */
    if (!scanned.isEmpty())
      for (int i=0; i<scanned.size(); ++i) {
	const Maemo::IcdScanResult ap = scanned.at(i); 

	if (ap.scan.network_attrs & ICD_NW_ATTR_IAPNAME) {
	    /* The network_id is IAP id, so the IAP is a known one */
	    QString iapid = ap.scan.network_id.data();
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iapid);
	    if (priv.data()) {
                bool stateChanged = false;
                // Check if state is not already Discovered or Active
                if (priv->state < QNetworkConfiguration::Discovered) {
                    priv->state = QNetworkConfiguration::Discovered; /* Defined is set automagically */
                    stateChanged = true;
                }
		priv->network_attrs = ap.scan.network_attrs;
		priv->service_id = ap.scan.service_id;
		priv->service_type = ap.scan.service_type;
		priv->service_attrs = ap.scan.service_attrs;

                if (stateChanged) {
                    configurationChanged(priv.data());
                }
#ifdef BEARER_MANAGEMENT_DEBUG
		qDebug("IAP: %s, ssid: %s, discovered", iapid.toAscii().data(), priv->network_id.data());
#endif

        if (!ap.scan.network_type.startsWith(QLatin1String("WLAN")))
		    continue; // not a wlan AP

                /* Remove scanned AP from discovered WLAN configurations so that we can
		 * emit configurationRemoved signal later
		 */
                QList<SSIDInfo* > known_iaps = notDiscoveredWLANConfigs.values(priv->network_id);
	    rescan_list:
		if (!known_iaps.isEmpty()) {
		    for (int k=0; k<known_iaps.size(); ++k) {
			SSIDInfo *iap = known_iaps.at(k);

#ifdef BEARER_MANAGEMENT_DEBUG
			//qDebug() << "iap" << iap->iap_id << "security" << iap->wlan_security << "scan" << network_attrs_to_security(ap.scan.network_attrs);
#endif

			if (iap->wlan_security == 
			    network_attrs_to_security(ap.scan.network_attrs)) {
			    /* Remove IAP from the list */
                            notDiscoveredWLANConfigs.remove(priv->network_id, iap);
#ifdef BEARER_MANAGEMENT_DEBUG
			    qDebug() << "Removed IAP" << iap->iap_id << "from unknown config";
#endif
			    known_iaps.removeAt(k);
			    delete iap;
			    goto rescan_list;
			}
		    }
		}
	    } else {
		qWarning() << "IAP" << iapid << "is missing in configuration.";
	    }

	} else {
            /* Non saved access point data */
	    QByteArray scanned_ssid = ap.scan.network_id;
            if (!accessPointConfigurations.contains(scanned_ssid)) {
                QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
                QString hrs = scanned_ssid.data();

                cpPriv->name = ap.network_name.isEmpty() ? hrs : ap.network_name;
                cpPriv->isValid = true;
                cpPriv->id = scanned_ssid.data();  // Note: id is now ssid, it should be set to IAP id if the IAP is saved
                cpPriv->network_id = scanned_ssid;
                cpPriv->iap_type = ap.scan.network_type;
                cpPriv->network_attrs = ap.scan.network_attrs;
                cpPriv->service_id = ap.scan.service_id;
                cpPriv->service_type = ap.scan.service_type;
                cpPriv->service_attrs = ap.scan.service_attrs;
                cpPriv->manager = this;

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
            } else {
                knownConfigs.removeOne(scanned_ssid);
            }
        }
    }

    if (!firstUpdate) {
        // Update Defined status to all defined WLAN IAPs which
        // could not be found when access points were scanned
        QHashIterator<QByteArray, SSIDInfo* > i(notDiscoveredWLANConfigs);
	while (i.hasNext()) {
	    i.next();
	    SSIDInfo *iap = i.value();
	    QString iap_id = iap->iap_id;
	    //qDebug() << i.key() << ": " << iap_id;

            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iap_id);
	    if (priv.data()) {
                // WLAN AccessPoint configuration could not be Discovered
                // => Make sure that configuration state is Defined
                if (priv->state > QNetworkConfiguration::Defined) {
                    priv->state = QNetworkConfiguration::Defined;
                    configurationChanged(priv.data());
                }
	    }
	}

        /* Remove non existing iaps since last update */
        foreach (QString oldIface, knownConfigs) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(oldIface);
            if (priv.data()) {
                priv->isValid = false;
                QNetworkConfiguration item;
                item.d = priv;
                emit configurationRemoved(item);
		//if we would have SNAP support we would have to remove the references
		//from existing ServiceNetworks to the removed access point configuration
            }
        }
    }

    QMutableHashIterator<QByteArray, SSIDInfo* > i(notDiscoveredWLANConfigs);
    while (i.hasNext()) {
	i.next();
	SSIDInfo *iap = i.value();
	delete iap;
	i.remove();
    }

    if (!firstUpdate)
	emit configurationUpdateComplete();

    if (firstUpdate)
        firstUpdate = false;
}

QNetworkConfiguration QNetworkConfigurationManagerPrivate::defaultConfiguration()
{
    /* Here we just return [ANY] request to icd and let the icd decide which
     * IAP to connect.
     */
    QNetworkConfiguration item;
    if (userChoiceConfigurations.contains(OSSO_IAP_ANY))
        item.d = userChoiceConfigurations.value(OSSO_IAP_ANY);
    return item;
}

void QNetworkConfigurationManagerPrivate::startListeningStateSignalsForAllConnections()
{
    // Start listening ICD_DBUS_API_STATE_SIG signals
    m_dbusInterface->connection().connect(ICD_DBUS_API_INTERFACE,
                                          ICD_DBUS_API_PATH,
                                          ICD_DBUS_API_INTERFACE,
                                          ICD_DBUS_API_STATE_SIG,
                                          this, SLOT(connectionStateSignalsSlot(QDBusMessage)));

    // Calling ICD_DBUS_API_STATE_REQ makes sure that initial state will be updated immediately
    m_gettingInitialConnectionState = true;
    m_dbusInterface->call(ICD_DBUS_API_STATE_REQ);
}

void QNetworkConfigurationManagerPrivate::connectionStateSignalsSlot(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    if (arguments[1].toUInt() != 0 || arguments.count() < 8) {
        return;
    }

    QString iapid = arguments[5].toByteArray().data();
    uint icd_connection_state = arguments[7].toUInt();

    switch (icd_connection_state) {
    case ICD_STATE_CONNECTED:
        {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr = accessPointConfigurations.value(iapid);
        if (ptr.data()) {
            ptr->type = QNetworkConfiguration::InternetAccessPoint;
            if (ptr->state != QNetworkConfiguration::Active) {
                ptr->state = QNetworkConfiguration::Active;
                if (!m_gettingInitialConnectionState) {
                    configurationChanged(ptr.data());
                    if (m_onlineIapId.isEmpty()) {
                        emit onlineStateChanged(true);
                    }
                }
                m_onlineIapId = iapid;
            }
        } else {
            // This gets called when new WLAN IAP is created using Connection dialog
            // At this point Undefined WLAN configuration has SSID as iap id
            // => Because of that configuration can not be found from
            //    accessPointConfigurations using correct iap id
            emit onlineStateChanged(true);
            m_onlineIapId = iapid;
        }
        break;
        }
    case ICD_STATE_DISCONNECTED:
        {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr = accessPointConfigurations.value(iapid);
        if (ptr.data()) {
            ptr->type = QNetworkConfiguration::InternetAccessPoint;
            if (ptr->state == QNetworkConfiguration::Active) {
                ptr->state = QNetworkConfiguration::Discovered;
                if (!m_gettingInitialConnectionState) {
                    configurationChanged(ptr.data());

                    // Note: If ICD switches used IAP from one to another:
                    //       1) new IAP is reported to be online first
                    //       2) old IAP is reported to be offline then
                    // => Device can be reported to be offline only
                    //    if last known online IAP is reported to be disconnected
                    if (iapid == m_onlineIapId) {
                        // It's known that there is only one global ICD connection
                        // => Because ICD state was reported to be DISCONNECTED, Device is offline
                        m_onlineIapId = QString();
                        emit onlineStateChanged(false);
                    }
                }
            }
        } else {
            // Disconnected IAP was not found from accessPointConfigurations
            // => Reason: Online IAP was removed which resulted ICD to disconnect
            if (iapid == m_onlineIapId) {
                // It's known that there is only one global ICD connection
                // => Because ICD state was reported to be DISCONNECTED, Device is offline
                m_onlineIapId = QString();
                emit onlineStateChanged(false);
            }
        }
        break;
        }
    default:
        break;
    }
    
    emit iapStateChanged(iapid, icd_connection_state);

    m_gettingInitialConnectionState = false;
}

void QNetworkConfigurationManagerPrivate::performAsyncConfigurationUpdate()
{
    if (m_scanGoingOn) {
        return;
    }
    m_scanGoingOn = true;

    m_dbusInterface->connection().connect(ICD_DBUS_API_INTERFACE,
                                          ICD_DBUS_API_PATH,
                                          ICD_DBUS_API_INTERFACE,
                                          ICD_DBUS_API_SCAN_SIG,
                                          this, SLOT(asyncUpdateConfigurationsSlot(QDBusMessage)));

    QDBusMessage msg = m_dbusInterface->call(ICD_DBUS_API_SCAN_REQ,
                                             (uint)ICD_SCAN_REQUEST_ACTIVE);
    m_typesToBeScanned = msg.arguments()[0].value<QStringList>();
    m_scanTimer.start(ICD_SHORT_SCAN_TIMEOUT);
}

void QNetworkConfigurationManagerPrivate::cancelAsyncConfigurationUpdate()
{
    if (!m_scanGoingOn) {
        return;
    }
    m_scanGoingOn = false;

    if (m_scanTimer.isActive()) {
        m_scanTimer.stop();
    }

    m_dbusInterface->connection().disconnect(ICD_DBUS_API_INTERFACE,
                                             ICD_DBUS_API_PATH,
                                             ICD_DBUS_API_INTERFACE,
                                             ICD_DBUS_API_SCAN_SIG,
                                             this, SLOT(asyncUpdateConfigurationsSlot(QDBusMessage)));

    // Stop scanning rounds by calling ICD_DBUS_API_SCAN_CANCEL
    // <=> If ICD_DBUS_API_SCAN_CANCEL is not called, new scanning round will
    //     be started after the module scan timeout.
    m_dbusInterface->call(ICD_DBUS_API_SCAN_CANCEL);
}

void QNetworkConfigurationManagerPrivate::finishAsyncConfigurationUpdate()
{
    cancelAsyncConfigurationUpdate();
    doUpdateConfigurations(m_scanResult);
    m_scanResult.clear();
}

void QNetworkConfigurationManagerPrivate::asyncUpdateConfigurationsSlot(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    uint icd_scan_status = arguments.takeFirst().toUInt();
    if (icd_scan_status == ICD_SCAN_COMPLETE) {
        m_typesToBeScanned.removeOne(arguments[6].toString());
        if (!m_typesToBeScanned.count()) {
            finishAsyncConfigurationUpdate();
        }
    } else {
        Maemo::IcdScanResult scanResult;
        scanResult.status = icd_scan_status;
        scanResult.timestamp = arguments.takeFirst().toUInt();
        scanResult.scan.service_type = arguments.takeFirst().toString();
        scanResult.service_name = arguments.takeFirst().toString();
        scanResult.scan.service_attrs = arguments.takeFirst().toUInt();
        scanResult.scan.service_id = arguments.takeFirst().toString();
        scanResult.service_priority = arguments.takeFirst().toInt();
        scanResult.scan.network_type = arguments.takeFirst().toString();
        scanResult.network_name = arguments.takeFirst().toString();
        scanResult.scan.network_attrs = arguments.takeFirst().toUInt();
        scanResult.scan.network_id = arguments.takeFirst().toByteArray();
        scanResult.network_priority = arguments.takeFirst().toInt();
        scanResult.signal_strength = arguments.takeFirst().toInt();
        scanResult.station_id = arguments.takeFirst().toString();
        scanResult.signal_dB = arguments.takeFirst().toInt();

        m_scanResult.append(scanResult);
    }
}

void QNetworkConfigurationManagerPrivate::cleanup()
{
    if (m_scanGoingOn) {
        m_scanTimer.stop();
        m_dbusInterface->call(ICD_DBUS_API_SCAN_CANCEL);
    }
    iapMonitor()->cleanup();
}

#include "qnetworkconfigmanager_maemo.moc"
#include "moc_qnetworkconfigmanager_maemo_p.cpp"

QTM_END_NAMESPACE
