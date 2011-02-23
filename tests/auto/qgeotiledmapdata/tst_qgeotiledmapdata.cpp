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

//TESTED_COMPONENT=src/location

#include "../qlocationtestutils_p.h"

#include <qgeocoordinate.h>
#include <qgraphicsgeomap.h>
#include <qgeomappixmapobject.h>
#include <qgeomapdata.h>
#include <qgeomappingmanager.h>
#include <qtest.h>

#include <QtGui/QApplication>
#include <QDebug>
#include <QtGui>
#include <QPixmap>

#include "../pixelindex/pixelindexengine.h"

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QGeoCoordinate::CoordinateFormat)
Q_DECLARE_METATYPE(QGeoCoordinate::CoordinateType)
Q_DECLARE_METATYPE(QGeoMappingManagerEngine*)
Q_DECLARE_METATYPE(QGeoMapData*)
Q_DECLARE_METATYPE(QGeoMapPixmapObject*)

class tst_QGeoTiledMapData : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void pixmapDraw_data();
    void pixmapDraw();

    void objectsAtPoint_data();
    void objectsAtPoint();

    void pixmapAtDateline_data();
    void pixmapAtDateline();

    void panTest_data();
    void panTest();

private:
    void makeFixtures(QGeoMapData *&gmd, QGeoMapPixmapObject *&obj,
                      const QGeoCoordinate &center, const QGeoCoordinate &pixmap,
                      const qreal &zoom, const QPixmap &targetPixmap,
                      QGeoMappingManagerEngine *mgr, const QSize &window,
                      const QSize &target);

};

void tst_QGeoTiledMapData::initTestCase()
{
    qRegisterMetaType<QGeoCoordinate>("QGeoCoordinate");
    qRegisterMetaType<QGeoMappingManagerEngine*>("QGeoMappingManagerEngine*");
    qRegisterMetaType<QGeoMapData*>("QGeoMapData*");
    qRegisterMetaType<QGeoMapPixmapObject*>("QGeoMapPixmapObject*");
    qRegisterMetaType<QPixmap>("QPixmap");
}

void tst_QGeoTiledMapData::makeFixtures(QGeoMapData *&gmd,
                                        QGeoMapPixmapObject *&obj,
                                        const QGeoCoordinate &center,
                                        const QGeoCoordinate &pixmap,
                                        const qreal &zoom,
                                        const QPixmap &targetPixmap,
                                        QGeoMappingManagerEngine *mgr,
                                        const QSize &window,
                                        const QSize &target)
{
    gmd = mgr->createMapData();
    // this call shouldn't be necessary?
    // but you get a nice happy sigsegv without it
    gmd->init();
    gmd->setWindowSize(window);
    gmd->setZoomLevel(zoom);
    gmd->setCenter(center);
    obj = new QGeoMapPixmapObject();
    obj->setPixmap(targetPixmap);
    obj->setCoordinate(pixmap);
    gmd->addMapObject(obj);
    obj->setVisible(true);
}

void tst_QGeoTiledMapData::pixmapDraw_data()
{
    QTest::addColumn<QGeoMappingManagerEngine*>("mgr");
    QTest::addColumn<QGeoMapData*>("gmd");
    QTest::addColumn<QGeoMapPixmapObject*>("obj");
    QTest::addColumn<QSize>("windowSize");
    QTest::addColumn<QSize>("targetSize");
    QTest::addColumn<QPoint>("offset");

    QMap<QString, QVariant> params;
    QGeoMappingManagerEngine *mgr = new WhiteTileEngine(params, this);

    QSize window = QSize(500, 500);
    QSize target = QSize(50, 50);
    QPoint offset = QPoint(0, 0);
    QPixmap targetPixmap = indexedPixmap(target.width(), target.height());

    QGeoMapData *gmd;
    QGeoCoordinate center;
    QGeoMapPixmapObject *obj;

    center = QGeoCoordinate(-27.58, 153.10);
    makeFixtures(gmd, obj, center, center, 3.0, targetPixmap, mgr, window, target);
    QTest::newRow("Brisbane @z=3") << mgr << gmd << obj << window << target << offset;

    center = QGeoCoordinate(45.6, -160.2);
    makeFixtures(gmd, obj, center, center, 2.0, targetPixmap, mgr, window, target);
    QTest::newRow("Somewhere up north") << mgr << gmd << obj << window << target << offset;

    center = QGeoCoordinate(0.0, 0.0);
    makeFixtures(gmd, obj, center, center, 1.0, targetPixmap, mgr, window, target);
    QTest::newRow("At 0,0") << mgr << gmd << obj << window << target << offset;

    center = QGeoCoordinate(0.0, 0.0);
    makeFixtures(gmd, obj, center, center, 0.0, targetPixmap, mgr, window, target);
    QTest::newRow("Zoom level 0") << mgr << gmd << obj << window << target << offset;

    center = QGeoCoordinate(0.0, 179.9);
    makeFixtures(gmd, obj, center, center, 2.0, targetPixmap, mgr, window, target);
    QTest::newRow("Positive dateline") << mgr << gmd << obj << window << target << offset;

    center = QGeoCoordinate(0.0, -180.0);
    makeFixtures(gmd, obj, center, center, 2.0, targetPixmap, mgr, window, target);
    QTest::newRow("Negative dateline") << mgr << gmd << obj << window << target << offset;

    center = QGeoCoordinate(-27.58, 153.10);
    makeFixtures(gmd, obj, center, center, 3.0, targetPixmap, mgr, window, target);
    offset = QPoint(5,5);
    obj->setOffset(offset);
    QTest::newRow("Brisbane with offset") << mgr << gmd << obj << window << target << offset;

    center = QGeoCoordinate(-27.58, 153.10);
    makeFixtures(gmd, obj, center, center, 3.0, targetPixmap, mgr, window, target);
    offset = QPoint(-5,-5);
    obj->setOffset(offset);
    QTest::newRow("Brisbane with -ve offset") << mgr << gmd << obj << window << target << offset;

    center = QGeoCoordinate(0.0, -180.0);
    makeFixtures(gmd, obj, center, center, 2.0, targetPixmap, mgr, window, target);
    offset = QPoint(-20, 0);
    obj->setOffset(offset);
    QTest::newRow("Negative dateline with offset") << mgr << gmd << obj << window << target << offset;
}

void tst_QGeoTiledMapData::objectsAtPoint_data()
{
    QTest::addColumn<QGeoMappingManagerEngine*>("mgr");
    QTest::addColumn<QGeoMapData*>("gmd");
    QTest::addColumn<QGeoMapPixmapObject*>("obj");
    QTest::addColumn<QGeoCoordinate>("center");

    QMap<QString, QVariant> params;
    QGeoMappingManagerEngine *mgr = new WhiteTileEngine(params, this);

    QSize window = QSize(500, 500);
    QSize target = QSize(50, 50);
    QPixmap targetPixmap = indexedPixmap(target.width(), target.height());

    QGeoMapData *gmd;
    QGeoCoordinate center;
    QGeoCoordinate pixmap;
    QGeoMapPixmapObject *obj;

    center = QGeoCoordinate(-27.58, 153.10);
    makeFixtures(gmd, obj, center, center, 3.0, targetPixmap, mgr, window, target);
    QTest::newRow("Brisbane @z=3") << mgr << gmd << obj << center;

    center = QGeoCoordinate(0.0, 0.0);
    makeFixtures(gmd, obj, center, center, 1.0, targetPixmap, mgr, window, target);
    QTest::newRow("At 0,0") << mgr << gmd << obj << center;

    center = QGeoCoordinate(0.0, 0.0);
    makeFixtures(gmd, obj, center, center, 0.0, targetPixmap, mgr, window, target);
    QTest::newRow("Zoom level 0") << mgr << gmd << obj << center;

    center = QGeoCoordinate(0.0, 179.8);
    makeFixtures(gmd, obj, center, center, 2.0, targetPixmap, mgr, window, target);
    QTest::newRow("Positive dateline") << mgr << gmd << obj << center;

    center = QGeoCoordinate(0.0, -179.9);
    pixmap = QGeoCoordinate(3.0, 175.0);
    makeFixtures(gmd, obj, center, pixmap, 3.0, targetPixmap, mgr, window, target);
    QTest::newRow("Crossing dateline") << mgr << gmd << obj << center;
}

void tst_QGeoTiledMapData::panTest_data()
{
    QTest::addColumn<QGeoMappingManagerEngine*>("mgr");
    QTest::addColumn<QGeoMapData*>("gmd");
    QTest::addColumn<QGeoMapPixmapObject*>("obj");
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<QPoint>("pxCenter");
    QTest::addColumn<QPoint>("pan");
    QTest::addColumn<qreal>("dist");
    QTest::addColumn<QPoint>("pxFinal");

    QMap<QString, QVariant> params;
    QGeoMappingManagerEngine *mgr = new WhiteTileEngine(params, this);

    QSize window = QSize(500, 500);
    QSize target = QSize(50, 50);
    QPoint pxCenter = QPoint(250, 250);
    QPoint pxFinal = pxCenter;
    QPixmap targetPixmap = indexedPixmap(target.width(), target.height());

    QGeoMapData *gmd;
    QGeoCoordinate center;
    QPoint pan;
    qreal dist;
    QGeoMapPixmapObject *obj;

    center = QGeoCoordinate(-27.58, 153.10);
    pan = QPoint(30, 50);
    pxFinal = pxCenter - pan;
    dist = 1800e3;
    makeFixtures(gmd, obj, center, center, 3.0, targetPixmap, mgr, window, target);
    QTest::newRow("Brisbane, pan +ve") << mgr << gmd << obj <<
                                          center << pxCenter << pan << dist << pxFinal;

    center = QGeoCoordinate(0.0, 0.0);
    pan = QPoint(-30, 0);
    pxFinal = pxCenter - pan;
    dist = 4700e3;
    makeFixtures(gmd, obj, center, center, 1.0, targetPixmap, mgr, window, target);
    QTest::newRow("At 0,0, pan -ve x") << mgr << gmd << obj <<
                                          center << pxCenter << pan << dist << pxFinal;

    window = QSize(200, 200);
    pxCenter = QPoint(100, 100);
    center = QGeoCoordinate(0.0, 179.8);
    pan = QPoint(50, -10);
    pxFinal = pxCenter - pan;
    dist = 3900e3;
    makeFixtures(gmd, obj, center, center, 2.0, targetPixmap, mgr, window, target);
    QTest::newRow("Positive dateline, pan +ve") << mgr << gmd << obj << center
                                                << pxCenter << pan << dist << pxFinal;

    center = QGeoCoordinate(0.0, 10.0);
    pan = QPoint(0, -30);
    pxFinal = pxCenter;
    dist = 0;
    makeFixtures(gmd, obj, center, center, 2.0, targetPixmap, mgr, window, target);
    gmd->pan(0, -500);
    center = gmd->screenPositionToCoordinate(pxCenter);
    QTest::newRow("Clipping at north pole") << mgr << gmd << obj << center
                                            << pxCenter << pan << dist << pxFinal;
}

void tst_QGeoTiledMapData::panTest()
{
    QFETCH(QGeoMappingManagerEngine*, mgr);
    QFETCH(QGeoMapData*, gmd);
    QFETCH(QGeoMapPixmapObject*, obj);
    QFETCH(QGeoCoordinate, center);
    QFETCH(QPoint, pxCenter);
    QFETCH(QPoint, pxFinal);
    QFETCH(QPoint, pan);
    QFETCH(qreal, dist);

    QPointF c = gmd->coordinateToScreenPosition(center);
    QCOMPARE(int(c.x()+0.5), pxCenter.x());
    QCOMPARE(int(c.y()+0.5), pxCenter.y());

    gmd->pan(pan.x(), pan.y());

    QPointF c2 = gmd->coordinateToScreenPosition(center);
    QCOMPARE(int(c2.x()+0.5), pxFinal.x());
    QCOMPARE(int(c2.y()+0.5), pxFinal.y());

    if (dist > 1e-5) {
        QGeoCoordinate nc = gmd->screenPositionToCoordinate(pxCenter);
        qreal d = nc.distanceTo(center);
        QVERIFY(d > 0.9*dist && d < 1.1*dist);
    }
}

void tst_QGeoTiledMapData::objectsAtPoint()
{
    QFETCH(QGeoMappingManagerEngine*, mgr);
    QFETCH(QGeoMapData*, gmd);
    QFETCH(QGeoMapPixmapObject*, obj);
    QFETCH(QGeoCoordinate, center);

    QVERIFY(gmd->mapObjects().contains(obj));
    QVERIFY(gmd->mapObjectsInViewport().contains(obj));
    QPointF centerPt = gmd->coordinateToScreenPosition(center);
    QVERIFY(gmd->mapObjectsAtScreenPosition(centerPt).contains(obj));
}

void tst_QGeoTiledMapData::pixmapDraw()
{
    QFETCH(QGeoMappingManagerEngine*, mgr);
    QFETCH(QGeoMapData*, gmd);
    QFETCH(QGeoMapPixmapObject*, obj);
    QFETCH(QSize, windowSize);
    QFETCH(QSize, targetSize);
    QFETCH(QPoint, offset);

    QPixmap pm(windowSize);
    QPainter *painter = new QPainter(&pm);
    pm.fill(Qt::black);

    QApplication::processEvents();

    gmd->paint(painter, NULL);
    painter->end();

    QImage im = pm.toImage();

    uint px = 0;
    uint x = windowSize.width()/2 + offset.x();
    for (; px < targetSize.width(); px++, x++) {
        uint py = 0;
        uint y = windowSize.height()/2 + offset.y();
        for (; py < targetSize.height(); py++, y++) {
            TilePixelValue tpv(im.pixel(x, y));
            QCOMPARE(tpv.px(), px);
            QCOMPARE(tpv.py(), py);
            QCOMPARE(tpv.zoom(), 1u);
        }
    }
}

void tst_QGeoTiledMapData::pixmapAtDateline_data()
{
    QTest::addColumn<QGeoMappingManagerEngine*>("mgr");
    QTest::addColumn<QGeoMapData*>("gmd");
    QTest::addColumn<QGeoMapPixmapObject*>("obj");
    QTest::addColumn<QSize>("windowSize");
    QTest::addColumn<QSize>("targetSize");

    QMap<QString, QVariant> params;
    QGeoMappingManagerEngine *mgr = new WhiteTileEngine(params, this);

    QSize window = QSize(500, 500);
    QSize target = QSize(50, 50);
    QPixmap targetPixmap = indexedPixmap(target.width(), target.height());

    QGeoMapData *gmd;
    QGeoCoordinate center, pixmap;
    QGeoMapPixmapObject *obj;
    QPoint offset;

    center = QGeoCoordinate(0.0, -179.9);
    pixmap = QGeoCoordinate(3.0, 175.0);
    makeFixtures(gmd, obj, center, pixmap, 3.0, targetPixmap, mgr, window, target);
    QTest::newRow("east to west") << mgr << gmd << obj << window << target;

    center = QGeoCoordinate(0.0, 179.0);
    pixmap = QGeoCoordinate(0.0, -179.0);
    makeFixtures(gmd, obj, center, pixmap, 3.0, targetPixmap, mgr, window, target);
    offset = QPoint(-20, -5);
    obj->setOffset(offset);
    QTest::newRow("west to east") << mgr << gmd << obj << window << target;
}

void tst_QGeoTiledMapData::pixmapAtDateline()
{
    QFETCH(QGeoMappingManagerEngine*, mgr);
    QFETCH(QGeoMapData*, gmd);
    QFETCH(QGeoMapPixmapObject*, obj);
    QFETCH(QSize, windowSize);
    QFETCH(QSize, targetSize);

    QPixmap pm(windowSize);
    QPainter *painter = new QPainter(&pm);
    pm.fill(Qt::black);

    QApplication::processEvents();

    gmd->paint(painter, NULL);
    painter->end();

    QImage im = pm.toImage();
    TilePixelValue tpv(im.pixel(windowSize.width()/2, windowSize.height()/2));
    QVERIFY(tpv.px() < targetSize.width());
    QVERIFY(tpv.py() < targetSize.height());
    QVERIFY(tpv.px() > 1);
    QVERIFY(tpv.py() > 1);
    QCOMPARE(tpv.zoom(), 1u);
}

QTEST_MAIN(tst_QGeoTiledMapData)
#include "tst_qgeotiledmapdata.moc"
