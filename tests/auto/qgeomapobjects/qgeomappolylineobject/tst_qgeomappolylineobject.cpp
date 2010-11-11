#include "testsuite.h"
#include "testhelper.h"
#include <QtTest/QtTest>
#include <testsuite.h>
#include "qgeomappolylineobject.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QList<QGeoCoordinate>)
Q_DECLARE_METATYPE(QPen)

class tst_QGeoMapPolylineObject: public QObject
{
Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgeomappolylineobject_data();
    void qgeomappolylineobject();

    void path_data();
    void path();
    void pen_data();
    void pen();

private:
    TestHelper *m_helper;
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGeoMapPolylineObject::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGeoMapPolylineObject::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QGeoMapPolylineObject::init()
{
    m_helper = new TestHelper();
    QTest::qWait(500);

    QGraphicsGeoMap* map = m_helper->map();

    if (!map)
        QFAIL("Could not create map!");
}

// This will be called after every test function.
void tst_QGeoMapPolylineObject::cleanup()
{
    delete (m_helper);
    m_helper = 0;
}

void tst_QGeoMapPolylineObject::qgeomappolylineobject_data()
{
}

void tst_QGeoMapPolylineObject::qgeomappolylineobject()
{
    QGeoMapPolylineObject* object = new QGeoMapPolylineObject();

    QCOMPARE(object->type(), QGeoMapObject::PolylineType);

    QCOMPARE(object->path(), QList<QGeoCoordinate>());
    QPen pen(QColor(Qt::black));
    pen.setCosmetic(true);
    QCOMPARE(object->pen(), pen);

    //check if can be added to map

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    map->removeMapObject(object);

    delete (object);

}

void tst_QGeoMapPolylineObject::path_data()
{

    QTest::addColumn<QList<QGeoCoordinate> >("path");

    QList<QGeoCoordinate> list1;
    list1 << QGeoCoordinate(10, 10) << QGeoCoordinate(10, 20) << QGeoCoordinate(10, 30);
    QTest::newRow("(10,10)-(10,20)-(10,30)") << list1;
    QList<QGeoCoordinate> list2;
    list2 << QGeoCoordinate(-10, 10) << QGeoCoordinate(-5, 10) << QGeoCoordinate(2, 10);
    QTest::newRow("(-10,10)-(-5,10)-(2,10)") << list2;

}

// public QList<QGeoCoordinate> path() const
void tst_QGeoMapPolylineObject::path()
{
    QFETCH(QList<QGeoCoordinate>, path);

    QGeoMapPolylineObject* object = new QGeoMapPolylineObject();

    QPen pen;

    pen.setWidth(5);

    object->setPen(pen);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(pathChanged(QList<QGeoCoordinate> const&)));
    QSignalSpy spy1(object, SIGNAL(penChanged(QPen const&)));

    //check if object is not there

    map->setCenter(path.at(1));

    QPointF point = map->coordinateToScreenPosition(path.at(1));

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setPath(path);

    QCOMPARE(object->path(), path);

    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 1);
    QCOMPARE(spy1.count(), 0);

}

void tst_QGeoMapPolylineObject::pen_data()
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
void tst_QGeoMapPolylineObject::pen()
{
    QFETCH(QPen, pen);

    QGeoMapPolylineObject* object = new QGeoMapPolylineObject();

    QList<QGeoCoordinate> path;
    path << QGeoCoordinate(10, 10) << QGeoCoordinate(10, 20) << QGeoCoordinate(10, 30);

    object->setPath(path);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(pathChanged(QList<QGeoCoordinate> const&)));
    QSignalSpy spy1(object, SIGNAL(penChanged(QPen const&)));

    //check if object is not there

    map->setCenter(path.at(1));

    QPointF point = map->coordinateToScreenPosition(path.at(1));

    QPointF diff(0,pen.width()/2-1);

    point+=diff;

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setPen(pen);

    QCOMPARE(object->pen(), pen);

    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 1);

}

ADD_TO_TESTSUITE(tst_QGeoMapPolylineObject)


#include "tst_qgeomappolylineobject.moc"

