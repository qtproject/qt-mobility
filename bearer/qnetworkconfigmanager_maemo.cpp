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

#include "qnetworkconfigmanager_p.h"

#include <QDebug>
#include <QtDBus>
#include <QRegExp>

#include <wlancond.h>
#include <libicd-network-wlan-dev.h>
#include <maemo_icd.h>
#include <iapconf.h>


QT_BEGIN_NAMESPACE

void QNetworkConfigurationManagerPrivate::registerPlatformCapabilities()
{
    capFlags |= QNetworkConfigurationManager::BearerManagement;
    capFlags |= QNetworkConfigurationManager::DataStatistics;
    capFlags |= QNetworkConfigurationManager::SystemSessionSupport;
    capFlags |= QNetworkConfigurationManager::ForcedRoaming;
}


static inline QString network_attrs_to_security(uint network_attrs)
{
    uint cap;
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

void QNetworkConfigurationManagerPrivate::updateConfigurations()
{
    /* Contains known network id (like ssid) from storage */
    QMultiHash<QByteArray, SSIDInfo* > knownConfigs;

    /* All the scanned access points */
    QList<Maemo::IcdScanResult> scanned;

    const QRegExp wlan = QRegExp("WLAN.*");

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
	    qDebug() << "IAP" << iap_id << "is temporary, skipping it.";
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
	    qDebug() << "IAP" << iap_id << "network type is" << iap_type;
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

	    qDebug("IAP: %s, name: %s, ssid: %s, added to known list", iap_id.toAscii().data(), cpPriv->name.toAscii().data(), ssid.size() ? ssid.data() : "-");
	} else {
	    qDebug("IAP: %s, ssid: %s, already exists in the known list", iap_id.toAscii().data(), ssid.size() ? ssid.data() : "-");
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
	    qDebug() << "Scan returned" << scanned.size() << "networks";
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
		qDebug("IAP: %s, ssid: %s, discovered", iapid.toAscii().data(), priv->network_id.data());

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
			qDebug() << "Removed IAP" << iap->iap_id << "from known config";
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

	    qDebug() << "IAP with network id" << cpPriv->id << "was found in the scan.";

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
		qDebug() << "IAP" << iap_id << "was removed as it was not found in scan.";

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
     * AP to connect.
     */
    QNetworkConfigurationPrivate *d = new QNetworkConfigurationPrivate; // TODO: where is this freed?
    if (d) {
	d->name = OSSO_IAP_ANY;
	d->id = OSSO_IAP_ANY;
	d->state = QNetworkConfiguration::Defined;
	d->type = QNetworkConfiguration::UserChoice;
	d->roamingSupported = false;
	d->isValid = true;
    }

    QNetworkConfiguration item;
    item.d = d;
    return item;
}


void QNetworkConfigurationManagerPrivate::performAsyncConfigurationUpdate()
{
    QTimer::singleShot(0, this, SLOT(updateConfigurations()));
}

QT_END_NAMESPACE
