#include "testsuite.h"
#include "testhelper.h"
#include <QtTest/QtTest>
#include "qgeomaprectangleobject.h"
#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"
#include "qgraphicsgeomap.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QGeoBoundingBox)
Q_DECLARE_METATYPE(QBrush)
Q_DECLARE_METATYPE(QPen)

class tst_QGeoMapRectangleObject: public QObject
{
Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgeomaprectangleobject_data();
    void qgeomaprectangleobject();

    void bottomRight_data();
    void bottomRight();
    void bounds_data();
    void bounds();
    void brush_data();
    void brush();
    void pen_data();
    void pen();
    void topLeft_data();
    void topLeft();

private:
    TestHelper *m_helper;
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGeoMapRectangleObject::initTestCase()
{
    qRegisterMetaType<QGeoCoordinate> ();

}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGeoMapRectangleObject::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QGeoMapRectangleObject::init()
{
    m_helper = new TestHelper();
    QTest::qWait(500);

    QGraphicsGeoMap* map = m_helper->map();

    if (!map)
        QFAIL("Could not create map!");
}

// This will be called after every test function.
void tst_QGeoMapRectangleObject::cleanup()
{
    delete (m_helper);
    m_helper = 0;
}

void tst_QGeoMapRectangleObject::qgeomaprectangleobject_data()
{
}

void tst_QGeoMapRectangleObject::qgeomaprectangleobject()
{

    QGeoMapRectangleObject* object = new QGeoMapRectangleObject();

    //check initialization
    QCOMPARE(object->type(), QGeoMapObject::RectangleType);
    QCOMPARE(object->topLeft(), QGeoCoordinate());
    QCOMPARE(object->bottomRight(), QGeoCoordinate());
    QCOMPARE(object->bounds(), QGeoBoundingBox());
    QCOMPARE(object->brush(), QBrush());
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

void tst_QGeoMapRectangleObject::bottomRight_data()
{

    QTest::addColumn<QGeoCoordinate>("testBottomRight");
    QTest::newRow("-10,10") << QGeoCoordinate(-10, 10);
    QTest::newRow("-20,20") << QGeoCoordinate(-20, 20);

}

// public QGeoCoordinate bottomRight() const
void tst_QGeoMapRectangleObject::bottomRight()
{
    QFETCH(QGeoCoordinate, testBottomRight);

    QGeoCoordinate topLeft(5.0, -5.0);
    QGeoCoordinate bottomRight(-5.0, 5.0);

    QGeoMapRectangleObject* object = new QGeoMapRectangleObject(topLeft, bottomRight);

    QCOMPARE(object->bottomRight(), bottomRight);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(bottomRightChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(topLeftChanged(QGeoCoordinate const&)));

    object->setBottomRight(testBottomRight);

    QCOMPARE(object->bottomRight(), testBottomRight);

    map->setCenter(object->boundingBox().center());
    QPointF point = map->coordinateToScreenPosition(object->boundingBox().center());

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 1);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);

}

void tst_QGeoMapRectangleObject::bounds_data()
{

    QTest::addColumn<QGeoBoundingBox>("bounds");
    QTest::newRow("50,10 x 53,13") << QGeoBoundingBox(QGeoCoordinate(53, 10),
        QGeoCoordinate(50, 13));
    QTest::newRow("-45,-13 x -50,-10") << QGeoBoundingBox(QGeoCoordinate(-45, -13), QGeoCoordinate(
        -50, -10));
}

// public QGeoBoundingBox bounds() const
void tst_QGeoMapRectangleObject::bounds()
{
    QFETCH(QGeoBoundingBox, bounds);

    QGeoCoordinate topLeft(5.0, -5.0);
    QGeoCoordinate bottomRight(-5.0, 5.0);

    QGeoMapRectangleObject* object = new QGeoMapRectangleObject(topLeft, bottomRight);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(bottomRightChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(topLeftChanged(QGeoCoordinate const&)));

    //check if rectangle is not there

    map->setCenter(bounds.center());

    QPointF point = map->coordinateToScreenPosition(bounds.center());

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setBounds(bounds);

    QCOMPARE(object->bounds(), bounds);

    //check if rectangle is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 1);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 1);

}

void tst_QGeoMapRectangleObject::brush_data()
{

    QTest::addColumn<QBrush>("brush");
    QTest::newRow("white") << QBrush(Qt::white);
    QTest::newRow("blue") << QBrush(Qt::blue);
}

// public QBrush brush() const
void tst_QGeoMapRectangleObject::brush()
{
    QFETCH(QBrush, brush);

    QGeoMapRectangleObject* object = new QGeoMapRectangleObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(bottomRightChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(topLeftChanged(QGeoCoordinate const&)));

    object->setBrush(brush);

    QCOMPARE(object->brush(), brush);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);

}

void tst_QGeoMapRectangleObject::pen_data()
{
    QTest::addColumn<QPen>("pen");
    QTest::newRow("blue") << QPen(QColor(Qt::blue));
    QTest::newRow("white") << QPen(QColor(Qt::white));
}

// public QPen pen() const
void tst_QGeoMapRectangleObject::pen()
{
    QFETCH(QPen, pen);

    QGeoMapRectangleObject* object = new QGeoMapRectangleObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(bottomRightChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(topLeftChanged(QGeoCoordinate const&)));

    object->setPen(pen);

    pen.setCosmetic(true);

    //according to specs it should be cosmetic
    QCOMPARE(object->pen(), pen);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(spy3.count(), 0);

}

void tst_QGeoMapRectangleObject::topLeft_data()
{

    QTest::addColumn<QGeoCoordinate>("testTopLeft");
    QTest::newRow("10,-10") << QGeoCoordinate(10, -10);
    QTest::newRow("20,-20") << QGeoCoordinate(20, -20);

}

// public QGeoCoordinate topLeft() const
void tst_QGeoMapRectangleObject::topLeft()
{
    QFETCH(QGeoCoordinate, testTopLeft);

    QGeoCoordinate topLeft(5.0, -5.0);
    QGeoCoordinate bottomRight(-5.0, 5.0);

    QGeoMapRectangleObject* object = new QGeoMapRectangleObject(topLeft, bottomRight);

    QCOMPARE(object->topLeft(), topLeft);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QCOMPARE(list.at(0),object);

    QSignalSpy spy0(object, SIGNAL(bottomRightChanged(QGeoCoordinate const&)));
    QSignalSpy spy1(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));
    QSignalSpy spy3(object, SIGNAL(topLeftChanged(QGeoCoordinate const&)));

    object->setTopLeft(testTopLeft);

    QCOMPARE(object->topLeft(), testTopLeft);

    map->setCenter(object->boundingBox().center());

    QPointF point = map->coordinateToScreenPosition(object->boundingBox().center());

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 1);

}
ADD_TO_TESTSUITE(tst_QGeoMapRectangleObject)

#include "tst_qgeomaprectangleobject.moc"

