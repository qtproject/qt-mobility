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
#include <QMenuBar>
#include <QPainter>
#include <QDesktopWidget>
#include <QMessageBox>

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
#include <qgeomappixmapobject.h>
#include <qgeomappolylineobject.h>
#include <qgeomappolygonobject.h>
#include <qgeomaprouteobject.h>
#include <qgeomaptextobject.h>
#include <qgeorouterequest.h>
#include <qnetworkconfigmanager.h>

#include <cmath>

#define MARKER_HEIGHT 36
#define MARKER_WIDTH 25
#define MARKER_PIN_LEN 10

QTM_USE_NAMESPACE

// TODO: Some of these could be exposed in a GUI and should probably be put elsewhere in that case (and made non-const)
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
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

MapWidget::MapWidget(QGeoMappingManager *manager) :
        QGraphicsGeoMap(manager),
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
        } else {
            if (coordQueryState) {
                emit coordQueryResult(screenPositionToCoordinate(event->lastPos()));
                return;
            }

            panActive = true;

            // When pressing, stop the timer and stop all current kinetic panning
            kineticTimer->stop();
            kineticPanSpeed = QPointF();

            lastMoveTime = QTime::currentTime();
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
                return;
            }

            kineticPanSpeed = QPointF();
            int entries_considered = 0;

            QTime currentTime = QTime::currentTime();
            foreach(MouseHistoryEntry entry, mouseHistory) {
                // first=speed, second=time
                int deltaTime = entry.second.msecsTo(currentTime);
                if (deltaTime < holdTimeThreshold) {
                    kineticPanSpeed += entry.first;
                    entries_considered++;
                }
            }
            if (entries_considered > 0) kineticPanSpeed /= entries_considered;
            lastMoveTime = currentTime;

            // When releasing the mouse button/finger while moving, start the kinetic panning timer
            kineticTimer->start();
            panDecellerate = true;
        }
    }

    event->accept();
}

void MapWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (lastCircle)
            lastCircle->setCenter(this->screenPositionToCoordinate(event->pos()));
    } else if (panActive) {
        // Calculate time delta
        QTime currentTime = QTime::currentTime();
        int deltaTime = lastMoveTime.msecsTo(currentTime);
        lastMoveTime = currentTime;

        // Calculate position delta
        QPointF delta = event->lastPos() - event->pos();

        // Calculate and set speed
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
        kineticPanSpeed *= pow(qreal(0.5), qreal(deltaTime / kineticPanningHalflife));

    QPointF scaledSpeed = kineticPanSpeed * deltaTime;

    if (kineticPanSpeed.manhattanLength() < kineticPanSpeedThreshold) {
        // Kinetic panning is almost halted -> stop it.
        kineticTimer->stop();
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
            if (mapType() == QGraphicsGeoMap::StreetMap)
                setMapType(QGraphicsGeoMap::SatelliteMapDay);
            else if (mapType() == QGraphicsGeoMap::SatelliteMapDay)
                setMapType(QGraphicsGeoMap::StreetMap);
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
        m_popupMenu(0),
        m_controlsVisible(true)
{
    setWindowTitle(tr("Map Viewer Demo"));

    // Set Internet Access Point
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities()
                              & QNetworkConfigurationManager::CanStartAndStopInterfaces);
    // Is there default access point, use it
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QNetworkConfiguration::Active)) {
        QMessageBox::information(this, tr("Map Viewer Demo"), tr(
                                     "Available Access Points not found."));
        return;
    }

    m_session = new QNetworkSession(cfg, this);
    connect(m_session, SIGNAL(opened()), this, SLOT(networkSessionOpened()));
    connect(m_session,
            SIGNAL(error(QNetworkSession::SessionError)),
            this,
            SLOT(error(QNetworkSession::SessionError)));

    m_session->open();
}

MainWindow::~MainWindow()
{
    delete m_serviceProvider;
}

void MainWindow::networkSessionOpened()
{
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    setProvider("nokia");
    setupUi();
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

void MainWindow::sceneSelectionChanged()
{
    if (!m_fullScreenButton->isSelected())
        return;

    m_fullScreenButton->setSelected(false);

    // toggle hide control mode
    setControlsVisible(!controlsVisible());
}

bool MainWindow::controlsVisible()
{
    return m_controlsVisible;
}

void MainWindow::setControlsVisible(bool controlsVisible)
{
    if (m_controlsVisible == controlsVisible)
        return;

    m_controlsVisible = controlsVisible;

    // obtain a flat item list, containing every widget
    QList<QLayoutItem*> items;
    items << m_layout;

    // Unroll the list until it is flat. start by assuming it isn't
    bool flat = false;
    while (!flat) {
        QList<QLayoutItem*> newItems;

        // Before the actual analysis, assume the list is flat.
        flat = true;

        foreach(QLayoutItem *item, items) {
            QLayout * layout = item->layout();
            if (layout) {
                // We have a sub-layout => list isn't flat
                flat = false;

                // loop through all items in that sub-layout
                for (int i = 0; ; ++i) {
                    QLayoutItem * subItem = layout->itemAt(i);
                    if (!subItem) break;

                    // add the sub-layout item to the new flat(ter) list
                    newItems << subItem;
                }
            }
            else {
                // not a layout? simply pass it through
                newItems << item;
            }
        }

        //
        items = newItems;
    }

    foreach(QLayoutItem *item, items) {
        QWidget * widget = item->widget();

        if (widget != m_qgv)
            widget->setVisible(controlsVisible);
    }
    m_layout->activate();

    // TODO: instead of copypasting from resizeEvent, trigger the resizing on layout changes/qgv size changes.
    m_qgv->setSceneRect(QRectF(QPointF(0.0, 0.0), m_qgv->size()));
    m_mapWidget->resize(m_qgv->size());
}

void MainWindow::setupUi()
{
    // setup exit menu for devices
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    QAction* exitAction = new QAction(tr("Exit"), this);
    menuBar()->addAction(exitAction);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
#endif

    // setup graphics view containing map widget

    QGraphicsScene* scene = new QGraphicsScene(this);
    m_qgv = new QGraphicsView(scene, this);
    m_qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_qgv->setVisible(true);
    m_qgv->setInteractive(true);

    createPixmapIcon();

    m_mapWidget = new MapWidget(m_mapManager);
    scene->addItem(m_mapWidget);
    //m_mapWidget->setCenter(QGeoCoordinate(52.5,13.0));
    //temporary change for dateline testing
    m_mapWidget->setCenter(QGeoCoordinate(-27.0, 152.0));
    m_mapWidget->setZoomLevel(5);

#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    // make full-screen button

    QPainterPath path;
    const int gaps = 3;
    const int innerwidth = 20;
    const int innerheight = 10;
    const int smallbox = 6;
    path.addRect(0, 0, innerwidth+smallbox+3*gaps, innerheight+smallbox+3*gaps);
    path.addRect(smallbox+2*gaps, gaps, innerwidth, innerheight);
    path.addRect(gaps, 2*gaps+innerheight, smallbox, smallbox);

    m_fullScreenButton = new QGraphicsPathItem(path); // TODO: make member
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(0,0,0,128));
    pen.setJoinStyle(Qt::MiterJoin);
    m_fullScreenButton->setPen(pen);
    m_fullScreenButton->setFlag(QGraphicsItem::ItemIsSelectable);

    connect(scene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()));

    scene->addItem(m_fullScreenButton);
#endif

    // setup slider control

    m_slider = new QSlider(Qt::Vertical, this);
    m_slider->setTickInterval(1);
    m_slider->setTickPosition(QSlider::TicksBothSides);
    m_slider->setMaximum(m_mapManager->maximumZoomLevel());
    m_slider->setMinimum(m_mapManager->minimumZoomLevel());
    m_slider->setSliderPosition(m_mapWidget->zoomLevel());

    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
    connect(m_mapWidget, SIGNAL(zoomLevelChanged(qreal)), this, SLOT(mapZoomLevelChanged(qreal)));

    // setup map type control

    QVBoxLayout *mapControlLayout = new QVBoxLayout();

    connect(m_mapWidget, SIGNAL(mapTypeChanged(QGraphicsGeoMap::MapType)), this, SLOT(mapTypeChanged(QGraphicsGeoMap::MapType)));

    QList<QGraphicsGeoMap::MapType> types = m_mapWidget->supportedMapTypes();
    for (int i = 0; i < types.size(); ++i) {
        QRadioButton *radio = new QRadioButton(this);

        switch (types.at(i)) {
            case QGraphicsGeoMap::StreetMap:
                radio->setText("Street");
                break;
            case QGraphicsGeoMap::SatelliteMapDay:
                radio->setText("Satellite");
                break;
            case QGraphicsGeoMap::SatelliteMapNight:
                radio->setText("Satellite - Night");
                break;
            case QGraphicsGeoMap::TerrainMap:
                radio->setText("Terrain");
                break;
            case QGraphicsGeoMap::NoMap:
                break;
        }

        if (types.at(i) == m_mapWidget->mapType())
            radio->setChecked(true);

        connect(radio, SIGNAL(toggled(bool)), this, SLOT(mapTypeToggled(bool)));

        m_mapControlButtons.append(radio);
        m_mapControlTypes.append(types.at(i));

        mapControlLayout->addWidget(radio);
    }

    // setup lat/lon control

    m_latitudeEdit = new QLineEdit();
    m_longitudeEdit = new QLineEdit();

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Latitude", m_latitudeEdit);
    formLayout->addRow("Longitude", m_longitudeEdit);

    m_captureCoordsButton = new QToolButton();
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    m_captureCoordsButton->setText("Get coords");
#else
    m_captureCoordsButton->setText("Capture coordinates");
#endif
    m_captureCoordsButton->setCheckable(true);

    connect(m_captureCoordsButton, SIGNAL(toggled(bool)), m_mapWidget, SLOT(setMouseClickCoordQuery(bool)));
    connect(m_mapWidget, SIGNAL(coordQueryResult(QGeoCoordinate)), this, SLOT(updateCoords(QGeoCoordinate)));

    m_setCoordsButton = new QPushButton();
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    m_setCoordsButton->setText("Set coords");
#else
    m_setCoordsButton->setText("Set coordinates");
#endif

    connect(m_setCoordsButton, SIGNAL(clicked()), this, SLOT(setCoordsClicked()));

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    buttonLayout->addWidget(m_captureCoordsButton);
    buttonLayout->addWidget(m_setCoordsButton);

    QVBoxLayout *coordControlLayout = new QVBoxLayout();
    coordControlLayout->addLayout(formLayout);
    coordControlLayout->addLayout(buttonLayout);

    QWidget *widget = new QWidget(this);

    QGridLayout *layout = new QGridLayout();

    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 0);
#if 1
    QGridLayout *topLayout = new QGridLayout();
    QGridLayout *bottomLayout = new QGridLayout();

    topLayout->setColumnStretch(0, 0);
    topLayout->setColumnStretch(1, 1);

    bottomLayout->setColumnStretch(0, 0);
    bottomLayout->setColumnStretch(1, 1);

    topLayout->addWidget(m_slider, 0, 0);
    topLayout->addWidget(m_qgv, 0, 1);

    bottomLayout->addLayout(mapControlLayout, 0, 0);
    bottomLayout->addLayout(coordControlLayout, 0, 1);

    layout->addLayout(topLayout,0,0);
    layout->addLayout(bottomLayout,1,0);
#else
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 1);

    layout->addWidget(m_slider, 0, 0);
    layout->addWidget(m_qgv, 0, 1);
    layout->addLayout(mapControlLayout, 1, 0);
    layout->addLayout(coordControlLayout, 1, 1);
#endif

    m_layout = layout;

    widget->setLayout(layout);
    setCentralWidget(widget);

    m_qgv->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(m_qgv, SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT(customContextMenuRequest(const QPoint&)));

    resize(652, 519);
}

void MainWindow::sliderValueChanged(int zoomLevel)
{
    m_mapWidget->setZoomLevel(zoomLevel);
}

void MainWindow::mapZoomLevelChanged(qreal zoomLevel)
{
    m_slider->setSliderPosition(qRound(zoomLevel));
}

void MainWindow::mapTypeToggled(bool checked)
{
    if (checked) {
        QRadioButton *button = qobject_cast<QRadioButton*>(sender());
        int index = m_mapControlButtons.indexOf(button);
        if (index != -1)
            m_mapWidget->setMapType(m_mapControlTypes.at(index));
    }

}

void MainWindow::mapTypeChanged(QGraphicsGeoMap::MapType type)
{
    int index = m_mapControlTypes.indexOf(type);
    if (index != -1)
        m_mapControlButtons.at(index)->setChecked(true);
}

void MainWindow::setCoordsClicked()
{
    bool ok = false;

    double lat = m_latitudeEdit->text().toDouble(&ok);

    if (!ok)
        return;

    double lon = m_longitudeEdit->text().toDouble(&ok);

    if (!ok)
        return;

    m_mapWidget->setCenter(QGeoCoordinate(lat, lon));
}

void MainWindow::updateCoords(const QGeoCoordinate &coords)
{
    if (!coords.isValid())
        return;

    m_latitudeEdit->setText(QString::number(coords.latitude()));
    m_longitudeEdit->setText(QString::number(coords.longitude()));

    m_captureCoordsButton->setChecked(false);
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
    m_qgv->setSceneRect(QRectF(QPointF(0.0, 0.0), m_qgv->size()));
    m_mapWidget->resize(m_qgv->size());
}

void MainWindow::showEvent(QShowEvent* event)
{
    m_qgv->setSceneRect(QRectF(QPointF(0.0, 0.0), m_qgv->size()));
    m_mapWidget->resize(m_qgv->size());
}

void MainWindow::createMenus()
{
    QAction* menuItem;
    QMenu* subMenuItem;
    m_popupMenu = new QMenu(this);

    /*

    // These are gread for testing, but not really what we want in
    // an example application.
    // We should probably creating a testing branch which tracks
    // the master branch and has various "test enabled" versions
    // of the examples at some point anyhow.

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

    menuItem = new QAction(tr("Clusters"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(demo3(bool)));

    */

    //**************************************************************
    subMenuItem = new QMenu(tr("Marker"), this);
    m_popupMenu->addMenu(subMenuItem);

    menuItem = new QAction(tr("Set marker"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(drawPixmap(bool)));

    menuItem = new QAction(tr("Remove markers"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(removePixmaps()));

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

    menuItem = new QAction(tr("Text"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(drawText(bool)));

    //**************************************************************
    subMenuItem = new QMenu(tr("Route"), this);
    m_popupMenu->addMenu(subMenuItem);

    menuItem = new QAction(tr("Calculate route"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(calcRoute(bool)));
}

#define MVTEST_MARK(pos) do { QGeoMapPixmapObject *marker = new QGeoMapPixmapObject(pos, QPoint(-(MARKER_WIDTH / 2), -MARKER_HEIGHT), m_markerIcon); m_mapWidget->addMapObject(marker); m_markerObjects.append(marker); } while (0)
#define MVTEST_MARK2(lat,lng) MVTEST_MARK(QGeoCoordinate(lat,lng))
#define MVTEST_RECT(topleft,bottomright) removePixmaps(); MVTEST_MARK(topleft); MVTEST_MARK(bottomright); drawRect(false);
#define MVTEST_RECT2(topleftlat,topleftlng,bottomrightlat,bottomrightlng) MVTEST_RECT(QGeoCoordinate(topleftlat,topleftlng),QGeoCoordinate(bottomrightlat,bottomrightlng))
void MainWindow::demo1(bool /*checked*/)
{
    MVTEST_MARK2(-30, 175);
    MVTEST_MARK2(-20, 170);
    MVTEST_MARK2(-25, -175);
    drawRect(false);
    drawPolygon(false);
    drawCircle(false);
}
void MainWindow::demo2(bool /*checked*/)
{
    const qreal density = 3; // 1 object each n degrees lat/lng

    int i = 0;
    for (qreal lat = -90 + density; lat < 90 - density; lat += density * 3) {
        for (qreal lng = -180 + density; lng < 180 - density; lng += density * 3) {
            MVTEST_RECT2(lat - density, lng - density, lat + density, lng + density);
            i++;
        }
    }

    qDebug("%i items added, %i items total.", i, m_mapWidget->mapObjects().count());

    QMessageBox *mb = new QMessageBox(QMessageBox::NoIcon, "MapViewer", QString::number(i) + " items");
    mb->open();

}
void MainWindow::demo3(bool /*checked*/)
{
    const qreal density = 10; // 1 cluster each n degrees lat/lng
    const qreal clusterSize = 2; // clusters extend for +/- n degrees lat/lng
    const qreal clusterDensity = 0.1*clusterSize; // 1 object each n degrees lat/lng (as part of a cluster)

    int i = 0;
    for (qreal latm = -90 + density; latm < 90 - density; latm += density * 3) {
        for (qreal lngm = -180 + density; lngm < 180 - density; lngm += density * 3) {
            for (qreal lat = latm-clusterSize+clusterDensity; lat < latm+clusterSize-clusterDensity; lat += clusterDensity * 3) {
                for (qreal lng = lngm-clusterSize+clusterDensity; lng < lngm+clusterSize-clusterDensity; lng += clusterDensity * 3) {
                    MVTEST_RECT2(lat - clusterDensity, lng - clusterDensity, lat + clusterDensity, lng + clusterDensity);
                    i++;
                }
            }
        }
    }

    qDebug("%i items added, %i items total.", i, m_mapWidget->mapObjects().count());

    QMessageBox *mb = new QMessageBox(QMessageBox::NoIcon, "MapViewer", QString::number(i) + " items");
    mb->open();
}

void MainWindow::drawRect(bool /*checked*/)
{
    if (m_markerObjects.count() < 2)  return;
    QGeoMapPixmapObject* p1 = m_markerObjects.at(0);
    QGeoMapPixmapObject* p2 = m_markerObjects.at(1);
    QPen pen(Qt::white);
    pen.setWidth(2);
    pen.setCosmetic(true);
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

    for (int i = 0; i < m_markerObjects.size(); i++) {
        QGeoMapPixmapObject* p = m_markerObjects.at(i);
        path.append(p->coordinate());
    }

    QPen pen(Qt::white);
    pen.setWidth(2);
    pen.setCosmetic(true);
    QGeoMapPolylineObject *polyline = new QGeoMapPolylineObject();
    polyline->setPen(pen);
    polyline->setPath(path);
    m_mapWidget->addMapObject(polyline);
}

void MainWindow::drawPolygon(bool /*checked*/)
{
    QList<QGeoCoordinate> path;

    for (int i = 0; i < m_markerObjects.size(); i++) {
        QGeoMapPixmapObject* p = m_markerObjects.at(i);
        path.append(p->coordinate());
    }

    QPen pen(Qt::white);
    pen.setWidth(2);
    pen.setCosmetic(true);
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
    if (m_markerObjects.count() < 1) return;

    QGeoMapPixmapObject* p1 = m_markerObjects.at(0);

    // center of the circle
    QGeoCoordinate center = p1->coordinate();

    // the circle's radius, in meters, defaults to 3000 km
    qreal radius = 3000000;

    // if a 2nd marker object is given, evaluate its distance to the first one to get the circle's radius.
    if (m_markerObjects.count() >= 2) {
        QGeoMapPixmapObject* p2 = m_markerObjects.at(1);

        radius = center.distanceTo(p2->coordinate());
    }

    QPen pen(Qt::white);
    pen.setWidth(2);
    pen.setCosmetic(true);
    QGeoMapCircleObject *circle = new QGeoMapCircleObject(center, radius);
    circle->setPen(pen);
    QColor fill(Qt::black);
    fill.setAlpha(65);
    circle->setBrush(QBrush(fill));
    m_mapWidget->addMapObject(circle);

    m_mapWidget->lastCircle = circle;
}

void MainWindow::drawText(bool /*checked*/)
{
    if (m_markerObjects.count() < 1) return;

    QGeoMapPixmapObject* p1 = m_markerObjects.at(0);

    // start of the text
    QGeoCoordinate start = p1->coordinate();

    QGeoMapTextObject *text = new QGeoMapTextObject(start, QString("text"));

    QColor fill(Qt::black);
    text->setBrush(QBrush(fill));
    m_mapWidget->addMapObject(text);
}

void MainWindow::drawPixmap(bool /*checked*/)
{
    QGeoCoordinate coordinate = m_mapWidget->screenPositionToCoordinate(m_lastClicked);

    static int markerIndex = 0;
    markerIndex++;

    const QString & indexString = QString::number(markerIndex);

    QGeoMapPixmapObject *marker;
    //marker = new QGeoMapPixmapObject(coordinate, QPoint(-(MARKER_WIDTH / 2), -MARKER_HEIGHT), m_markerIcon);

    //switch (rand()%3) {
    //switch ((markerIndex-1)%3) {
    switch (0) {
        case 0:
            marker = QGeoMapPixmapObject::createStandardMarker(coordinate, SHAPE_BALLOON, indexString, QPen(), QPen(QColor(Qt::white)), QBrush(QColor(0, 0, 136)));
            break;
        case 1:
            marker = QGeoMapPixmapObject::createStandardMarker(coordinate, SHAPE_STAR, indexString, QPen(QColor(0, 0, 136)), QPen(QColor(Qt::white)), QBrush(QColor(0, 0, 136, 64)));
            break;
        case 2:
            marker = QGeoMapPixmapObject::createStandardMarker(coordinate, SHAPE_STAR, indexString, QPen(QColor(0, 0, 136, 128)), QPen(QColor(Qt::white)), QBrush(QColor(0, 128, 255, 32)));
            break;
    }
    m_mapWidget->addMapObject(marker);
    m_markerObjects.append(marker);
}

void MainWindow::removePixmaps()
{
    while (m_markerObjects.size() > 0) {
        QGeoMapPixmapObject *marker = m_markerObjects.takeFirst();
        m_mapWidget->removeMapObject(marker);
        marker->deleteLater();
    }
}

void MainWindow::customContextMenuRequest(const QPoint& point)
{
    m_lastClicked = point;
#ifdef Q_OS_SYMBIAN
    // Work around a bug with QGraphicsView's customContextMenuRequested signal on Symbian
    // TODO: adjust this #ifdef, so it doesn't break with versions that fix the bug
    m_lastClicked -= m_qgv->geometry().topLeft();
#endif

    if (focusWidget() == m_qgv) {

        if (!m_popupMenu)
            createMenus();

        m_popupMenu->popup(m_qgv->mapToGlobal(m_lastClicked));
    }
}

void MainWindow::createPixmapIcon()
{
    m_markerIcon = QPixmap(MARKER_WIDTH, MARKER_HEIGHT);
    m_markerIcon.fill(Qt::transparent);
    QPainter painter(&m_markerIcon);

    QPointF p1(MARKER_WIDTH / 2, MARKER_HEIGHT - 1);
    QPointF p2(MARKER_WIDTH / 2, MARKER_HEIGHT - 1 - MARKER_PIN_LEN);
    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCosmetic(true);
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
    if (m_markerObjects.count() < 2)
        return;

    QList<QGeoCoordinate> waypoints;

    for (int i = 0; i < m_markerObjects.count(); i++) {
        QGeoMapPixmapObject* p = m_markerObjects.at(i);
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
    pen.setCosmetic(true);
    pen.setCapStyle(Qt::RoundCap);
    route->setPen(pen);
    m_mapWidget->addMapObject(route);
}

void MainWindow::selectObjects()
{
    QList<QGeoMapObject*> allObjects = m_mapWidget->mapObjects();
    for (int i = 0; i < allObjects.size(); ++i)
        allObjects[i]->setSelected(false);

    if (m_markerObjects.count() < 2)
        return;

    QGeoMapPixmapObject* bottomRight = m_markerObjects.takeLast();
    QGeoMapPixmapObject* topLeft = m_markerObjects.takeLast();
    m_mapWidget->removeMapObject(topLeft);
    m_mapWidget->removeMapObject(bottomRight);

    QList<QGeoMapObject*> selectedObjects = m_mapWidget->mapObjectsInScreenRect(
                                           QRectF(m_mapWidget->coordinateToScreenPosition(topLeft->coordinate()),
                                                  m_mapWidget->coordinateToScreenPosition(bottomRight->coordinate()))
                                       );
    for (int i = 0; i < selectedObjects.size(); ++i)
        selectedObjects[i]->setSelected(true);
}
