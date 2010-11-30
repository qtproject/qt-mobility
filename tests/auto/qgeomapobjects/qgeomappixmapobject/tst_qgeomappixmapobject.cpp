#include "testsuite.h"
#include "testhelper.h"
#include <QtTest/QtTest>
#include "qgeomappixmapobject.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"
#include "qgeoboundingbox.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QPoint)

class tst_QGeoMapPixmapObject: public QObject
{
Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgeomappixmapobject_data();
    void qgeomappixmapobject();

    void coordinate_data();
    void coordinate();
    void offset_data();
    void offset();
    void pixmap_data();
    void pixmap();
    void zvalue_data();
    void zvalue();
    void boundingBox();
    void contains_data();
    void contains();
    void isSelected();
    void isVisible();


private:
    TestHelper *m_helper;
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGeoMapPixmapObject::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGeoMapPixmapObject::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QGeoMapPixmapObject::init()
{
    m_helper = new TestHelper();
    QTest::qWait(500);

    QGraphicsGeoMap* map = m_helper->map();

    if (!map)
        QFAIL("Could not create map!");
}

// This will be called after every test function.
void tst_QGeoMapPixmapObject::cleanup()
{
    delete (m_helper);
    m_helper = 0;
}

void tst_QGeoMapPixmapObject::qgeomappixmapobject_data()
{
}

void tst_QGeoMapPixmapObject::qgeomappixmapobject()
{

    QGeoMapPixmapObject* object = new QGeoMapPixmapObject();

    QCOMPARE(object->type(), QGeoMapObject::PixmapType);

    QCOMPARE(object->coordinate(), QGeoCoordinate());
    QCOMPARE(object->offset(), QPoint());
    QCOMPARE(object->pixmap(), QPixmap());
    QCOMPARE(object->zValue(), 0);
    QCOMPARE(object->isSelected(),false);
    QCOMPARE(object->isVisible(),true);
    QCOMPARE(object->boundingBox(),QGeoBoundingBox());
    QCOMPARE(object->contains(QGeoCoordinate()),false);
    //check if can be added to map

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QVERIFY2(object->info(),"info object not created");
    QVERIFY2(object->mapData(),"no map data set");

    map->removeMapObject(object);

    QVERIFY2(!object->info(),"info object not deleted");
    QVERIFY2(!object->mapData(),"no map data still set");

    delete (object);

}

Q_DECLARE_METATYPE(QGeoCoordinate)
void tst_QGeoMapPixmapObject::coordinate_data()
{

    QTest::addColumn<QGeoCoordinate>("coordinate");
    QTest::newRow("52.5, 13.0, 0") << QGeoCoordinate(52.5, 13.0, 0);
    QTest::newRow("-52.5, 13.0, 0") << QGeoCoordinate(-52.5, 13.0, 0);
    QTest::newRow("52.5, -13.0, 0") << QGeoCoordinate(52.5, -13.0, 0);
    QTest::newRow("-52.5, -13.0, 0") << QGeoCoordinate(-52.5, -13.0, 0);

}

// public QGeoCoordinate coordinate() const
void tst_QGeoMapPixmapObject::coordinate()
{
    QFETCH(QGeoCoordinate, coordinate);

    QGeoCoordinate center(10, 10, 0);

    QGeoMapPixmapObject* object = new QGeoMapPixmapObject();

    object->setCoordinate(center);

    QCOMPARE(object->coordinate(), center);

    QPixmap pixmap(10, 10);

    object->setPixmap(pixmap);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(coordinateChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(object, SIGNAL(offsetChanged(QPoint const&)));
    QSignalSpy spy2(object, SIGNAL(pixmapChanged(QPixmap const&)));

    map->setCenter(coordinate);

    //check if circle is not there
    QPointF point = map->coordinateToScreenPosition(coordinate);

    QPointF diff(1, 1);

    point += diff;

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setCoordinate(coordinate);

    QCOMPARE(object->coordinate(), coordinate);

    //check if circle is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 1);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);

}

void tst_QGeoMapPixmapObject::offset_data()
{

    QTest::addColumn<QPoint>("offset");
    QTest::newRow("10,10") << QPoint(10, 10);
    QTest::newRow("-10,10") << QPoint(-10, 10);
    QTest::newRow("10,-10") << QPoint(10, -10);
    QTest::newRow("-10,-10") << QPoint(-10, -10);

}

// public QPoint offset() const
void tst_QGeoMapPixmapObject::offset()
{
    QFETCH(QPoint, offset);

    QGeoCoordinate center(10, 10, 0);

    QGeoMapPixmapObject* object = new QGeoMapPixmapObject();

    object->setCoordinate(center);

    QCOMPARE(object->coordinate(), center);

    QPixmap pixmap(5, 5);

    object->setPixmap(pixmap);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(coordinateChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(object, SIGNAL(offsetChanged(QPoint const&)));
    QSignalSpy spy2(object, SIGNAL(pixmapChanged(QPixmap const&)));

    map->setCenter(center);

    QPointF point = map->coordinateToScreenPosition(center);

    point += offset;

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setOffset(offset);

    QCOMPARE(object->offset(), offset);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 0);

}

void tst_QGeoMapPixmapObject::pixmap_data()
{
    QTest::addColumn<QPixmap>("pixmap");
    QTest::newRow("100,100") << QPixmap(100, 100);
    QTest::newRow("200,200") << QPixmap(200, 200);
    QTest::newRow("300,300") << QPixmap(300, 300);
}

// public QPixmap pixmap() const
void tst_QGeoMapPixmapObject::pixmap()
{
    QFETCH(QPixmap, pixmap);

    QGeoCoordinate center(10, 10, 0);

    QGeoMapPixmapObject* object = new QGeoMapPixmapObject();

    object->setCoordinate(center);

    QCOMPARE(object->coordinate(), center);

    QPixmap p(5, 5);

    object->setPixmap(p);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(coordinateChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(object, SIGNAL(offsetChanged(QPoint const&)));
    QSignalSpy spy2(object, SIGNAL(pixmapChanged(QPixmap const&)));

    map->setCenter(center);

    QPointF point = map->coordinateToScreenPosition(center);

    QPoint diff(pixmap.width() - 1, pixmap.height() - 1);

    point += diff;

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setPixmap(pixmap);

    QCOMPARE(object->pixmap(), pixmap);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

}

void tst_QGeoMapPixmapObject::zvalue_data()
{
    QTest::addColumn<int>("zValue1");
    QTest::addColumn<int>("zValue2");
    QTest::addColumn<int>("zValue3");
    QTest::addColumn<int>("offset1");
    QTest::addColumn<int>("offset2");
    QTest::addColumn<int>("offset3");
    QTest::addColumn<int>("result1");
    QTest::addColumn<int>("result2");
    QTest::addColumn<int>("result3");
    QTest::newRow("z:1,2,3 offset:0,0,0") << 1 << 2 << 3 << 0 << 0 << 0 << 0 << 1 << 2;
    QTest::newRow("z:3,2,1 offset:0,0,0") << 3 << 2 << 1 << 0 << 0 << 0 << 2 << 1 << 0;
    QTest::newRow("z:2,1,3 offset:0,0,0") << 2 << 1 << 3 << 0 << 0 << 0 << 1 << 0 << 2;

    QTest::newRow("z:1,2,3 offset:2,1,3") << 1 << 2 << 3 << 2 << 1 << 3 << 1 << 2 << 0;
    QTest::newRow("z:3,2,1 offset:1,2,3") << 3 << 2 << 1 << 1 << 2 << 3 << 2 << 1 << 0;
    QTest::newRow("z:2,1,3 offset:1,1,3") << 2 << 1 << 3 << 1 << 1 << 3 << 2 << 1 << 0;
}

// public int zValue() const
void tst_QGeoMapPixmapObject::zvalue()
{

    QFETCH(int, zValue1);
    QFETCH(int, zValue2);
    QFETCH(int, zValue3);
    QFETCH(int, offset1);
    QFETCH(int, offset2);
    QFETCH(int, offset3);
    QFETCH(int, result1);
    QFETCH(int, result2);
    QFETCH(int, result3);

    QGeoCoordinate center(1.0, 1.0, 0);

    QPixmap image(100, 100);

    QGeoMapPixmapObject* object1 = new QGeoMapPixmapObject(center, QPoint(offset1, offset1), image);
    QGeoMapPixmapObject* object2 = new QGeoMapPixmapObject(center, QPoint(offset2, offset2), image);
    QGeoMapPixmapObject* object3 = new QGeoMapPixmapObject(center, QPoint(offset3, offset3), image);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object1);
    map->addMapObject(object2);
    map->addMapObject(object3);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.count(),3);

    QVERIFY(list.at(0)==object1);
    QVERIFY(list.at(1)==object2);
    QVERIFY(list.at(2)==object3);

    QSignalSpy spy0(object1, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object1, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object1, SIGNAL(zValueChanged(int)));

    map->setCenter(center);

    QPointF point = map->coordinateToScreenPosition(center);
    QPointF diff(image.width() / 2, image.height() / 2);

    point += diff;

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),3);

    QVERIFY(map->mapObjectsAtScreenPosition(point).at(0)==object1);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(1)==object2);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(2)==object3);

    object1->setZValue(zValue1);
    object2->setZValue(zValue2);
    object3->setZValue(zValue3);

    QCOMPARE(object1->zValue(), zValue1);
    QCOMPARE(object2->zValue(), zValue2);
    QCOMPARE(object3->zValue(), zValue3);
    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),3);

    QVERIFY(map->mapObjectsAtScreenPosition(point).at(result1)==object1);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(result2)==object2);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(result3)==object3);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

}

// public bool isVisible() const
void tst_QGeoMapPixmapObject::isVisible()
{

    QGeoCoordinate center(1.0, 1.0, 0);

    QPixmap image(100, 100);

    QGeoMapPixmapObject* object = new QGeoMapPixmapObject(center, QPoint(0, 0), image);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object, SIGNAL(zValueChanged(int)));

    map->setCenter(center);

    QPointF point = map->coordinateToScreenPosition(center);
    QPointF diff(image.width() / 2, image.height() / 2);

    point += diff;

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    object->setVisible(false);

    QCOMPARE(object->isVisible(), false);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setVisible(true);

    QCOMPARE(object->isVisible(), true);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 2);
    QCOMPARE(spy2.count(), 0);

}

// public bool isSelected() const
void tst_QGeoMapPixmapObject::isSelected()
{
#if 0

    QSignalSpy spy0(object, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object, SIGNAL(zValueChanged(int)));

    QCOMPARE(object->isSelected(), isSelected);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_QGeoMapPixmapObject::contains_data()
{

    QTest::addColumn<QGeoCoordinate>("coordinate");
    QTest::newRow("5,5") << QGeoCoordinate(5, 5, 0);
    QTest::newRow("1,1") << QGeoCoordinate(1, 1, 0);
    QTest::newRow("0,0") << QGeoCoordinate(0, 0, 0);

}

// public bool contains(QGeoCoordinate const& coordinate) const
void tst_QGeoMapPixmapObject::contains()
{
    QFETCH(QGeoCoordinate, coordinate);

    QGeoCoordinate center(0, 0, 0);

    QPixmap image(100, 100);

    QGeoMapPixmapObject* object = new QGeoMapPixmapObject(center, QPoint(0, 0), image);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object, SIGNAL(zValueChanged(int)));

    map->setCenter(coordinate);

    QPointF point = map->coordinateToScreenPosition(coordinate);

    bool contains = map->mapObjectsAtScreenPosition(point).size() == 1;

    QCOMPARE(object->contains(coordinate), contains);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);

}

// public QGeoBoundingBox boundingBox() const
void tst_QGeoMapPixmapObject::boundingBox()
{

    QGeoCoordinate center(0, 0, 0);

    QPixmap image(100, 100);

    QGeoMapPixmapObject* object = new QGeoMapPixmapObject(center, QPoint(0, 0), image);

    QVERIFY2(object->boundingBox().width()!=0,"no bounding box");
    QVERIFY2(object->boundingBox().height()!=0,"no bounding box");

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QVERIFY2(object->boundingBox().width()!=0,"no bounding box");
    QVERIFY2(object->boundingBox().height()!=0,"no bounding box");

}
ADD_TO_TESTSUITE(tst_QGeoMapPixmapObject)

#include "tst_qgeomappixmapobject.moc"

