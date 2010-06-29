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
#include <QNetworkProxyFactory>
#include <QMessageBox>
#include <QTimer>
#include <QAction>
#include <QPainter>

#include <qgeocoordinate.h>
#include <qgeomaprectangleobject.h>
#include <qgeomapmarkerobject.h>
#include <qgeomappolylineobject.h>

#ifdef Q_OS_SYMBIAN
#include <QMessageBox>
#include <qnetworksession.h>
#include <qnetworkconfigmanager.h>
#endif

#define MARKER_HEIGHT 36
#define MARKER_WIDTH 25
#define MARKER_PIN_LEN 10

QTM_USE_NAMESPACE

MapWidget::MapWidget(QGeoMappingManager *manager)
    : QGeoMapWidget(manager),
    panActive(false) {}

MapWidget::~MapWidget() {}

void MapWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    setFocus();
    if (event->button() == Qt::LeftButton)
        panActive = true;

    event->accept();
}

void MapWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        panActive = false;

    event->accept();
}

void MapWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (panActive) {
        int deltaLeft = event->lastPos().x() - event->pos().x();
        int deltaTop  = event->lastPos().y() - event->pos().y();
        pan(deltaLeft, deltaTop);
    }

    event->accept();
}

void MapWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();

    pan(event->lastPos().x() -  size().width() / 2.0, event->lastPos().y() - size().height() / 2.0);
    if (zoomLevel() < maximumZoomLevel())
        setZoomLevel(zoomLevel() + 1);

    event->accept();
}

void MapWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Minus) {
        if (zoomLevel() > minimumZoomLevel()) {
            setZoomLevel(zoomLevel() - 1);
        }
    } else if (event->key() == Qt::Key_Plus) {
        if (zoomLevel() < maximumZoomLevel()) {
            setZoomLevel(zoomLevel() + 1);
        }
    }

    event->accept();
}

void MapWidget::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if (event->delta() > 0) { //zoom in
        if (zoomLevel() < maximumZoomLevel()) {
            setZoomLevel(zoomLevel() + 1);
        }
    } else { //zoom out
        if (zoomLevel() > minimumZoomLevel()) {
            setZoomLevel(zoomLevel() - 1);
        }
    }
    event->accept();
}

/**************************************************************************************
**************************************************************************************/

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        m_serviceProvider(0),
        m_popupMenu(0)
{
    ui->setupUi(this);

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    setProvider("nokia");

    qgv = new QGraphicsView(this);
    qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qgv->setVisible(true);
    qgv->setGeometry(QRect(0, 0, width(), height()));
    qgv->setInteractive(true);

    QGraphicsScene* scene = new QGraphicsScene(0, 0, width(), height());
    qgv->setScene(scene);

    createMarkerIcon();

    m_mapWidget = new MapWidget(m_mapManager);
    qgv->scene()->addItem(m_mapWidget);
    m_mapWidget->setGeometry(0, 0, width(), height());
    //m_mapWidget->setZoomLevel(8);
    m_mapWidget->setCenter(QGeoCoordinate(52.5,13.0));

    setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT(customContextMenuRequest(const QPoint&)));

    setWindowTitle(tr("Map Viewer Demo"));

    QTimer::singleShot(0, this, SLOT(delayedInit()));
}

MainWindow::~MainWindow()
{
    delete m_serviceProvider;
    delete ui;
}

void MainWindow::delayedInit()
{
    // TODO: remove this dirty, dirty hack
    m_mapWidget->setZoomLevel(m_mapWidget->zoomLevel());
    m_mapWidget->update();
}

void MainWindow::setProvider(QString providerId)
{
    if (m_serviceProvider)
        delete m_serviceProvider ;
    m_serviceProvider = new QGeoServiceProvider(providerId);
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

void MainWindow::createMenus()
{
    QAction* menuItem;
    m_popupMenu = new QMenu(this);

    QMenu* subMenuItem = new QMenu(tr("Draw"), this);
    m_popupMenu->addMenu(subMenuItem);

    menuItem = new QAction(tr("Marker"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(drawMarker(bool)));

    menuItem = new QAction(tr("Rectangle"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(drawRect(bool)));

    menuItem = new QAction(tr("Polyline"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(drawPolyline(bool)));

}

void MainWindow::drawRect(bool /*checked*/)
{
    while (markers.count() >= 2) {
        QPoint p1 = markers.takeFirst();
        QPoint p2 = markers.takeFirst();
        QPen pen(Qt::white);
        pen.setWidth(2);
        QColor fill(Qt::black);
        fill.setAlpha(65);
        QGeoMapRectangleObject *rectangle = new QGeoMapRectangleObject(m_mapWidget->screenPositionToCoordinate(p1),
                                                             m_mapWidget->screenPositionToCoordinate(p2));
        rectangle->setPen(pen);
        rectangle->setBrush(QBrush(fill));
        m_mapWidget->addMapObject(rectangle);
    }

    markers.clear();
}

void MainWindow::drawPolyline(bool /*checked*/)
{
    QList<QGeoCoordinate> path;

    while (markers.count() >= 1) {
        QPoint p = markers.takeFirst();
        path.append(m_mapWidget->screenPositionToCoordinate(p));
    }

    QPen pen(Qt::white);
    pen.setWidth(2);
    QGeoMapPolylineObject *polyline = new QGeoMapPolylineObject();
    polyline->setPen(pen);
    polyline->setPath(path);
    m_mapWidget->addMapObject(polyline);
}

void MainWindow::drawMarker(bool /*checked*/)
{
    m_mapWidget->addMapObject(new QGeoMapMarkerObject(m_mapWidget->screenPositionToCoordinate(lastClicked), 
                              QPoint(-(MARKER_WIDTH / 2), -MARKER_HEIGHT), m_markerIcon));
    markers.append(lastClicked);
}

void MainWindow::customContextMenuRequest(const QPoint& point)
{
    lastClicked = point;

    if (focusWidget() == qgv) {

        if (!m_popupMenu)
            createMenus();

        m_popupMenu->popup(mapToGlobal(point));
    }
}

void MainWindow::createMarkerIcon()
{
    m_markerIcon = QPixmap(MARKER_WIDTH, MARKER_HEIGHT);
    m_markerIcon.fill(Qt::transparent);
    QPainter painter(&m_markerIcon);

    QPointF p1(MARKER_WIDTH / 2, MARKER_HEIGHT - 1);
    QPointF p2(MARKER_WIDTH / 2, MARKER_HEIGHT - 1 - MARKER_PIN_LEN);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLine(p1, p2);
    QRectF ellipse(0, 0, MARKER_WIDTH - 1, MARKER_WIDTH - 1);
    pen.setWidth(1);
    painter.setPen(pen);
    QColor color(Qt::green);
    color.setAlpha(127);
    QBrush brush(color);
    painter.setBrush(brush);
    painter.drawEllipse(ellipse);
}
