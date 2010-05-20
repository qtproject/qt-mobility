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

#include <QTabWidget>
#include <QVBoxLayout>
#include "tabbeddialog.h"
#include "routetab.h"
#include "geocodingtab.h"
#include "revgeocodingtab.h"
//#include "maptiletab.h"

#include <QApplication>
#include <QTimer>
#include <QMessageBox>

#ifdef Q_OS_SYMBIAN
#include <qnetworkconfigmanager.h>
#endif

TabbedDialog::TabbedDialog(QWidget *parent)
    : QDialog(parent)
{
#ifdef Q_OS_SYMBIAN
    // Set Internet Access Point
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities()
                              & QNetworkConfigurationManager::CanStartAndStopInterfaces);
    // Is there default access point, use it
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QNetworkConfiguration::Active)) {
        QMessageBox::information(this, tr("QGeoApiUI Example"), tr(
                                     "Available Access Points not found."));
        return;
    }

    session = new QNetworkSession(cfg, this);
    session->open();
    session->waitForOpened(-1);
#endif
    serviceProvider = new QGeoServiceProvider("nokia");
    if (serviceProvider->error() != QGeoServiceProvider::NoError) {
        QMessageBox::information(this, tr("QGeoApiUI Example"), tr(
                                     "Unable to find the nokia geoservices plugin."));
        QTimer::singleShot(0, qApp, SLOT(quit()));
        return;
    }

    tabWidget = new QTabWidget;
    tabWidget->addTab(new RouteTab(serviceProvider->routingManager()), tr("Route"));
    tabWidget->addTab(new GeocodingTab(serviceProvider->placesManager()), tr("Geocoding"));
    tabWidget->addTab(new ReverseGeocodingTab(serviceProvider->placesManager()), tr("Reverse Geocoding"));
    //tabWidget->addTab(new MapTileTab(), tr("Map Tile"));
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
    setWindowTitle(tr("QGeoApiUI Example"));    
}

TabbedDialog::~TabbedDialog()
{
    delete serviceProvider;
}

