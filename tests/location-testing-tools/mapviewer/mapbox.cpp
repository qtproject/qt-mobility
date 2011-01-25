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

#include "mapbox.h"
#include "geomap.h"

#include <qgeoserviceprovider.h>
#include <qgeomappingmanager.h>
#include <qgeocoordinate.h>
#include <qgeomaprectangleobject.h>
#include <qgeomapcircleobject.h>
#include <qgeomappolylineobject.h>
#include <qgeomappolygonobject.h>
#include <qgeomaprouteobject.h>

#include <qgeoroutingmanager.h>
#include <qgeoroutereply.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGraphicsView>
#include <QGraphicsSceneContextMenuEvent>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QTimer>
#include <QSvgRenderer>
#include <QMenu>
#include <QSignalMapper>
#include <QLabel>

MapBox::MapBox(QWidget * parent) :
    QWidget(parent),
    m_qgv(0),
    m_scene(0),
    m_mapWidget(0),
    m_serviceProvider(0),
    m_mapManager(0),
    m_connectivityMode(QGraphicsGeoMap::HybridMode),
    m_popupMenu(0),
    m_popupMenuMapObject(0),
    m_displayMode(DisplayNone),
    m_optionsWidget(0)
{
    m_scene = new QGraphicsScene(this);

    m_qgv = new QGraphicsView(m_scene, this);
    m_qgv->setOptimizationFlag(QGraphicsView::IndirectPainting, true);
    m_qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_qgv->setFrameShape(QFrame::NoFrame);

    m_optionsButton = new QPushButton("Options", m_qgv);
    m_optionsButton->resize(m_optionsButton->sizeHint());
    connect(m_optionsButton, SIGNAL(clicked()), this, SLOT(showOptions()));

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);
    setDisplayMode(DisplayMap);

    createDialogs();
    createPixmapIcon();
}

MapBox::~MapBox()
{
}

void MapBox::createDialogs()
{
    // Build coordinates dialog
    m_latitudeEdit = new QLineEdit();
    m_longitudeEdit = new QLineEdit();

    QFormLayout * latitudeLayout = new QFormLayout();
    QFormLayout * longitudeLayout = new QFormLayout();

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

    QGridLayout * coordControlLayout = new QGridLayout();
    coordControlLayout->addLayout(latitudeLayout, 0, 0);
    coordControlLayout->addLayout(longitudeLayout, 0, 1);
    coordControlLayout->addWidget(m_setCoordsButton, 1, 1, 1, 2);

    m_coordControlDialog = new QDialog(this);
    m_coordControlDialog->setWindowTitle("Coordinates");
    m_coordControlDialog->setLayout(coordControlLayout);

    connect(m_setCoordsButton, SIGNAL(clicked()), this, SLOT(gotoCoordsClicked()));
    connect(m_setCoordsButton, SIGNAL(clicked()), m_coordControlDialog, SLOT(hide()));
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

void MapBox::createPixmapIcon()
{
    if (!m_markerIcon.isNull())
        return;

    QPair<QPixmap, QPoint> markerPair = makeStandardMarker(SHAPE_BALLOON, "", QPen(), QPen(QColor(Qt::white)), QBrush(QColor(0, 0, 136)));
    m_markerIcon = markerPair.first;
    m_markerOffset = markerPair.second;
}


void MapBox::setProvider(const QString & providerId)
{
    if (m_providerId == providerId)
        return;

    m_providerId = providerId;

    createProvider();
}

void MapBox::createProvider()
{
    delete m_serviceProvider;

    qDebug("Creating provider %s", m_providerId.toLocal8Bit().constData());
    m_serviceProvider = new QGeoServiceProvider(m_providerId, m_parameters);

    if (m_serviceProvider->error() != QGeoServiceProvider::NoError) {
        QMessageBox::information(this, tr("MapViewer Example"), tr(
            "Unable to find the %1 geoservices plugin.").arg(m_providerId));
        qApp->quit();
    }

    m_mapManager = m_serviceProvider->mappingManager();

    createMapWidget();
}

void MapBox::createMapWidget()
{
    delete m_mapWidget;

    m_mapWidget = new GeoMap(m_mapManager);

    m_mapWidget->setConnectivityMode(m_connectivityMode);
    m_mapWidget->setCenter(QGeoCoordinate(52.5, 13.0));
    m_mapWidget->setZoomLevel(5);

    m_scene->addItem(m_mapWidget);

    connect(m_mapWidget, SIGNAL(contextMenu(QGraphicsSceneContextMenuEvent *, QGeoMapObject *)),
            this, SLOT(mapContextMenu(QGraphicsSceneContextMenuEvent *, QGeoMapObject *)));

    createOptionsDialog();
}

void MapBox::setConnectivityMode(QGraphicsGeoMap::ConnectivityMode mode)
{
    m_connectivityMode = mode;
    if (m_mapWidget)
        m_mapWidget->setConnectivityMode(m_connectivityMode);
}

void MapBox::createOptionsDialog()
{
    if (m_optionsWidget)
        delete m_optionsWidget;
    m_optionsWidget = new QWidget(this);
    QGridLayout * optionsLayout = new QGridLayout;

    QVector<QString> mapTypeNames;
    QVector<QGraphicsGeoMap::MapType> mapControlTypes;

    mapControlTypes.append(QGraphicsGeoMap::StreetMap);         mapTypeNames.append(tr("Street"));
    mapControlTypes.append(QGraphicsGeoMap::SatelliteMapDay);   mapTypeNames.append(tr("Satellite"));
    mapControlTypes.append(QGraphicsGeoMap::SatelliteMapNight); mapTypeNames.append(tr("Satellite - Night"));
    mapControlTypes.append(QGraphicsGeoMap::TerrainMap);        mapTypeNames.append(tr("Terrain"));

    {
        // map types
        QSignalMapper * mapper = new QSignalMapper(m_optionsWidget);
        connect(mapper, SIGNAL(mapped(int)), this, SLOT(mapTypeToggled(int)));

        QLabel * mapTypesLabel = new QLabel(tr("Map types"));

        mapTypesLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        mapTypesLabel->setAlignment(Qt::AlignHCenter);

        optionsLayout->addWidget(mapTypesLabel, 0, 0);
        QList<QGraphicsGeoMap::MapType> types = m_mapWidget->supportedMapTypes();
        for (int controlIndex = 0; controlIndex < mapControlTypes.size(); ++controlIndex) {
            QPushButton *button = new QPushButton(mapTypeNames[controlIndex], m_optionsWidget);
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            QFont font = button->font();
            font.setPointSize(24);
            button->setFont(font);

            optionsLayout->addWidget(button, controlIndex+1, 0);
            int supportedTypeIndex = types.indexOf(mapControlTypes[controlIndex]);

            if (supportedTypeIndex == -1) {
                button->setEnabled(false);
            } else {
                connect(button, SIGNAL(clicked()), mapper, SLOT(map()));
                mapper->setMapping(button, mapControlTypes[controlIndex]);

                button->setEnabled(true);
                //button->setChecked(mapControlTypes[controlIndex] == m_mapWidget->mapType());
            }

        }
    }
    {
        // connectivity modes
        QVector<QString> connectivityModeNames;
        QVector<QGraphicsGeoMap::ConnectivityMode> connectivityModes;

        connectivityModes.append(QGraphicsGeoMap::NoConnectivity); connectivityModeNames.append(tr("No connectivity"));
        connectivityModes.append(QGraphicsGeoMap::OfflineMode);    connectivityModeNames.append(tr("Offline"));
        connectivityModes.append(QGraphicsGeoMap::OnlineMode);     connectivityModeNames.append(tr("Online"));
        connectivityModes.append(QGraphicsGeoMap::HybridMode);     connectivityModeNames.append(tr("Hybrid"));

        // map types
        QSignalMapper * mapper = new QSignalMapper(m_optionsWidget);
        connect(mapper, SIGNAL(mapped(int)), this, SLOT(connectivityModeToggled(int)));

        QLabel * connectivityModesLabel = new QLabel(tr("Connectivity"));

        connectivityModesLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connectivityModesLabel->setAlignment(Qt::AlignHCenter);

        optionsLayout->addWidget(connectivityModesLabel, 0, 1);
        QList<QGraphicsGeoMap::ConnectivityMode> modes = m_mapWidget->supportedConnectivityModes();
        for (int controlIndex = 0; controlIndex < connectivityModes.size(); ++controlIndex) {
            QPushButton *button = new QPushButton(connectivityModeNames[controlIndex], m_optionsWidget);
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            QFont font = button->font();
            font.setPointSize(24);
            button->setFont(font);

            optionsLayout->addWidget(button, controlIndex+1, 1);
            int supportedTypeIndex = modes.indexOf(connectivityModes[controlIndex]);

            if (supportedTypeIndex == -1) {
                button->setEnabled(false);
            } else {
                connect(button, SIGNAL(clicked()), mapper, SLOT(map()));
                mapper->setMapping(button, connectivityModes[controlIndex]);

                button->setEnabled(true);
                //button->setChecked(connectivityModes[controlIndex] == m_mapWidget->connectivityMode());
            }

        }
    }

    /*
    {
        // plugins, TODO
        QList<QString> providers = QGeoServiceProvider::availableServiceProviders();

        for (int i = 0; i < providers.count(); ++i) {
            QAction *providerAction = new QAction(providers[i], this);
            providerAction->setCheckable(true);
            providerAction->setChecked(i == 0);
            m_pluginMenu->addAction(providerAction);
        }
    }
    */

    m_optionsWidget->setLayout(optionsLayout);

    if (displayMode() == DisplayOptions) {
        setDisplayMode(DisplayNone);
        setDisplayMode(DisplayOptions);
    }
}

void MapBox::showOptions()
{
    setDisplayMode(DisplayOptions);
}

void MapBox::mapTypeToggled(int type)
{
    m_mapWidget->setMapType((QGraphicsGeoMap::MapType)type);
    setDisplayMode(DisplayMap);
}

void MapBox::connectivityModeToggled(int type)
{
    m_mapWidget->setConnectivityMode((QGraphicsGeoMap::ConnectivityMode)type);
    setDisplayMode(DisplayMap);
}

void MapBox::resizeEvent(QResizeEvent * event)
{
    m_qgv->setSceneRect(QRectF(QPointF(0.0, 0.0), m_qgv->size()));
    if (m_mapWidget)
        m_mapWidget->resize(m_qgv->size());

    m_optionsButton->move(m_qgv->width()  - m_optionsButton->width()  - 9,
                          m_qgv->height() - m_optionsButton->height() - 9);

    if (event)
        QWidget::resizeEvent(event);
}

static QPen defaultPen()
{
    QPen pen(Qt::white);
    pen.setWidth(2);
    pen.setCosmetic(true);
    return pen;
}

static QBrush defaultBrush()
{
    return QBrush(QColor(0,0,0,65));
}

QGeoMapRectangleObject * MapBox::addRectangle(qreal top, qreal left, qreal bottom, qreal right)
{
    return addRectangle(QGeoCoordinate(top, left), QGeoCoordinate(bottom, right));
}

QGeoMapRectangleObject * MapBox::addRectangle(const QGeoCoordinate & topLeft, const QGeoCoordinate & bottomRight)
{
    QGeoMapRectangleObject * rectangle = new QGeoMapRectangleObject(topLeft, bottomRight);

    rectangle->setPen(defaultPen());
    rectangle->setBrush(defaultBrush());

    m_mapWidget->addMapObject(rectangle);

    return rectangle;
}

QGeoMapPolylineObject * MapBox::addPolyline(const QList<QGeoCoordinate> & path)
{
    QGeoMapPolylineObject * polyline = new QGeoMapPolylineObject();

    polyline->setPen(defaultPen()); // TODO: check
    polyline->setPath(path);

    m_mapWidget->addMapObject(polyline);

    return polyline;
}

QGeoMapPolygonObject * MapBox::addPolygon(const QList<QGeoCoordinate> & path)
{
    QGeoMapPolygonObject * polygon = new QGeoMapPolygonObject();

    polygon->setPen(defaultPen());
    polygon->setBrush(defaultBrush());
    polygon->setPath(path);

    m_mapWidget->addMapObject(polygon);

    return polygon;
}

QGeoMapCircleObject * MapBox::addCircle(const QGeoCoordinate & center, qreal radius)
{
    QGeoMapCircleObject * circle = new QGeoMapCircleObject(center, radius);

    circle->setPen(defaultPen());
    circle->setBrush(defaultBrush());

    m_mapWidget->addMapObject(circle);

    return circle;
}

void MapBox::addRoute(const QGeoCoordinate & start, const QGeoCoordinate & end)
{
    QList<QGeoCoordinate> waypoints;
    waypoints << start << end;
    addRoute(waypoints);
}

void MapBox::addRoute(const QList<QGeoCoordinate> & waypoints)
{
    QGeoRoutingManager * routingManager = m_serviceProvider->routingManager();

    QGeoRouteRequest req(waypoints);
    QGeoRouteReply * reply = routingManager->calculateRoute(req);

    connect(reply, SIGNAL(finished()),
            this, SLOT(routeFinished()));
}

void MapBox::routeFinished()
{
    QGeoRouteReply * reply = static_cast<QGeoRouteReply *>(sender());

    if (!reply)
        return;

    if (reply->routes().size() < 1)
        return;

    QPen pen(QColor(0, 0, 255, 127)); // blue, semi-transparent
    pen.setWidth(7);
    //pen.setCosmetic(true);
    pen.setCapStyle(Qt::RoundCap);

    QGeoMapRouteObject * route = new QGeoMapRouteObject(reply->routes().at(0));

    route->setPen(pen);

    m_mapWidget->addMapObject(route);
}

void MapBox::mapContextMenu(QGraphicsSceneContextMenuEvent * event, QGeoMapObject * clickedMapObject)
{
    if (!m_popupMenu)
        createMenus();

    m_lastClicked = event->scenePos();

    m_lastClickedMapObject = clickedMapObject;
    if (clickedMapObject) {
        // There is an object here, store it and open the appropriate context menu
        m_popupMenuMapObject->popup(event->screenPos());
    }
    else {
        // No objects, display the default context menu
        m_popupMenu->popup(event->screenPos());
    }

}

void MapBox::createMenus()
{
    QAction * menuItem;
    QMenu * subMenuItem;
    m_popupMenu = new QMenu(this);
    m_popupMenuMapObject = new QMenu(this);

    //**************************************************************

    // MapObject-specific context menu. The map object is in m_lastClickedMapObject.
    subMenuItem = m_popupMenuMapObject->addMenu(tr("MapObject"));

    menuItem = subMenuItem->addAction(tr("Remove"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(removeMapObject()));

    //**************************************************************

    /*

    // These are gread for testing, but not really what we want in
    // an example application.
    // We should probably creating a testing branch which tracks
    // the master branch and has various "test enabled" versions
    // of the examples at some point anyhow.

    subMenuItem = m_popupMenuMapObject->addMenu(tr("Spawn stuff"));
    m_popupMenu->addMenu(subMenuItem);

    menuItem = subMenuItem->addAction(tr("Items near the dateline"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(demo1()));

    menuItem = subMenuItem->addAction(tr("Regular grid of items"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(demo2()));

    menuItem = subMenuItem->addAction(tr("Clusters"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(demo3()));

    */

    //**************************************************************
    subMenuItem = m_popupMenuMapObject->addMenu(tr("Marker"));
    m_popupMenu->addMenu(subMenuItem);

    menuItem = subMenuItem->addAction(tr("Set marker"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(drawPixmap()));

    menuItem = subMenuItem->addAction(tr("Remove markers"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(removePixmaps()));

    menuItem = subMenuItem->addAction(tr("Select objects"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(selectObjects()));

    //**************************************************************
    subMenuItem = m_popupMenu->addMenu(tr("Draw"));
    m_popupMenu->addMenu(subMenuItem);

    menuItem = subMenuItem->addAction(tr("Rectangle"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(drawRect()));

    menuItem = subMenuItem->addAction(tr("Polyline"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(drawPolyline()));

    menuItem = subMenuItem->addAction(tr("Polygon"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(drawPolygon()));

    menuItem = subMenuItem->addAction(tr("Circle"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(drawCircle()));

    menuItem = subMenuItem->addAction(tr("Text"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(drawText()));

    //**************************************************************
    subMenuItem = m_popupMenuMapObject->addMenu(tr("Route"));
    m_popupMenu->addMenu(subMenuItem);

    menuItem = subMenuItem->addAction(tr("Calculate route"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(calcRoute()));

    //**************************************************************
    subMenuItem = m_popupMenuMapObject->addMenu(tr("Coordinates"));
    m_popupMenu->addMenu(subMenuItem);

    menuItem = subMenuItem->addAction(tr("Capture"));
    connect(menuItem, SIGNAL(triggered()),
            this, SLOT(captureCoordinates()));
    connect(menuItem, SIGNAL(triggered()),
            m_coordControlDialog, SLOT(show()));

    menuItem = subMenuItem->addAction(tr("Go to"));
    connect(menuItem, SIGNAL(triggered()),
            m_coordControlDialog, SLOT(show()));
}

void MapBox::gotoCoordsClicked()
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

void MapBox::setDisplayMode(DisplayMode mode) {
    if (m_displayMode == mode)
        return;

    m_displayMode = mode;

    QWidget * newWidget;

    switch (m_displayMode) {
        case DisplayMap:
            newWidget = m_qgv;
            break;

        case DisplayOptions:
            newWidget = m_optionsWidget;
            break;

        default:
            newWidget = 0;
    }

    // remove all current widgets
    while (QLayoutItem * item = layout()->takeAt(0)) {
        item->widget()->setVisible(false);
    }

    // add the new widget
    if (newWidget) {
        layout()->addWidget(newWidget);
        newWidget->setVisible(true);
    }
    resizeEvent(0);
}

MapBox::DisplayMode MapBox::displayMode() {
    return m_displayMode;
}
