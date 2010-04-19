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
#include "ui_mainwindow.h"

#include "routepresenter.h"
#include "placepresenter.h"

#include "qgeoroutingservice_nokia_p.h"
#include "qmaptileservice_nokia_p.h"

#include <QTimer>
#ifdef Q_OS_SYMBIAN
#include <QMessageBox>
#include <qnetworksession.h>
#include <qnetworkconfigmanager.h>
#endif

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    geocodingService = new QGeocodingServiceNokia();
    geocodingService->setProxy( QNetworkProxy(QNetworkProxy::HttpProxy, "172.16.42.137", 8080) );
    geocodingService->setHost("loc.desktop.maps.svc.ovi.com");
    routingService = new QGeoRoutingServiceNokia();
    QMapTileServiceNokia *mService = new QMapTileServiceNokia();

    QNetworkProxy proxy(QNetworkProxy::HttpProxy, "172.16.42.40", 8080);
    mService->setProxy(proxy);
    mService->setHost("origin.maptile.svc.tst.s2g.gate5.de");

    mapService = mService;


    QObject::connect(routingService, SIGNAL(finished(QGeoRouteReply*)),
                     this, SLOT(routeReplyFinished(QGeoRouteReply*)));

    QObject::connect(geocodingService, SIGNAL(finished(QGeocodingReply*)),
                     this, SLOT(codingReplyFinished(QGeocodingReply*)));

    QObject::connect(mapService, SIGNAL(finished(QMapTileReply*)),
                     this, SLOT(mapTileReplyFinished(QMapTileReply*)));

    ui->mapTileLabel->setVisible(false);

#ifdef QGEOAPIUI_USEPOPUPMENU
    showRouteRequestControls(true);
#endif

    QStringList labels;
    labels << "Elements" << "Value";
    ui->treeWidget->setHeaderLabels(labels);

    QTimer::singleShot(0, this, SLOT(delayedInit()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete geocodingService;
    delete routingService;
    delete mapService;
}

void MainWindow::delayedInit()
{
    ui->srcLat->setText("50");
    ui->srcLong->setText("10");
    ui->dstLat->setText("48");
    ui->dstLong->setText("1");

    ui->locLong->setText("13.377");
    ui->locLat->setText("52.51");

    ui->obloc->setText("Deutschland, München");

    ui->tileLong->setText("-74");
    ui->tileLat->setText("40.7");
    ui->tileZoomLevel->setText("8");

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

#ifdef QGEOAPIUI_USEPOPUPMENU
    popupMenu = new QMenu(this);
    QAction* menuItem;
    menuItem = new QAction(tr("Request Route"), this);
    popupMenu->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(showRouteRequestControls()));

    menuItem = new QAction(tr("Geocoding"), this);
    popupMenu->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(showGeocodingControls()));

    menuItem = new QAction(tr("Reverse Geocoding"), this);
    popupMenu->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(showReverseGeocodingControls()));

    menuItem = new QAction(tr("Map Tile"), this);
    popupMenu->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(showMapTileControls()));

    menuItem = new QAction(tr("Exit"), this);
    popupMenu->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(close()));

    setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT(customContextMenuRequest(const QPoint&)));
#endif
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void MainWindow::on_btnRequest_clicked()
{
    QGeoCoordinate src(ui->srcLat->toPlainText().toDouble(), ui->srcLong->toPlainText().toDouble());
    QGeoCoordinate dst(ui->dstLat->toPlainText().toDouble(), ui->dstLong->toPlainText().toDouble());
    QGeoRouteRequest request;

    request.setSource(src);
    request.setDestination(dst);

    ui->mapTileLabel->setVisible(false);
    ui->treeWidget->setVisible(true);
    ui->treeWidget->clear();

    routingService->getRoute(request);
}

void MainWindow::on_btnReverseCoding_clicked()
{
    QGeoCoordinate coord(ui->locLat->toPlainText().toDouble(), ui->locLong->toPlainText().toDouble());
    QReverseGeocodingRequest request(coord);

    ui->mapTileLabel->setVisible(false);
    ui->treeWidget->setVisible(true);
    ui->treeWidget->clear();

    geocodingService->reverseGeocode(request);
}

void MainWindow::on_btnCoding_clicked()
{
    QGeocodingRequest request;
    QString s = ui->obloc->toPlainText();

    if (!s.isEmpty()) {
        request.setOneBoxLocation(s);
    } else {
        request.setCountry(ui->country->toPlainText());
        request.setState(ui->state->toPlainText());
        request.setCity(ui->city->toPlainText());
        request.setPostCode(ui->zip->toPlainText());
        request.setStreet(ui->street->toPlainText());
        request.setNumber(ui->streetNumber->toPlainText());
    }

    ui->mapTileLabel->setVisible(false);
    ui->treeWidget->setVisible(true);
    ui->treeWidget->clear();

    QGeocodingReply* reply = geocodingService->geocode(request);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(testReplyFinishedSignal()));
}

void MainWindow::on_btnRequestTile_clicked()
{
    QGeoCoordinate coord(ui->tileLat->toPlainText().toDouble(),
                         ui->tileLong->toPlainText().toDouble()
                        );
    quint32 zoomLevel = ui->tileZoomLevel->toPlainText().toInt();

    ui->mapTileLabel->setVisible(true);
    ui->treeWidget->setVisible(false);

    quint32 col;
    quint32 row;
    mapService->getMercatorTileIndex(coord, zoomLevel, &row, &col);
    mapService->request(zoomLevel, row, col);
}

void MainWindow::routeReplyFinished(QGeoRouteReply* reply)
{
    RoutePresenter presenter(ui->treeWidget, reply);
    presenter.show();
    reply->deleteLater();
}

void MainWindow::codingReplyFinished(QGeocodingReply* reply)
{
    PlacePresenter presenter(ui->treeWidget, reply);
    presenter.show();
    reply->deleteLater();
}

void MainWindow::mapTileReplyFinished(QMapTileReply* reply)
{
    QPixmap pixmap;
    pixmap.loadFromData(reply->data());
    ui->mapTileLabel->setPixmap(pixmap);
    reply->deleteLater();
}

void MainWindow::testReplyFinishedSignal()
{
    int x = 1;
    x++;
}

#ifdef QGEOAPIUI_USEPOPUPMENU
void MainWindow::customContextMenuRequest(const QPoint& point)
{
    popupMenu->popup(mapToGlobal(point));
}

void MainWindow::showRouteRequestControls(bool visible)
{
    if (visible) {
        showGeocodingControls(false);
        showReverseGeocodingControls(false);
        showMapTileControls(false);
    }

    for (int i = 0;i < ui->routeLayout->count();++i)
        ui->routeLayout->itemAt(i)->widget()->setVisible(visible);
}
void MainWindow::showGeocodingControls(bool visible)
{
    if (visible) {
        showRouteRequestControls(false);
        showReverseGeocodingControls(false);
        showMapTileControls(false);
    }

    for (int i = 0;i < ui->geocoding_1_Layout->count();++i)
        ui->geocoding_1_Layout->itemAt(i)->widget()->setVisible(visible);

    for (int i = 0;i < ui->geocodind_2_Layout->count();++i)
        ui->geocodind_2_Layout->itemAt(i)->widget()->setVisible(visible);
}

void MainWindow::showReverseGeocodingControls(bool visible)
{
    if (visible) {
        showGeocodingControls(false);
        showRouteRequestControls(false);
        showMapTileControls(false);
    }

    for (int i = 0;i < ui->revGeocodingLayout->count();++i)
        ui->revGeocodingLayout->itemAt(i)->widget()->setVisible(visible);
}

void MainWindow::showMapTileControls(bool visible)
{
    if (visible) {
        showGeocodingControls(false);
        showReverseGeocodingControls(false);
        showRouteRequestControls(false);
    }

    for (int i = 0;i < ui->mapTileLayout->count();++i)
        ui->mapTileLayout->itemAt(i)->widget()->setVisible(visible);
}
#endif
