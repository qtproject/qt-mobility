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

#include "mainwindow.h"

#include <QApplication>
#include <QTabWidget>
#include <QAction>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QTimer>
#include <QMessageBox>
#include <QNetworkProxyFactory>
#include <QProcessEnvironment>
#include <QSignalMapper>
#include <QUrl>
#include <qnetworkconfigmanager.h>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), m_serviceProvider(0)
{
    setWindowTitle(tr("Maps Navigator"));

    m_markers = new MarkerList();
    m_mapsWidget = new MapsWidget();
    m_searchWidget = new SearchWidget();
    m_directionsWidget = new DirectionsWidget();

    connect(m_mapsWidget, SIGNAL(addMarker(MarkerObject*)),
            m_markers, SLOT(addMarker(MarkerObject*)));

    connect(m_mapsWidget, SIGNAL(removeMarker(MarkerObject*)),
            m_markers, SLOT(removeMarker(MarkerObject*)));

    connect(m_mapsWidget, SIGNAL(markerPressed(MarkerObject*)),
            m_markers, SLOT(markerPressed(MarkerObject*)));

    connect(m_mapsWidget, SIGNAL(markerReleased()),
            m_markers, SLOT(markerReleased()));

    connect(m_mapsWidget, SIGNAL(moveMarker(QGeoCoordinate,QGeoCoordinate)),
            m_markers, SLOT(moveMarker(QGeoCoordinate,QGeoCoordinate)));

    connect(m_mapsWidget, SIGNAL(mapMoved()), m_markers, SLOT(mapMoved()));

    connect(m_markers, SIGNAL(showMarkerInfo(MarkerObject*)),
            this, SLOT(showSearchDialog(MarkerObject*)));

    connect(m_searchWidget, SIGNAL(centerOnMarker(MarkerObject*)),
            m_mapsWidget, SLOT(centerOnMarker(MarkerObject*)));

    connect(m_searchWidget, SIGNAL(deleteMarker(MarkerObject*)),
            m_mapsWidget, SLOT(deleteMarker(MarkerObject*)));

    connect(m_searchWidget, SIGNAL(addMarker(QGeoCoordinate)),
            this, SLOT(addSearchMarker(QGeoCoordinate)));

    connect(m_searchWidget, SIGNAL(addMarker(QString)),
            this, SLOT(addSearchMarker(QString)));

    connect(m_directionsWidget, SIGNAL(selectWaypoint(MarkerObject*)),
            this, SLOT(selectWaypointMarker(MarkerObject*)));

    connect(m_mapsWidget, SIGNAL(waypointSelected()),
            m_directionsWidget, SLOT(waypointSelected()));

    connect(m_searchWidget, SIGNAL(setWaypoint(MarkerObject*)),
            m_directionsWidget, SLOT(setWaypoint(MarkerObject*)));

    // Setup menus
    QAction *myLocationAction = new QAction(tr("My Location"), this);
    connect(myLocationAction, SIGNAL(triggered()), m_mapsWidget, SLOT(centerOnMyLocation()));

    QAction *directionsAction = new QAction(tr("Directions"), this);
    connect(directionsAction, SIGNAL(triggered()), m_directionsWidget, SLOT(showDirectionsDialog()));

    QAction *exitAction = new QAction(tr("Exit"), this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    QMenu *searchMenu = new QMenu(tr("Search"), this);
    QAction *geocodeAction = new QAction(tr("By Address"), this);
    QAction *revgeocodeAction = new QAction(tr("By Coordinate"), this);
    searchMenu->addAction(geocodeAction);
    searchMenu->addAction(revgeocodeAction);
    connect(geocodeAction, SIGNAL(triggered()), m_searchWidget, SLOT(showGeocodeDialog()));
    connect(revgeocodeAction, SIGNAL(triggered()), m_searchWidget, SLOT(showRevGeocodeDialog()));

    QSignalMapper *mapper = new QSignalMapper(this);
    QMenu* mapTypeMenu = new QMenu(tr("Map Type"), this);

    // TODO: actually look at available map types
    QStringList mapTypes;
    mapTypes << tr("Street") << tr("Satellite")
             << tr("Satellite - Night") << tr("Terrain");

    foreach (QString type, mapTypes) {
        QAction *mapTypeAction = new QAction(type, this);
        mapTypeMenu->addAction(mapTypeAction);
        mapper->setMapping(mapTypeAction, type);
        connect(mapTypeAction, SIGNAL(triggered()),
                mapper, SLOT(map()));
    }
    connect(mapper, SIGNAL(mapped(QString)),
            m_mapsWidget, SLOT(setMapType(QString)));

    menuBar()->addAction(myLocationAction);
    menuBar()->addAction(directionsAction);
    menuBar()->addMenu(searchMenu);
    menuBar()->addMenu(mapTypeMenu);
    menuBar()->addAction(exitAction);

    // Set Internet Access Point
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities()
                              & QNetworkConfigurationManager::CanStartAndStopInterfaces);

    // Is there default access point, use it
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QNetworkConfiguration::Active)) {
        QMessageBox::information(this, tr("Geo Service Demo"),
                                 tr("Available Access Points not found."));
        return;
    }

    m_session = new QNetworkSession(cfg, this);
    connect(m_session, SIGNAL(opened()), this, SLOT(networkSessionOpened()));
    connect(m_session, SIGNAL(error(QNetworkSession::SessionError)),
            this, SLOT(error(QNetworkSession::SessionError)));

    m_session->open();
    resize(300, 480);

    setCentralWidget(m_mapsWidget);
}

MainWindow::~MainWindow()
{
    delete m_serviceProvider;
}

void MainWindow::networkSessionOpened()
{
    QString urlEnv = QProcessEnvironment::systemEnvironment().value("http_proxy");
    if (!urlEnv.isEmpty()) {
        qDebug("got proxy from env: %s", qPrintable(urlEnv));
        QUrl url = QUrl(urlEnv, QUrl::TolerantMode);
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(url.host());
        proxy.setPort(url.port(8080));
        QNetworkProxy::setApplicationProxy(proxy);
    } else
        QNetworkProxyFactory::setUseSystemConfiguration(true);

    QTimer::singleShot(0, this, SLOT(setProvider()));
}

void MainWindow::error(QNetworkSession::SessionError error)
{
    if (error == QNetworkSession::UnknownSessionError) {
        if (m_session->state() == QNetworkSession::Connecting) {
            QMessageBox msgBox(qobject_cast<QWidget *>(parent()));
            msgBox.setText("This application requires network access to function.");
            msgBox.setInformativeText("Press Cancel to quit the application.");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Retry);
            int ret = msgBox.exec();
            if (ret == QMessageBox::Retry) {
                QTimer::singleShot(0, m_session, SLOT(open()));
            } else if (ret == QMessageBox::Cancel) {
                close();
            }
        }
    } else if (error == QNetworkSession::SessionAbortedError) {
        QMessageBox msgBox(qobject_cast<QWidget *>(parent()));
        msgBox.setText("Out of range of network.");
        msgBox.setInformativeText("Move back into range and press Retry, or press Cancel to quit the application.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Retry);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Retry) {
            QTimer::singleShot(0, m_session, SLOT(open()));
        } else if (ret == QMessageBox::Cancel) {
            close();
        }
    }
}

void MainWindow::setProvider()
{
    if (m_serviceProvider)
        delete m_serviceProvider;

    // Check for geo service providers
    QList<QString> providers = QGeoServiceProvider::availableServiceProviders();
    if (providers.size() < 1) {
        QMessageBox::information(this, tr("Maps Navigator"),
                                 tr("No service providers are available"));
        QTimer::singleShot(0, qApp, SLOT(quit()));
        return;
    }

    // Grab first provider and check for errors
    // TODO: allow provider selection at startup
    m_serviceProvider = new QGeoServiceProvider(providers[0]);
    if (m_serviceProvider->error() != QGeoServiceProvider::NoError) {
        QMessageBox::information(this, tr("Geo Service Demo"),
                                 tr("Error loading geoservice plugin: %1").arg(providers[0]));
        QTimer::singleShot(0, qApp, SLOT(quit()));
        return;
    }

    // Initialize the provider managers
    m_markers->initialize(m_serviceProvider->searchManager());
    m_mapsWidget->initialize(m_serviceProvider->mappingManager());
    m_searchWidget->initialize(m_serviceProvider->searchManager(), m_mapsWidget->getMyLocation());
    m_directionsWidget->initialize(m_serviceProvider->routingManager(),
                                   m_serviceProvider->searchManager(),
                                   m_mapsWidget);
}

void MainWindow::showSearchDialog(MarkerObject *marker)
{
    m_searchWidget->setupWidget(marker);
    m_searchWidget->exec();
}

void MainWindow::addSearchMarker(const QGeoCoordinate& coord)
{
    MarkerObject *marker = m_mapsWidget->setMarkerCoordinate(coord);
    marker->setCenter(true);
    showSearchDialog(marker);
}

void MainWindow::addSearchMarker(const QString& address)
{
    MarkerObject *marker = m_mapsWidget->setMarkerAddress(address);
    marker->setCenter(true);
    showSearchDialog(marker);
}

void MainWindow::selectWaypointMarker(MarkerObject *marker)
{
    m_mapsWidget->beginWaypointSelect(marker);
}
