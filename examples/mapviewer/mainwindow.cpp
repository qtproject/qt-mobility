/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "mapwidget.h"

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
#include <QDialog>
#include <QProcessEnvironment>
#include <QUrl>
#include <QSvgRenderer>

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
#include <QSignalMapper>

#include <qnetworkconfigmanager.h>
#include <qgeoboundingcircle.h>
#include <qgeocoordinate.h>
#include <qgeomaprectangleobject.h>
#include <qgeomappixmapobject.h>
#include <qgeomappolylineobject.h>
#include <qgeomappolygonobject.h>
#include <qgeomaprouteobject.h>
#include <qgeomaptextobject.h>
#include <qgeomapcircleobject.h>
#include <qgeorouterequest.h>
#include <qgeoboundingbox.h>

QTM_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        m_serviceProvider(0),
        m_mapWidget(0),
        m_popupMenu(0),
        m_controlsVisible(true)
{
    setWindowTitle(tr("Map Viewer Demo"));

    // initial ui setup
    // setup graphics view containing map widget

    QGraphicsScene* scene = new QGraphicsScene(this);
    m_qgv = new QGraphicsView(scene, this);
    m_qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_qgv->setVisible(true);
    m_qgv->setInteractive(true);
    m_qgv->setMouseTracking(true);
    m_qgv->viewport()->setMouseTracking(true);

    // setup slider control

    m_slider = new QSlider(Qt::Vertical, this);
    m_slider->setTickInterval(1);
    m_slider->setTickPosition(QSlider::TicksBothSides);

    // setup lat/lon control

    m_latitudeEdit = new QLineEdit();
    m_longitudeEdit = new QLineEdit();

    QFormLayout *latitudeLayout = new QFormLayout();
    QFormLayout *longitudeLayout = new QFormLayout();

#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    latitudeLayout->addRow(tr("Lat:"), m_latitudeEdit);
    longitudeLayout->addRow(tr("Lng:"), m_longitudeEdit);
#else
    latitudeLayout->addRow(tr("Latitude"), m_latitudeEdit);
    longitudeLayout->addRow(tr("Longitude"), m_longitudeEdit);
#endif

    m_setCoordsButton = new QPushButton();
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    m_setCoordsButton->setText(tr("Goto coords"));
#else
    m_setCoordsButton->setText(tr("Go to coordinates"));
#endif

    m_searchEdit = new QLineEdit();

    m_searchButton = new QPushButton();
    m_searchButton->setText(tr("Search"));

    QGridLayout *searchLayout = new QGridLayout();
    searchLayout->setColumnStretch(0, 1);
    searchLayout->setColumnStretch(1, 0);
    searchLayout->addWidget(m_searchEdit, 0, 0);
    searchLayout->addWidget(m_searchButton, 0, 1);

    QGridLayout *coordControlLayout = new QGridLayout();
    coordControlLayout->addLayout(latitudeLayout, 0, 0);
    coordControlLayout->addLayout(longitudeLayout, 0, 1);
    coordControlLayout->addWidget(m_setCoordsButton, 1, 1, 1, 2);

    QMenu * coordsMenu = new QMenu(tr("Coordinates"), this);

    m_captureCoordsAction = new QAction(tr("Capture"), this);
    coordsMenu->addAction(m_captureCoordsAction);
    m_captureCoordsAction->setCheckable(true);

    // Build coordinates dialog

    m_coordControlDialog = new QDialog(this);
    m_coordControlDialog->setLayout(coordControlLayout);

    QAction * setCoordsAction = new QAction(tr("Go to"), this);
    coordsMenu->addAction(setCoordsAction);
    connect(m_setCoordsButton, SIGNAL(clicked()), m_coordControlDialog, SLOT(hide()));
    connect(setCoordsAction, SIGNAL(triggered(bool)), m_coordControlDialog, SLOT(show()));

    // Setup map type controls

    QVector<QString> mapTypeNames;

    m_mapControlTypes.append(QGraphicsGeoMap::StreetMap);         mapTypeNames.append(tr("Street"));
    m_mapControlTypes.append(QGraphicsGeoMap::SatelliteMapDay);   mapTypeNames.append(tr("Satellite"));
    m_mapControlTypes.append(QGraphicsGeoMap::SatelliteMapNight); mapTypeNames.append(tr("Satellite - Night"));
    m_mapControlTypes.append(QGraphicsGeoMap::TerrainMap);        mapTypeNames.append(tr("Terrain"));


    QMenu * mapTypeMenu = new QMenu(tr("Map type"), this);

    for (int i = 0; i < m_mapControlTypes.size(); ++i) {
        QAction* action = new QAction(mapTypeNames[i], this);
        action->setCheckable(true);

        mapTypeMenu->addAction(action);
        m_mapControlActions.append(action);
    }

    QGridLayout *layout = new QGridLayout();
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 0);

    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 1);

    layout->addWidget(m_slider, 0, 0, 2, 1);
    layout->addWidget(m_qgv, 0, 1);
    layout->addLayout(searchLayout, 1, 1);

    m_layout = layout;

    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);
    setCentralWidget(widget);

    menuBar()->addMenu(mapTypeMenu);
    menuBar()->addMenu(coordsMenu);

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

    resize(652, 519);
}

MainWindow::~MainWindow()
{
    delete m_serviceProvider;
}

void MainWindow::setupUi()
{
    // setup exit menu for devices
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    QAction* exitAction = new QAction(tr("Exit"), this);
    menuBar()->addAction(exitAction);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
#endif

    createPixmapIcon();

    m_mapWidget = new MapWidget(m_mapManager);
    m_qgv->scene()->addItem(m_mapWidget);
    //m_mapWidget->setCenter(QGeoCoordinate(52.5,13.0));
    //temporary change for dateline testing
    m_mapWidget->setCenter(QGeoCoordinate(-27.0, 152.0));
    m_mapWidget->setZoomLevel(5);

#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    // make full-screen button

    QPainterPath path;
    const int gaps = 5;
    const int innerwidth = 40;
    const int innerheight = 20;
    const int smallbox = 12;
    path.addRect(0, 0, innerwidth + smallbox + 3*gaps, innerheight + smallbox + 3*gaps);
    path.addRect(smallbox + 2*gaps, gaps, innerwidth, innerheight);
    path.addRect(gaps, 2*gaps + innerheight, smallbox, smallbox);

    m_fullScreenButton = new QGraphicsPathItem(path); // TODO: make member
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(0, 0, 0, 128));
    pen.setJoinStyle(Qt::MiterJoin);
    m_fullScreenButton->setPen(pen);
    m_fullScreenButton->setFlag(QGraphicsItem::ItemIsSelectable);

    connect(m_qgv->scene(), SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()));

    m_qgv->scene()->addItem(m_fullScreenButton);
#endif

    m_slider->setMaximum(m_mapManager->maximumZoomLevel());
    m_slider->setMinimum(m_mapManager->minimumZoomLevel());
    m_slider->setSliderPosition(m_mapWidget->zoomLevel());

    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
    connect(m_mapWidget, SIGNAL(zoomLevelChanged(qreal)), this, SLOT(mapZoomLevelChanged(qreal)));

    // setup map type control
    connect(m_mapWidget, SIGNAL(mapTypeChanged(QGraphicsGeoMap::MapType)), this, SLOT(mapTypeChanged(QGraphicsGeoMap::MapType)));

    QSignalMapper * mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(mapTypeToggled(int)));

    QList<QGraphicsGeoMap::MapType> types = m_mapWidget->supportedMapTypes();
    for (int controlIndex = 0; controlIndex < m_mapControlTypes.size(); ++controlIndex) {
        QAction *action = m_mapControlActions.at(controlIndex);
        int supportedTypeIndex = types.indexOf(m_mapControlTypes[controlIndex]);

        if (supportedTypeIndex == -1) {
            action->setEnabled(false);
        } else {
            connect(action, SIGNAL(triggered(bool)), mapper, SLOT(map()));
            mapper->setMapping(action, m_mapControlTypes[controlIndex]);

            action->setEnabled(true);
            action->setChecked(m_mapControlTypes[controlIndex] == m_mapWidget->mapType());
        }

    }

    connect(m_captureCoordsAction, SIGNAL(toggled(bool)), m_mapWidget, SLOT(setMouseClickCoordQuery(bool)));
    connect(m_mapWidget, SIGNAL(coordQueryResult(QGeoCoordinate)), this, SLOT(updateCoords(QGeoCoordinate)));
    connect(m_mapWidget, SIGNAL(coordQueryResult(QGeoCoordinate)), m_coordControlDialog, SLOT(show()));
    connect(m_setCoordsButton, SIGNAL(clicked()), this, SLOT(setCoordsClicked()));
    connect(m_searchEdit, SIGNAL(returnPressed()), this, SLOT(searchClicked()));
    connect(m_searchButton, SIGNAL(clicked()), this, SLOT(searchClicked()));

    m_qgv->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(m_qgv, SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT(customContextMenuRequest(const QPoint&)));
    resizeEvent(0);
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
            } else {
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
    resizeEvent(0);
}

void MainWindow::sliderValueChanged(int zoomLevel)
{
    m_mapWidget->setZoomLevel(zoomLevel);
}

void MainWindow::mapZoomLevelChanged(qreal zoomLevel)
{
    m_slider->setSliderPosition(qRound(zoomLevel));
}

void MainWindow::mapTypeToggled(int type)
{
    m_mapWidget->setMapType((QGraphicsGeoMap::MapType)type);
}

void MainWindow::mapTypeChanged(QGraphicsGeoMap::MapType type)
{
    for (int i = 0; i < m_mapControlTypes.size(); ++i) {
        QAction *action = m_mapControlActions[i];
        action->setChecked(m_mapControlTypes[i] == type);
    }
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

    m_captureCoordsAction->setChecked(false);
}

void MainWindow::setProvider(QString providerId)
{
    if (m_serviceProvider)
        delete m_serviceProvider;
    m_serviceProvider = new QGeoServiceProvider(providerId);
    if (m_serviceProvider->error() != QGeoServiceProvider::NoError) {
        QMessageBox::information(this, tr("MapViewer Example"), tr(
                                     "Unable to find the %1 geoservices plugin.").arg(providerId));
        qApp->quit();
        return;
    }

    m_mapManager = m_serviceProvider->mappingManager();
    m_routingManager = m_serviceProvider->routingManager();
    m_searchManager = m_serviceProvider->searchManager();
}

void MainWindow::searchClicked()
{
    QGeoSearchReply *reply = m_searchManager->search(m_searchEdit->text());

    QObject::connect(reply, SIGNAL(finished()), this,
                     SLOT(searchReplyFinished()));
    QObject::connect(reply,
                     SIGNAL(error(QGeoSearchReply::Error, QString)), this,
                     SLOT(resultsError(QGeoSearchReply::Error, QString)));

    m_qgv->setFocus();
}

void MainWindow::searchReplyFinished()
{
    QGeoSearchReply* reply = static_cast<QGeoSearchReply *>(sender());

    if (reply->error() != QGeoSearchReply::NoError) {
        // Errors are handled in a different slot (resultsError)
        return;
    }

    QList<QGeoPlace> places = reply->places();
    if (places.length() == 0) {
        (new QMessageBox(
             QMessageBox::Information,
             tr("MapViewer Example"),
             tr("Search did not find anything."),
             0,
             this
         ))->show();
    } else {

        for (int i = 0; i < places.length(); ++i) {
            QGeoPlace & place = places[i];
            QGeoMapPixmapObject *marker
            = new QGeoMapPixmapObject(place.coordinate(),
                                      m_markerOffset,
                                      m_markerIcon);
            m_mapWidget->addMapObject(marker);
            m_markerObjects.append(marker); // TODO: add to different marker list, clear markers from list before searching
        }

        if (places.length() == 1) {
            m_mapWidget->setCenter(places[0].coordinate());
        } else {
            QGeoBoundingBox bbox(places.at(0).coordinate(), places.at(0).coordinate());
            for (int i = 1; i < places.length(); ++i) {
                bbox |= QGeoBoundingBox(places.at(i).coordinate(), places.at(i).coordinate());
            }
            m_mapWidget->fitInViewport(bbox);
        }

        /*
        QGeoBoundingArea * viewport = reply->viewport();
        if (viewport) {
            if (viewport->type() == QGeoBoundingArea::BoxType) {
                m_mapWidget->fitInViewport(*static_cast<QGeoBoundingBox *>(viewport));
                reply->deleteLater();
                return;
            }
            else if (viewport->type() == QGeoBoundingArea::CircleType) {
                m_mapWidget->setCenter(static_cast<QGeoBoundingCircle *>(viewport)->center());
                reply->deleteLater();
                return;
            }
        }
        m_mapWidget->setCenter(places[0].coordinate());
        */
    }

    disconnect(reply, SIGNAL(finished()), this,
               SLOT(searchReplyFinished()));
    disconnect(reply,
               SIGNAL(error(QGeoSearchReply::Error, QString)), this,
               SLOT(resultsError(QGeoSearchReply::Error, QString)));
    reply->deleteLater();
}

void MainWindow::resultsError(QGeoSearchReply::Error errorCode, QString errorString)
{
    QObject* reply = static_cast<QGeoSearchReply *>(sender());

    (new QMessageBox(
         QMessageBox::Information,
         tr("MapViewer Example"),
         tr("Error #%1 while trying to process your search query.\n\"%2\"").arg(errorCode).arg(errorString),
         0,
         this
     ))->show();

    disconnect(reply, SIGNAL(finished()), this,
               SLOT(searchReplyFinished()));
    disconnect(reply,
               SIGNAL(error(QGeoSearchReply::Error, QString)), this,
               SLOT(resultsError(QGeoSearchReply::Error, QString)));
    reply->deleteLater();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    m_qgv->setSceneRect(QRectF(QPointF(0.0, 0.0), m_qgv->size()));
    if (m_mapWidget)
        m_mapWidget->resize(m_qgv->size());
}

void MainWindow::showEvent(QShowEvent* event)
{
    resizeEvent(0);
}

void MainWindow::createMenus()
{
    QAction* menuItem;
    QMenu* subMenuItem;
    m_popupMenu = new QMenu(this);
    m_popupMenuMapObject = new QMenu(this);

    //**************************************************************

    // MapObject-specific context menu. The map object is in m_lastClickedMapObject.
    subMenuItem = new QMenu(tr("MapObject"), this);
    m_popupMenuMapObject->addMenu(subMenuItem);

    menuItem = new QAction(tr("Remove"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(removeMapObject()));

    //**************************************************************

    /*

    // These are gread for testing, but not really what we want in
    // an example application.
    // We should probably creating a testing branch which tracks
    // the master branch and has various "test enabled" versions
    // of the examples at some point anyhow.

    subMenuItem = new QMenu(tr("Spawn stuff"), this);
    m_popupMenu->addMenu(subMenuItem);
    m_popupMenuMapObject->addMenu(subMenuItem);

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
    m_popupMenuMapObject->addMenu(subMenuItem);

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
    m_popupMenuMapObject->addMenu(subMenuItem);

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
    m_popupMenuMapObject->addMenu(subMenuItem);

    menuItem = new QAction(tr("Calculate route"), this);
    subMenuItem->addAction(menuItem);
    QObject::connect(menuItem, SIGNAL(triggered(bool)),
                     this, SLOT(calcRoute(bool)));
}

#define MVTEST_MARK(pos) do { QGeoMapPixmapObject *marker = new QGeoMapPixmapObject(pos, m_markerOffset, m_markerIcon); m_mapWidget->addMapObject(marker); m_markerObjects.append(marker); } while (0)
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
    const qreal clusterDensity = 0.1 * clusterSize; // 1 object each n degrees lat/lng (as part of a cluster)

    int i = 0;
    for (qreal latm = -90 + density; latm < 90 - density; latm += density * 3) {
        for (qreal lngm = -180 + density; lngm < 180 - density; lngm += density * 3) {
            for (qreal lat = latm - clusterSize + clusterDensity; lat < latm + clusterSize - clusterDensity; lat += clusterDensity * 3) {
                for (qreal lng = lngm - clusterSize + clusterDensity; lng < lngm + clusterSize - clusterDensity; lng += clusterDensity * 3) {
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
    QGeoMapPixmapObject *marker = new QGeoMapPixmapObject(m_mapWidget->screenPositionToCoordinate(m_lastClicked),
            m_markerOffset, m_markerIcon);
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
void MainWindow::removeMapObject()
{
    m_mapWidget->removeMapObject(m_lastClickedMapObject);
    if (m_lastClickedMapObject->type() == QGeoMapObject::PixmapType)
        m_markerObjects.removeAll(static_cast<QGeoMapPixmapObject*>(m_lastClickedMapObject));
    m_lastClickedMapObject->deleteLater();
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

        QList<QGeoMapObject*> objectsAtCursor = m_mapWidget->mapObjectsAtScreenPosition(m_lastClicked);
        if (objectsAtCursor.isEmpty()) {
            // No objects, display the default context menu
            m_lastClickedMapObject = 0;
            m_popupMenu->popup(m_qgv->mapToGlobal(m_lastClicked));
        }
        else {
            // There is an object here, store it and open the appropriate context menu
            m_lastClickedMapObject = objectsAtCursor.last();
            m_popupMenuMapObject->popup(m_qgv->mapToGlobal(m_lastClicked));
        }

    }
}

enum MapShapeType {
    SHAPE_BALLOON = 0,
    SHAPE_STAR = 1
};

QPair<QPixmap, QPoint> makeStandardMarker(MapShapeType shapeType, const QString & text, const QPen & pen, const QPen & textPen, const QBrush & brush)
{
    QString svgTemplate;
    QPoint offset;
    switch (shapeType) {
        case SHAPE_BALLOON:
            svgTemplate = "<svg>"
                          "<path "
                          "style=\"fill:#000000;fill-opacity:.4\" "
                          "d=\"m 18.948,33.432051 c 0,1.41694 -2.238,2.567949 -5,2.567949 -2.762,0 -5,-1.151009 -5,-2.567949 0,-1.416939 2.238,-2.567948 5,-2.567948 2.762,-0.002 5,1.149994 5,2.567948\" "
                          "/>"
                          "<path "
                          "style=\"fill:#666\" "
                          "d=\"M 14 0 C 6.28 0 0 6.3348796 0 14.125 C 0 16.209809 0.4425 18.209911 1.3125 20.09375 L 1.84375 21.125 C 2.96275 23.084964 12.2565 32.190555 13.3125 33.21875 L 14 33.875 L 14.6875 33.21875 C 15.7435 32.188525 25.026 23.109909 26.125 21.1875 L 26.65625 20.15625 C 27.54825 18.225721 28 16.204734 28 14.125 C 28.001 6.3348796 21.72 0 14 0 z M 14 0.90625 C 21.175 0.90625 27 6.7640687 27 14 C 27 15.982294 26.55825 17.873541 25.78125 19.5625 L 25.28125 20.53125 C 24.22125 22.386669 14 32.375 14 32.375 C 14 32.375 3.77875 22.387684 2.71875 20.53125 L 2.21875 19.5625 C 1.43975 17.875571 1 15.982294 1 14 C 1 6.7640687 6.823 0.90625 14 0.90625 z \" "
                          "/>"
                          "<path "
                          "style=\"fill:#fff\" "
                          "d=\"M 14 0.90625 C 6.823 0.90625 1 6.7640687 1 14 C 1 15.982294 1.43975 17.875571 2.21875 19.5625 L 2.71875 20.53125 C 3.77875 22.387684 14 32.375 14 32.375 C 14 32.375 24.22125 22.386669 25.28125 20.53125 L 25.78125 19.5625 C 26.55825 17.873541 27 15.982294 27 14 C 27 6.7640687 21.175 0.90625 14 0.90625 z M 14 1.90625 C 20.616 1.90625 26 7.3294234 26 14 C 25.910217 16.115917 25.491329 18.196844 24.40625 20.03125 C 23.776565 21.248455 17.937 27.075427 14 30.9375 C 10.063 27.075427 4.206579 21.245609 3.59375 20.0625 C 2.614452 18.171896 1.90079 16.186259 2 14 C 2 7.3294234 7.383 1.90625 14 1.90625 z \" "
                          "/>"
                          "<path "
                          "style=\"fill:%brush%;fill-opacity:%brushOpacity%\" "
                          "d=\"m 13.996268,1.9105251 c -6.617,0 -12.001,5.4261873 -12.001,12.0967639 -0.09921,2.186259 0.631702,4.174018 1.611,6.064622 0.612829,1.183109 6.453,7.017707 10.39,10.87978 3.937,-3.862073 9.777315,-9.70013 10.407,-10.917335 1.085079,-1.834406 1.503217,-3.91115 1.593,-6.027067 0,-6.6705766 -5.384,-12.0967639 -12,-12.0967639 l 0,0 z\" "
                          "/>"
                          "<path style=\"fill:#ffffff;fill-opacity:0.7\" "
                          "d=\"M 20.968528,6.089997 C 17.785074,3.5240085 13.725682,2.5379787 11.919627,3.8924751 11.436514,4.2548035 11.151528,4.750748 11.073071,5.3665525 c 4.1092,0.6284223 7.111478,2.6511761 8.521564,4.4853881 2.388133,3.1064364 1.967904,3.9515754 1.967904,3.9515754 0.774274,-0.0387 1.422004,-0.242256 1.904023,-0.603757 1.806055,-1.354497 0.684944,-4.5441591 -2.498034,-7.109762 l 0,0 z\" "
                          "/>"
                          "<text "
                          "x=\"14\" y=\"19\" "
                          "font-size=\"10pt\" font-family=\"arial\" font-weight=\"bold\" text-anchor=\"middle\" "
                          "fill=\"%textPen%\" "
                          "textContent = \"%textContent%\""
                          ">"
                          "%text%"
                          "</text>"
                          "</svg>";
            offset = QPoint(-14, -34);
            break;

        case SHAPE_STAR:
            svgTemplate = "<svg><path style=\"fill:%brush%;fill-opacity:%brushOpacity%\" d=\"M 16.00663,.0000019037492 C 7.1692812,.0000019037492 0,7.1692841 0,16.006632 0,24.843981 7.1692812,32 16.00663,32 24.84398,32 32,24.843981 32,16.006632 32,7.1692841 24.84398,.0000019037492 16.00663,.0000019037492 z M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><path style=\"fill:%pen%;fill-opacity:%penOpacity%\" d=\"M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><text x=\"16\" y=\"20\" font-size=\"10pt\" font-family=\"arial\" font-weight=\"bold\" text-anchor=\"middle\" fill=\"%textPen%\" textContent = \"%textContent%\">%text%</text></svg>";
            offset = QPoint(-16, -16);
            break;
    }

    svgTemplate.replace("%text%", text, Qt::CaseSensitive);
    svgTemplate.replace("%pen%", pen.color().name(), Qt::CaseSensitive);
    svgTemplate.replace("%penOpacity%", QString::number(pen.color().alpha() / 255.0), Qt::CaseSensitive);
    svgTemplate.replace("%textPen%", textPen.color().name(), Qt::CaseSensitive);
    svgTemplate.replace("%brush%", brush.color().name(), Qt::CaseSensitive);
    svgTemplate.replace("%brushOpacity%", QString::number(brush.color().alpha() / 255.0), Qt::CaseSensitive);
    svgTemplate.replace("%textContent%", "", Qt::CaseSensitive);

    QSvgRenderer renderer(svgTemplate.toAscii());
    QPixmap pixmap(renderer.defaultSize());
    pixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter(&pixmap);
    renderer.render(&painter);

    return qMakePair(pixmap, offset);
}

void MainWindow::createPixmapIcon()
{
    QPair<QPixmap, QPoint> markerPair = makeStandardMarker(SHAPE_BALLOON, "", QPen(), QPen(QColor(Qt::white)), QBrush(QColor(0, 0, 136)));
    m_markerIcon = markerPair.first;
    m_markerOffset = markerPair.second;
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

void MainWindow::networkSessionOpened()
{
    QString urlEnv = QProcessEnvironment::systemEnvironment().value("http_proxy");
    if (!urlEnv.isEmpty()) {
        QUrl url = QUrl(urlEnv, QUrl::TolerantMode);
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(url.host());
        proxy.setPort(url.port(8080));
        QNetworkProxy::setApplicationProxy(proxy);
    } else
        QNetworkProxyFactory::setUseSystemConfiguration(true);

    setProvider("nokia");
    // finalize ui setup
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
