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
#include <QPointF>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QMessageBox>
#include <QTimer>
#include <QAction>
#include <QPainter>
#include <QDesktopWidget>

#include <QGridLayout>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QRadioButton>
#include <QSlider>

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
    coordQueryState(false),
    panActive(false),
    kineticTimer(new QTimer),
    lastCircle(0)
{
    for (int i = 0; i < 5; ++i) mouseHistory.append(MouseHistoryEntry());

    connect(kineticTimer, SIGNAL(timeout()), this, SLOT(kineticTimerEvent()));
    kineticTimer->setInterval(kineticPanningResolution);
}

MapWidget::~MapWidget() {}

void MapWidget::setMouseClickCoordQuery(bool state)
{
    coordQueryState = state;
}

void MapWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    setFocus();
    if (event->button() == Qt::LeftButton) {
        if (event->modifiers() & Qt::ControlModifier) {
        }
        else {
            if (coordQueryState) {
                emit coordQueryResult(screenPositionToCoordinate(event->lastPos()));
                return;
            }

            panActive = true;

            // When pressing, stop the timer and stop all current kinetic panning
            kineticTimer->stop();
            kineticPanSpeed = QPointF();

            lastMoveTime = QTime::currentTime();
            // TODO: Maybe call stopPanning or skip the call to startPanning if the timer was still running.
            startPanning();
        }
    }

    event->accept();
}

void MapWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (panActive) {
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
    }

    event->accept();
}

void MapWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (lastCircle) {
            removeMapObject(lastCircle);
            lastCircle->setCenter(this->screenPositionToCoordinate(event->pos()));
            addMapObject(lastCircle);
            lastCircle->objectUpdate();
        }
    }
    else if (panActive) {
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

    if (panDecellerate)
        kineticPanSpeed *= pow(qreal(0.5), qreal(deltaTime/kineticPanningHalflife));

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
                        panDir.setX(-1);
                        break;

                    case Qt::Key_Right:
                        panDir.setX(1);
                        break;

                    case Qt::Key_Up:
                        panDir.setY(-1);
                        break;

                    case Qt::Key_Down:
                        panDir.setY(1);
                        break;
                }

                lastMoveTime = QTime::currentTime();
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
        case Qt::Key_Right:
            panDir.setX(0);
            break;

        case Qt::Key_Up:
        case Qt::Key_Down:
            panDir.setY(0);
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
        m_serviceProvider(0),
        m_popupMenu(0)
{
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    setProvider("nokia");

    setupUi();

    setWindowTitle(tr("Map Viewer Demo"));
}

MainWindow::~MainWindow()
{
    delete m_serviceProvider;
}

void MainWindow::setupUi()
{
    // setup graphics view containing map widget

    QGraphicsScene* scene = new QGraphicsScene(this);
    qgv = new QGraphicsView(scene, this);
    qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qgv->setVisible(true);
    qgv->setInteractive(true);

    createMarkerIcon();

    m_mapWidget = new MapWidget(m_mapManager);
    qgv->scene()->addItem(m_mapWidget);
    //m_mapWidget->setCenter(QGeoCoordinate(52.5,13.0));
    //temporary change for dateline testing
    m_mapWidget->setCenter(QGeoCoordinate(-27.0, 152.0));
    m_mapWidget->setZoomLevel(5);

    // setup slider control

    slider = new QSlider(Qt::Vertical, this);
    slider->setTickInterval(1);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setMaximum(m_mapManager->maximumZoomLevel());
    slider->setMinimum(m_mapManager->minimumZoomLevel());
    slider->setSliderPosition(m_mapWidget->zoomLevel());

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
    connect(m_mapWidget, SIGNAL(zoomLevelChanged(qreal)), this, SLOT(mapZoomLevelChanged(qreal)));

    // setup map type control

    QVBoxLayout *mapControlLayout = new QVBoxLayout();

    connect(m_mapWidget, SIGNAL(mapTypeChanged(QGeoMapWidget::MapType)), this, SLOT(mapTypeChanged(QGeoMapWidget::MapType)));

    QList<QGeoMapWidget::MapType> types = m_mapWidget->supportedMapTypes();
    for (int i = 0; i < types.size(); ++i) {
        QRadioButton *radio = new QRadioButton(this);

        switch (types.at(i)) {
            case QGeoMapWidget::StreetMap:
                radio->setText("Street");
                break;
            case QGeoMapWidget::SatelliteMapDay:
                radio->setText("Satellite");
                break;
            case QGeoMapWidget::SatelliteMapNight:
                radio->setText("Satellite - Night");
                break;
            case QGeoMapWidget::TerrainMap:
                radio->setText("Terrain");
                break;
        }

        if (types.at(i) == m_mapWidget->mapType())
            radio->setChecked(true);

        connect(radio, SIGNAL(toggled(bool)), this, SLOT(mapTypeToggled(bool)));

        mapControlButtons.append(radio);
        mapControlTypes.append(types.at(i));

        mapControlLayout->addWidget(radio);
    }

    // setup lat/lon control

    latitudeEdit = new QLineEdit();
    longitudeEdit = new QLineEdit();

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Latitude", latitudeEdit);
    formLayout->addRow("Longitude", longitudeEdit);

    captureCoordsButton = new QToolButton();
    captureCoordsButton->setText("Capture coordinates");
    captureCoordsButton->setCheckable(true);

    connect(captureCoordsButton, SIGNAL(toggled(bool)), m_mapWidget, SLOT(setMouseClickCoordQuery(bool)));
    connect(m_mapWidget, SIGNAL(coordQueryResult(QGeoCoordinate)), this, SLOT(updateCoords(QGeoCoordinate)));

    setCoordsButton = new QPushButton();
    setCoordsButton->setText("Set coordinates");

    connect(setCoordsButton, SIGNAL(clicked()), this, SLOT(setCoordsClicked()));

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    buttonLayout->addWidget(captureCoordsButton);
    buttonLayout->addWidget(setCoordsButton);

    QVBoxLayout *coordControlLayout = new QVBoxLayout();
    coordControlLayout->addLayout(formLayout);
    coordControlLayout->addLayout(buttonLayout);

    QWidget *widget = new QWidget(this);

    QGridLayout *layout = new QGridLayout();

    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 0);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 1);

    layout->addWidget(slider, 0, 0);
    layout->addWidget(qgv, 0, 1);
    layout->addLayout(mapControlLayout, 1, 0);
    layout->addLayout(coordControlLayout, 1, 1);

    widget->setLayout(layout);
    setCentralWidget(widget);

    qgv->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(qgv, SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT(customContextMenuRequest(const QPoint&)));

    resize(652, 519);
}

void MainWindow::sliderValueChanged(int zoomLevel)
{
    m_mapWidget->setZoomLevel(zoomLevel);
}

void MainWindow::mapZoomLevelChanged(qreal zoomLevel)
{
    slider->setSliderPosition(qRound(zoomLevel));
}

void MainWindow::mapTypeToggled(bool checked)
{
    if (checked) {
        QRadioButton *button = qobject_cast<QRadioButton*>(sender());
        int index = mapControlButtons.indexOf(button);
        if (index != -1)
            m_mapWidget->setMapType(mapControlTypes.at(index));
    }

}

void MainWindow::mapTypeChanged(QGeoMapWidget::MapType type)
{
    int index = mapControlTypes.indexOf(type);
    if (index != -1)
        mapControlButtons.at(index)->setChecked(true);
}

void MainWindow::setCoordsClicked()
{
    bool ok = false;

    double lat = latitudeEdit->text().toDouble(&ok);

    if (!ok)
        return;

    double lon = longitudeEdit->text().toDouble(&ok);

    if (!ok)
        return;

    m_mapWidget->setCenter(QGeoCoordinate(lat, lon));
}

void MainWindow::updateCoords(const QGeoCoordinate &coords)
{
    if (!coords.isValid())
        return;

    latitudeEdit->setText(QString::number(coords.latitude()));
    longitudeEdit->setText(QString::number(coords.longitude()));

    captureCoordsButton->setChecked(false);
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
    qgv->setSceneRect(QRectF(QPointF(0.0,0.0), qgv->size()));
    m_mapWidget->resize(qgv->size());
}

void MainWindow::showEvent(QShowEvent* event)
{
    qgv->setSceneRect(QRectF(QPointF(0.0,0.0), qgv->size()));
    m_mapWidget->resize(qgv->size());
}

void MainWindow::createMenus()
{
    QAction* menuItem;
    QMenu* subMenuItem
    m_popupMenu = new QMenu(this);

    subMenuItem = new QMenu(tr("Spawn stuff"), this);
    m_popupMenu->addMenu(subMenuItem);

    menuItem = new QAction(tr("Items near the dateline"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(demo1(bool)));

    menuItem = new QAction(tr("Regular grid of items"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(demo2(bool)));

    menuItem = new QAction(tr("Random items"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(demo3(bool)));

    //**************************************************************
    subMenuItem = new QMenu(tr("Marker"), this);
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

    menuItem = new QAction(tr("Circle"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(drawCircle(bool)));

    //**************************************************************
    subMenuItem = new QMenu(tr("Route"), this);
    m_popupMenu->addMenu(subMenuItem);

    menuItem = new QAction(tr("Calculate route"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(calcRoute(bool)));
}

#define MVTEST_MARK(pos) do { QGeoMapMarkerObject *marker = new QGeoMapMarkerObject(pos, QPoint(-(MARKER_WIDTH / 2), -MARKER_HEIGHT), m_markerIcon); m_mapWidget->addMapObject(marker); markerObjects.append(marker); } while (0)
#define MVTEST_MARK2(lat,lng) MVTEST_MARK(QGeoCoordinate(lat,lng))
#define MVTEST_RECT(topleft,bottomright) removeMarkers(); MVTEST_MARK(topleft); MVTEST_MARK(bottomright); drawRect(false);
#define MVTEST_RECT2(topleftlat,topleftlng,bottomrightlat,bottomrightlng) MVTEST_RECT(QGeoCoordinate(topleftlat,topleftlng),QGeoCoordinate(bottomrightlat,bottomrightlng))
void MainWindow::demo1(bool /*checked*/)
{
    MVTEST_MARK2(-30,175);
    MVTEST_MARK2(-20,170);
    MVTEST_MARK2(-25,-175);
    drawRect(false);
    drawPolygon(false);
    drawCircle(false);
}
void MainWindow::demo2(bool /*checked*/)
{
    int i = 0;
    qreal sz = 3;
    for (qreal lat = -90+sz; lat < 90-sz; lat += sz*3) {
        for (qreal lng = -180+sz; lng < 180-sz; lng += sz*3) {
            MVTEST_RECT2(lat-sz,lng-sz,lat+sz,lng+sz);
            i++;
        }
    }
    qDebug("%i items added, %i items total.", i, m_mapWidget->mapObjects().size());


    QMessageBox *mb = new QMessageBox(QMessageBox::NoIcon, "MapViewer", QString::number(i) + " items");
    mb->open();

}
void MainWindow::demo3(bool /*checked*/)
{

}

void MainWindow::drawRect(bool /*checked*/)
{
    if (markerObjects.count() < 2)  return;
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

void MainWindow::drawCircle(bool /*checked*/)
{
    if (markerObjects.count() < 1) return;

    QGeoMapMarkerObject* p1 = markerObjects.at(0);

    // center of the circle
    QGeoCoordinate center = p1->coordinate();

    // the circle's radius, in meters, defaults to 3000 km
    qreal radius = 3000000;

    // if a 2nd marker object is given, evaluate its distance to the first one to get the circle's radius.
    if (markerObjects.count() >= 2) {
        QGeoMapMarkerObject* p2 = markerObjects.at(1);

        radius = center.distanceTo(p2->coordinate());
    }

    QPen pen(Qt::white);
    pen.setWidth(2);
    QGeoMapCircleObject *circle = new QGeoMapCircleObject(center, radius);
    circle->setPen(pen);
    QColor fill(Qt::black);
    fill.setAlpha(65);
    circle->setBrush(QBrush(fill));
    m_mapWidget->addMapObject(circle);

    m_mapWidget->lastCircle = circle;
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
#ifdef Q_OS_SYMBIAN
    // Work around a bug with QGraphicsView's customContextMenuRequeste signal on Symbian
    // TODO: adjust this #ifdef, so it doesn't break with versions that fix the bug
    lastClicked -= qgv->geometry().topLeft();
#endif

    if (focusWidget() == qgv) {

        if (!m_popupMenu)
            createMenus();

        m_popupMenu->popup(qgv->mapToGlobal(lastClicked));
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
