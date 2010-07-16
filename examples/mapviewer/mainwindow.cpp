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
#include <QDesktopWidget>

#include <qgeocoordinate.h>
#include <qgeomaprectangleobject.h>
#include <qgeomapmarkerobject.h>
#include <qgeomappolylineobject.h>
#include <qgeomappolygonobject.h>
#include <qgeomaprouteobject.h>
#include <qgeorouterequest.h>

#ifdef Q_OS_SYMBIAN
#include <QMessageBox>
#include <qnetworksession.h>
#include <qnetworkconfigmanager.h>
#endif

#include <cmath>

#define MARKER_HEIGHT 36
#define MARKER_WIDTH 25
#define MARKER_PIN_LEN 10

QTM_USE_NAMESPACE

// TODO: Some of these could be exposed in a GUI and should probably be put elsewhere in that case (and made non-const)
#ifdef Q_OS_SYMBIAN
static const bool enableKineticPanning = true;
static const qreal kineticPanningHalflife = 200.0; // time until kinetic panning speed slows down to 50%, in msec
static const qreal panSpeedNormal = 0.3; // keyboard panning speed without modifiers, in pixels/msec
static const qreal panSpeedFast = 1.0; // keyboard panning speed with shift, in pixels/msec
static const qreal kineticPanSpeedThreshold = 0.02; // minimum panning speed, in pixels/msec
static const int kineticPanningResolution = 75; // temporal resolution. Smaller values take more CPU but improve visual quality
static const int holdTimeThreshold = 200; // maximum time between last mouse move and mouse release for kinetic panning to kick in
#else
static const bool enableKineticPanning = true;
static const qreal kineticPanningHalflife = 300.0; // time until kinetic panning speed slows down to 50%, in msec
static const qreal panSpeedNormal = 0.3; // keyboard panning speed without modifiers, in pixels/msec
static const qreal panSpeedFast = 1.0; // keyboard panning speed with shift, in pixels/msec
static const qreal kineticPanSpeedThreshold = 0.005; // minimum panning speed, in pixels/msec
static const int kineticPanningResolution = 30; // temporal resolution. Smaller values take more CPU but improve visual quality
static const int holdTimeThreshold = 100; // maximum time between last mouse move and mouse release for kinetic panning to kick in
#endif

static inline qreal qPointLength(const QPointF &p) {
    qreal x = p.x();
    qreal y = p.y();

    return std::sqrt(x * x + y * y);
}

MapWidget::MapWidget(QGeoMappingManager *manager) :
    QGeoMapWidget(manager),
    panActive(false),
    kineticTimer(new QTimer)
{
    for (int i = 0; i < 5; ++i) mouseHistory.append(MouseHistoryEntry());

    connect(kineticTimer, SIGNAL(timeout()), this, SLOT(kineticTimerEvent()));
    kineticTimer->setInterval(kineticPanningResolution);
}

MapWidget::~MapWidget() {}

void MapWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    setFocus();
    if (event->button() == Qt::LeftButton) {
        panActive = true;

        // When pressing, stop the timer and stop all current kinetic panning
        kineticTimer->stop();
        kineticPanSpeed = QPointF();

        lastMoveTime = QTime::currentTime();
        // TODO: Maybe call stopPanning or skip the call to startPanning if the timer was still running.
        startPanning();
    }

    event->accept();
}

void MapWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        panActive = false;

        if (!enableKineticPanning || lastMoveTime.msecsTo(QTime::currentTime()) > holdTimeThreshold) {
            stopPanning();
            return;
        }

        kineticPanSpeed = QPointF();
        int entries_considered = 0;

        QTime currentTime = QTime::currentTime();
        foreach (MouseHistoryEntry entry, mouseHistory) {
            // first=speed, second=time
            int deltaTime = entry.second.msecsTo(currentTime);
            if (deltaTime < holdTimeThreshold) {
                kineticPanSpeed += entry.first;
                entries_considered++;
            }
        }
        if (entries_considered > 0) kineticPanSpeed /= entries_considered;
        lastMoveTime = currentTime;

        // When releasing the mouse button/finger while moving, start the kinetic panning timer (which also takes care of calling stopPanning).
        kineticTimer->start();
        panDecellerate = true;
    }

    event->accept();
}

void MapWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (panActive) {
        // Calculate time delta
        QTime currentTime = QTime::currentTime();
        int deltaTime = lastMoveTime.msecsTo(currentTime);
        lastMoveTime = currentTime;

        // Calculate position delta
        QPointF delta = event->lastPos() - event->pos();

        // Calculate and set speed (TODO: average over 3 events)
        if (deltaTime > 0) {
            kineticPanSpeed = delta / deltaTime;

            mouseHistory.push_back(MouseHistoryEntry(kineticPanSpeed, currentTime));
            mouseHistory.pop_front();
        }

        // Pan map
        panFloatWrapper(delta);
    }

    event->accept();
}

void MapWidget::kineticTimerEvent()
{
    QTime currentTime = QTime::currentTime();
    int deltaTime = lastMoveTime.msecsTo(currentTime);
    lastMoveTime = currentTime;

    if (panDecellerate) kineticPanSpeed *= pow(0.5, deltaTime/kineticPanningHalflife);

    QPointF scaledSpeed = kineticPanSpeed * deltaTime;

    if (kineticPanSpeed.manhattanLength() < kineticPanSpeedThreshold) {
        // Kinetic panning is almost halted -> stop it.
        kineticTimer->stop();
        stopPanning();
        return;
    }
    panFloatWrapper(scaledSpeed);
}

// Wraps the pan(int, int) method to achieve floating point accuracy, which is needed to scroll smoothly.
void MapWidget::panFloatWrapper(const QPointF& delta)
{
    // Add to previously stored panning distance
    remainingPan += delta;

    // Convert to integers
    QPoint move = remainingPan.toPoint();

    // Commit mouse movement
    pan(move.x(), move.y());

    // Store committed mouse movement
    remainingPan -= move;

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
    switch (event->key()) {
        case Qt::Key_Minus:
            if (zoomLevel() > minimumZoomLevel()) {
                setZoomLevel(zoomLevel() - 1);
            }
            break;

        case Qt::Key_Plus:
            if (zoomLevel() < maximumZoomLevel()) {
                setZoomLevel(zoomLevel() + 1);
            }
            break;
        case Qt::Key_T:
            if (mapType() == QGeoMapWidget::StreetMap)
                setMapType(QGeoMapWidget::SatelliteMapDay);
            else if (mapType() == QGeoMapWidget::SatelliteMapDay)
                setMapType(QGeoMapWidget::StreetMap);
            break;

        case Qt::Key_Shift:
            // If there's no current movement, we don't need to handle shift.
            if (panDir.manhattanLength() == 0) break;
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            if (!event->isAutoRepeat()) {
                switch (event->key()) {
                    case Qt::Key_Left:
                        panDir += QPoint(-1, 0);
                        break;

                    case Qt::Key_Right:
                        panDir += QPoint(1, 0);
                        break;

                    case Qt::Key_Up:
                        panDir += QPoint(0, -1);
                        break;

                    case Qt::Key_Down:
                        panDir += QPoint(0, 1);
                        break;
                }

                kineticTimer->start();
                panDecellerate = false;

                applyPan(event->modifiers());
            }
            break;
    }

    event->accept();
}

void MapWidget::keyReleaseEvent(QKeyEvent* event)
{
    event->accept();

    // Qt seems to have auto-repeated release events too...
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
        case Qt::Key_Left:
            panDir -= QPoint(-1, 0);
            break;

        case Qt::Key_Right:
            panDir -= QPoint(1, 0);
            break;

        case Qt::Key_Up:
            panDir -= QPoint(0, -1);
            break;

        case Qt::Key_Down:
            panDir -= QPoint(0, 1);
            break;

        case Qt::Key_Shift:
            if (panDir.manhattanLength() == 0) return;
            break;

        default:
            return;
    }

    applyPan(event->modifiers());
}

// Evaluates the panDir field and sets kineticPanSpeed accordingly. Used in MapWidget::keyPressEvent and MapWidget::keyReleaseEvent
void MapWidget::applyPan(const Qt::KeyboardModifiers& modifiers)
{
    Q_ASSERT(panDir.manhattanLength() <= 2);

    if (panDir.manhattanLength() == 0) {
        // If no more direction keys are held down, decellerate
        panDecellerate = true;
    } else {
        // Otherwise, set new direction
        qreal panspeed = (modifiers & Qt::ShiftModifier) ? panSpeedFast : panSpeedNormal;

        if (panDir.manhattanLength() == 2) {
            // If 2 keys are held down, adjust speed to achieve the same speed in all 8 possible directions
            panspeed *= sqrt(0.5);
        }

        // Finally set the current panning speed
        kineticPanSpeed = QPointF(panDir) * panspeed;
    }
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
    m_routingManager = m_serviceProvider->routingManager();
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

    //**************************************************************
    QMenu* subMenuItem = new QMenu(tr("Marker"), this);
    m_popupMenu->addMenu(subMenuItem);

    menuItem = new QAction(tr("Set marker"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(drawMarker(bool)));

    menuItem = new QAction(tr("Remove markers"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(removeMarkers()));

    menuItem = new QAction(tr("Select objects"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(selectObjects()));

    //**************************************************************
    subMenuItem = new QMenu(tr("Draw"), this);
    m_popupMenu->addMenu(subMenuItem);

    menuItem = new QAction(tr("Rectangle"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(drawRect(bool)));

    menuItem = new QAction(tr("Polyline"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(drawPolyline(bool)));

    menuItem = new QAction(tr("Polygon"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(drawPolygon(bool)));

    //**************************************************************
    subMenuItem = new QMenu(tr("Route"), this);
    m_popupMenu->addMenu(subMenuItem);

    menuItem = new QAction(tr("Calculate route"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(calcRoute(bool)));
}

void MainWindow::drawRect(bool /*checked*/)
{
    if (markerObjects.count() >= 2) {
        QGeoMapMarkerObject* p1 = markerObjects.at(0);
        QGeoMapMarkerObject* p2 = markerObjects.at(1);
        QPen pen(Qt::white);
        pen.setWidth(2);
        QColor fill(Qt::black);
        fill.setAlpha(65);
        QGeoMapRectangleObject *rectangle = new QGeoMapRectangleObject(p1->coordinate(), p2->coordinate());
        rectangle->setPen(pen);
        rectangle->setBrush(QBrush(fill));
        m_mapWidget->addMapObject(rectangle);
    }
}

void MainWindow::drawPolyline(bool /*checked*/)
{
    QList<QGeoCoordinate> path;

    for (int i = 0; i < markerObjects.size(); i++) {
        QGeoMapMarkerObject* p = markerObjects.at(i);
        path.append(p->coordinate());
    }

    QPen pen(Qt::white);
    pen.setWidth(2);
    QGeoMapPolylineObject *polyline = new QGeoMapPolylineObject();
    polyline->setPen(pen);
    polyline->setPath(path);
    m_mapWidget->addMapObject(polyline);
}

void MainWindow::drawPolygon(bool /*checked*/)
{
    QList<QGeoCoordinate> path;

    for (int i = 0; i < markerObjects.size(); i++) {
        QGeoMapMarkerObject* p = markerObjects.at(i);
        path.append(p->coordinate());
    }

    QPen pen(Qt::white);
    pen.setWidth(2);
    QGeoMapPolygonObject *polygon = new QGeoMapPolygonObject();
    polygon->setPen(pen);
    QColor fill(Qt::black);
    fill.setAlpha(65);
    polygon->setBrush(QBrush(fill));
    polygon->setPath(path);
    m_mapWidget->addMapObject(polygon);
}

void MainWindow::drawMarker(bool /*checked*/)
{
    QGeoMapMarkerObject *marker = new QGeoMapMarkerObject(m_mapWidget->screenPositionToCoordinate(lastClicked), 
                                                          QPoint(-(MARKER_WIDTH / 2), -MARKER_HEIGHT), m_markerIcon);
    m_mapWidget->addMapObject(marker);
    markerObjects.append(marker);
}

void MainWindow::removeMarkers()
{
    while (markerObjects.size() > 0) {
        QGeoMapMarkerObject *marker = markerObjects.takeFirst();
        m_mapWidget->removeMapObject(marker);
        marker->deleteLater();
    }
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

void MainWindow::calcRoute(bool /*checked*/)
{
    if (markerObjects.count() < 2)
        return;

    QList<QGeoCoordinate> waypoints;

    for (int i = 0; i < markerObjects.count(); i++) {
        QGeoMapMarkerObject* p = markerObjects.at(i);
        waypoints.append(p->coordinate());
    }

    QGeoRouteRequest req(waypoints);
    QGeoRouteReply *reply = m_routingManager->calculateRoute(req);

    QObject::connect(reply, SIGNAL(finished()),
                     this, SLOT(routeFinished()));
}

void MainWindow::routeFinished()
{
    QGeoRouteReply *reply = static_cast<QGeoRouteReply*>(sender());

    if (!reply)
        return;

    if (reply->routes().size() < 1)
        return;

    QGeoMapRouteObject *route = new QGeoMapRouteObject(reply->routes().at(0));
    QColor routeColor(Qt::blue);
    routeColor.setAlpha(127); //semi-transparent
    QPen pen(routeColor);
    pen.setWidth(7);
    pen.setCapStyle(Qt::RoundCap);
    route->setPen(pen);
    m_mapWidget->addMapObject(route);
}

void MainWindow::selectObjects()
{
    if (markerObjects.count() < 2)
        return;

    QGeoMapMarkerObject* bottomRight = markerObjects.takeLast();
    QGeoMapMarkerObject* topLeft = markerObjects.takeLast();
    m_mapWidget->removeMapObject(topLeft);
    m_mapWidget->removeMapObject(bottomRight);
    QList<QGeoMapObject*> mapObjects = m_mapWidget->mapObjectsInScreenRect(
                                        QRectF(m_mapWidget->coordinateToScreenPosition(topLeft->coordinate()),
                                               m_mapWidget->coordinateToScreenPosition(bottomRight->coordinate()))
                                       );
}
