/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

//TESTED_COMPONENT=src/location

#include <QtTest/QtTest>
#include <qgeoboundingbox.h>
#include <qgeocoordinate.h>

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QGeoBoundingBox)

class tst_QGeoBoundingBox : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgeoboundingbox_data();
    void qgeoboundingbox();

    void bottomRight_data();
    void bottomRight();
    void contains_data();
    void contains();
    void containsbbox_data();
    void containsbbox();
    void center_data();
    void center();
    void height_data();
    void height();
    void width_data();
    void width();
    void intersects_data();
    void intersects();
    void isEmpty_data();
    void isEmpty();
    void resizeToCenter_data();
    void resizeToCenter();
    void setBottomRight_data();
    void setBottomRight();
    void setTopLeft_data();
    void setTopLeft();
    void topLeft_data();
    void topLeft();
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGeoBoundingBox::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGeoBoundingBox::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QGeoBoundingBox::init()
{
}

// This will be called after every test function.
void tst_QGeoBoundingBox::cleanup()
{
}

void tst_QGeoBoundingBox::qgeoboundingbox_data()
{
}

void tst_QGeoBoundingBox::qgeoboundingbox()
{
    QGeoBoundingBox box;
#if 0
    QCOMPARE(box.bottomRight(), QGeoCoordinate);
    QCOMPARE(box.contains(QGeoCoordinate const), false);
    QCOMPARE(box.contains(QGeoBoundingBox const), false);
    QCOMPARE(box.center(), QGeoCoordinate);
    QCOMPARE(box.height(), double);
    QCOMPARE(box.width(), double);
    QCOMPARE(box.intersects(QGeoBoundingBox const), false);
    QCOMPARE(box.isEmpty(), false);
    box.resizeToCenter(QGeoCoordinate const);
    box.setBottomRight(QGeoCoordinate const);
    box.setTopLeft(QGeoCoordinate const);
    QCOMPARE(box.topLeft(), QGeoCoordinate);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_QGeoBoundingBox::bottomRight_data()
{
#if 0
    QTest::addColumn<QGeoCoordinate>("bottomRight");
    QTest::newRow("null") << QGeoCoordinate();
#endif
}

// public QGeoCoordinate bottomRight() const
void tst_QGeoBoundingBox::bottomRight()
{
#if 0
    QFETCH(QGeoCoordinate, bottomRight);

    QGeoBoundingBox box;

    QCOMPARE(box.bottomRight(), bottomRight);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_QGeoBoundingBox::contains_data()
{
    QTest::addColumn<QGeoCoordinate>("coord");
    QTest::addColumn<bool>("contains");
    QTest::addColumn<QGeoCoordinate>("topLeft");
    QTest::addColumn<QGeoCoordinate>("bottomRight");
    QTest::newRow("Contains_1") << QGeoCoordinate(2.0, 2.0) << true << QGeoCoordinate(5.0, -5.0) << QGeoCoordinate(-5.0, 5.0);
    QTest::newRow("Contains_2") << QGeoCoordinate(5.0, -175.0) << true << QGeoCoordinate(10.0, 150.0) << QGeoCoordinate(-10.0, -155.0);
    QTest::newRow("Contains_3") << QGeoCoordinate(5.0, 175.0) << true << QGeoCoordinate(10.0, 150.0) << QGeoCoordinate(-10.0, -155.0);
    QTest::newRow("DontContain_1") << QGeoCoordinate(10.0, 2.0) << false << QGeoCoordinate(5.0, -5.0) << QGeoCoordinate(-5.0, 5.0);
    QTest::newRow("DontContain_2") << QGeoCoordinate(5.0, -145.0) << false << QGeoCoordinate(10.0, 150.0) << QGeoCoordinate(-10.0, -155.0);
    QTest::newRow("DontContain_3") << QGeoCoordinate(5.0, 140.0) << false << QGeoCoordinate(10.0, 150.0) << QGeoCoordinate(-10.0, -155.0);
}

// public bool contains(QGeoCoordinate const coord)
void tst_QGeoBoundingBox::contains()
{
    QFETCH(QGeoCoordinate, coord);
    QFETCH(bool, contains);
    QFETCH(QGeoCoordinate, topLeft);
    QFETCH(QGeoCoordinate, bottomRight);
    
    QGeoBoundingBox box(topLeft,bottomRight);

    QCOMPARE(box.contains(coord), contains);
}

// public bool contains(QGeoBoundingBox const bbox)
void tst_QGeoBoundingBox::containsbbox_data()
{
    QTest::addColumn<QGeoBoundingBox>("bbox");
    QTest::addColumn<bool>("contains");
    QTest::addColumn<QGeoCoordinate>("topLeft");
    QTest::addColumn<QGeoCoordinate>("bottomRight");
    QTest::newRow("Contains_1") << QGeoBoundingBox(QGeoCoordinate(2.0, -2.0),QGeoCoordinate(-2.0, 2.0)) << true << QGeoCoordinate(5.0, -5.0) << QGeoCoordinate(-5.0, 5.0);
    QTest::newRow("ContainsOD_1") << QGeoBoundingBox(QGeoCoordinate(75.0,170.0),QGeoCoordinate(70.0, -130.0)) << true << QGeoCoordinate(80.0, 120.0) << QGeoCoordinate(-20.0,-120.0);
    QTest::newRow("ContainsOD_2") << QGeoBoundingBox(QGeoCoordinate(75.0,-140.0),QGeoCoordinate(70.0, -125.0)) << true << QGeoCoordinate(80.0, 120.0) << QGeoCoordinate(-20.0,-120.0);
    QTest::newRow("ContainsOD_3") << QGeoBoundingBox(QGeoCoordinate(75.0,165.0),QGeoCoordinate(70.0, 165.0)) << true << QGeoCoordinate(80.0, 120.0) << QGeoCoordinate(-20.0,170.0);
    QTest::newRow("ContainsOD_4") << QGeoBoundingBox(QGeoCoordinate(75.0,165.0),QGeoCoordinate(70.0, 175.0)) << true << QGeoCoordinate(80.0, 120.0) << QGeoCoordinate(-20.0,-170.0);
    QTest::newRow("DontContain_1") << QGeoBoundingBox(QGeoCoordinate(6.0, -5.0),QGeoCoordinate(-4.0, 2.0)) << false << QGeoCoordinate(5.0, -5.0) << QGeoCoordinate(-5.0, 5.0);
    QTest::newRow("DontContain_2") << QGeoBoundingBox(QGeoCoordinate(5.0, -5.0),QGeoCoordinate(-5.0, 6.0)) << false << QGeoCoordinate(5.0, -5.0) << QGeoCoordinate(-5.0, 5.0);
    QTest::newRow("DontContainOD_1") << QGeoBoundingBox(QGeoCoordinate(85.0,-115.0),QGeoCoordinate(70.0, -80.0)) << false << QGeoCoordinate(80.0, 120.0) << QGeoCoordinate(-20.0,-120.0);
}

// public bool contains(QGeoCoordinate const coord)
void tst_QGeoBoundingBox::containsbbox()
{
    QFETCH(QGeoBoundingBox, bbox);
    QFETCH(bool, contains);
    QFETCH(QGeoCoordinate, topLeft);
    QFETCH(QGeoCoordinate, bottomRight);

    QGeoBoundingBox box(topLeft,bottomRight);

    QCOMPARE(box.contains(bbox), contains);
}

void tst_QGeoBoundingBox::center_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<QGeoCoordinate>("topLeft");
    QTest::addColumn<QGeoCoordinate>("bottomRight");
    QTest::newRow("center1") << QGeoCoordinate(0.0, -0.0) << QGeoCoordinate(5.0, -5.0) << QGeoCoordinate(-5.0, 5.0);
    QTest::newRow("center2") << QGeoCoordinate(-35.0, 175.0) << QGeoCoordinate(-20.0, 150.0) << QGeoCoordinate(-50.0, -160.0);
    QTest::newRow("center3") << QGeoCoordinate(-35.0, -155.0) << QGeoCoordinate(-20.0, 150.0) << QGeoCoordinate(-50.0, -100.0);
    QTest::newRow("center4") << QGeoCoordinate(-10.0, -30.0) << QGeoCoordinate(15.0, -50.0) << QGeoCoordinate(-35.0, -10.0);
}

// public QGeoCoordinate center() const
void tst_QGeoBoundingBox::center()
{
    QFETCH(QGeoCoordinate, center);
    QFETCH(QGeoCoordinate, topLeft);
    QFETCH(QGeoCoordinate, bottomRight);
    
    QGeoBoundingBox box(topLeft,bottomRight);

    QCOMPARE(box.center(), center);
}

Q_DECLARE_METATYPE(double)
void tst_QGeoBoundingBox::height_data()
{
    QTest::addColumn<double>("height");
    QTest::addColumn<QGeoCoordinate>("topLeft");
    QTest::addColumn<QGeoCoordinate>("bottomRight");
    QTest::newRow("height_10") << double(10) << QGeoCoordinate(5.0, -5.0) << QGeoCoordinate(-5.0, 5.0);
    QTest::newRow("height_100") << double(100) << QGeoCoordinate(80.0, 120.0) << QGeoCoordinate(-20.0,-120.0);
}

// public double height() const
void tst_QGeoBoundingBox::height()
{
    QFETCH(double, height);
    QFETCH(QGeoCoordinate, topLeft);
    QFETCH(QGeoCoordinate, bottomRight);
    
    QGeoBoundingBox box(topLeft,bottomRight);

    QCOMPARE(box.height(), height);
}

void tst_QGeoBoundingBox::width_data()
{
    QTest::addColumn<double>("width");
    QTest::addColumn<QGeoCoordinate>("topLeft");
    QTest::addColumn<QGeoCoordinate>("bottomRight");
    QTest::newRow("width_10") << double(10) << QGeoCoordinate(5.0, -5.0) << QGeoCoordinate(-5.0, 5.0);
    QTest::newRow("widthOD_120") << double(120) << QGeoCoordinate(80.0, 120.0) << QGeoCoordinate(-20.0,-120.0);
}

// public double width() const
void tst_QGeoBoundingBox::width()
{
    QFETCH(double, width);
    QFETCH(QGeoCoordinate, topLeft);
    QFETCH(QGeoCoordinate, bottomRight);
    
    QGeoBoundingBox box(topLeft,bottomRight);

    QCOMPARE(box.width(), width);
}

void tst_QGeoBoundingBox::intersects_data()
{
    QTest::addColumn<QGeoBoundingBox>("bbox");
    QTest::addColumn<bool>("intersects");
    QTest::addColumn<QGeoCoordinate>("topLeft");
    QTest::addColumn<QGeoCoordinate>("bottomRight");
    QTest::newRow("Intersecting_1") << QGeoBoundingBox(QGeoCoordinate(8.0, -10.0),QGeoCoordinate(4.0, -2.0)) << true << QGeoCoordinate(5.0, -5.0) << QGeoCoordinate(-5.0, 5.0);
    QTest::newRow("IntersectingOD_1") << QGeoBoundingBox(QGeoCoordinate(85.0,179.0),QGeoCoordinate(70.0, -80.0)) << true << QGeoCoordinate(80.0, 120.0) << QGeoCoordinate(-20.0,-120.0);
    QTest::newRow("IntersectingOD_2") << QGeoBoundingBox(QGeoCoordinate(85.0,-130.0),QGeoCoordinate(70.0, -80.0)) << true << QGeoCoordinate(80.0, 120.0) << QGeoCoordinate(-20.0,-120.0);
    QTest::newRow("IntersectingOD_3") << QGeoBoundingBox(QGeoCoordinate(85.0,165.0),QGeoCoordinate(70.0, -80.0)) << true << QGeoCoordinate(80.0, 120.0) << QGeoCoordinate(-20.0,170.0);
    QTest::newRow("NotIntersecting_1") << QGeoBoundingBox(QGeoCoordinate(8.0, -10.0),QGeoCoordinate(6.0, -2.0)) << false << QGeoCoordinate(5.0, -5.0) << QGeoCoordinate(-5.0, 5.0);
    QTest::newRow("NotIntersecting_2") << QGeoBoundingBox(QGeoCoordinate(8.0, -10.0),QGeoCoordinate(3.0, -6.0)) << false << QGeoCoordinate(5.0, -5.0) << QGeoCoordinate(-5.0, 5.0);
    QTest::newRow("NotIntersectingOD_1") << QGeoBoundingBox(QGeoCoordinate(85.0,-115.0),QGeoCoordinate(70.0, -80.0)) << false << QGeoCoordinate(80.0, 120.0) << QGeoCoordinate(-20.0,-120.0);
}

// public bool intersects(QGeoBoundingBox const bbox) const
void tst_QGeoBoundingBox::intersects()
{
    QFETCH(QGeoBoundingBox, bbox);
    QFETCH(bool, intersects);
    QFETCH(QGeoCoordinate, topLeft);
    QFETCH(QGeoCoordinate, bottomRight);

    QGeoBoundingBox box(topLeft,bottomRight);

    QCOMPARE(box.intersects(bbox), intersects);
}

void tst_QGeoBoundingBox::isEmpty_data()
{
    QTest::addColumn<bool>("isEmpty");
    QTest::newRow("true") << true;
    QTest::newRow("false") << false;
}

// public bool isEmpty() const
void tst_QGeoBoundingBox::isEmpty()
{
#if 0
    QFETCH(bool, isEmpty);

    QGeoBoundingBox box;

    QCOMPARE(box.isEmpty(), isEmpty);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_QGeoBoundingBox::resizeToCenter_data()
{
#if 0
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::newRow("null") << QGeoCoordinate();
#endif
}

// public void resizeToCenter(QGeoCoordinate const center)
void tst_QGeoBoundingBox::resizeToCenter()
{
#if 0
    QFETCH(QGeoCoordinate, center);

    QGeoBoundingBox box;

    box.resizeToCenter(center);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_QGeoBoundingBox::setBottomRight_data()
{
#if 0
    QTest::addColumn<QGeoCoordinate>("bottomRight");
    QTest::newRow("null") << QGeoCoordinate();
#endif
}

// public void setBottomRight(QGeoCoordinate const bottomRight)
void tst_QGeoBoundingBox::setBottomRight()
{
#if 0
    QFETCH(QGeoCoordinate, bottomRight);

    QGeoBoundingBox box;

    box.setBottomRight(bottomRight);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_QGeoBoundingBox::setTopLeft_data()
{
#if 0
    QTest::addColumn<QGeoCoordinate>("topLeft");
    QTest::newRow("null") << QGeoCoordinate();
#endif
}

// public void setTopLeft(QGeoCoordinate const topLeft)
void tst_QGeoBoundingBox::setTopLeft()
{
#if 0
    QFETCH(QGeoCoordinate, topLeft);

    QGeoBoundingBox box;

    box.setTopLeft(topLeft);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_QGeoBoundingBox::topLeft_data()
{
#if 0
    QTest::addColumn<QGeoCoordinate>("topLeft");
    QTest::newRow("null") << QGeoCoordinate();
#endif
}

// public QGeoCoordinate topLeft() const
void tst_QGeoBoundingBox::topLeft()
{
#if 0
    QFETCH(QGeoCoordinate, topLeft);

    QGeoBoundingBox box;

    QCOMPARE(box.topLeft(), topLeft);
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

QTEST_MAIN(tst_QGeoBoundingBox)
#include "tst_qgeoboundingbox.moc"

