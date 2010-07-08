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

#include <qlandmark.h>

#include <qtest.h>
#include <QMetaType>
#include <QHash>
#include <QVariant>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QLandmark);
Q_DECLARE_METATYPE(QGeoCoordinate);
Q_DECLARE_METATYPE(QList<QLandmarkCategory>);

class tst_QLandmark : public QObject
{
    Q_OBJECT

public:
    typedef QHash<QString, QVariant> variantMap;

private slots:
    void settersAndGetters() {
        QFETCH(QString, name);
        QFETCH(QString, icon);
        QFETCH(QString, description);
        QFETCH(QString, street);
        QFETCH(QString, locality);
        QFETCH(QString, region);
        QFETCH(QString, country);
        QFETCH(QString, postcode);
        QFETCH(QString, phone);
        QFETCH(QString, url);
        QFETCH(double, radius);
        QFETCH(QGeoCoordinate, coordinate);
        QFETCH(QList<QLandmarkCategory>, categories);
        QFETCH(variantMap, attributes);

        QLandmark lm;

        lm.setName(name);
        lm.setIcon(icon);
        lm.setDescription(description);
        lm.setStreet(street);
        lm.setLocality(locality);
        lm.setRegion(region);
        lm.setCountry(country);
        lm.setPostcode(postcode);
        lm.setPhone(phone);
        lm.setUrl(url);
        lm.setRadius(radius);
        lm.setCoordinate(coordinate);
        lm.setCategories(categories);

        QList<QString> keys = attributes.keys();
        for (int i = 0; i < keys.size(); ++i) {
            lm.setAttribute(keys.at(i), attributes.value(keys.at(i)));
        }

        QCOMPARE(lm.name(), name);
        QCOMPARE(lm.icon(), icon);
        QCOMPARE(lm.description(), description);
        QCOMPARE(lm.street(), street);
        QCOMPARE(lm.locality(), locality);
        QCOMPARE(lm.region(), region);
        QCOMPARE(lm.country(), country);
        QCOMPARE(lm.postcode(), postcode);
        QCOMPARE(lm.phone(), phone);
        QCOMPARE(lm.url(), url);
        QCOMPARE(lm.radius(), radius);
        QCOMPARE(lm.coordinate(), coordinate);
        QCOMPARE(lm.categories(), categories);
        QCOMPARE(lm.attributeList(), QStringList(keys));

        for (int i = 0; i < keys.size(); ++i) {
            QCOMPARE(lm.attribute(keys.at(i)), attributes.value(keys.at(i)));
        }
    }

    void settersAndGetters_data() {
        genericData();
    }

    void copyContructor() {
        QFETCH(QString, name);
        QFETCH(QString, icon);
        QFETCH(QString, description);
        QFETCH(QString, street);
        QFETCH(QString, locality);
        QFETCH(QString, region);
        QFETCH(QString, country);
        QFETCH(QString, postcode);
        QFETCH(QString, phone);
        QFETCH(QString, url);
        QFETCH(double, radius);
        QFETCH(QGeoCoordinate, coordinate);
        QFETCH(QList<QLandmarkCategory>, categories);
        QFETCH(variantMap, attributes);

        QLandmark lm1;

        lm1.setName(name);
        lm1.setIcon(icon);
        lm1.setDescription(description);
        lm1.setStreet(street);
        lm1.setLocality(locality);
        lm1.setRegion(region);
        lm1.setCountry(country);
        lm1.setPostcode(postcode);
        lm1.setPhone(phone);
        lm1.setUrl(url);
        lm1.setRadius(radius);
        lm1.setCoordinate(coordinate);
        lm1.setCategories(categories);

        QList<QString> keys = attributes.keys();
        for (int i = 0; i < keys.size(); ++i) {
            lm1.setAttribute(keys.at(i), attributes.value(keys.at(i)));
        }

        QLandmark lm2(lm1);

        QCOMPARE(lm2.name(), name);
        QCOMPARE(lm2.icon(), icon);
        QCOMPARE(lm2.description(), description);
        QCOMPARE(lm2.street(), street);
        QCOMPARE(lm2.locality(), locality);
        QCOMPARE(lm2.region(), region);
        QCOMPARE(lm2.country(), country);
        QCOMPARE(lm2.postcode(), postcode);
        QCOMPARE(lm2.phone(), phone);
        QCOMPARE(lm2.url(), url);
        QCOMPARE(lm2.radius(), radius);
        QCOMPARE(lm2.coordinate(), coordinate);
        QCOMPARE(lm2.categories(), categories);
        QCOMPARE(lm2.attributeList(), QStringList(keys));

        for (int i = 0; i < keys.size(); ++i) {
            QCOMPARE(lm2.attribute(keys.at(i)), attributes.value(keys.at(i)));
        }
    }

    void copyContructor_data() {
        genericData();
    }

    void assignment() {
        QFETCH(QString, name);
        QFETCH(QString, icon);
        QFETCH(QString, description);
        QFETCH(QString, street);
        QFETCH(QString, locality);
        QFETCH(QString, region);
        QFETCH(QString, country);
        QFETCH(QString, postcode);
        QFETCH(QString, phone);
        QFETCH(QString, url);
        QFETCH(double, radius);
        QFETCH(QGeoCoordinate, coordinate);
        QFETCH(QList<QLandmarkCategory>, categories);
        QFETCH(variantMap, attributes);

        QLandmark lm1;
        QLandmark lm2;

        lm2.setName("test");

        lm1.setName(name);
        lm1.setIcon(icon);
        lm1.setDescription(description);
        lm1.setStreet(street);
        lm1.setLocality(locality);
        lm1.setRegion(region);
        lm1.setCountry(country);
        lm1.setPostcode(postcode);
        lm1.setPhone(phone);
        lm1.setUrl(url);
        lm1.setRadius(radius);
        lm1.setCoordinate(coordinate);
        lm1.setCategories(categories);

        QList<QString> keys = attributes.keys();
        for (int i = 0; i < keys.size(); ++i) {
            lm1.setAttribute(keys.at(i), attributes.value(keys.at(i)));
        }

        lm2 = lm1;

        QCOMPARE(lm2.name(), name);
        QCOMPARE(lm2.icon(), icon);
        QCOMPARE(lm2.description(), description);
        QCOMPARE(lm2.street(), street);
        QCOMPARE(lm2.locality(), locality);
        QCOMPARE(lm2.region(), region);
        QCOMPARE(lm2.country(), country);
        QCOMPARE(lm2.postcode(), postcode);
        QCOMPARE(lm2.phone(), phone);
        QCOMPARE(lm2.url(), url);
        QCOMPARE(lm2.radius(), radius);
        QCOMPARE(lm2.coordinate(), coordinate);
        QCOMPARE(lm2.categories(), categories);
        QCOMPARE(lm2.attributeList(), QStringList(keys));

        for (int i = 0; i < keys.size(); ++i) {
            QCOMPARE(lm2.attribute(keys.at(i)), attributes.value(keys.at(i)));
        }
    }

    void assignment_data() {
        genericData();
    }

    void equality() {
        QFETCH(QLandmark, lop);
        QFETCH(QLandmark, rop);
        QFETCH(bool, isEqual);

        QCOMPARE((lop == rop), isEqual);
    }

    void equality_data() {
        QTest::addColumn<QLandmark>("lop");
        QTest::addColumn<QLandmark>("rop");
        QTest::addColumn<bool>("isEqual");

        QLandmarkCategory cat0;
        cat0.setName("cat0Name");
        cat0.setIcon("cat0Icon");
        cat0.setDescription("cat0Description");

        QLandmarkCategory cat1;
        cat1.setName("cat1Name");
        cat1.setIcon("cat1Icon");
        cat1.setDescription("cat1Description");

        QLandmarkCategory cat2;
        cat2.setName("cat2Name");
        cat2.setIcon("cat2Icon");
        cat2.setDescription("cat2Description");

        QList<QLandmarkCategory> equalCategories;
        equalCategories << cat0 << cat1;

        QList<QLandmarkCategory> unequalCategoriesBySize;
        unequalCategoriesBySize << cat0 << cat1 << cat2;

        QList<QLandmarkCategory> unequalCategoriesByValue;
        unequalCategoriesBySize << cat0 << cat2;

        QHash<QString, QVariant> equalAttributes;
        equalAttributes["one"] = QVariant(1);
        equalAttributes["two"] = QVariant(2);

        QHash<QString, QVariant> unequalAttributesBySize;
        unequalAttributesBySize["one"] = QVariant(1);
        unequalAttributesBySize["two"] = QVariant(2);
        unequalAttributesBySize["three"] = QVariant(3);

        QHash<QString, QVariant> unequalAttributesByKey;
        unequalAttributesByKey["one"] = QVariant(1);
        unequalAttributesByKey["three"] = QVariant(2);

        QHash<QString, QVariant> unequalAttributesByValue;
        unequalAttributesByValue["one"] = QVariant(1);
        unequalAttributesByValue["two"] = QVariant(3);

        QLandmark lm0;
        lm0.setName("equalName");
        lm0.setIcon("equalIcon");
        lm0.setDescription("equalDescription");
        lm0.setStreet("equalStreet");
        lm0.setLocality("equalLocality");
        lm0.setRegion("equalRegion");
        lm0.setCountry("equalCountry");
        lm0.setPostcode("equalPostcode");
        lm0.setPhone("equalPhone");
        lm0.setUrl("equalUrl");
        lm0.setRadius(1.0);
        lm0.setCoordinate(QGeoCoordinate(1.0, 1.0, 1.0));
        lm0.setCategories(equalCategories);
        for (int i = 0; i < equalAttributes.size(); ++i) {
            QString key = equalAttributes.keys().at(i);
            lm0.setAttribute(key, equalAttributes.value(key));
        }

        QLandmark lm1(lm0);
        lm1.setName("unequalName");
        lm1.setIcon("unequalIcon");
        lm1.setDescription("unequalDescription");
        lm1.setStreet("unequalStreet");
        lm1.setLocality("unequalLocality");
        lm1.setRegion("unequalRegion");
        lm1.setCountry("unequalCountry");
        lm1.setPostcode("unequalPostcode");
        lm1.setPhone("unequalPhone");
        lm1.setUrl("unequalUrl");
        lm1.setRadius(0.0);
        lm1.setCoordinate(QGeoCoordinate(0.0, 0.0, 0.0));
        lm1.setCategories(unequalCategoriesBySize);
        for (int i = 0; i < unequalAttributesBySize.size(); ++i) {
            QString key = unequalAttributesBySize.keys().at(i);
            lm1.setAttribute(key, unequalAttributesBySize.value(key));
        }

        QTest::newRow("allUnequal") << lm0 << lm1 << false;

        QLandmark lm2(lm0);
        lm2.setName("unequalName");
        QTest::newRow("nameUnequal") << lm0 << lm2 << false;

        QLandmark lm3(lm0);
        lm3.setIcon("unequalIcon");
        QTest::newRow("iconUnequal") << lm0 << lm3 << false;

        QLandmark lm4(lm0);
        lm4.setDescription("unequalDescription");
        QTest::newRow("descriptionUnequal") << lm0 << lm4 << false;

        QLandmark lm5(lm0);
        lm5.setStreet("unequalStreet");
        QTest::newRow("streetUnequal") << lm0 << lm5 << false;

        QLandmark lm6(lm0);
        lm6.setLocality("unequalLocality");
        QTest::newRow("localityUnequal") << lm0 << lm6 << false;

        QLandmark lm7(lm0);
        lm7.setRegion("unequalRegion");
        QTest::newRow("regionUnequal") << lm0 << lm7 << false;

        QLandmark lm8(lm0);
        lm8.setCountry("unequalCountry");
        QTest::newRow("countryUnequal") << lm0 << lm8 << false;

        QLandmark lm9(lm0);
        lm9.setPostcode("unequalPostcode");
        QTest::newRow("postcodeUnequal") << lm0 << lm9 << false;

        QLandmark lm10(lm0);
        lm10.setPhone("unequalPhone");
        QTest::newRow("phoneUnequal") << lm0 << lm10 << false;

        QLandmark lm11(lm0);
        lm11.setUrl("unequalUrl");
        QTest::newRow("urlUnequal") << lm0 << lm11 << false;

        QLandmark lm12(lm0);
        lm12.setRadius(0.0);
        QTest::newRow("radiusUnequal") << lm0 << lm12 << false;

        QLandmark lm13(lm0);
        lm13.setCoordinate(QGeoCoordinate(0.0, 0.0, 0.0));
        QTest::newRow("coordinateUnequal") << lm0 << lm13 << false;

        QLandmark lm14(lm0);
        lm14.setCategories(unequalCategoriesBySize);
        QTest::newRow("categoriesUnequalBySize") << lm0 << lm14 << false;

        QLandmark lm15(lm0);
        lm15.setCategories(unequalCategoriesByValue);
        QTest::newRow("categoriesUnequalByValue") << lm0 << lm15 << false;

        QLandmark lm16(lm0);
        for (int i = 0; i < unequalAttributesBySize.size(); ++i) {
            QString key = unequalAttributesBySize.keys().at(i);
            lm16.setAttribute(key, unequalAttributesBySize.value(key));
        }
        QTest::newRow("attributesUnequalBySize") << lm0 << lm16 << false;

        QLandmark lm17(lm0);
        for (int i = 0; i < unequalAttributesByKey.size(); ++i) {
            QString key = unequalAttributesByKey.keys().at(i);
            lm17.setAttribute(key, unequalAttributesByKey.value(key));
        }
        QTest::newRow("attributesUnequalByKey") << lm0 << lm17 << false;

        QLandmark lm18(lm0);
        for (int i = 0; i < unequalAttributesByValue.size(); ++i) {
            QString key = unequalAttributesByValue.keys().at(i);
            lm18.setAttribute(key, unequalAttributesByValue.value(key));
        }
        QTest::newRow("attributesUnequalByValue") << lm0 << lm18 << false;

        QLandmark lm19(lm0);
        QTest::newRow("allEqual") << lm0 << lm19 << true;
    }

private:
    void genericData() {
        QTest::addColumn<QString>("name");
        QTest::addColumn<QString>("icon");
        QTest::addColumn<QString>("description");
        QTest::addColumn<QString>("street");
        QTest::addColumn<QString>("locality");
        QTest::addColumn<QString>("region");
        QTest::addColumn<QString>("country");
        QTest::addColumn<QString>("postcode");
        QTest::addColumn<QString>("phone");
        QTest::addColumn<QString>("url");
        QTest::addColumn<double>("radius");
        QTest::addColumn<QGeoCoordinate>("coordinate");
        QTest::addColumn<QList<QLandmarkCategory> >("categories");
        QTest::addColumn<QHash<QString, QVariant> >("attributes");


        QTest::newRow("Row0") << "" << "" << "" << "" << "" << "" << "" << "" << "" << ""
                                << 0.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QTest::newRow("Row1") << "landmarkName" << "" << "" << "" << "" << "" << "" << "" << ""
                                << ""
                                << 0.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QTest::newRow("Row2") << "" << "landmarkIcon" << "" << "" << "" << "" << "" << "" << ""
                                << ""
                                << 0.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QTest::newRow("Row3") << "" << "" << "landmarkDescription" << "" << "" << "" << "" << ""
                                << ""
                                << ""
                                << 0.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QTest::newRow("Row4") << "" << "" << "" << "landmarkStreet" << "" << "" << "" << "" << ""
                                << ""
                                << 0.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QTest::newRow("Row5") << "" << "" << "" << "" << "landmarkLocality" << "" << "" << "" << ""
                                << ""
                                << 0.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QTest::newRow("Row6") << "" << "" << "" << "" << "" << "landmarkRegion" << "" << "" << ""
                                << ""
                                << 0.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QTest::newRow("Row7") << "" << "" << "" << "" << "" << "" << "landmarkCountry" << "" << ""
                                << ""
                                << 0.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QTest::newRow("Row8") << "" << "" << "" << "" << "" << "" << "" << "landmarkPostcode" << ""
                                << ""
                                << 0.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QTest::newRow("Row9") << "" << "" << "" << "" << "" << "" << "" << "" << "landmarkPhone"
                                << ""
                                << 0.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QTest::newRow("Row10") << "" << "" << "" << "" << "" << "" << "" << "" << ""
                                << "landmarkUrl"
                                << 0.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QTest::newRow("Row11") << "" << "" << "" << "" << "" << "" << "" << "" << "" << ""
                                << 10.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QTest::newRow("Row12") << "" << "" << "" << "" << "" << "" << "" << "" << "" << ""
                                << 0.0
                                << QGeoCoordinate(10.0, 10.0, 10.0)
                                << QList<QLandmarkCategory>()
                                << QHash<QString, QVariant>();

        QList<QLandmarkCategory> categories;

        QLandmarkCategory cat0;
        cat0.setName("cat0Name");
        cat0.setIcon("cat0Icon");
        cat0.setDescription("cat0Description");

        QLandmarkCategory cat1;
        cat1.setName("cat1Name");
        cat1.setIcon("cat1Icon");
        cat1.setDescription("cat1Description");

        QLandmarkCategory cat2;
        cat2.setName("cat2Name");
        cat2.setIcon("cat2Icon");
        cat2.setDescription("cat2Description");

        categories << cat0 << cat1 << cat2;

        QTest::newRow("Row13") << "" << "" << "" << "" << "" << "" << "" << "" << "" << ""
                                << 0.0
                                << QGeoCoordinate()
                                << categories
                                << QHash<QString, QVariant>();

        QHash<QString, QVariant> attributes;
        attributes["attribute1"] = QVariant(1);
        attributes["attribute2"] = QVariant(2);
        attributes["attribute3"] = QVariant(3);

        QTest::newRow("Row14") << "" << "" << "" << "" << "" << "" << "" << "" << "" << ""
                                << 0.0
                                << QGeoCoordinate()
                                << QList<QLandmarkCategory>()
                                << attributes;

        QTest::newRow("Row15") << "landmarkName"
                                << "landmarkIcon"
                                << "landmarkDescription"
                                << "landmarkStreet"
                                << "landmarkLocality"
                                << "landmarkRegion"
                                << "landmarkCountry"
                                << "landmarkPostcode"
                                << "landmarkPhone"
                                << "landmarkUrl"
                                << 10.0
                                << QGeoCoordinate(10.0, 10.0, 10.0)
                                << categories
                                << attributes;
    }
};

QTEST_MAIN(tst_QLandmark)
#include "tst_qlandmark.moc"
