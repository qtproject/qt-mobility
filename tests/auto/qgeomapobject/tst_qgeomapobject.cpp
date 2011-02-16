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

#include <QtTest/QtTest>

#include "testhelper.h"

#include "qgeomapobject.h"
#include "qgraphicsgeomap.h"
#include "qgeocoordinate.h"
#include <QGraphicsRectItem>
#include <QPointer>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QSignalSpy>
#include <QStyleOptionGraphicsItem>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoCoordinate)

class tst_QGeoMapObject : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void init();
    void cleanup();

    // basic property tests
    void holdsGraphicsItem();
    void ownsGraphicsItem();
    void type();
    void holdsVisible();
    void holdsOrigin();

    void addToMap();
    void drawsPixelEllipse();
    void findsPixelEllipse();
    void drawsBilinearEllipse();
    void drawsExactEllipse();

private:
    TestHelper *m_helper;
};

void tst_QGeoMapObject::initTestCase()
{
    qRegisterMetaType<QGeoCoordinate>();
}

void tst_QGeoMapObject::init()
{
    m_helper = new TestHelper();
    QGraphicsGeoMap *map = m_helper->map();

    if (!map)
        QFAIL("Could not create map!");
}

void tst_QGeoMapObject::cleanup()
{
    delete m_helper;
    m_helper = 0;
}

void tst_QGeoMapObject::ownsGraphicsItem()
{
    QPointer<QGeoMapObject> obj = new QGeoMapObject;
    QPointer<QGraphicsTextItem> ri = new QGraphicsTextItem;
    obj->setGraphicsItem(ri);

    delete obj;
    QVERIFY(!ri);
}

void tst_QGeoMapObject::holdsGraphicsItem()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QVERIFY(!obj->graphicsItem());

    QSignalSpy spy(obj, SIGNAL(graphicsItemChanged(QGraphicsItem*)));
    QGraphicsRectItem *ri = new QGraphicsRectItem;
    obj->setGraphicsItem(ri);

    QVERIFY(spy.count() == 1);
    QVERIFY(obj->graphicsItem() == ri);

    obj->setGraphicsItem(0);
    delete ri;
    delete obj;
}

void tst_QGeoMapObject::addToMap()
{
    // verify that you can add a blank QGeoMapObject
    // with no crashes

    QGeoMapObject *obj = new QGeoMapObject;

    QVERIFY(!obj->mapData());
    m_helper->map()->addMapObject(obj);
    QVERIFY(obj->mapData());
    QList<QGeoMapObject*> objs = m_helper->map()->mapObjects();
    QVERIFY(objs.contains(obj));

    QPixmap px(300, 300);
    QPainter p(&px);
    QStyleOptionGraphicsItem style;
    style.rect = QRect(0,0,300,300);

    m_helper->map()->paint(&p, &style, 0);

    m_helper->map()->removeMapObject(obj);
    delete obj;
}

#define verifyRgb(rgb,red,green,blue) \
    QCOMPARE(qRed(rgb), red); \
    QCOMPARE(qGreen(rgb), green); \
    QCOMPARE(qBlue(rgb), blue);

#define verifyNotRgb(rgb,red,green,blue) \
    QVERIFY(qRed(rgb) != red); \
    QVERIFY(qGreen(rgb) != green); \
    QVERIFY(qBlue(rgb) != blue);

static int centredEllipseRadius(const QPixmap &pxmap)
{
    QImage im = pxmap.toImage();

    QRgb center = im.pixel(150, 150);

    int r = 1;
    while (r < 300) {
        QRgb pxs[4];
        pxs[0] = im.pixel(150+r, 150);
        pxs[1] = im.pixel(150-r, 150);
        pxs[2] = im.pixel(150, 150+r);
        pxs[3] = im.pixel(150, 150-r);
        int i = 0;
        for (; i <3; i++)
            if (pxs[i] != center)
                break;
        if (pxs[i] != center)
            break;
        r++;
    }

    return r;
}

void tst_QGeoMapObject::findsPixelEllipse()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QGraphicsEllipseItem *el = new QGraphicsEllipseItem;
    el->setRect(-10, -10, 20, 20);
    el->setBrush(QBrush(Qt::black));

    obj->setGraphicsItem(el);
    obj->setOrigin(QGeoCoordinate(0, 0));

    QCOMPARE(obj->units(), QGeoMapObject::PixelUnit);
    QCOMPARE(obj->transformType(), QGeoMapObject::ExactTransform);

    m_helper->map()->setCenter(QGeoCoordinate(0, 0));
    m_helper->map()->resize(300, 300);

    QList<QGeoMapObject*> list;

    list = m_helper->map()->mapObjectsAtScreenPosition(QPointF(150, 150));
    QCOMPARE(list.size(), 0);

    m_helper->map()->addMapObject(obj);
    QTest::qWait(10);

    QList<QPointF> testPoints;
    testPoints << QPointF(150, 150) << QPointF(141, 150) << QPointF(150, 159)
               << QPointF(145, 145);

    foreach (QPointF tp, testPoints) {
        list = m_helper->map()->mapObjectsAtScreenPosition(tp);
        QCOMPARE(list.size(), 1);
        QVERIFY(list.contains(obj));
    }

    testPoints.clear();
    testPoints << QPointF(150, 161) << QPointF(139, 150) << QPointF(158, 158);

    foreach (QPointF tp, testPoints) {
        list = m_helper->map()->mapObjectsAtScreenPosition(tp);
        QCOMPARE(list.size(), 0);
    }
}

void tst_QGeoMapObject::drawsPixelEllipse()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QGraphicsEllipseItem *el = new QGraphicsEllipseItem;
    el->setRect(-10, -10, 20, 20);
    el->setBrush(QBrush(Qt::black));

    obj->setGraphicsItem(el);
    obj->setOrigin(QGeoCoordinate(0, 0));

    QCOMPARE(obj->units(), QGeoMapObject::PixelUnit);
    QCOMPARE(obj->transformType(), QGeoMapObject::ExactTransform);

    m_helper->map()->setCenter(QGeoCoordinate(0, 0));
    m_helper->map()->resize(300, 300);

    QPixmap px(300, 300);
    QPainter p(&px);
    QStyleOptionGraphicsItem style;
    style.rect = QRect(0,0,300,300);
    m_helper->map()->paint(&p, &style, 0);

    QImage im = px.toImage();
    QRgb pixel = im.pixel(150,150);
    verifyNotRgb(pixel, 0, 0, 0);

    m_helper->map()->addMapObject(obj);
    QTest::qWait(10);

    QPixmap px2(300, 300);
    QPainter p2(&px2);
    m_helper->map()->paint(&p2, &style, 0);
    QImage im2 = px2.toImage();

    verifyRgb(im2.pixel(150, 150), 0, 0, 0);
    // avoid the last pixel due to antialiasing
    verifyRgb(im2.pixel(141, 150), 0, 0, 0);
    verifyRgb(im2.pixel(150, 158), 0, 0, 0);
    verifyRgb(im2.pixel(145, 145), 0, 0, 0);

    verifyNotRgb(im2.pixel(150, 161), 0, 0, 0);
    verifyNotRgb(im2.pixel(139, 150), 0, 0, 0);
    verifyNotRgb(im2.pixel(157, 157), 0, 0, 0);

    int r = centredEllipseRadius(px2);
    QVERIFY(r >= 9 && r <= 11);

    m_helper->map()->removeMapObject(obj);
    delete obj;
}

void tst_QGeoMapObject::drawsBilinearEllipse()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QGraphicsEllipseItem *el = new QGraphicsEllipseItem;
    el->setRect(-100, -100, 200, 200);
    el->setBrush(QBrush(Qt::black));

    obj->setGraphicsItem(el);
    obj->setOrigin(QGeoCoordinate(0, 0));
    obj->setUnits(QGeoMapObject::MeterUnit);
    QCOMPARE(obj->transformType(), QGeoMapObject::BilinearTransform);

    m_helper->map()->setCenter(QGeoCoordinate(0, 0));
    m_helper->map()->setZoomLevel(15.0);
    m_helper->map()->resize(300, 300);

    m_helper->map()->addMapObject(obj);
    QTest::qWait(10);

    QPixmap *px[2];
    QPainter *p[2];
    for (int i=0; i < 2; i++) {
        px[i] = new QPixmap(300, 300);
        p[i] = new QPainter(px[i]);
    }
    QStyleOptionGraphicsItem style;
    style.rect = QRect(0,0,300,300);

    m_helper->map()->paint(p[0], &style, 0);
    int r15 = centredEllipseRadius(*px[0]);

    m_helper->map()->setZoomLevel(14.0);
    QTest::qWait(10);
    m_helper->map()->paint(p[1], &style, 0);
    int r14 = centredEllipseRadius(*px[1]);

    QVERIFY(r15 > r14);
    QVERIFY(r15 - r14 > 2);

    for (int i=0; i < 2; i++) {
        delete p[i];
        delete px[i];
    }
}

void tst_QGeoMapObject::drawsExactEllipse()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QGraphicsEllipseItem *el = new QGraphicsEllipseItem;
    el->setRect(-100, -100, 200, 200);
    el->setBrush(QBrush(Qt::black));

    obj->setGraphicsItem(el);
    obj->setOrigin(QGeoCoordinate(0, 0));
    obj->setUnits(QGeoMapObject::MeterUnit);
    QCOMPARE(obj->transformType(), QGeoMapObject::BilinearTransform);

    m_helper->map()->setCenter(QGeoCoordinate(0, 0));
    m_helper->map()->setZoomLevel(13.0);
    m_helper->map()->resize(300, 300);

    m_helper->map()->addMapObject(obj);
    QTest::qWait(10);

    QPixmap *px[4];
    QPainter *p[4];
    for (int i=0; i < 4; i++) {
        px[i] = new QPixmap(300, 300);
        p[i] = new QPainter(px[i]);
    }
    QStyleOptionGraphicsItem style;
    style.rect = QRect(0,0,3000,3000);

    m_helper->map()->paint(p[0], &style, 0);
    int r13 = centredEllipseRadius(*px[0]);

    m_helper->map()->setZoomLevel(12.0);
    QTest::qWait(10);
    m_helper->map()->paint(p[1], &style, 0);
    int r12 = centredEllipseRadius(*px[1]);

    obj->setTransformType(QGeoMapObject::ExactTransform);
    QCOMPARE(obj->transformType(), QGeoMapObject::ExactTransform);
    QTest::qWait(10);

    m_helper->map()->setZoomLevel(13.0);
    QTest::qWait(10);
    m_helper->map()->paint(p[2], &style, 0);
    int r13_ex = centredEllipseRadius(*px[2]);

    m_helper->map()->setZoomLevel(12.0);
    QTest::qWait(10);
    m_helper->map()->paint(p[3], &style, 0);
    int r12_ex = centredEllipseRadius(*px[3]);

    QVERIFY(r13_ex - r13 <= 1 && r13 - r13_ex <= 1);
    QVERIFY(r12_ex - r12 <= 1 && r12 - r12_ex <= 1);

    for (int i=0; i < 4; i++) {
        delete p[i];
        delete px[i];
    }
}

void tst_QGeoMapObject::type()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QCOMPARE(obj->type(), QGeoMapObject::NullType);

    QGraphicsRectItem *ri = new QGraphicsRectItem;

    obj->setGraphicsItem(ri);
    QCOMPARE(obj->type(), QGeoMapObject::CustomType);

    obj->setGraphicsItem(0);
    delete ri;
    delete obj;
}

void tst_QGeoMapObject::holdsVisible()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QVERIFY(obj->isVisible());
    QSignalSpy spy(obj, SIGNAL(visibleChanged(bool)));

    obj->setVisible(true);
    QCOMPARE(spy.count(), 0);

    obj->setVisible(false);
    QCOMPARE(spy.count(), 1);
    QVERIFY(!obj->isVisible());

    delete obj;
}

void tst_QGeoMapObject::holdsOrigin()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QCOMPARE(obj->origin(), QGeoCoordinate());

    QSignalSpy spy(obj, SIGNAL(originChanged(QGeoCoordinate)));

    QGeoCoordinate c(10, 5);
    obj->setOrigin(c);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(obj->origin(), c);

    delete obj;
}

QTEST_MAIN(tst_QGeoMapObject)
#include "tst_qgeomapobject.moc"
