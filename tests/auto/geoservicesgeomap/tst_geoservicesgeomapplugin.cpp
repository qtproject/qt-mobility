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

//TESTED_COMPONENT=src/location

#include <QtTest/QtTest>
#include <qgraphicsgeomap.h>
#include <qgeocoordinate.h>
#include <qgeoboundingbox.h>
#include <qgeoserviceprovider.h>
#include <qgeomapoverlay.h>
#include <qgeomaprectangleobject.h>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE( QGeoBoundingBox)
Q_DECLARE_METATYPE( QGraphicsGeoMap::ConnectivityMode)
Q_DECLARE_METATYPE( QGraphicsGeoMap::MapType)
Q_DECLARE_METATYPE( QList<QGeoMapObject*> )
Q_DECLARE_METATYPE( QList<QGeoMapOverlay*> )
Q_DECLARE_METATYPE( QPainterPath)
Q_DECLARE_METATYPE( QList<QGraphicsGeoMap::ConnectivityMode> )
Q_DECLARE_METATYPE( QList<QGraphicsGeoMap::MapType> )

class tst_SubQGeoMapOverlay: public QGeoMapOverlay
{
public:
    tst_SubQGeoMapOverlay() :
        QGeoMapOverlay()
    {
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option)
    {
Q_UNUSED    (painter) Q_UNUSED(option)}
};

class tst_GeoServicesGeoMapPlugin: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void mapObjects_data();
    void mapObjects();
    void mapOverlays_data();
    void mapOverlays();
    void coordinateToScreenPosition_data();
    void coordinateToScreenPosition();
    void screenPositionToCoordinate_data();
    void screenPositionToCoordinate();
    void fitInViewport_data();
    void fitInViewport();
    void mapObjectsAtScreenPosition_data();
    void mapObjectsAtScreenPosition();
    void mapObjectsInScreenRect_data();
    void mapObjectsInScreenRect();
    void maximumZoomLevel();
    void minimumZoomLevel();
    void zoomLevel_data();
    void zoomLevel();
    void paint_data();
    void paint();
    void pan_data();
    void pan();
    void center_data();
    void center();
    void connectivityMode_data();
    void connectivityMode();
    void mapType_data();
    void mapType();
    void supportedConnectivityModes_data();
    void supportedConnectivityModes();
    void supportedMapTypes_data();
    void supportedMapTypes();
    void viewport();
    void bearing_data();
    void bearing();
    void maximumTilt();
    void minimumTilt();
    void tilting_data();
    void tilting();
private:
    QGeoServiceProvider* m_serviceProvider;
    QGraphicsView *m_qgv;
    QGraphicsGeoMap* m_map;
};

void tst_GeoServicesGeoMapPlugin::initTestCase()
{
    qRegisterMetaType<QGraphicsGeoMap::ConnectivityMode> ();
    qRegisterMetaType<QGraphicsGeoMap::MapType> ();
    qRegisterMetaType<QGeoCoordinate> ();
    m_serviceProvider = new QGeoServiceProvider("static.geomap.test.plugin");

    QGraphicsScene* scene = new QGraphicsScene(this);
    m_qgv = new QGraphicsView(scene);
    m_qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_qgv->setVisible(true);
    m_qgv->setInteractive(true);
    m_qgv->resize(652, 519);
    m_qgv->setSceneRect(QRectF(QPointF(0.0, 0.0), m_qgv->size()));
    m_map = new QGraphicsGeoMap(m_serviceProvider->mappingManager());
    m_qgv->scene()->addItem(m_map);
    m_map->resize(m_qgv->size());

    m_qgv->show();
    QApplication::setActiveWindow(m_qgv);
    QTest::qWaitForWindowShown(m_qgv);
}

void tst_GeoServicesGeoMapPlugin::cleanupTestCase()
{
    delete m_map;
    delete m_serviceProvider;
}

void tst_GeoServicesGeoMapPlugin::init()
{
    QTest::qWait(100);
}

void tst_GeoServicesGeoMapPlugin::cleanup()
{
}

void tst_GeoServicesGeoMapPlugin::mapObjects_data()
{
    QTest::addColumn<int>("mapObjectCount");
    QTest::newRow("0") << 0;
    QTest::newRow("1") << 1;
    QTest::newRow("10") << 10;
}

// public void clearMapObjects()
// public void addMapObject(QGeoMapObject* mapObject)
// public QList<QGeoMapObject*> mapObjects() const;
// public void removeMapObject(QGeoMapObject* mapObject)
void tst_GeoServicesGeoMapPlugin::mapObjects()
{
    QFETCH(int, mapObjectCount);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    for (int i = 0; i < mapObjectCount; ++i)
        m_map->addMapObject(new QGeoMapObject());

    QList<QGeoMapObject*> mapObjects = m_map->mapObjects();
    QCOMPARE(mapObjects.count(), mapObjectCount);

    if (mapObjectCount > 0) {
        m_map->removeMapObject(mapObjects[0]);
        delete mapObjects[0];
        QCOMPARE(m_map->mapObjects().count(), mapObjectCount - 1);
    }

    m_map->clearMapObjects();
    QCOMPARE(m_map->mapObjects().count(), 0);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

void tst_GeoServicesGeoMapPlugin::mapOverlays_data()
{
    QTest::addColumn<int>("overlayCount");
    QTest::newRow("0") << 0;
    QTest::newRow("1") << 1;
    QTest::newRow("10") << 10;
}

// public void addMapOverlay(QGeoMapOverlay* overlay)
// public QList<QGeoMapOverlay*> mapOverlays() const
// public void removeMapOverlay(QGeoMapOverlay* overlay)
// public void clearMapOverlays()
void tst_GeoServicesGeoMapPlugin::mapOverlays()
{
    QFETCH(int, overlayCount);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    for (int i = 0; i < overlayCount; ++i)
        m_map->addMapOverlay(new tst_SubQGeoMapOverlay());

    QList<QGeoMapOverlay*> mapOverlays = m_map->mapOverlays();
    QCOMPARE(mapOverlays.count(), overlayCount);

    if (overlayCount > 0) {
        m_map->removeMapOverlay(mapOverlays[0]);
        delete mapOverlays[0];
        QCOMPARE(m_map->mapOverlays().count(), overlayCount - 1);
    }

    m_map->clearMapOverlays();
    QCOMPARE(m_map->mapOverlays().count(), 0);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

void tst_GeoServicesGeoMapPlugin::screenPositionToCoordinate_data()
{
    QTest::addColumn<QPointF>("screenPosition");
    QTest::addColumn<QGeoCoordinate>("screenPositionToCoordinate");
    QTest::newRow("null") << QPointF(qQNaN(), qQNaN()) << QGeoCoordinate();
    QTest::newRow("screen position 10,10 to coordinates") << QPointF(10.0, 10.0) << QGeoCoordinate(
        10.0, 10.0);
}

// public QGeoCoordinate screenPositionToCoordinate(QPointF screenPosition) const
void tst_GeoServicesGeoMapPlugin::screenPositionToCoordinate()
{
    QFETCH(QPointF, screenPosition);
    QFETCH(QGeoCoordinate, screenPositionToCoordinate);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    QCOMPARE(m_map->screenPositionToCoordinate(screenPosition), screenPositionToCoordinate);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

void tst_GeoServicesGeoMapPlugin::coordinateToScreenPosition_data()
{
    QTest::addColumn<QGeoCoordinate>("coordinate");
    QTest::addColumn<QPointF>("coordinateToScreenPosition");
    QTest::newRow("coordinates 10,10 to screen position") << QGeoCoordinate(10.0, 10.0) << QPointF(
        10.0, 10.0);
    QTest::newRow("coordinates 75.8,50.3 to screen position") << QGeoCoordinate(75.8, 50.3)
        << QPointF(50.3, 75.8);
}

// public QPointF coordinateToScreenPosition(QGeoCoordinate const& coordinate) const
void tst_GeoServicesGeoMapPlugin::coordinateToScreenPosition()
{
    QFETCH(QGeoCoordinate, coordinate);
    QFETCH(QPointF, coordinateToScreenPosition);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    QCOMPARE(m_map->coordinateToScreenPosition(coordinate), coordinateToScreenPosition);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

void tst_GeoServicesGeoMapPlugin::fitInViewport_data()
{
    QTest::addColumn<QGeoBoundingBox>("bounds");
    QTest::addColumn<bool>("preserveViewportCenter");
    QTest::addColumn<int>("signalcount");
    QTest::newRow("Don't preserve center") << QGeoBoundingBox(QGeoCoordinate(15.0, 25.0), 20.0,
        20.0) << false << 1;
    QTest::newRow("Preserve center") << QGeoBoundingBox(QGeoCoordinate(15.0, 25.0), 20.0, 20.0)
        << true << 0;
}

// public void fitInViewport(QGeoBoundingBox const& bounds, bool preserveViewportCenter = false)
void tst_GeoServicesGeoMapPlugin::fitInViewport()
{
    QFETCH(QGeoBoundingBox, bounds);
    QFETCH(bool, preserveViewportCenter);
    QFETCH(int, signalcount);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    m_map->fitInViewport(bounds, preserveViewportCenter);

    QCOMPARE(spy0.count(), signalcount);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

void tst_GeoServicesGeoMapPlugin::mapObjectsAtScreenPosition_data()
{
    QTest::addColumn<QPointF>("screenPosition");
    QTest::addColumn<int>("expectedcount");
    QTest::newRow("null") << QPointF() << 0;
    QTest::newRow("Find 2 objects") << QPointF(17.0, 17.0) << 2;
    QTest::newRow("Find 2 more objects") << QPointF(17.0, 23.0) << 2;
    QTest::newRow("Find 1 object") << QPointF(30.0, 46.0) << 1;
    QTest::newRow("Find 0 objects") << QPointF(60.0, 60.0) << 0;
}

// public QList<QGeoMapObject*> mapObjectsAtScreenPosition(QPointF const& screenPosition) const
void tst_GeoServicesGeoMapPlugin::mapObjectsAtScreenPosition()
{
    QFETCH(QPointF, screenPosition);
    QFETCH(int, expectedcount);

    m_map->clearMapObjects();

    m_map->addMapObject(new QGeoMapRectangleObject(QGeoCoordinate(20.0, 10.0), QGeoCoordinate(10.0,
        20.0)));
    m_map->addMapObject(new QGeoMapRectangleObject(QGeoCoordinate(25.0, 15.0), QGeoCoordinate(15.0,
        25.0)));
    m_map->addMapObject(new QGeoMapRectangleObject(QGeoCoordinate(40.0, 10.0), QGeoCoordinate(22.0,
        20.0)));
    m_map->addMapObject(new QGeoMapRectangleObject(QGeoCoordinate(55.0, 25.0), QGeoCoordinate(45.0,
        35.0)));

    QTest::qWait(100);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    QCOMPARE(m_map->mapObjectsAtScreenPosition(screenPosition).count(), expectedcount);

    m_map->clearMapObjects();

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

void tst_GeoServicesGeoMapPlugin::mapObjectsInScreenRect_data()
{
    QTest::addColumn<QRectF>("screenRect");
    QTest::addColumn<int>("expectedcount");
    QTest::newRow("null") << QRectF() << 0;
    QTest::newRow("Find 1 object") << QRectF(QPointF(5.0, 5.0), QPointF(13.0, 13.0)) << 1;
    QTest::newRow("Find 0 objects (negative rect)") << QRectF(QPointF(5.0, 13.0), QPointF(13.0, 5.0)) << 0;
    QTest::newRow("Find 2 objects") << QRectF(QPointF(13.0, 13.0), QPointF(23.0, 21.0)) << 2;
    QTest::newRow("Find 3 objects") << QRectF(QPointF(13.0, 13.0), QPointF(23.0, 30.0)) << 3;
    QTest::newRow("Find 4 objects") << QRectF(QPointF(13.0, 13.0), QPointF(30.0, 60.0)) << 4;
    QTest::newRow("Find 0 objects") << QRectF(QPointF(60.0, 60.0), QPointF(70.0, 70.0)) << 0;
}

// public QList<QGeoMapObject*> mapObjectsInScreenRect(QRectF const& screenRect) const
void tst_GeoServicesGeoMapPlugin::mapObjectsInScreenRect()
{
    QFETCH(QRectF, screenRect);
    QFETCH(int, expectedcount);

    m_map->clearMapObjects();

    m_map->addMapObject(new QGeoMapRectangleObject(QGeoCoordinate(20.0, 10.0), QGeoCoordinate(10.0,
        20.0)));
    m_map->addMapObject(new QGeoMapRectangleObject(QGeoCoordinate(25.0, 15.0), QGeoCoordinate(15.0,
        25.0)));
    m_map->addMapObject(new QGeoMapRectangleObject(QGeoCoordinate(40.0, 10.0), QGeoCoordinate(22.0,
        20.0)));
    m_map->addMapObject(new QGeoMapRectangleObject(QGeoCoordinate(55.0, 25.0), QGeoCoordinate(45.0,
        35.0)));

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    QCOMPARE(m_map->mapObjectsInScreenRect(screenRect).count(), expectedcount);

    m_map->clearMapObjects();

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

void tst_GeoServicesGeoMapPlugin::paint_data()
{
    QTest::addColumn<int>("overlays");
    QTest::newRow("no overlays") << 0;
    QTest::newRow("two overlays") << 2;
}

// public void paint(QPainter* painter, QStyleOptionGraphicsItem const* option, QWidget* parent)
void tst_GeoServicesGeoMapPlugin::paint()
{
    QFETCH(int, overlays);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    for (int i = 0; i < overlays; ++i)
        m_map->addMapOverlay(new tst_SubQGeoMapOverlay());

    QWidget widget;
    QPainter painter;
    QStyleOptionGraphicsItem item;

    m_map->paint(&painter, &item, &widget);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

void tst_GeoServicesGeoMapPlugin::pan_data()
{
    QTest::addColumn<int>("dx");
    QTest::addColumn<int>("dy");
    QTest::addColumn<QGeoCoordinate>("expectedcenter");
    QTest::addColumn<int>("signalcount");
    QTest::newRow("pan null") << 0 << 0 << QGeoCoordinate(10.0, 10.0) << 1;
    QTest::newRow("pan 10x10") << 10 << 10 << QGeoCoordinate(20.0, 20.0) << 1;
    QTest::newRow("pan 100x100") << 100 << 100 << QGeoCoordinate(110.0, 110.0) << 2;
    QTest::newRow("pan -17x-88") << -17 << -88 << QGeoCoordinate(-78.0, -7.0) << 2;
}

// public void pan(int dx, int dy)
void tst_GeoServicesGeoMapPlugin::pan()
{
    QFETCH(int, dx);
    QFETCH(int, dy);
    QFETCH(QGeoCoordinate, expectedcenter);
    QFETCH(int, signalcount);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    m_map->setCenter(QGeoCoordinate(10.0, 10.0));
    m_map->pan(dx, dy);

    QCOMPARE(spy0.count(), signalcount);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(m_map->center(), expectedcenter);
}

void tst_GeoServicesGeoMapPlugin::center_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<int>("signalcount");
    QTest::newRow("null center") << QGeoCoordinate() << 1;
    QTest::newRow("center at 10,10") << QGeoCoordinate(10.0, 10.0) << 1;
    QTest::newRow("2nd center at 10,10") << QGeoCoordinate(10.0, 10.0) << 0;
    QTest::newRow("center at 30,30") << QGeoCoordinate(30.0, 30.0) << 1;
    QTest::newRow("center at 63.5,-103.7") << QGeoCoordinate(63.5, -103.7) << 1;
}

// public void setCenter(QGeoCoordinate const& center)
// public QGeoCoordinate center()
void tst_GeoServicesGeoMapPlugin::center()
{
    QFETCH(QGeoCoordinate, center);
    QFETCH(int, signalcount);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    m_map->setCenter(center);
    QCOMPARE(spy0.count(), signalcount);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(m_map->center(), center);
}

void tst_GeoServicesGeoMapPlugin::connectivityMode_data()
{
    QTest::addColumn<QGraphicsGeoMap::ConnectivityMode>("connectivityMode");
    QTest::addColumn<QGraphicsGeoMap::ConnectivityMode>("expectedMode");
    QTest::addColumn<int>("signalcount");
    QTest::newRow("Not supported-NoConnectivity") << QGraphicsGeoMap::NoConnectivity
        << QGraphicsGeoMap::OfflineMode << 0;
    QTest::newRow("OnlineMode") << QGraphicsGeoMap::OnlineMode << QGraphicsGeoMap::OnlineMode << 1;
    QTest::newRow("OfflineMode") << QGraphicsGeoMap::OfflineMode << QGraphicsGeoMap::OfflineMode
        << 1;
    QTest::newRow("HybridMode") << QGraphicsGeoMap::HybridMode << QGraphicsGeoMap::HybridMode << 1;
    QTest::newRow("set HybridMode again") << QGraphicsGeoMap::HybridMode
        << QGraphicsGeoMap::HybridMode << 0;
}

// public QGraphicsGeoMap::ConnectivityMode connectivityMode() const
// public void setConnectivityMode(QGraphicsGeoMap::ConnectivityMode connectivityMode)
void tst_GeoServicesGeoMapPlugin::connectivityMode()
{
    QFETCH(QGraphicsGeoMap::ConnectivityMode, connectivityMode);
    QFETCH(QGraphicsGeoMap::ConnectivityMode, expectedMode);
    QFETCH(int, signalcount);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    m_map->setConnectivityMode(connectivityMode);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), signalcount);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(m_map->connectivityMode(), expectedMode);
}

void tst_GeoServicesGeoMapPlugin::mapType_data()
{
    QTest::addColumn<QGraphicsGeoMap::MapType>("mapType");
    QTest::addColumn<QGraphicsGeoMap::MapType>("expectedMapType");
    QTest::addColumn<int>("signalcount");
    QTest::newRow("NoMap - not supported") << QGraphicsGeoMap::NoMap << QGraphicsGeoMap::StreetMap
        << 0;
    QTest::newRow("SatelliteMapDay") << QGraphicsGeoMap::SatelliteMapDay
        << QGraphicsGeoMap::SatelliteMapDay << 1;
    QTest::newRow("StreetMap") << QGraphicsGeoMap::StreetMap << QGraphicsGeoMap::StreetMap << 1;
    QTest::newRow("SatelliteMapNight - not supported") << QGraphicsGeoMap::SatelliteMapNight
        << QGraphicsGeoMap::StreetMap << 0;
    QTest::newRow("TerrainMap") << QGraphicsGeoMap::TerrainMap << QGraphicsGeoMap::TerrainMap << 1;
    QTest::newRow("set TerrainMap again") << QGraphicsGeoMap::TerrainMap
        << QGraphicsGeoMap::TerrainMap << 0;
}

// public QGraphicsGeoMap::MapType mapType() const
// public void setMapType(QGraphicsGeoMap::MapType mapType)
void tst_GeoServicesGeoMapPlugin::mapType()
{
    QFETCH(QGraphicsGeoMap::MapType, mapType);
    QFETCH(QGraphicsGeoMap::MapType, expectedMapType);
    QFETCH(int, signalcount);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    m_map->setMapType(mapType);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), signalcount);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(m_map->mapType(), expectedMapType);
}

void tst_GeoServicesGeoMapPlugin::supportedConnectivityModes_data()
{
    QTest::addColumn<QList<QGraphicsGeoMap::ConnectivityMode> >("supportedConnectivityModes");
    QTest::addColumn<bool>("expecttofail");
    QTest::newRow("Supported connectivity modes") << (QList<QGraphicsGeoMap::ConnectivityMode> ()
        << QGraphicsGeoMap::OfflineMode << QGraphicsGeoMap::OnlineMode
        << QGraphicsGeoMap::HybridMode) << false;
    QTest::newRow("Not supported connectivity modes")
        << (QList<QGraphicsGeoMap::ConnectivityMode> () << QGraphicsGeoMap::OfflineMode
            << QGraphicsGeoMap::NoConnectivity) << true;
}

// public QList<QGraphicsGeoMap::ConnectivityMode> supportedConnectivityModes() const
void tst_GeoServicesGeoMapPlugin::supportedConnectivityModes()
{
    QFETCH(QList<QGraphicsGeoMap::ConnectivityMode> , supportedConnectivityModes);
    QFETCH(bool, expecttofail);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    if (expecttofail)
        QEXPECT_FAIL("", "Expecting the compare to fail", Continue);

    QCOMPARE(m_map->supportedConnectivityModes(), supportedConnectivityModes);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

void tst_GeoServicesGeoMapPlugin::supportedMapTypes_data()
{
    QTest::addColumn<QList<QGraphicsGeoMap::MapType> >("supportedMapTypes");
    QTest::addColumn<bool>("expecttofail");
    QTest::newRow("Supported map types") << (QList<QGraphicsGeoMap::MapType> ()
        << QGraphicsGeoMap::StreetMap << QGraphicsGeoMap::SatelliteMapDay
        << QGraphicsGeoMap::TerrainMap) << false;
    QTest::newRow("Not supported map types") << (QList<QGraphicsGeoMap::MapType> ()
        << QGraphicsGeoMap::StreetMap << QGraphicsGeoMap::SatelliteMapNight) << true;
}

// public QList<QGraphicsGeoMap::MapType> supportedMapTypes() const
void tst_GeoServicesGeoMapPlugin::supportedMapTypes()
{
    QFETCH(QList<QGraphicsGeoMap::MapType> , supportedMapTypes);
    QFETCH(bool, expecttofail);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    if (expecttofail)
        QEXPECT_FAIL("", "Expecting the compare to fail", Continue);

    QCOMPARE(m_map->supportedMapTypes(), supportedMapTypes);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

// public QGeoBoundingBox viewport() const
void tst_GeoServicesGeoMapPlugin::viewport()
{
    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    QCOMPARE(m_map->viewport(), QGeoBoundingBox(QGeoCoordinate(0,0), QGeoCoordinate(m_map->size().height(), m_map->size().width())));

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

void tst_GeoServicesGeoMapPlugin::zoomLevel_data()
{
    QTest::addColumn<qreal>("zoomLevel");
    QTest::addColumn<int>("signalcount");
    QTest::addColumn<qreal>("expectedZoomLevel");
    QTest::newRow("null") << qreal(0.0) << 1 << qreal(10.0);
    QTest::newRow("below minimum") << qreal(5.0) << 0 << qreal(10.0);
    QTest::newRow("valid minimum") << qreal(10.0) << 0 << qreal(10.0);
    QTest::newRow("valid") << qreal(15.0) << 1 << qreal(15.0);
    QTest::newRow("valid maximum") << qreal(20.0) << 1 << qreal(20.0);
    QTest::newRow("over maximum") << qreal(25.0) << 0 << qreal(20.0);
}

// public void setZoomLevel(qreal zoomLevel)
// public qreal zoomLevel() const
void tst_GeoServicesGeoMapPlugin::zoomLevel()
{
    QFETCH(qreal, zoomLevel);
    QFETCH(int, signalcount);
    QFETCH(qreal, expectedZoomLevel);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    m_map->setZoomLevel(zoomLevel);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), signalcount);
    QCOMPARE(m_map->zoomLevel(), expectedZoomLevel);
}
// public qreal maximumZoomLevel() const
void tst_GeoServicesGeoMapPlugin::maximumZoomLevel()
{
    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    QCOMPARE(m_map->maximumZoomLevel(), qreal(20.0));

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

// public qreal minimumZoomLevel() const
void tst_GeoServicesGeoMapPlugin::minimumZoomLevel()
{
    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));

    QCOMPARE(m_map->minimumZoomLevel(), qreal(10.0));

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
}

// public qreal maximumTilt() const
void tst_GeoServicesGeoMapPlugin::maximumTilt()
{
    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));
    QSignalSpy spy4(m_map, SIGNAL(bearingChanged(qreal)));
    QSignalSpy spy5(m_map, SIGNAL(tiltChanged(qreal)));

    QCOMPARE(m_map->maximumTilt(), qreal(90.0));

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);
    QCOMPARE(spy5.count(), 0);
}

// public qreal minimumTilt() const
void tst_GeoServicesGeoMapPlugin::minimumTilt()
{
    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));
    QSignalSpy spy4(m_map, SIGNAL(bearingChanged(qreal)));
    QSignalSpy spy5(m_map, SIGNAL(tiltChanged(qreal)));

    QCOMPARE(m_map->minimumTilt(), qreal(0.0));

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);
    QCOMPARE(spy5.count(), 0);
}

void tst_GeoServicesGeoMapPlugin::bearing_data()
{
    QTest::addColumn<qreal>("bearing");
    QTest::addColumn<int>("signalcount");
    QTest::addColumn<qreal>("expectedBearing");
    QTest::newRow("under minimum") << qreal(-1.0) << 0 << qreal(0.0);
    QTest::newRow("valid 45") << qreal(45.0) << 1 << qreal(45.0);
    QTest::newRow("null") << qreal(0.0) << 1 << qreal(0.0);
    QTest::newRow("maximum") << qreal(360.0) << 1 << qreal(360.0);
    QTest::newRow("valid 180") << qreal(180.0) << 1 << qreal(180.0);
    QTest::newRow("over maximum") << qreal(540.0) << 1 << qreal(360.0);
}

// public void setBearing(qreal bearing)
// public qreal bearing() const
void tst_GeoServicesGeoMapPlugin::bearing()
{
    QFETCH(qreal, bearing);
    QFETCH(int, signalcount);
    QFETCH(qreal, expectedBearing);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));
    QSignalSpy spy4(m_map, SIGNAL(bearingChanged(qreal)));
    QSignalSpy spy5(m_map, SIGNAL(tiltChanged(qreal)));

    m_map->setBearing(bearing);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), signalcount);
    QCOMPARE(spy5.count(), 0);
    QCOMPARE(m_map->bearing(), expectedBearing);
}

void tst_GeoServicesGeoMapPlugin::tilting_data()
{
    QTest::addColumn<qreal>("tilt");
    QTest::addColumn<int>("signalcount");
    QTest::addColumn<qreal>("expectedTilt");
    QTest::newRow("valid") << qreal(45.0) << 1 << qreal(45.0);
    QTest::newRow("null") << qreal(0.0) << 1 << qreal(0.0);
    QTest::newRow("under minimum") << qreal(-1.0) << 0 << qreal(0.0);
    QTest::newRow("valid maximum") << qreal(90.0) << 1 << qreal(90.0);
    QTest::newRow("over maximum") << qreal(180.0) << 0 << qreal(90.0);
}

// public void setTilting(qreal tilt)
// public qreal tilting() const
void tst_GeoServicesGeoMapPlugin::tilting()
{
    QFETCH(qreal, tilt);
    QFETCH(int, signalcount);
    QFETCH(qreal, expectedTilt);

    QSignalSpy spy0(m_map, SIGNAL( centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(m_map, SIGNAL(connectivityModeChanged( QGraphicsGeoMap::ConnectivityMode)));
    QSignalSpy spy2(m_map, SIGNAL(mapTypeChanged( QGraphicsGeoMap::MapType)));
    QSignalSpy spy3(m_map, SIGNAL(zoomLevelChanged(qreal)));
    QSignalSpy spy4(m_map, SIGNAL(bearingChanged(qreal)));
    QSignalSpy spy5(m_map, SIGNAL(tiltChanged(qreal)));

    m_map->setTilt(tilt);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    QCOMPARE(spy4.count(), 0);
    QCOMPARE(spy5.count(), signalcount);
    QCOMPARE(m_map->tilt(), expectedTilt);
}

QTEST_MAIN( tst_GeoServicesGeoMapPlugin)
#include "tst_geoservicesgeomapplugin.moc"

