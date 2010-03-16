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
#include "qmaptile.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        geoNetworkManager("", "")
{
    ui->setupUi(this);

    QObject::connect(&geoNetworkManager, SIGNAL(finished(QRouteReply*)),
                     this, SLOT(routeReplyFinished(QRouteReply*)));

    QObject::connect(&geoNetworkManager, SIGNAL(finished(QGeocodingReply*)),
                     this, SLOT(codingReplyFinished(QGeocodingReply*)));

    QObject::connect(&geoNetworkManager, SIGNAL(finished(QMapTileReply*)),
                     this, SLOT(mapTileReplyFinished(QMapTileReply*)));

    ui->mapTileLabel->setVisible(false);

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

    QStringList labels;
    labels << "Elements" << "Value";
    ui->treeWidget->setHeaderLabels(labels);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    QGeoCoordinate src(ui->srcLat->toPlainText().toDouble(),ui->srcLong->toPlainText().toDouble());
    QGeoCoordinate dst(ui->dstLat->toPlainText().toDouble(), ui->dstLong->toPlainText().toDouble());
    QRouteRequest request;

    request.setSource(src);
    request.setDestination(dst);

    ui->mapTileLabel->setVisible(false);
    ui->treeWidget->setVisible(true);
    ui->treeWidget->clear();

    geoNetworkManager.get(request);
}

void MainWindow::on_btnReverseCoding_clicked()
{
    QGeoCoordinate coord(ui->locLat->toPlainText().toDouble(), ui->locLong->toPlainText().toDouble());
    QReverseGeocodingRequest request(coord);

    ui->mapTileLabel->setVisible(false);
    ui->treeWidget->setVisible(true);
    ui->treeWidget->clear();

    geoNetworkManager.get(request);
}

void MainWindow::on_btnCoding_clicked()
{
    QGeocodingRequest request;
    QString s = ui->obloc->toPlainText();

    if (!s.isEmpty())
        request.setOneBoxLocation(s);
    else {
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

    QGeocodingReply* reply = geoNetworkManager.get(request);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(testReplyFinishedSignal()));
}

void MainWindow::on_btnRequestTile_clicked()
{
    QGeoCoordinate coord(ui->tileLat->toPlainText().toDouble(),
                            ui->tileLong->toPlainText().toDouble()
                            );
    quint16 zoomLevel = ui->tileZoomLevel->toPlainText().toInt();

    ui->mapTileLabel->setVisible(true);
    ui->treeWidget->setVisible(false);

    QNetworkProxy proxy(QNetworkProxy::HttpProxy, "172.16.42.41", 8080);
    geoNetworkManager.setMapProxy(proxy);
    geoNetworkManager.setMapServer("origin.maptile.svc.tst.s2g.gate5.de");

    QMapTileRequest request;
    request.setVersion(MapVersion(MapVersion::Newest));
    request.setScheme(MapScheme(MapScheme::Normal_Day));
    request.setResolution(MapResolution(MapResolution::Res_256_256));
    request.setFormat(MapFormat(MapFormat::Png));

    quint32 col;
    quint32 row;
    QGeoEngine::getMercatorTileIndex(coord, zoomLevel, &col, &row);
    request.setCol((quint32) col);
    request.setRow((quint32) row);
    request.setZoomLevel(zoomLevel);

    geoNetworkManager.get(request);
}

void MainWindow::routeReplyFinished(QRouteReply* reply)
{
    RoutePresenter presenter(ui->treeWidget, reply);
    presenter.show();
    geoNetworkManager.release(reply);
}

void MainWindow::codingReplyFinished(QGeocodingReply* reply)
{
    PlacePresenter presenter(ui->treeWidget, reply);
    presenter.show();
    geoNetworkManager.release(reply);
}

void MainWindow::mapTileReplyFinished(QMapTileReply* reply)
{
    QPixmap pixmap;
    pixmap.loadFromData(reply->rawData());
    ui->mapTileLabel->setPixmap(pixmap);
    geoNetworkManager.release(reply);
}

void MainWindow::testReplyFinishedSignal()
{
    int x = 1;
    x++;
}
