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
#include <QList>
#include "qgeomappolygonobject.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"
#include "qgeoboundingbox.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QBrush)
Q_DECLARE_METATYPE(QList<QGeoCoordinate>)
Q_DECLARE_METATYPE(QPen)
Q_DECLARE_METATYPE(QGeoCoordinate)

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

    QVERIFY(list.at(0)==object);

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
    list2 << QGeoCoordinate(-5, 10) << QGeoCoordinate(-10, 10) << QGeoCoordinate(-10, 20)
        << QGeoCoordinate(-5, 20);
    QTest::newRow("(-5,10)-(-10,10)-(-10,20)-(-5,20)") << list2;

}

// public QList<QGeoCoordinate> path() const
void tst_QGeoMapPolygonObject::path()
{
    QFETCH(QList<QGeoCoordinate>, path);

    QGeoMapPolygonObject* object = new QGeoMapPolygonObject();

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(pathChanged(QList<QGeoCoordinate> const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));

    //check if object is not there

    QGeoBoundingBox rectangle(path.at(0), path.at(2));

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

    QPen pen1(Qt::blue);

    pen1.setWidth(10);

    pen1.setCosmetic(true);

    QTest::newRow("blue,10") << pen1;

    QPen pen2(Qt::white);

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

    QVERIFY(list.at(0)==object);

    QSignalSpy spy0(object, SIGNAL(brushChanged(QBrush const&)));
    QSignalSpy spy1(object, SIGNAL(pathChanged(QList<QGeoCoordinate> const&)));
    QSignalSpy spy2(object, SIGNAL(penChanged(QPen const&)));

    //check if object is not there

    map->setCenter(path.at(1));

    QPointF point = map->coordinateToScreenPosition(path.at(1));

    QPointF diff(0, object->pen().width() + 1);
    //TODO: uncomment if pen area should also be selectable
    //point -= diff;
    //QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),0);

    object->setPen(pen);

    QCOMPARE(object->pen(), pen);

    //check if object is there

    QCOMPARE(map->mapObjectsAtScreenPosition(point).size(),1);

    QCOMPARE(spy0.count(), 0);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

}

void tst_QGeoMapPolygonObject::zvalue_data()
{
    QTest::addColumn<int>("zValue1");
    QTest::addColumn<int>("zValue2");
    QTest::addColumn<int>("zValue3");
    QTest::newRow("1,2,3") << 1 << 2 << 3;
    QTest::newRow("3,2,1") << 3 << 2 << 1;
    QTest::newRow("2,1,3") << 2 << 1 << 3;
}

// public int zValue() const
void tst_QGeoMapPolygonObject::zvalue()
{

    QFETCH(int, zValue1);
    QFETCH(int, zValue2);
    QFETCH(int, zValue3);

    QList<QGeoCoordinate> path;

    path << QGeoCoordinate(2.0, -1.0, 0);
    path << QGeoCoordinate(2.0, 1.0, 0);
    path << QGeoCoordinate(-2.0, 1.0, 0);
    path << QGeoCoordinate(-2.0, -1.0, 0);

    QGeoBoundingBox box(path.at(0), path.at(2));

    QGeoMapPolygonObject* object1 = new QGeoMapPolygonObject();
    object1->setPath(path);
    QGeoMapPolygonObject* object2 = new QGeoMapPolygonObject();
    object2->setPath(path);
    QGeoMapPolygonObject* object3 = new QGeoMapPolygonObject();
    object3->setPath(path);

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
void tst_QGeoMapPolygonObject::isVisible()
{
    QList<QGeoCoordinate> path;

    path << QGeoCoordinate(2.0, -1.0, 0);
    path << QGeoCoordinate(2.0, 1.0, 0);
    path << QGeoCoordinate(-2.0, 1.0, 0);
    path << QGeoCoordinate(-2.0, -1.0, 0);

    QGeoBoundingBox box(path.at(0), path.at(2));

    QGeoMapPolygonObject* object = new QGeoMapPolygonObject();
    object->setPath(path);

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
void tst_QGeoMapPolygonObject::isSelected()
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

void tst_QGeoMapPolygonObject::contains_data()
{

    QTest::addColumn<QGeoCoordinate>("coordinate");
    QTest::newRow("10,10") << QGeoCoordinate(10, 10, 0);
    QTest::newRow("20,20") << QGeoCoordinate(20, 20, 0);
    QTest::newRow("0,0") << QGeoCoordinate(0, 0, 0);

}

// public bool contains(QGeoCoordinate const& coordinate) const
void tst_QGeoMapPolygonObject::contains()
{
    QFETCH(QGeoCoordinate, coordinate);

    QList<QGeoCoordinate> path;

    path << QGeoCoordinate(2.0, -1.0, 0);
    path << QGeoCoordinate(2.0, 1.0, 0);
    path << QGeoCoordinate(-2.0, 1.0, 0);
    path << QGeoCoordinate(-2.0, -1.0, 0);

    QGeoBoundingBox box(path.at(0), path.at(2));

    QGeoMapPolygonObject* object = new QGeoMapPolygonObject();

    object->setPath(path);

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
void tst_QGeoMapPolygonObject::boundingBox()
{

    QList<QGeoCoordinate> path;

    path << QGeoCoordinate(2.0, -1.0, 0);
    path << QGeoCoordinate(2.0, 1.0, 0);
    path << QGeoCoordinate(-2.0, 1.0, 0);
    path << QGeoCoordinate(-2.0, -1.0, 0);

    QGeoMapPolygonObject* object = new QGeoMapPolygonObject();

    object->setPath(path);

    QGraphicsGeoMap* map = m_helper->map();

    map->addMapObject(object);

    QList<QGeoMapObject *> list = map->mapObjects();

    QVERIFY(list.at(0)==object);

    QVERIFY2(object->boundingBox().width()>0,"no bounding box");
    QVERIFY2(object->boundingBox().height()>0,"no bounding box");

}

QTEST_MAIN(tst_QGeoMapPolygonObject)
#include "tst_qgeomappolygonobject.moc"

