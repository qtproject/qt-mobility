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

#include <duivaluespace.h>
#include <maemo_icd.h>

#define CONF_IAP "system.osso.connectivity.IAP"

QT_BEGIN_NAMESPACE

void QNetworkConfigurationManagerPrivate::registerPlatformCapabilities()
{
    capFlags |= QNetworkConfigurationManager::BearerManagement;
    capFlags |= QNetworkConfigurationManager::DataStatistics;
    capFlags |= QNetworkConfigurationManager::SystemSessionSupport;
    capFlags |= QNetworkConfigurationManager::ForcedRoaming;
}


void QNetworkConfigurationManagerPrivate::updateConfigurations()
{
    QHash<QString, bool> knownConfigs; /* Contains known network id (like ssid) */
    QList<Maemo::IcdScanResult> scanned;
    QHash<QString, QString> network_id_and_iap_id_mapping;

    /* We return currently configured IAPs in the first run and
     * do the WLAN scan in subsequent runs.
     */
    DuiConfItem configured_iaps(CONF_IAP);
    QList<QString> all_iaps = configured_iaps.listDirs();

    foreach (QString iap_path, all_iaps) {
	const bool found = true;

	QString str = iap_path.section('.', 4);
	DuiConfItem ap_id(iap_path + ".wlan_ssid"); // TODO: fix GPRS case as it has no ssid
	QString ssid = ap_id.value().toByteArray().data();
	knownConfigs.insert(ssid, found);

	if (!accessPointConfigurations.contains(str)) {
	    DuiConfItem iap_info(iap_path + ".name");
	    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
	    cpPriv->name = iap_info.value().toString();
	    if (cpPriv->name.isEmpty())
		cpPriv->name = ssid;
	    cpPriv->isValid = true;
	    cpPriv->id = str;
	    cpPriv->network_id = ssid;
	    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
	    cpPriv->state = QNetworkConfiguration::Defined;

	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
	    accessPointConfigurations.insert(str, ptr);

	    qDebug() << "IAP" << str << cpPriv->name << ssid << "added to known list.";
	} else {
	    qDebug() << "IAP" << str << ssid << "already exists in the known list.";
	}
    }

#if 0
    foreach (QString iap, knownConfigs) {
	qDebug() << "Value" << iap;
    }
#endif

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

    /* This is skipped in the first update */
    if (scanned.size()) {
	/* In order to speed up things within next for loop, we create a separate hash
	 * that maps network id and IAP id.
	 */
	// TODO: network type needs to be also in the hash
	network_id_and_iap_id_mapping.clear();
	QHashIterator<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > i(accessPointConfigurations);
	while (i.hasNext()) {
	    i.next();
	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = i.value();
	    qDebug() << "Mapping:" << priv->network_id << "-->" << priv->id;
	    network_id_and_iap_id_mapping.insert(priv->network_id, priv->id);
	}
    }

    /* This is skipped in the first update */
    for (int i=0; i<scanned.size(); ++i) {
	const Maemo::IcdScanResult &ap = scanned.at(i); 
	QString ap_id = ap.scan.network_id.data();
	QString iap_id = network_id_and_iap_id_mapping.value(ap_id);

	/* Remove scanned AP from known configurations so that we can
	 * emit configurationRemoved signal later
	 */
	if (!iap_id.isEmpty() && knownConfigs.contains(iap_id)) {
	    qDebug() << "IAP" << iap_id << ap_id << "already known.";
	    knownConfigs.remove(iap_id);
	}

	/* Be very carefull with the IAP id and network id concept!
	 * Now next block mixes these two things (because we do not know
	 * the IAP id).
	 */
	if (iap_id.isEmpty()) {
	    // The IAP is not found in permanent storage so accessPointConfigurations
	    // will not contain the IAP id
	    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
	    QString hrs = ap_id;
	    cpPriv->name = ap.network_name.isEmpty() ? hrs : ap.network_name;
	    cpPriv->isValid = true;
	    cpPriv->id = ap_id;  // Note: id is now ssid, it must be set to IAP id if the IAP is saved
	    cpPriv->network_id = ap_id;
	    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
	    cpPriv->state = QNetworkConfiguration::Undefined;

	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
	    accessPointConfigurations.insert(ap_id, ptr);

	    qDebug() << "IAP with network id" << ap_id << "was found in the scan.";

	    QNetworkConfiguration item;
	    item.d = ptr;
	    emit configurationAdded(item);

	} else {
	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(iap_id);
	    if (priv) {
		priv->state = QNetworkConfiguration::Discovered; /* Defined is set automagically */
		accessPointConfigurations.insert(iap_id, priv);
		qDebug() << "IAP" << iap_id << ap_id << "discovered.";
	    } else {
		qWarning() << "IAP is missing!!!";
	    }
	}
    }

    network_id_and_iap_id_mapping.clear();

    /* Remove non existing iaps since last update */
    if (!firstUpdate) {
	QHashIterator<QString, bool> i(knownConfigs);
	while (i.hasNext()) {
	    i.next();
	    //qDebug() << i.key() << ": " << i.value();

	    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(i.key());
	    if (priv) {
		priv->isValid = false;

		qDebug() << "IAP" << i.key() << "was removed as it was not found in scan.";

		QNetworkConfiguration item;
		item.d = priv;
		emit configurationRemoved(item);

		//if we would have SNAP support we would have to remove the references
		//from existing ServiceNetworks to the removed access point configuration
	    }
	}
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
