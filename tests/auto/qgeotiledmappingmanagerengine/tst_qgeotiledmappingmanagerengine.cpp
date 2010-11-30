#include "../qlocationtestutils_p.h"

#include <qgeocoordinate.h>
#include <qgraphicsgeomap.h>
#include <qgeomapdata.h>
#include <qgeomappingmanager.h>
#include <qtest.h>

#include <QtGui/QApplication>
#include <QDebug>
#include <QtGui>

#include "pixelindexengine.h"

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QGeoCoordinate::CoordinateFormat)
Q_DECLARE_METATYPE(QGeoCoordinate::CoordinateType)

class tst_QGeoTiledMappingManagerEngine : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void zoomLevels_data();
    void zoomLevels();
    void centering_data();
    void centering();
    void stitching();
    void sizes_data();
    void sizes();
    void paintLimits();

private:
    QGeoMapData *gmd;

};

void tst_QGeoTiledMappingManagerEngine::initTestCase()
{
    QMap<QString, QVariant> params;
    QGeoMappingManagerEngine *mgr = new PixelIndexEngine(params);
    gmd = mgr->createMapData();
}

void tst_QGeoTiledMappingManagerEngine::cleanupTestCase()
{
    delete gmd;
}

void tst_QGeoTiledMappingManagerEngine::centering_data()
{
    QTest::addColumn<qreal>("zoom");
    QTest::addColumn<qreal>("lat");
    QTest::addColumn<qreal>("lon");
    QTest::addColumn<QPoint>("centerPixel");

    QTest::newRow("Brisbane @z=3") << 3.0 << -27.58 << 153.10;
    QTest::newRow("Somewhere random") << 3.0 << 50.0 << -120.0;
    QTest::newRow("Just before the date line") << 3.0 << -85.0 << 179.99;
}

void tst_QGeoTiledMappingManagerEngine::centering()
{
    QFETCH(qreal, zoom);
    QFETCH(qreal, lat);
    QFETCH(qreal, lon);

    QGeoCoordinate center(lat, lon);
    gmd->setWindowSize(QSizeF(500,500));
    gmd->setZoomLevel(zoom);
    gmd->setCenter(center);
    QGeoTiledMapData *gtmd = dynamic_cast<QGeoTiledMapData*>(gmd);
    QPoint centerPixel = gtmd->coordinateToWorldReferencePosition(center);

    QPixmap pm(500,500);
    QPainter *painter = new QPainter(&pm);
    pm.fill(Qt::black);

    QApplication::processEvents();

    gmd->paint(painter, NULL);
    painter->end();

    QImage im = pm.toImage();
    TilePixelValue tpv;
    tpv.rgb = im.pixel(250,250);

    QCOMPARE(tpv.zoom, static_cast<uint>(zoom));
    QCOMPARE(tpv.px, static_cast<uint>(centerPixel.x()));
    QCOMPARE(tpv.py, static_cast<uint>(centerPixel.y()));
}

void tst_QGeoTiledMappingManagerEngine::stitching()
{
    QGeoCoordinate center(0.0, -135.0);
    gmd->setZoomLevel(3.0);
    gmd->setCenter(center);

    QPixmap pm(500,500);
    gmd->setWindowSize(QSizeF(500,500));
    QPainter *painter = new QPainter(&pm);
    pm.fill(Qt::black);

    QApplication::processEvents();

    gmd->paint(painter, NULL);
    painter->end();

    QImage im = pm.toImage();
    TilePixelValue tpv;

    uint x = 120;
    int px = 242;
    for (; x < 130; x++, px++) {
        tpv.rgb = im.pixel(px, 255);
        QCOMPARE(tpv.px, x);
    }

    uint y = 512;
    int py = 250;
    for (; y < 512+150; y++, py++) {
        tpv.rgb = im.pixel(312, py);
        QCOMPARE(tpv.py, y);
    }

}

void tst_QGeoTiledMappingManagerEngine::zoomLevels_data()
{
    QTest::addColumn<qreal>("zoom");
    QTest::addColumn<uint>("midx");
    QTest::addColumn<uint>("midy");

    QTest::newRow("zoom level 1") << 1.0 << 128u << 128u;
    QTest::newRow("zoom level 2") << 2.0 << 256u << 256u;
    QTest::newRow("zoom level 3") << 3.0 << 512u << 512u;
}

void tst_QGeoTiledMappingManagerEngine::zoomLevels()
{
    QFETCH(qreal, zoom);
    QFETCH(uint, midx);
    QFETCH(uint, midy);

    gmd->setWindowSize(QSizeF(500,500));
    gmd->setCenter(QGeoCoordinate(0, 0));
    gmd->setZoomLevel(zoom);

    QPixmap pm(500,500);
    QPainter *painter = new QPainter(&pm);
    pm.fill(Qt::black);

    QApplication::processEvents();

    gmd->paint(painter, NULL);
    painter->end();

    QImage im = pm.toImage();
    TilePixelValue tpv;
    tpv.rgb = im.pixel(250,250);

    QCOMPARE(tpv.zoom, static_cast<uint>(zoom));
    QCOMPARE(tpv.px, midx);
    QCOMPARE(tpv.py, midy);
}

void tst_QGeoTiledMappingManagerEngine::sizes_data()
{
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<QPoint>("center");
    QTest::addColumn<QPoint>("topLeft");
    QTest::addColumn<QPoint>("topRight");

    QTest::newRow("Even dimensions, square") << 300 << 300
                                             << QPoint(512, 512)
                                             << QPoint(512-150, 512-150)
                                             << QPoint(512+150-1, 512-150);

    QTest::newRow("Even dimensions, rectangular") << 800 << 100
                                                  << QPoint(512, 512)
                                                  << QPoint(512-400, 512-50)
                                                  << QPoint(512+400-1, 512-50);

    QTest::newRow("Odd dimensions, square") << 105 << 105
                                            << QPoint(512, 512)
                                            << QPoint(512-52, 512-52)
                                            << QPoint(512+52, 512-52);

    QTest::newRow("Odd dimensions, rectangular") << 815 << 105
                                                 << QPoint(512, 512)
                                                 << QPoint(512-407, 512-52)
                                                 << QPoint(512+407, 512-52);
}

void tst_QGeoTiledMappingManagerEngine::sizes()
{
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(QPoint, center);
    QFETCH(QPoint, topLeft);
    QFETCH(QPoint, topRight);

    gmd->setWindowSize(QSizeF(width, height));
    gmd->setCenter(QGeoCoordinate(0, 0));
    gmd->setZoomLevel(3.0);

    QPixmap pm(width, height);
    QPainter *painter = new QPainter(&pm);
    pm.fill(Qt::black);
    QApplication::processEvents();

    gmd->paint(painter, NULL);
    painter->end();

    QImage im = pm.toImage();
    TilePixelValue tpv;
    tpv.rgb = im.pixel(width/2, height/2);

    QCOMPARE(tpv.zoom, 3u);
    QCOMPARE(tpv.px, static_cast<uint>(center.x()));
    QCOMPARE(tpv.py, static_cast<uint>(center.y()));

    tpv.rgb = im.pixel(0, 0);
    QCOMPARE(tpv.zoom, 3u);
    QCOMPARE(tpv.px, static_cast<uint>(topLeft.x()));
    QCOMPARE(tpv.py, static_cast<uint>(topLeft.y()));

    tpv.rgb = im.pixel(width-1, 0);
    QCOMPARE(tpv.zoom, 3u);
    QCOMPARE(tpv.px, static_cast<uint>(topRight.x()));
    QCOMPARE(tpv.py, static_cast<uint>(topRight.y()));
}

void tst_QGeoTiledMappingManagerEngine::paintLimits()
{
    gmd->setWindowSize(QSizeF(200, 200));
    gmd->setCenter(QGeoCoordinate(0, 0));
    gmd->setZoomLevel(1.0);

    QPixmap pm(300, 300);
    QPainter *painter = new QPainter(&pm);
    pm.fill(Qt::black);
    QApplication::processEvents();

    gmd->paint(painter, NULL);
    painter->end();

    QImage im = pm.toImage();

    QCOMPARE(QColor::fromRgb(im.pixel(200, 5)), QColor(Qt::black));
    QCOMPARE(QColor::fromRgb(im.pixel(80, 200)), QColor(Qt::black));
    QVERIFY(QColor::fromRgb(im.pixel(15, 199)) != QColor(Qt::black));
    QVERIFY(QColor::fromRgb(im.pixel(199, 20)) != QColor(Qt::black));
}

QTEST_MAIN(tst_QGeoTiledMappingManagerEngine)
#include "tst_qgeotiledmappingmanagerengine.moc"
