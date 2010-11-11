#include "testsuite.h"
#include "testhelper.h"
#include <QtTest/QtTest>
#include <QList>
#include <testsuite.h>
#include "qgeomappolygonobject.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"
#include "qgeoboundingbox.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QBrush)
Q_DECLARE_METATYPE(QList<QGeoCoordinate>)
Q_DECLARE_METATYPE(QPen)

class tst_QGeoMapPolygonObject: public QObject
{
Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgeomappolygonobject_data();
    void qgeomappolygonobject();

    void brush_data();
    void brush();
    void path_data();
    void path();
    void pen_data();
    void pen();

private:
    TestHelper *m_helper;
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGeoMapPolygonObject::initTestCase()
{
    qRegisterMetaType<QList<QGeoCoordinate> > ();
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGeoMapPolygonObject::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QGeoMapPolygonObject::init()
{
    m_helper = new TestHelper();
    QTest::qWait(500);

    QGraphicsGeoMap* map = m_helper->map();

    if (!map)
        QFAIL("Could not create map!");
}

// This will be called after every test function.
void tst_QGeoMapPolygonObject::cleanup()
{
    delete (m_helper);
    m_helper = 0;
}

void tst_QGeoMapPolygonObject::qgeomappolygonobject_data()
{
}

void tst_QGeoMapPolygonObject::qgeomappolygonobject()
{
    QGeoMapPolygonObject* object = new QGeoMapPolygonObject();

    QCOMPARE(object->type(), QGeoMapObject::PolygonType);

    QCOMPARE(object->brush(), QBrush());
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

void tst_QGeoMapPolygonObject::brush_data()
{
    QTest::addColumn<QBrush>("brush");
    QTest::newRow("white") << QBrush(Qt::white);
    QTest::newRow("blue") << QBrush(Qt::blue);
}

// public QBrush brush() const
void tst_QGeoMapPolygonObject::brush()
{
    QFETCH(QBrush, brush);

    QGeoMapPolygonObject* object = new QGeoMapPolygonObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(pathChanged(QList<QGeoCoordinate> const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));

    object->setBrush(brush);

    QCOMPARE(object->brush(), brush);

    QCOMPARE(spy0.count(), 1);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);

}

void tst_QGeoMapPolygonObject::path_data()
{
    QTest::addColumn<QList<QGeoCoordinate> >("path");

    QList<QGeoCoordinate> list1;
    list1 << QGeoCoordinate(10, 10) << QGeoCoordinate(10, 20) << QGeoCoordinate(-10, 20)
        << QGeoCoordinate(-10, 10);
    QTest::newRow("(10,10)-(10,20)-(-10,20)-(-10,10)") << list1;
    QList<QGeoCoordinate> list2;
    list2 << QGeoCoordinate(-10, 10) << QGeoCoordinate(-5, 10) << QGeoCoordinate(-5, 20)
        << QGeoCoordinate(-10, 20);
    QTest::newRow("(-10,10)-(-5,10)-(-5,20)-(-10,20)") << list2;

}

// public QList<QGeoCoordinate> path() const
void tst_QGeoMapPolygonObject::path()
{
    QFETCH(QList<QGeoCoordinate>, path);

    QGeoMapPolygonObject* object = new QGeoMapPolygonObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(pathChanged(QList<QGeoCoordinate> const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));

    //check if object is not there

    QGeoBoundingBox rectangle(path.first(), path.last());

    map->setCenter(rectangle.center());

    QPointF point = map->coordinateToScreenPosition(rectangle.center());

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setPath(path);

    QCOMPARE(object->path(), path);

    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 0);

}

void tst_QGeoMapPolygonObject::pen_data()
{

    QTest::addColumn<QPen>("pen");

    QPen pen1(QColor(Qt::blue));

    pen1.setWidth(10);

    pen1.setCosmetic(true);

    QTest::newRow("blue,10") << pen1;

    QPen pen2(QColor(Qt::white));

    pen2.setWidth(15);

    pen2.setCosmetic(true);

    QTest::newRow("white,15") << pen2;

}

// public QPen pen() const
void tst_QGeoMapPolygonObject::pen()
{
    QFETCH(QPen, pen);

    QList<QGeoCoordinate> path;
    path << QGeoCoordinate(10, 10) << QGeoCoordinate(10, 15) << QGeoCoordinate(10, 20)
        << QGeoCoordinate(5, 20) << QGeoCoordinate(5, 10);

    QGeoMapPolygonObject* object = new QGeoMapPolygonObject();

    object->setPath(path);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(pathChanged(QList<QGeoCoordinate> const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));

    //check if object is not there

    map->setCenter(path.at(1));

    QPointF point = map->coordinateToScreenPosition(path.at(1));

    QPointF diff(0, object->pen().width() + 1);

    point -= diff;

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setPen(pen);

    QCOMPARE(object->pen(), pen);

    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

}
ADD_TO_TESTSUITE(tst_QGeoMapPolygonObject)

#include "tst_qgeomappolygonobject.moc"

