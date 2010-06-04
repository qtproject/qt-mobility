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

#include <QPointF>
#include <QNetworkProxy>
#include <QMessageBox>
#include <QTimer>

#ifdef Q_OS_SYMBIAN
#include <QMessageBox>
#include <qnetworksession.h>
#include <qnetworkconfigmanager.h>
#endif

QTM_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        m_serviceProvider(0)
{
    ui->setupUi(this);
    setProvider("nokia");

    qgv = new QGraphicsView(this);
    qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qgv->setVisible(true);
    qgv->setGeometry(QRect(0, 0, width(), height()));
    qgv->setInteractive(true);

    QGraphicsScene* scene = new QGraphicsScene(0, 0, width(), height());
    qgv->setScene(scene);

    m_mapWidget = new QGeoMapWidget(m_mapManager);
    qgv->scene()->addItem(m_mapWidget);
    m_mapWidget->setGeometry(0, 0, width(), height());
    //m_mapWidget->setZoomLevel(8);
    //m_mapWidget->setCenter(QGeoCoordinate(52.5,13.0));

    setWindowTitle(tr("Map Viewer Demo"));

    //QTimer::singleShot(0, this, SLOT(delayedInit()));
}

MainWindow::~MainWindow()
{
    delete m_serviceProvider;
    delete ui;
}

void MainWindow::setProvider(QString providerId)
{
    QMap<QString, QString> parameters;
    if (providerId == "nokia") {
        parameters.insert("places.proxy", "172.16.42.137");
        parameters.insert("places.host", "dev-a7.bln.gate5.de");
        parameters.insert("routing.proxy", "172.16.42.137");
        parameters.insert("routing.host", "172.24.32.155");
        parameters.insert("mapping.proxy", "172.16.42.40");
        parameters.insert("mapping.host", "maptile.svc.nokia.com.edgesuite.net");
    }

    if (m_serviceProvider)
        delete m_serviceProvider ;
    m_serviceProvider = new QGeoServiceProvider(providerId, parameters);
    if (m_serviceProvider->error() != QGeoServiceProvider::NoError) {
        QMessageBox::information(this, tr("MapViewer Example"), tr(
                                     "Unable to find the %1 geoservices plugin.").arg(providerId));
        qApp->quit();
        return;
    }

    m_mapManager = m_serviceProvider->mappingManager();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    qgv->resize(event->size());
    qgv->setSceneRect(0, 0, event->size().width(), event->size().height());
    m_mapWidget->resize(event->size());
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
