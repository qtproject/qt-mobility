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
#include "qgeomapcustomobject.h"
#include "qgraphicsgeomap.h"
#include "qgeocoordinate.h"
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QPointer>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
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
    void type();
    void holdsVisible();
    void holdsOrigin();

    void addToMap();
    void drawsPixelEllipse();
    void findsPixelEllipse();
    void drawsBilinearEllipse();
    void drawsExactEllipse();
    void pixelChildren();
    void bilinearChildren();

    void autoUpdate();

private:
    TestHelper *m_helper;
};

void tst_QGeoMapObject::initTestCase()
{
    qRegisterMetaType<QGeoCoordinate>();
    qRegisterMetaType<QGraphicsItem*>();
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

void tst_QGeoMapObject::holdsGraphicsItem()
{
    QGeoMapCustomObject *obj = new QGeoMapCustomObject;

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

// only useful once we add custom graphics items back into the mix

void tst_QGeoMapObject::findsPixelEllipse()
{
    QGeoMapCustomObject *obj = new QGeoMapCustomObject;

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
    QGeoMapCustomObject *obj = new QGeoMapCustomObject;

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
    QGeoMapCustomObject *obj = new QGeoMapCustomObject;

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
    QGeoMapCustomObject *obj = new QGeoMapCustomObject;

    QGraphicsEllipseItem *el = new QGraphicsEllipseItem;
    el->setRect(-1000, -1000, 2000, 2000);
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
    style.rect = QRect(0,0,300,300);

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

void tst_QGeoMapObject::pixelChildren()
{
    QGeoMapCustomObject *obj = new QGeoMapCustomObject;

    QGraphicsEllipseItem *el = new QGraphicsEllipseItem;
    el->setRect(-40, -40, 80, 80);
    el->setBrush(QBrush(Qt::black));

    QGraphicsEllipseItem *childEl = new QGraphicsEllipseItem(el);
    childEl->setRect(-20, -20, 40, 40);
    childEl->setBrush(QBrush(Qt::blue));

    obj->setGraphicsItem(el);
    obj->setOrigin(QGeoCoordinate(0, 0));

    QCOMPARE(obj->units(), QGeoMapObject::PixelUnit);
    QCOMPARE(obj->transformType(), QGeoMapObject::ExactTransform);

    m_helper->map()->setCenter(QGeoCoordinate(0, 0));
    m_helper->map()->resize(300, 300);
    m_helper->map()->addMapObject(obj);

    QTest::qWait(10);

    QPixmap px(300, 300);
    QPainter p(&px);
    QStyleOptionGraphicsItem style;
    style.rect = QRect(0,0,300,300);
    m_helper->map()->paint(&p, &style, 0);

    int r = centredEllipseRadius(px);
    QVERIFY(r <= 21 && r >= 19);
}

void tst_QGeoMapObject::bilinearChildren()
{
    QGeoMapCustomObject *obj = new QGeoMapCustomObject;

    QGraphicsEllipseItem *el = new QGraphicsEllipseItem;
    el->setRect(-200, -200, 400, 400);
    el->setBrush(QBrush(Qt::black));

    obj->setGraphicsItem(el);
    obj->setOrigin(QGeoCoordinate(0, 0));
    obj->setUnits(QGeoMapObject::MeterUnit);
    QCOMPARE(obj->transformType(), QGeoMapObject::BilinearTransform);

    m_helper->map()->setCenter(QGeoCoordinate(0, 0));
    m_helper->map()->resize(300, 300);
    m_helper->map()->setZoomLevel(16.0);
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

    QTest::qWait(10);
    m_helper->map()->paint(p[0], &style, 0);
    int rParent = centredEllipseRadius(*px[0]);

    QGraphicsEllipseItem *childEl = new QGraphicsEllipseItem(el);
    childEl->setRect(-100, -100, 200, 200);
    childEl->setBrush(QBrush(Qt::blue));

    obj->update();

    QTest::qWait(10);
    m_helper->map()->paint(p[1], &style, 0);
    int rChild = centredEllipseRadius(*px[1]);

    QVERIFY(rParent > rChild);
    QVERIFY(rParent - rChild > 2);

    double ratio = rChild;
    ratio /= rParent;
    QVERIFY(ratio >= 0.49 && ratio <= 0.51);

    for (int i=0; i < 2; i++) {
        delete p[i];
        delete px[i];
    }
}

static void malteseCross(QPainterPath &path, const qreal &width,
                         const qreal &height, const qreal &thickness)
{
    path.moveTo(-0.5*thickness, -0.5*thickness);
    path.lineTo(-0.5*width, -0.5*thickness);
    path.lineTo(-0.5*width, 0.5*thickness);
    path.lineTo(-0.5*thickness, 0.5*thickness);
    path.lineTo(-0.5*thickness, 0.5*height);
    path.lineTo(0.5*thickness, 0.5*height);
    path.lineTo(0.5*thickness, 0.5*thickness);
    path.lineTo(0.5*width, 0.5*thickness);
    path.lineTo(0.5*width, -0.5*thickness);
    path.lineTo(0.5*thickness, -0.5*thickness);
    path.lineTo(0.5*thickness, -0.5*height);
    path.lineTo(-0.5*thickness, -0.5*height);
    path.closeSubpath();
}

struct Cross {
    int width;
    int height;
    int thickness;
};

static struct Cross findCross(QGraphicsGeoMap *map)
{
    struct Cross result;
    QGeoMapObject *obj = map->mapObjectsAtScreenPosition(QPointF(150, 150)).first();
    Q_ASSERT(obj);

    QList<QGeoMapObject*> list;

    for (int x = 0; x < 300; x++) {
        list = map->mapObjectsAtScreenPosition(QPointF(150-x, 150));
        if (!list.contains(obj)) {
            result.width = x - 1;
            break;
        }
        list = map->mapObjectsAtScreenPosition(QPointF(150+x, 150));
        if (!list.contains(obj)) {
            result.width = x - 1;
            break;
        }
    }

    for (int y = 0; y < 300; y++) {
        list = map->mapObjectsAtScreenPosition(QPointF(150, 150-y));
        if (!list.contains(obj)) {
            result.height = y - 1;
            break;
        }
        list = map->mapObjectsAtScreenPosition(QPointF(150, 150+y));
        if (!list.contains(obj)) {
            result.height = y - 1;
            break;
        }
    }

    for (int t = 0; t < 300; t++) {
        list = map->mapObjectsAtScreenPosition(QPointF(150-t, 150-t));
        if (!list.contains(obj)) {
            result.thickness = t - 1;
            break;
        }
        list = map->mapObjectsAtScreenPosition(QPointF(150+t, 150+t));
        if (!list.contains(obj)) {
            result.thickness = t - 1;
            break;
        }
    }

    return result;
}

#define EXF_NO_AUTOUPDATE   QEXPECT_FAIL("", "Auto-update not implemented yet", Continue)

void tst_QGeoMapObject::autoUpdate()
{
    QGeoMapCustomObject *obj = new QGeoMapCustomObject;

    QGraphicsPathItem *item = new QGraphicsPathItem;
    obj->setGraphicsItem(item);
    obj->setUnits(QGeoMapObject::MeterUnit);
    obj->setTransformType(QGeoMapObject::ExactTransform);
    obj->setOrigin(QGeoCoordinate(0, 0));

    QPainterPath bigPath;
    malteseCross(bigPath, 500, 500, 40);

    QPainterPath smallPath;
    malteseCross(smallPath, 300, 300, 60);

    item->setPath(bigPath);

    m_helper->map()->setCenter(QGeoCoordinate(0, 0));
    m_helper->map()->resize(300, 300);
    m_helper->map()->setZoomLevel(16.0);
    m_helper->map()->addMapObject(obj);

    QTest::qWait(10);
    struct Cross c1 = findCross(m_helper->map());

    item->setPath(smallPath);
    QTest::qWait(10);
    struct Cross c2 = findCross(m_helper->map());

    EXF_NO_AUTOUPDATE;
    QVERIFY(c1.width > c2.width);
    EXF_NO_AUTOUPDATE;
    QVERIFY(c1.width - c2.width > 2);
    EXF_NO_AUTOUPDATE;
    QVERIFY(c1.height > c2.height);
    EXF_NO_AUTOUPDATE;
    QVERIFY(c1.height - c2.height > 2);
    EXF_NO_AUTOUPDATE;
    QVERIFY(c2.thickness > c1.thickness);
    EXF_NO_AUTOUPDATE;
    QVERIFY(c2.thickness - c1.thickness > 2);

    obj->update();
    QTest::qWait(10);
    struct Cross c3 = findCross(m_helper->map());

    QVERIFY(c1.width > c3.width);
    QVERIFY(c1.width - c3.width > 2);
    QVERIFY(c1.height > c3.height);
    QVERIFY(c1.height - c3.height > 2);
    QVERIFY(c3.thickness > c1.thickness);
    QVERIFY(c3.thickness - c1.thickness > 2);

    qreal ratio = c3.width;
    ratio /= c1.width;
    QVERIFY(ratio <= 0.61 && ratio >= 0.59);
}

void tst_QGeoMapObject::type()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QCOMPARE(obj->type(), QGeoMapObject::NullType);

    QGraphicsRectItem *ri = new QGraphicsRectItem;

//    obj->setGraphicsItem(ri);
//    QCOMPARE(obj->type(), QGeoMapObject::CustomType);

//    obj->setGraphicsItem(0);
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
    QGeoMapCustomObject *obj = new QGeoMapCustomObject;

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
