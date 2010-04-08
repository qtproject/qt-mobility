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

#include "../../../src/location/qlandmarklmxhandler_p.h"
#include "../qlandmarkcategory/mocklandmarkmanager.h"

#include <qtest.h>
#include <QMetaType>
#include <QFile>

#include <QDebug>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QList<QLandmark>);

class tst_QLandmarkLmxHandler : public QObject
{
    Q_OBJECT

private:
    QLandmarkManager *m_manager;

private slots:

    void initTestCase() {
        m_manager = new MockLandmarkManager("test", 0);
    }

    void cleanupTestCase() {
        delete m_manager;
    }

    /*
        // add errors in later?
        void validation() {
            QFETCH(QString, filename);
            QFETCH(bool, valid);

            QLandmarkLmxHandler handler(m_manager);

            QFile file(filename);
            file.open(QIODevice::ReadOnly);
            QByteArray dataIn = file.readAll();

            QCOMPARE(handler.importData(dataIn), valid);
        }

        void validation_data() {
            QTest::addColumn<QString>("filename");
            QTest::addColumn<bool>("valid");

            QTest::newRow("valid-single")
                    << ":/data/convert-single-in.xml"
                    << true;
            QTest::newRow("valid-collection")
                    << ":/data/convert-collection-in.xml"
                    << true;
        }

        void fileImport() {
            QFETCH(QString, fileIn);
            QFETCH(QString, fileOut);
            QFETCH(QString, exportPrefix);
            QFETCH(QList<QLandmark>, landmarks);

            QLandmarkLmxHandler handler(m_manager);

            QFile file(fileIn);
            file.open(QIODevice::ReadOnly);

            QByteArray data = file.readAll();

            QVERIFY(handler.importData(data));
            QCOMPARE(handler.landmarks(), landmarks);
        }

        void fileImport_data() {
            commonData();
        }

        void dataExport() {
            QFETCH(QString, fileIn);
            QFETCH(QString, fileOut);
            QFETCH(QString, exportPrefix);
            QFETCH(QList<QLandmark>, landmarks);

            QLandmarkLmxHandler handler(m_manager);

            handler.setLandmarks(landmarks);

            QByteArray dataExported;
            QVERIFY(handler.exportData(dataExported, exportPrefix));

            QFile file(fileOut);
            file.open(QIODevice::ReadOnly);

            QByteArray testData = file.readAll();

            //qWarning() << dataExported << testData;

            QCOMPARE(dataExported, testData);
        }

        void dataExport_data() {
            commonData();
        }
    */
    // importThenExport?
    // exportThenImport?

    void fileImportErrors() {
        QFETCH(QString, file);
        QFETCH(QString, error);

        QLandmarkLmxHandler handler(m_manager);

        QString filename = ":/data/errors/";
        filename += file;
        QFile fileIn(filename);
        fileIn.open(QIODevice::ReadOnly);

        QByteArray data = fileIn.readAll();
        bool result = handler.importData(data);
        QVERIFY(!result);
        QCOMPARE(handler.errorString(), error);
    }

    void fileImportErrors_data() {
        QTest::addColumn<QString>("file");
        QTest::addColumn<QString>("error");

        QTest::newRow("No root element")
        << "lmx/noroot.xml"
        << "Expected a root element named \"lmx\" (no root element found).";
        QTest::newRow("Wrong root element")
        << "lmx/wrongroot.xml"
        << "The root element is expected to have the name \"lmx\" (root element was named \"notlmx\").";
        QTest::newRow("Two root elements")
        << "lmx/tworoots.xml"
        << "A single root element named \"lmx\" was expected (second root element was named \"otherlmx\").";
        QTest::newRow("Wrong options to lmx choice")
        << "lmx/wrongchoice.xml"
        << "The element \"lmx\" expected a single child element named either \"landmark\" or \"landmarkCollection\" (child element was named \"notlandmark\").";
        QTest::newRow("Two children to lmx")
        << "lmx/twochildren.xml"
        <<  "The element \"lmx\" expected a single child element (second child element was named \"landmark\").";
        QTest::newRow("No landmarks in collection")
        << "collection/nolandmarks.xml"
        << "The element \"landmarkCollection\" expected 1 or more child elements named \"landmark\" (0 found).";
        QTest::newRow("Wrong order of collection elements")
        << "collection/wrongorder.xml"
        << "The element \"landmarkCollection\" did not expect a child element named \"description\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first collection element")
        << "collection/invalidfirst.xml"
        << "The element \"landmarkCollection\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle collection element")
        << "collection/invalidmid.xml"
        << "The element \"landmarkCollection\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last collection element")
        << "collection/invalidlast.xml"
        << "The element \"landmarkCollection\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Non-double radius element")
        << "landmark/nondoubleradius.xml"
        << "The element \"coverageRadius\" expected a value convertable to type double (value was \"twenty three\").";
        QTest::newRow("Negative radius element")
        << "landmark/negativeradius.xml"
        << "The element \"coverageRadius\" is expected to have a non-negative value (value was \"-23.0\").";
        QTest::newRow("Wrong order of landmark elements")
        << "landmark/wrongorder.xml"
        << "The element \"landmark\" did not expect a child element named \"name\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first landmark element")
        << "landmark/invalidfirst.xml"
        << "The element \"landmark\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle landmark element")
        << "landmark/invalidmid.xml"
        << "The element \"landmark\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last landmark element")
        << "landmark/invalidlast.xml"
        << "The element \"landmark\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("No latitude element")
        << "coord/nolat.xml"
        << "The element \"coordinates\" did not have the required child element \"latitude\".";
        QTest::newRow("No longitude element")
        << "coord/nolon.xml"
        << "The element \"coordinates\" did not have the required child element \"longitude\".";
        QTest::newRow("Non-double latitude element")
        << "coord/nondoublelat.xml"
        << "The element \"latitude\" expected a value convertable to type double (value was \"one\").";
        QTest::newRow("Non-double longitude element")
        << "coord/nondoublelon.xml"
        << "The element \"longitude\" expected a value convertable to type double (value was \"two\").";
        QTest::newRow("Latitude element out of range")
        << "coord/outofrangelat.xml"
        << "The element \"latitude\" fell outside of the bounds -90.0 <= latitude <= 90.0 (value was \"1000.0\").";
        QTest::newRow("Longitude element out of range")
        << "coord/outofrangelon.xml"
        << "The element \"longitude\" fell outside of the bounds -180.0 <= longitude < 180.0 (value was \"2000.0\").";
        QTest::newRow("Non-double altitude element")
        << "coord/nondoublealt.xml"
        << "The element \"altitude\" expected a value convertable to type float (value was \"three\").";
        QTest::newRow("Wrong order of coordinate elements")
        << "coord/wrongorder.xml"
        << "The element \"coordinate\" did not expect a child element named \"altitude\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first coordinate element")
        << "coord/invalidfirst.xml"
        << "The element \"coordinate\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle coordinate element")
        << "coord/invalidmid.xml"
        << "The element \"coordinate\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last coordinate element")
        << "coord/invalidlast.xml"
        << "The element \"coordinate\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Duplicate address elements")
        << "address/duplicate.xml"
        << "The element \"addressInfo\" did not expect more then one occurrence of the child element named \"state\".";
        QTest::newRow("Invalid first address element")
        << "address/invalidfirst.xml"
        << "The element \"addressInfo\" did not expect a child element named \"invalid\".";
        QTest::newRow("Invalid middle address element")
        << "address/invalidmid.xml"
        << "The element \"addressInfo\" did not expect a child element named \"invalid\".";
        QTest::newRow("Invalid last address element")
        << "address/invalidlast.xml"
        << "The element \"addressInfo\" did not expect a child element named \"invalid\".";
        QTest::newRow("No URL in link element")
        << "link/nourl.xml"
        << "The element \"mediaLink\" did not have the required child element \"url\".";
        QTest::newRow("Wrong order of link elements")
        << "link/wrongorder.xml"
        << "The element \"url\" did not expect a child element named \"name\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first link element")
        << "link/invalidfirst.xml"
        << "The element \"url\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle link element")
        << "link/invalidmid.xml"
        << "The element \"url\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last link element")
        << "link/invalidlast.xml"
        << "The element \"url\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("No name in category element")
        << "category/noname.xml"
        << "The element \"category\" did not have the required child element \"name\".";
        QTest::newRow("Non-unsigned int id element")
        << "category/nonintid.xml"
        << "The element \"id\" expected a value convertable to type unsigned short (value was \"two\").";
        QTest::newRow("Wrong order of category elements")
        << "category/wrongorder.xml"
        << "The element \"category\" did not expect a child element named \"id\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid first category element")
        << "category/invalidfirst.xml"
        << "The element \"category\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid middle category element")
        << "category/invalidmid.xml"
        << "The element \"category\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
        QTest::newRow("Invalid last category element")
        << "category/invalidlast.xml"
        << "The element \"category\" did not expect a child element named \"invalid\" at this point (unknown child element or child element out of order).";
    }

private:
    void commonData() {
        QTest::addColumn<QString>("fileIn");
        QTest::addColumn<QString>("fileOut");
        QTest::addColumn<QString>("exportPrefix");
        QTest::addColumn<QList<QLandmark> >("landmarks");

        QList<QLandmarkCategoryId> categories;

        QLandmarkCategory cat0;
        cat0.setName("cat0");

        m_manager->saveCategory(&cat0);

        QLandmarkCategory cat1;
        cat1.setName("cat1");

        m_manager->saveCategory(&cat1);

        QLandmarkCategory cat2;
        cat2.setName("cat2");

        m_manager->saveCategory(&cat2);

        categories
        << cat0.categoryId()
        << cat1.categoryId()
        << cat2.categoryId();

        QList<QLandmark> w;

        QLandmark w0;

        w0.setName("w0");
        w0.setDescription("Test data");
        w0.setCoordinate(QGeoCoordinate(1.0, 2.0, 3.0));
        w0.setRadius(4.0);
        w0.setStreet("1 Main St");
        w0.setLocality("Brisbane");
        w0.setRegion("Queensland");
        w0.setCountry("Australia");
        w0.setPostcode("4000");
        w0.setPhone("123456789");
        w0.setUrl("http://example.com/testUrl");
        w0.setCategories(categories);
        w << w0;

        QTest::newRow("convert-single")
        << ":/data/convert-single-in.xml"
        << ":/data/convert-single-out.xml"
        << ""
        << w;

        QTest::newRow("convert-single-prefixed")
        << ":/data/convert-single-prefixed-in.xml"
        << ":/data/convert-single-prefixed-out.xml"
        << "lm"
        << w;

        QLandmark w1;
        w << w1;

        QLandmark w2;
        w2.setName("w0");
        w << w2;

        QLandmark w3;
        w3.setDescription("Test data");
        w << w3;

        QLandmark w4;
        w4.setCoordinate(QGeoCoordinate(1.0, 2.0));
        w << w4;

        QLandmark w5;
        w5.setCoordinate(QGeoCoordinate(1.0, 2.0, 3.0));
        w << w5;

        QLandmark w6;
        w6.setRadius(4.0);
        w << w6;

        QLandmark w7;
        w7.setStreet("1 Main St");
        w << w7;

        QLandmark w8;
        w8.setLocality("Brisbane");
        w << w8;

        QLandmark w9;
        w9.setRegion("Queensland");
        w << w9;

        QLandmark w10;
        w10.setCountry("Australia");
        w << w10;

        QLandmark w11;
        w11.setPostcode("4000");
        w << w11;

        QLandmark w12;
        w12.setPhone("123456789");
        w << w12;

        QLandmark w13;
        w13.setStreet("1 Main St");
        w13.setLocality("Brisbane");
        w13.setRegion("Queensland");
        w13.setCountry("Australia");
        w13.setPostcode("4000");
        w13.setPhone("123456789");
        w << w13;

        QLandmark w14;
        w14.setUrl("http://example.com/testUrl");
        w << w14;

        QLandmark w15;
        w15.setCategories(categories);
        w << w15;

        QTest::newRow("convert-collection")
        << ":/data/convert-collection-in.xml"
        << ":/data/convert-collection-out.xml"
        << ""
        << w;
    }
};

QTEST_MAIN(tst_QLandmarkLmxHandler)
#include "tst_qlandmarklmxhandler.moc"
