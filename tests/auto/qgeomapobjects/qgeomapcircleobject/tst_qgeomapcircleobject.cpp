#include "testhelper.h"
#include "testsuite.h"
#include <QtTest/QtTest>
#include <QMetaType>
#include "qgeomapcircleobject.h"
#include "qgeocoordinate.h"
#include "qgeoboundingcircle.h"
#include "qgraphicsgeomap.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QGeoBoundingCircle)
Q_DECLARE_METATYPE(QBrush)
Q_DECLARE_METATYPE(QPen)

class tst_QGeoMapCircleObject: public QObject
{
Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgeomapcircleobject_data();
    void qgeomapcircleobject();

    void brush_data();
    void brush();
    void center_data();
    void center();
    void circle_data();
    void circle();
    void pen_data();
    void pen();
    void radius_data();
    void radius();

private:
    TestHelper *m_helper;

};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGeoMapCircleObject::initTestCase()
{
    qRegisterMetaType<QGeoCoordinate> ();
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGeoMapCircleObject::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QGeoMapCircleObject::init()
{
    m_helper = new TestHelper();
    QTest::qWait(500);

    QGraphicsGeoMap* map = m_helper->map();

    if (!map)
        QFAIL("Could not create map!");
}

// This will be called after every test function.
void tst_QGeoMapCircleObject::cleanup()
{
    delete (m_helper);
    m_helper = 0;
}

void tst_QGeoMapCircleObject::qgeomapcircleobject_data()
{
}

void tst_QGeoMapCircleObject::qgeomapcircleobject()
{
    QGeoMapCircleObject* object = new QGeoMapCircleObject();

    //check initialization
    QCOMPARE(object->type(), QGeoMapObject::CircleType);
    QCOMPARE(object->brush(), QBrush());
    QCOMPARE(object->center(), QGeoCoordinate());
    QCOMPARE(object->circle(), QGeoBoundingCircle());
    QPen pen(QColor(Qt::black));
    pen.setCosmetic(true);
    QCOMPARE(object->pen(), pen);
    //TODO: is this value really correct ?
    QCOMPARE(object->radius(), -1.0);

    //check if can be added to map

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    map->removeMapObject(object);

    delete (object);

}

void tst_QGeoMapCircleObject::brush_data()
{
    QTest::addColumn<QBrush>("brush");
    QTest::newRow("white") << QBrush(Qt::white);
    QTest::newRow("blue") << QBrush(Qt::blue);
}

// public QBrush brush() const
void tst_QGeoMapCircleObject::brush()
{
    QFETCH(QBrush, brush);

    QGeoMapCircleObject* object = new QGeoMapCircleObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(radiusChanged(qreal)));

    object->setBrush(brush);

    QCOMPARE(object->brush(), brush);

    QCOMPARE(spy0.count(), 1);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);

}

void tst_QGeoMapCircleObject::center_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::newRow("52.5, 13.0, 0") << QGeoCoordinate(52.5, 13.0, 0);
    QTest::newRow("-52.5, 13.0, 0") << QGeoCoordinate(-52.5, 13.0, 0);
    QTest::newRow("52.5, -13.0, 0") << QGeoCoordinate(52.5, -13.0, 0);
    QTest::newRow("-52.5, -13.0, 0") << QGeoCoordinate(-52.5, -13.0, 0);
}

// public QGeoCoordinate center() const

void tst_QGeoMapCircleObject::center()
{
    QFETCH(QGeoCoordinate, center);

    QGeoMapCircleObject* object = new QGeoMapCircleObject(QGeoCoordinate(10.0, 10.0, 0.0), 1000);

    QCOMPARE(object->center(), QGeoCoordinate(10.0,10.0,0.0));

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(radiusChanged(qreal)));

    map->setCenter(center);

    //check if circle is not there
    QPointF point = map->coordinateToScreenPosition(center);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setCenter(center);

    QCOMPARE(object->center(), center);

    //check if circle is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);

}

void tst_QGeoMapCircleObject::circle_data()
{
    QTest::addColumn<QGeoBoundingCircle>("circle");
    QTest::newRow("52.5, 13.0, 2000") << QGeoBoundingCircle(QGeoCoordinate(52.5, 13.0, 0), 2000);
    QTest::newRow("-52.5, 13.0, 3000") << QGeoBoundingCircle(QGeoCoordinate(-52.5, 13.0, 0), 3000);
    QTest::newRow("52.5, -13.0, 4000") << QGeoBoundingCircle(QGeoCoordinate(52.5, -13.0, 0), 4000);
    QTest::newRow("-52.5, -13.0, 5000")
        << QGeoBoundingCircle(QGeoCoordinate(-52.5, -13.0, 0), 5000);

}

// public QGeoBoundingCircle circle() const
void tst_QGeoMapCircleObject::circle()
{
    QFETCH(QGeoBoundingCircle, circle);

    QGeoMapCircleObject* object = new QGeoMapCircleObject(QGeoCoordinate(10.0, 10.0, 0.0), 1000);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(radiusChanged(qreal)));

    //check if object is not there

    map->setCenter(circle.center());

    QPointF point = map->coordinateToScreenPosition(circle.center());

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setCircle(circle);

    QCOMPARE(object->circle(), circle);

    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 1);

}

void tst_QGeoMapCircleObject::pen_data()
{
    QTest::addColumn<QPen>("pen");
    QTest::newRow("blue") << QPen(QColor(Qt::blue));
    QTest::newRow("white") << QPen(QColor(Qt::white));
}

// public QPen pen() const
void tst_QGeoMapCircleObject::pen()
{
    QFETCH(QPen, pen);

    QGeoMapCircleObject* object = new QGeoMapCircleObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(radiusChanged(qreal)));

    object->setPen(pen);

    pen.setCosmetic(true);
    //according to specs it should be cosmetic
    QCOMPARE(object->pen(), pen);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(spy3.count(), 0);

}

void tst_QGeoMapCircleObject::radius_data()
{
    QTest::addColumn<QGeoCoordinate>("radiusTo");
    QTest::newRow("10.5, 10.0, 0") << QGeoCoordinate(10.5, 10.0, 0);
    QTest::newRow("-10.5, 10.0, 0") << QGeoCoordinate(-10.5, 10.0, 0);
    QTest::newRow("10.5, -10.0, 0") << QGeoCoordinate(10.5, -10.0, 0);
    QTest::newRow("-10.5, -10.0, 0") << QGeoCoordinate(-10.5, -10.0, 0);
}

// public qreal radius() const
void tst_QGeoMapCircleObject::radius()
{
    QFETCH(QGeoCoordinate, radiusTo);

    QGeoCoordinate center(1.0, 1.0, 0);

    QGeoMapCircleObject* object = new QGeoMapCircleObject(center, 10);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(centerChanged(QGeoCoordinate const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(radiusChanged(qreal)));

    map->setCenter(radiusTo);

    QPointF point = map->coordinateToScreenPosition(radiusTo);

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    qreal radius = radiusTo.distanceTo(center)*1.2;

    object->setRadius(radius);

    QCOMPARE(object->radius(), radius);

    //now it is bigger
    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 1);

}
ADD_TO_TESTSUITE(tst_QGeoMapCircleObject)

#include "tst_qgeomapcircleobject.moc"
