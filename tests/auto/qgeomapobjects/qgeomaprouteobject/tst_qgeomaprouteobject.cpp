#include "testsuite.h"
#include "testhelper.h"
#include <QtTest/QtTest>
#include "qgeomaprouteobject.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoRoute)
Q_DECLARE_METATYPE(QPen)
Q_DECLARE_METATYPE(quint32)

class tst_QGeoMapRouteObject: public QObject
{
Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgeomaprouteobject_data();
    void qgeomaprouteobject();

    void detailLevel_data();
    void detailLevel();
    void pen_data();
    void pen();
    void route_data();
    void route();

private:
    TestHelper *m_helper;
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGeoMapRouteObject::initTestCase()
{
    qRegisterMetaType<QGeoRoute> ();
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGeoMapRouteObject::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QGeoMapRouteObject::init()
{

    m_helper = new TestHelper();

    QTest::qWait(500);

    QGraphicsGeoMap* map = m_helper->map();

    if (!map)
        QFAIL("Could not create map!");
}

// This will be called after every test function.
void tst_QGeoMapRouteObject::cleanup()
{
    delete (m_helper);
    m_helper = 0;
}

void tst_QGeoMapRouteObject::qgeomaprouteobject_data()
{
}

void tst_QGeoMapRouteObject::qgeomaprouteobject()
{

    QGeoMapRouteObject* object = new QGeoMapRouteObject();

    QCOMPARE(object->type(), QGeoMapObject::RouteType);

    QCOMPARE((int)object->detailLevel(), 6);
    QPen pen(QColor(Qt::black));
    pen.setCosmetic(true);
    QCOMPARE(object->pen(), pen);
    QCOMPARE(object->route(),QGeoRoute());

    //check if can be added to map

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    map->removeMapObject(object);

    delete (object);
}

void tst_QGeoMapRouteObject::detailLevel_data()
{

    QTest::addColumn<quint32>("detailLevel");
    QTest::newRow("8") << quint32(8);
    QTest::newRow("10") << quint32(10);
    QTest::newRow("12") << quint32(12);

}

// public quint32 detailLevel() const
void tst_QGeoMapRouteObject::detailLevel()
{
    QFETCH(quint32, detailLevel);

    QGeoCoordinate center(10, 20);

    QGeoRoute route;

    QList<QGeoCoordinate> path;

    path << center;

    QGraphicsGeoMap* map = m_helper->map();

    map->setCenter(center);

    QPointF point = map->coordinateToScreenPosition(center);

    QPointF diff1(detailLevel / 2, detailLevel / 2);

    point += diff1;

    QGeoCoordinate coord = map->screenPositionToCoordinate(point);

    map->setCenter(coord);

    path << coord;

    QPointF diff2(-detailLevel / 2, detailLevel / 2);

    point += diff2;

    coord = map->screenPositionToCoordinate(point);

    path << coord;

    route.setPath(path);

    QGeoMapRouteObject* object = new QGeoMapRouteObject();

    object->setRoute(route);

    QPen p;

    p.setWidth(10);

    object->setPen(p);

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(detailLevelChanged(quint32)));
    QSignalSpy spy1(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy2(object, SIGNAL(routeChanged(QGeoRoute const&)));

    //check if we have the point

    point = map->coordinateToScreenPosition(path.at(1));

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    object->setDetailLevel(detailLevel);

    QCOMPARE(object->detailLevel(), detailLevel);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    QCOMPARE(spy0.count(), 1);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);

}

void tst_QGeoMapRouteObject::pen_data()
{

    QTest::addColumn<QPen>("pen");

    QPen pen1(QColor(Qt::blue));

    pen1.setWidth(5);

    pen1.setCosmetic(true);

    QTest::newRow("blue,5") << pen1;

    QPen pen2(QColor(Qt::white));

    pen2.setWidth(10);

    pen2.setCosmetic(true);

    QTest::newRow("white,10") << pen2;

    QPen pen3(QColor(Qt::black));

    pen3.setWidth(15);

    pen3.setCosmetic(true);

    QTest::newRow("black,15") << pen3;

}

// public QPen pen() const
void tst_QGeoMapRouteObject::pen()
{
    QFETCH(QPen, pen);

    QGeoMapRouteObject* object = new QGeoMapRouteObject();

    QGeoRoute route;

    QList<QGeoCoordinate> path;

    path << QGeoCoordinate(10, 10) << QGeoCoordinate(10, 20) << QGeoCoordinate(10, 30);

    route.setPath(path);

    object->setRoute(route);

    QPen p;

    p.setWidth(1);

    object->setPen(p);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(detailLevelChanged(quint32)));
    QSignalSpy spy1(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy2(object, SIGNAL(routeChanged(QGeoRoute const&)));

    //check if object is not there

    map->setCenter(path.at(1));

    QPointF point = map->coordinateToScreenPosition(path.at(1));

    QPointF diff(0, pen.width() / 2 - 1);

    point += diff;

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setPen(pen);

    QCOMPARE(object->pen(), pen);

    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 0);

}

void tst_QGeoMapRouteObject::route_data()
{

    QTest::addColumn<QGeoRoute>("route");
    QGeoRoute route1;
    QList<QGeoCoordinate> list1;
    list1 << QGeoCoordinate(10, 10) << QGeoCoordinate(10, 20) << QGeoCoordinate(10, 30);
    route1.setPath(list1);
    QTest::newRow("(10,10)-(10,20)-(10,30)") << route1;
    QGeoRoute route2;
    QList<QGeoCoordinate> list2;
    list2 << QGeoCoordinate(-10, 10) << QGeoCoordinate(-5, 10) << QGeoCoordinate(-2, 10);
    route2.setPath(list2);
    QTest::newRow("(-10,10)-(-5,10)-(-2,10)") << route2;
    QGeoRoute route3;
    QList<QGeoCoordinate> list3;
    list3 << QGeoCoordinate(-10, 10) << QGeoCoordinate(-5, 10) << QGeoCoordinate(2, 10);
    route3.setPath(list3);
    QTest::newRow("(-10,10)-(-5,10)-(2,10)") << route3;
}

// public QGeoRoute route() const
void tst_QGeoMapRouteObject::route()
{
    QFETCH(QGeoRoute, route);

    QGeoMapRouteObject* object = new QGeoMapRouteObject();

    QPen pen;

    pen.setWidth(10);

    object->setPen(pen);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(detailLevelChanged(quint32)));
    QSignalSpy spy1(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy2(object, SIGNAL(routeChanged(QGeoRoute const&)));

    //check if object is not there

    map->setCenter(route.path().at(1));

    QPointF point = map->coordinateToScreenPosition(route.path().at(1));

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setRoute(route);

    QCOMPARE(object->route(), route);

    //check if object is there
    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

}

ADD_TO_TESTSUITE(tst_QGeoMapRouteObject)

#include "tst_qgeomaprouteobject.moc"

