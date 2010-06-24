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

#include "tabbedwindow.h"
#include "servicestab.h"

#include <QApplication>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QTimer>
#include <QMessageBox>
#include <QNetworkProxyFactory>

#ifdef Q_OS_SYMBIAN
#include <QNetworkConfigurationManager>
#endif

TabbedWindow::TabbedWindow(QWidget *parent)
        : QMainWindow(parent), m_serviceProvider(NULL)
{

    setWindowTitle(tr("QGeoApiUI Example"));

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

    m_session = new QNetworkSession(cfg, this);
    m_session->open();
    m_session->waitForOpened(-1);
#endif

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    ServicesTab* servicesTab = new ServicesTab();

    QObject::connect(servicesTab, SIGNAL(serviceProviderChanged(QString)), this,
                     SLOT(setProvider(QString)), Qt::QueuedConnection);

    m_geocodingTab = new GeocodingTab();
    m_reverseTab = new ReverseGeocodingTab();
    m_routingTab = new RouteTab();
    m_mappingTab = new MapTileTab();

    m_tabWidget = new QTabWidget;
    m_tabWidget->addTab(servicesTab, tr("Service Providers"));
    m_tabWidget->addTab(m_routingTab, tr("Route"));
    m_tabWidget->addTab(m_geocodingTab, tr("Geocoding"));
    m_tabWidget->addTab(m_reverseTab, tr("Reverse Geocoding"));
    m_tabWidget->addTab(m_mappingTab, tr("Map Tile"));

    setCentralWidget(m_tabWidget);
    QTimer::singleShot(0, servicesTab, SLOT(initialize()));
}

TabbedWindow::~TabbedWindow()
{
    delete m_serviceProvider;
}

void TabbedWindow::setProvider(QString providerId)
{
    QMap<QString, QString> parameters;
    if (providerId == "nokia") {
        // TODO remove proxy lines ahead of release
        //parameters.insert("places.proxy", "172.16.42.137");
        //parameters.insert("places.host", "dev-a7.bln.gate5.de");
        parameters.insert("places.host", "loc.desktop.maps.svc.ovi.com");
        //parameters.insert("routing.proxy", "172.16.42.137");
//        parameters.insert("routing.host", "172.24.32.155");
        //parameters.insert("mapping.proxy", "172.16.42.40");
        parameters.insert("mapping.host", "maptile.svc.nokia.com.edgesuite.net");
    }

    if (m_serviceProvider)
        delete m_serviceProvider ;
    m_serviceProvider = new QGeoServiceProvider(providerId, parameters);
    if (m_serviceProvider->error() != QGeoServiceProvider::NoError) {
        QMessageBox::information(this, tr("QGeoApiUI Example"), tr(
                                     "Unable to find the %1 geoservices plugin.").arg(providerId));
        QTimer::singleShot(0, qApp, SLOT(quit()));
        return;
    }

    m_geocodingTab->initialize(m_serviceProvider->searchManager());
    m_reverseTab->initialize(m_serviceProvider->searchManager());
    m_routingTab->initialize(m_serviceProvider->routingManager());
    m_mappingTab->initialize(m_serviceProvider->mappingManager());
}

