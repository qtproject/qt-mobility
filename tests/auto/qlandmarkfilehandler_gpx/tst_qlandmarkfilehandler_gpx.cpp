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

#define private public
#include "../../../src/location/qlandmarkmanager.h"

#include "../../../src/location/qlandmarkfilehandler_gpx_p.h"

#include "qgeocoordinate.h"

#include <qtest.h>
#include <QMetaType>
#include <QFile>
#include <QBuffer>

#include <QDebug>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QList<QLandmark>);
Q_DECLARE_METATYPE(QList<QList<QLandmark> >);

class tst_QLandmarkGpxHandler : public QObject
{
    Q_OBJECT

private:
    QLandmarkManager *m_manager;
    QLandmarkFileHandlerGpx *m_handler;

private slots:

    void init() {
        QMap<QString, QString> map;
        map["filename"] = "test.db";
        m_manager = new QLandmarkManager("com.nokia.qt.landmarks.engines.sqlite", map);

        m_handler = new QLandmarkFileHandlerGpx(m_manager->engine());
    }

    void cleanup() {
        delete m_handler;
        delete m_manager;

        QFile file("test.db");
        file.remove();
    }

    void cleanupTestCase() {
        QFile file("test.db");
        file.remove();
    }

    void fileImport() {
        QFETCH(QString, fileIn);
        QFETCH(QString, fileOut);
        QFETCH(QString, exportPrefix);
        QFETCH(QList<QLandmark>, waypoints);
        QFETCH(QList<QList<QLandmark> >, tracks);
        QFETCH(QList<QList<QLandmark> >, routes);

        QFile file(fileIn);
        file.open(QIODevice::ReadOnly);

        QVERIFY(m_handler->importData(&file));

        file.close();

        QCOMPARE(m_handler->waypoints(), waypoints);
        QCOMPARE(m_handler->tracks(), tracks);
        QCOMPARE(m_handler->routes(), routes);
    }

    void fileImport_data() {
        commonData();
    }

    void dataExport() {
        QFETCH(QString, fileIn);
        QFETCH(QString, fileOut);
        QFETCH(QString, exportPrefix);
        QFETCH(QList<QLandmark>, waypoints);
        QFETCH(QList<QList<QLandmark> >, tracks);
        QFETCH(QList<QList<QLandmark> >, routes);

        m_handler->setWaypoints(waypoints);
        m_handler->setTracks(tracks);
        m_handler->setRoutes(routes);

        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QVERIFY(m_handler->exportData(&buffer, exportPrefix));
        QByteArray dataExported = buffer.buffer();
        buffer.close();

        QFile file(fileOut);
        file.open(QIODevice::ReadOnly);

        QByteArray testData = file.readAll();

        file.close();

        QCOMPARE(dataExported, testData);
    }

    void dataExport_data() {
        commonData();
    }

    void fileImportErrors() {
        QFETCH(QString, file);
        QFETCH(QString, error);

        QString filename = ":/data/errors/";
        filename += file;
        QFile fileIn(filename);
        fileIn.open(QIODevice::ReadOnly);

        bool result = m_handler->importData(&fileIn);

        QVERIFY(!result);
        QCOMPARE(m_handler->errorString(), error);
    }

    void fileImportErrors_data() {
        QTest::addColumn<QString>("file");
        QTest::addColumn<QString>("error");

        QTest::newRow("No root element")
        << "gpx/noroot.xml"
        << "Expected a root element named \"gpx\" (no root element found).";
        QTest::newRow("Wrong root element")
        << "gpx/wrongroot.xml"
        << "The root element is expected to have the name \"gpx\" (root element was named \"notgpx\")." ;
        QTest::newRow("Two root elements")
        << "gpx/tworoots.xml"
        << "A single root element named \"gpx\" was expected (second root element was named \"notgpx\")";
        QTest::newRow("No version attribute")
        << "gpx/noversion.xml"
        << "The element \"gpx\" did not have the required attribute \"version\".";
        QTest::newRow("Wrong version attribute")
        << "gpx/wrongversion.xml"
        << "Only version 1.1. of the GPX schema is supported (version found was \"0.1\").";
        QTest::newRow("No creator attribute")
        << "gpx/nocreator.xml"
        << "The element \"gpx\" did not have the required attribute \"creator\".";
        QTest::newRow("Wrong order of gpx elements")
        << "gpx/wrongorder.xml"
        << "The element \"gpx\" did not expect a child element named \"wpt\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first gpx element")
        << "gpx/invalidfirst.xml"
        << "The element \"gpx\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle gpx element")
        << "gpx/invalidmid.xml"
        << "The element \"gpx\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last gpx element")
        << "gpx/invalidlast.xml"
        << "The element \"gpx\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("No latitude attribute")
        << "wpt/nolat.xml"
        << "The element \"wpt\" did not have the required attribute \"lat\".";
        QTest::newRow("Non-double latitude attribute")
        << "wpt/nondoublelat.xml"
        << "The attribute \"lat\" expected a value convertable to type double (value was \"forty two\").";
        QTest::newRow("Latitude attribute out of range")
        << "wpt/outofrangelat.xml"
        << "The attribute \"lat\" fell outside of the bounds -90.0 <= lat <= 90.0 (value was \"242.062835\").";
        QTest::newRow("No longitude attribute")
        << "wpt/nolon.xml"
        << "The element \"wpt\" did not have the required attribute \"lon\".";
        QTest::newRow("Non-double longitude attribute")
        << "wpt/nondoublelon.xml"
        << "The attribute \"lon\" expected a value convertable to type double (value was \"seventy one\").";
        QTest::newRow("Longitude attribute out of range")
        << "wpt/outofrangelon.xml"
        << "The attribute \"lon\" fell outside of the bounds -180.0 <= lat < 180.0 (value was \"-271.279118\").";
        QTest::newRow("Non-double elevation element")
        << "wpt/nondoubleele.xml"
        << "The element \"ele\" expected a value convertable to type double (value was \"ninety seven\").";
        QTest::newRow("Wrong order of wpt elements")
        << "wpt/wrongorder.xml"
        << "The element \"wpt\" did not expect a child element named \"name\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first wpt element")
        << "wpt/invalidfirst.xml"
        << "The element \"wpt\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle wpt element")
        << "wpt/invalidmid.xml"
        <<  "The element \"wpt\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last wpt element")
        << "wpt/invalidlast.xml"
        <<  "The element \"wpt\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Wrong order of rte elements")
        << "rte/wrongorder.xml"
        << "The element \"rte\" did not expect a child element named \"name\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first rte element")
        << "rte/invalidfirst.xml"
        << "The element \"rte\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle rte element")
        << "rte/invalidmid.xml"
        << "The element \"rte\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last rte element")
        << "rte/invalidlast.xml"
        << "The element \"rte\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Wrong order of trk elements")
        << "trk/wrongorder.xml"
        << "The element \"trk\" did not expect a child element named \"name\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first trk element")
        << "trk/invalidfirst.xml"
        << "The element \"trk\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle trk element")
        << "trk/invalidmid.xml"
        << "The element \"trk\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last trk element")
        << "trk/invalidlast.xml"
        << "The element \"trk\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Wrong order of trkseg elements")
        << "trkseg/wrongorder.xml"
        << "The element \"trkseg\" did not expect a child element named \"trkpt\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first trkseg element")
        << "trkseg/invalidfirst.xml"
        << "The element \"trkseg\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle trkseg element")
        << "trkseg/invalidmid.xml"
        << "The element \"trkseg\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last trkseg element")
        << "trkseg/invalidlast.xml"
        << "The element \"trkseg\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
    }

private:
    void commonData() {
        QTest::addColumn<QString>("fileIn");
        QTest::addColumn<QString>("fileOut");
        QTest::addColumn<QString>("exportPrefix");
        QTest::addColumn<QList<QLandmark> >("waypoints");
        QTest::addColumn<QList<QList<QLandmark> > >("tracks");
        QTest::addColumn<QList<QList<QLandmark> > >("routes");

        QList<QLandmark> w;

        QLandmark w0;

        w0.setName("w0");
        w0.setDescription("Test data");
        w0.setCoordinate(QGeoCoordinate(1.0, 2.0, 3.0));

        w << w0;

        QLandmark w1;
        w1.setName("w1");
        w1.setCoordinate(QGeoCoordinate(4.0, 5.0));

        w << w1;

        QLandmark w2;
        w2.setDescription("Test data");
        w2.setCoordinate(QGeoCoordinate(6.0, 7.0));

        w << w2;

        QLandmark w3;
        w3.setCoordinate(QGeoCoordinate(8.0, 9.0, 10.0));

        w << w3;

        QLandmark w4;
        w4.setCoordinate(QGeoCoordinate(11.0, 12.0));

        w << w4;

        QList<QList<QLandmark> > t;

        QList<QLandmark> t0;

        QLandmark t00;
        t00.setCoordinate(QGeoCoordinate(13.0, 14.0));

        t0 << t00;

        QLandmark t01;
        t01.setCoordinate(QGeoCoordinate(15.0, 16.0));

        t0 << t01;

        QLandmark t02;
        t02.setCoordinate(QGeoCoordinate(17.0, 18.0));

        t0 << t02;

        QLandmark t03;
        t03.setCoordinate(QGeoCoordinate(19.0, 20.0));

        t0 << t03;

        t << t0;

        QList<QLandmark> t1;

        QLandmark t10;
        t10.setCoordinate(QGeoCoordinate(21.0, 22.0));

        t1 << t10;

        QLandmark t11;
        t11.setCoordinate(QGeoCoordinate(23.0, 24.0));

        t1 << t11;

        QLandmark t12;
        t12.setCoordinate(QGeoCoordinate(25.0, 26.0));

        t1 << t12;

        QLandmark t13;
        t13.setCoordinate(QGeoCoordinate(27.0, 28.0));

        t1 << t13;

        t << t1;

        QList<QList<QLandmark> > r;

        QList<QLandmark> r0;

        QLandmark r00;
        r00.setCoordinate(QGeoCoordinate(29.0, 30.0));

        r0 << r00;

        QLandmark r01;
        r01.setCoordinate(QGeoCoordinate(31.0, 32.0));

        r0 << r01;

        QLandmark r02;
        r02.setCoordinate(QGeoCoordinate(33.0, 34.0));

        r0 << r02;

        QLandmark r03;
        r03.setCoordinate(QGeoCoordinate(35.0, 36.0));

        r0 << r03;

        r << r0;

        QList<QLandmark> r1;

        QLandmark r10;
        r10.setCoordinate(QGeoCoordinate(37.0, 38.0));

        r1 << r10;

        QLandmark r11;
        r11.setCoordinate(QGeoCoordinate(39.0, 40.0));

        r1 << r11;

        QLandmark r12;
        r12.setCoordinate(QGeoCoordinate(41.0, 42.0));

        r1 << r12;

        QLandmark r13;
        r13.setCoordinate(QGeoCoordinate(43.0, 44.0));

        r1 << r13;

        r << r1;

        QTest::newRow("convert-empty")
        << ":/data/convert-empty.xml"
        << ":/data/convert-empty.xml"
        << ""
        << QList<QLandmark>()
        << QList<QList<QLandmark> >()
        << QList<QList<QLandmark> >();
        QTest::newRow("convert-waypoints")
        << ":/data/convert-waypoints.xml"
        << ":/data/convert-waypoints.xml"
        << ""
        << w
        << QList<QList<QLandmark> >()
        << QList<QList<QLandmark> >();

        QTest::newRow("convert-tracks")
        << ":/data/convert-tracks-in.xml"
        << ":/data/convert-tracks-out.xml"
        << ""
        << QList<QLandmark>()
        << t
        << QList<QList<QLandmark> >();

        QTest::newRow("convert-routes")
        << ":/data/convert-routes.xml"
        << ":/data/convert-routes.xml"
        << ""
        << QList<QLandmark>()
        << QList<QList<QLandmark> >()
        << r;

        QTest::newRow("convert-all")
        << ":/data/convert-all-in.xml"
        << ":/data/convert-all-out.xml"
        << ""
        << w
        << t
        << r;

        QTest::newRow("convert-all-in-prefixed")
        << ":/data/convert-all-in-prefixed.xml"
        << ":/data/convert-all-out.xml"
        << ""
        << w
        << t
        << r;

        QTest::newRow("convert-all-out-prefixed")
        << ":/data/convert-all-in.xml"
        << ":/data/convert-all-out-prefixed.xml"
        << "gpx"
        << w
        << t
        << r;
    }
};

QTEST_MAIN(tst_QLandmarkGpxHandler)
#include "tst_qlandmarkfilehandler_gpx.moc"
