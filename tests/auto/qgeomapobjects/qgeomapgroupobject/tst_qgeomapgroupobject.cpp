#include "testsuite.h"
#include "testhelper.h"
#include <QtTest/QtTest>
#include "qgeomapgroupobject.h"
#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"
#include "qgraphicsgeomap.h"
#include "qgeomapcircleobject.h"
#include "qgeomappixmapobject.h"
#include "qgeomaprectangleobject.h"
#include "qgeomappolygonobject.h"
#include "qgeomappolylineobject.h"
#include "qgeomaprouteobject.h"
#include "qgeomaptextobject.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoBoundingBox)
Q_DECLARE_METATYPE(QGeoMapObject*)
Q_DECLARE_METATYPE(QList<QGeoMapObject*>)
Q_DECLARE_METATYPE(QGeoCoordinate)

class tst_QGeoMapGroupObject: public QObject
{
Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgeomapgroupobject_data();
    void qgeomapgroupobject();

    void addChildObject_data();
    void addChildObject();
    void boundingBox_data();
    void boundingBox();
    void childObjects_data();
    void childObjects();
    void clearChildObjects_data();
    void clearChildObjects();
    void contains_data();
    void contains();
    void removeChildObject_data();
    void removeChildObject();
    void setMapData_data();
    void setMapData();

private:
    TestHelper *m_helper;
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGeoMapGroupObject::initTestCase()
{
    qRegisterMetaType<QGeoMapObject*> ();
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGeoMapGroupObject::cleanupTestCase()
{

}

// This will be called before each test function is executed.
void tst_QGeoMapGroupObject::init()
{

    m_helper = new TestHelper();

    QTest::qWait(500);

    QGraphicsGeoMap* map = m_helper->map();

    if (!map)
        QFAIL("Could not create map!");
}

// This will be called after every test function.
void tst_QGeoMapGroupObject::cleanup()
{
    delete (m_helper);
    m_helper = 0;
}

void tst_QGeoMapGroupObject::qgeomapgroupobject_data()
{
}

void tst_QGeoMapGroupObject::qgeomapgroupobject()
{

    QGeoMapGroupObject* object = new QGeoMapGroupObject();

    QCOMPARE(object->type(), QGeoMapObject::GroupType);
    QCOMPARE(object->boundingBox(), QGeoBoundingBox());
    QCOMPARE(object->childObjects(), QList<QGeoMapObject*>());
    QCOMPARE(object->contains(QGeoCoordinate()), false);

    object->addChildObject((QGeoMapObject*) 0);
    object->removeChildObject((QGeoMapObject*) 0);
    object->clearChildObjects();
    object->setMapData((QGeoMapData*) 0);

    //check if can be added to map

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    map->removeMapObject(object);

    delete (object);
}

void tst_QGeoMapGroupObject::addChildObject_data()
{
    QTest::addColumn<int>("childObjectCount");
    QTest::newRow("1") << 1;
    QTest::newRow("2") << 2;
    QTest::newRow("3") << 3;
}

// public void addChildObject(QGeoMapObject* childObject)
void tst_QGeoMapGroupObject::addChildObject()
{

    QFETCH(int, childObjectCount);

    QGeoMapGroupObject* object = new QGeoMapGroupObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(childAdded(QGeoMapObject*)));
    QSignalSpy spy1(object, SIGNAL(childRemoved(QGeoMapObject*)));

    for (int i = 0; i < childObjectCount; i++) {
        QGeoMapObject* childObject = new QGeoMapObject();
        object->addChildObject(childObject);
    }

    QCOMPARE(object->childObjects().size(),childObjectCount );

    QCOMPARE(spy0.count(), childObjectCount );
    QCOMPARE(spy1.count(), 0);

}

void tst_QGeoMapGroupObject::boundingBox_data()
{
#if 0
    QTest::addColumn<QGeoBoundingBox>("boundingBox");
    QTest::newRow("null") << QGeoBoundingBox();
#endif
}

// public QGeoBoundingBox boundingBox() const
void tst_QGeoMapGroupObject::boundingBox()
{
#if 0
    QFETCH(QGeoBoundingBox, boundingBox);

    SubQGeoMapGroupObject m_object;

    QSignalSpy spy0(&m_object, SIGNAL(childAdded(QGeoMapObject*)));
    QSignalSpy spy1(&m_object, SIGNAL(childRemoved(QGeoMapObject*)));

    QCOMPARE(m_object.boundingBox(), boundingBox);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_QGeoMapGroupObject::childObjects_data()
{

    QTest::addColumn<QList<QGeoMapObject*> >("childObjects");
    QList<QGeoMapObject*> list;

    QList<QGeoCoordinate> path;

    path.append(QGeoCoordinate(10, 10));
    path.append(QGeoCoordinate(10, 20));
    path.append(QGeoCoordinate(0, 20));
    path.append(QGeoCoordinate(0, 10));
    QGeoBoundingBox box(QGeoCoordinate(10, 10), QGeoCoordinate(0, 20));

    list.append(new QGeoMapCircleObject(box.center(), 1000));
    list.append(new QGeoMapPixmapObject(box.center(), QPoint(-200, -200), QPixmap(400, 400)));
    list.append(new QGeoMapRectangleObject(box));
    list.append(new QGeoMapTextObject(box.center(), "TEST"));

    QTest::newRow(
        "QGeoMapCircleObject,QGeoMapPixmapObject,QGeoMapRectangleObject,QGeoMapTextObject") << list;

    QList<QGeoMapObject*> list2;

    QGeoMapPolygonObject* polygon = new QGeoMapPolygonObject();
    polygon->setPath(path);

    list2.append(polygon);

    QGeoMapPolylineObject* polyline = new QGeoMapPolylineObject();
    //make path accross rectangle
    path.removeAt(1);
    path.removeAt(3);

    polyline->setPath(path);

    list2.append(polyline);

    QGeoMapRouteObject* route = new QGeoMapRouteObject();

    QGeoRoute r;
    r.setPath(path);
    route->setRoute(r);
    //list2.append(route);

    QTest::newRow("QGeoMapPolygonObject,QGeoMapPolylineObject,QGeoMapRouteObject") << list2;

}

// public QList<QGeoMapObject*> childObjects() const
void tst_QGeoMapGroupObject::childObjects()
{
    QFETCH(QList<QGeoMapObject*>, childObjects);

    QGeoBoundingBox box(QGeoCoordinate(10, 10), QGeoCoordinate(0, 20));

    QGeoMapGroupObject* object = new QGeoMapGroupObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(childAdded(QGeoMapObject*)));
    QSignalSpy spy1(object, SIGNAL(childRemoved(QGeoMapObject*)));

    map->setCenter(box.center());

    QPointF point = map->coordinateToScreenPosition(box.center());

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    foreach (QGeoMapObject* child, childObjects) {
        map->addMapObject(child);
    }

    QCOMPARE(object->childObjects().count(), childObjects.count());

    //check if object are there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),childObjects.count());

    QCOMPARE(spy0.count(), childObjects.count());
    QCOMPARE(spy1.count(), 0);

}

void tst_QGeoMapGroupObject::clearChildObjects_data()
{
    QTest::addColumn<int>("childObjectCount");
    QTest::newRow("1") << 1;
    QTest::newRow("2") << 2;
    QTest::newRow("3") << 3;
}

// public void clearChildObjects()
void tst_QGeoMapGroupObject::clearChildObjects()
{

    QFETCH(int, childObjectCount);

    QGeoMapGroupObject* object = new QGeoMapGroupObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    for (int i = 0; i < childObjectCount; i++) {
        QGeoMapObject* childObject = new QGeoMapObject();
        object->addChildObject(childObject);
    }

    QSignalSpy spy0(object, SIGNAL(childAdded(QGeoMapObject*)));
    QSignalSpy spy1(object, SIGNAL(childRemoved(QGeoMapObject*)));

    object->clearChildObjects();

    list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), childObjectCount);

}

void tst_QGeoMapGroupObject::contains_data()
{
#if 0
    QTest::addColumn<QGeoCoordinate>("coordinate");
    QTest::addColumn<bool>("contains");
    QTest::newRow("null") << QGeoCoordinate() << false;
#endif
}

// public bool contains(QGeoCoordinate const& coordinate) const
void tst_QGeoMapGroupObject::contains()
{
#if 0
    QFETCH(QGeoCoordinate, coordinate);
    QFETCH(bool, contains);

    QSignalSpy spy0(object, SIGNAL(childAdded(QGeoMapObject*)));
    QSignalSpy spy1(object, SIGNAL(childRemoved(QGeoMapObject*)));

    QCOMPARE(object->contains(coordinate), contains);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_QGeoMapGroupObject::removeChildObject_data()
{
    QTest::addColumn<int>("childObjectCount");
    QTest::newRow("1") << 1;
    QTest::newRow("2") << 2;
    QTest::newRow("3") << 3;
}

// public void removeChildObject(QGeoMapObject* childObject)
void tst_QGeoMapGroupObject::removeChildObject()
{

    QFETCH(int, childObjectCount);

    QGeoMapGroupObject* object = new QGeoMapGroupObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    for (int i = 0; i < childObjectCount; i++) {
        QGeoMapObject* childObject = new QGeoMapObject();
        object->addChildObject(childObject);
    }

    QSignalSpy spy0(object, SIGNAL(childAdded(QGeoMapObject*)));
    QSignalSpy spy1(object, SIGNAL(childRemoved(QGeoMapObject*)));

    for (int i = 0; i < childObjectCount; i++) {
        QGeoMapObject* childObject = new QGeoMapObject();
        object->removeChildObject(childObject);
    }

    QCOMPARE(object->childObjects().size(),childObjectCount );

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), childObjectCount);
}

void tst_QGeoMapGroupObject::setMapData_data()
{
    QTest::addColumn<int>("mapDataCount");
    QTest::newRow("0") << 0;
    QTest::newRow("-1") << -1;
}

// public void setMapData(QGeoMapData* mapData)
void tst_QGeoMapGroupObject::setMapData()
{
#if 0
    QFETCH(int, mapDataCount);

    SubQGeoMapGroupObject m_object;

    QSignalSpy spy0(&m_object, SIGNAL(childAdded(QGeoMapObject*)));
    QSignalSpy spy1(&m_object, SIGNAL(childRemoved(QGeoMapObject*)));

    m_object.setMapData(mapData);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}
ADD_TO_TESTSUITE(tst_QGeoMapGroupObject)

#include "tst_qgeomapgroupobject.moc"

