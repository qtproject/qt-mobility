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
    QPen pen(Qt::black);
    pen.setCosmetic(true);
    QCOMPARE(object->pen(), pen);
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

    QVERIFY(list.at(0)==object);

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

    QVERIFY(list.at(0)==object);

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

    QVERIFY(list.at(0)==object);

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
    QTest::newRow("blue") << QPen(Qt::blue);
    QTest::newRow("white") << QPen(Qt::white);
}

// public QPen pen() const
void tst_QGeoMapRectangleObject::pen()
{
    QFETCH(QPen, pen);

    QGeoMapRectangleObject* object = new QGeoMapRectangleObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

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

    QVERIFY(list.at(0)==object);

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
void tst_QGeoMapRectangleObject::zvalue_data()
{
    QTest::addColumn<int>("zValue1");
    QTest::addColumn<int>("zValue2");
    QTest::addColumn<int>("zValue3");
    QTest::newRow("1,2,3") << 1 << 2 << 3;
    QTest::newRow("3,2,1") << 3 << 2 << 1;
    QTest::newRow("2,1,3") << 2 << 1 << 3;
}

// public int zValue() const
void tst_QGeoMapRectangleObject::zvalue()
{

    QFETCH(int, zValue1);
    QFETCH(int, zValue2);
    QFETCH(int, zValue3);

    QGeoCoordinate topLeft(2.0, -1.0, 0);
    QGeoCoordinate bottomRight(-2.0, 1.0, 0);

    QGeoBoundingBox box(topLeft, bottomRight);

    QGeoMapRectangleObject* object1 = new QGeoMapRectangleObject(box);
    QGeoMapRectangleObject* object2 = new QGeoMapRectangleObject(box);
    QGeoMapRectangleObject* object3 = new QGeoMapRectangleObject(box);

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

    map->setCenter(box.center());

    QPointF point = map->coordinateToScreenPosition(box.center());

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

    QVERIFY(map->mapObjectsAtScreenPosition(point).at(zValue1-1)==object1);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(zValue2-1)==object2);
    QVERIFY(map->mapObjectsAtScreenPosition(point).at(zValue3-1)==object3);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

}

// public bool isVisible() const
void tst_QGeoMapRectangleObject::isVisible()
{

    QGeoCoordinate topLeft(2.0, -1.0, 0);
    QGeoCoordinate bottomRight(-2.0, 1.0, 0);

    QGeoBoundingBox box(topLeft, bottomRight);

    QGeoMapRectangleObject* object = new QGeoMapRectangleObject(box);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(selectedChanged(bool)));
    QSignalSpy spy1(object, SIGNAL(visibleChanged(bool)));
    QSignalSpy spy2(object, SIGNAL(zValueChanged(int)));

    map->setCenter(box.center());

    QPointF point = map->coordinateToScreenPosition(box.center());

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
void tst_QGeoMapRectangleObject::isSelected()
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

void tst_QGeoMapRectangleObject::contains_data()
{

    QTest::addColumn<QGeoCoordinate>("coordinate");
    QTest::newRow("10,10") << QGeoCoordinate(10, 10, 0);
    QTest::newRow("20,20") << QGeoCoordinate(20, 20, 0);
    QTest::newRow("0,0") << QGeoCoordinate(0, 0, 0);

}

// public bool contains(QGeoCoordinate const& coordinate) const
void tst_QGeoMapRectangleObject::contains()
{
    QFETCH(QGeoCoordinate, coordinate);

    QGeoCoordinate topLeft(2.0, -1.0, 0);
    QGeoCoordinate bottomRight(-2.0, 1.0, 0);

    QGeoBoundingBox box(topLeft, bottomRight);

    QGeoMapRectangleObject* object = new QGeoMapRectangleObject(box);

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
void tst_QGeoMapRectangleObject::boundingBox()
{

    QGeoCoordinate topLeft(2.0, -1.0, 0);

    QGeoCoordinate bottomRight(-2.0, 1.0, 0);

    QGeoBoundingBox box(topLeft, bottomRight);

    QGeoMapRectangleObject* object = new QGeoMapRectangleObject(box);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QVERIFY2(object->boundingBox().width()>0,"no bounding box");
    QVERIFY2(object->boundingBox().height()>0,"no bounding box");

}

QTEST_MAIN(tst_QGeoMapRectangleObject)
#include "tst_qgeomaprectangleobject.moc"

