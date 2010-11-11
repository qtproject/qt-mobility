#include "testsuite.h"
#include "testhelper.h"
#include <QtTest/QtTest>
#include "qgeomappixmapobject.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"

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

    //check if can be added to map

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    map->removeMapObject(object);

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

    QCOMPARE(list.at(0),object);

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

    QCOMPARE(list.at(0),object);

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

    QCOMPARE(list.at(0),object);

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
ADD_TO_TESTSUITE(tst_QGeoMapPixmapObject)

#include "tst_qgeomappixmapobject.moc"

