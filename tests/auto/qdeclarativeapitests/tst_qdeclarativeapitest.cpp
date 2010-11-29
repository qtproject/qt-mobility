/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <QtTest/QSignalSpy>
#include <QMetaObject>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativeproperty.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <qmobilityglobal.h>
#include <QPluginLoader>

#ifdef API_TEST_DECLARATIVE_LOCATION
#include "qdeclarativegeomapobject_p.h"
#include "qdeclarativegeomaptextobject_p.h"
#include "qdeclarativegraphicsgeomap_p.h"
#include "qdeclarativegeoserviceprovider_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeomapobjectborder_p.h"
#include "qdeclarativegeomappixmapobject_p.h"

#endif

// Eventually these will make it into qtestcase.h
// but we might need to tweak the timeout values here.
#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

#ifndef QTRY_VERIFY
#define QTRY_VERIFY(__expr) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)
#endif

#ifndef QTRY_WAIT
#define QTRY_WAIT(code, __expr) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            do { code } while(0); \
            QTest::qWait(__step); \
        } \
    } while(0)
#endif

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

QTM_USE_NAMESPACE

class tst_QDeclarativeApiTest : public QObject
{
    Q_OBJECT

public:
    tst_QDeclarativeApiTest() {}
    virtual ~tst_QDeclarativeApiTest() {}

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void clean();

private slots:
    void basicApiTest();
    void basicApiTest_data();

private:
    QObject* createComponent(const QString& componentString);
    QDeclarativeEngine m_engine;
};

// Extend PropertyValues as needed to data-drive the test to extent possible
typedef struct {
    QVariant defaultValue;
    QVariant newValue;
    bool (*customTypeCompFn)(QVariant,QVariant);
} PropertyValues;
Q_DECLARE_METATYPE(PropertyValues);
typedef QMap<QString,PropertyValues> PropertyMap;
Q_DECLARE_METATYPE(PropertyMap);

#ifdef API_TEST_DECLARATIVE_LOCATION
// Assign custom enum types a QMetaTypeId so that QVariant can handle them
Q_DECLARE_METATYPE(QDeclarativeGraphicsGeoMap::MapType);
Q_DECLARE_METATYPE(QDeclarativeGraphicsGeoMap::ConnectivityMode);
Q_DECLARE_METATYPE(QDeclarativeGeoMapTextObject::HorizontalAlignment);
Q_DECLARE_METATYPE(QDeclarativeGeoMapTextObject::VerticalAlignment);
Q_DECLARE_METATYPE(QDeclarativeGeoMapPixmapObject::Status);
#endif

void tst_QDeclarativeApiTest::initTestCase()
{
#ifdef API_TEST_DECLARATIVE_LOCATION
    // const keyword in signal parms is a pain
    qRegisterMetaType<const QDeclarativeCoordinate*>("const QDeclarativeCoordinate*");
    qRegisterMetaType<QDeclarativeGeoMapTextObject::HorizontalAlignment>("HorizontalAlignment");
    qRegisterMetaType<QDeclarativeGeoMapTextObject::VerticalAlignment>("VerticalAlignment");
#endif
}
void tst_QDeclarativeApiTest::cleanupTestCase() {}
void tst_QDeclarativeApiTest::init() {}
void tst_QDeclarativeApiTest::clean() {}

void tst_QDeclarativeApiTest::basicApiTest()
{
    QFETCH(QString, componentString);
    QFETCH(QString, expectedClassName);
    QFETCH(PropertyMap, expectedProperties);
    QFETCH(bool, checkPropertiesAreCovered);

    QDeclarativeComponent component(&m_engine);
    component.setData(componentString.toLatin1(), QUrl::fromLocalFile(""));
    QObject* obj = component.create();
    if (obj == 0 && component.isError())
        qDebug() << "QDeclarativeComponent::errors(): " << component.errors();
    QVERIFY(obj != 0);
    const QMetaObject* meta_obj = obj->metaObject();
    QCOMPARE(meta_obj->className(), expectedClassName.toAscii().constData());
    qDebug() << "************************** Testing element: " << meta_obj->className();
    for (int property_index = meta_obj->propertyOffset(); property_index < meta_obj->propertyCount(); ++property_index) {
        QMetaProperty meta_prop = meta_obj->property(property_index);
        qDebug() << "*** Testing property: " << meta_prop.name();
        QVERIFY(meta_prop.isReadable());
        // Verify that the property is expected, otherwise testcase is outdated.
        if (checkPropertiesAreCovered) {
            QVERIFY(expectedProperties.contains(meta_prop.name()));
        } else if (!expectedProperties.contains(meta_prop.name())) {
            continue;
        }
        QVariant expectedPropertyValue = expectedProperties.value(meta_prop.name()).defaultValue;
        QVariant newPropertyValue = expectedProperties.value(meta_prop.name()).newValue;
        bool (*userTypeCompFn)(QVariant,QVariant) = expectedProperties.value(meta_prop.name()).customTypeCompFn;
        // Verify the default value of the property. Invalid QVariant means that step is skipped
        if (!expectedPropertyValue.isValid()) {
            qDebug() << "*** Default-value check skipped for " << meta_obj->className() << "::" << meta_prop.name();
        } else {
            // Check validity and value
            QVariant propertyValue = meta_prop.read(obj);
            QVERIFY(propertyValue.isValid());
            if (propertyValue.type() == QVariant::UserType) {
                QVERIFY(userTypeCompFn);
                QVERIFY(userTypeCompFn(propertyValue, expectedPropertyValue));
            } else {
                QCOMPARE(propertyValue, expectedPropertyValue);
            }
        }
        // If writable, check the changed -signal
        if (meta_prop.isWritable() && newPropertyValue.isValid()) {
            QVERIFY(meta_prop.hasNotifySignal());
            QMetaMethod notifySignal = meta_prop.notifySignal();
            // Can't use SIGNAL macro with run-time-known signature -> prepend signal signature manually
            QString signature = QString::number(QSIGNAL_CODE) + notifySignal.signature();
            QSignalSpy* attributeChangedSpy = new QSignalSpy(obj, signature.toAscii().constData());
            // Change the value and make sure signal is emitted and value changed
            // Hackyish but true: use QDeclarativeProperty's write because it avoids moc
            // namespace issues which the QMetaProperty::write() hits; without this we would
            // not be able to write UserTypes defined in namespaces, such as declarative pointer
            // variables (e.g. QDeclarativeCoordinate*).
            QDeclarativeProperty decl_prop(obj, meta_prop.name());
            QVERIFY(decl_prop.isValid());
            QVERIFY(decl_prop.write(newPropertyValue));
            QTRY_VERIFY(!attributeChangedSpy->isEmpty());
            QVariant propertyValue = meta_prop.read(obj);
            QVERIFY(propertyValue.isValid());
            // QVariant::operator== works only for builtin types, for custom types provide a comparison function
            if (propertyValue.type() == QVariant::UserType) {
                QVERIFY(userTypeCompFn);
                QVERIFY(userTypeCompFn(propertyValue, newPropertyValue));
            } else {
                QCOMPARE(propertyValue, newPropertyValue);
            }
            // Change the value with same value and make sure signal is not emitted again
            attributeChangedSpy->clear();
            QVERIFY(decl_prop.write(newPropertyValue));
            QTest::qWait(10);
            QVERIFY(attributeChangedSpy->isEmpty());
            delete attributeChangedSpy;
        } else if (meta_prop.isWritable()) {
            qDebug() << "*** Changed signal -check skipped for " << meta_obj->className() << "::" << meta_prop.name();
        }
        if (checkPropertiesAreCovered)
            expectedProperties.remove(meta_prop.name());
    }
    // Verify that all properties were checked. Otherwise testcase is outdated
    if (checkPropertiesAreCovered) {
        if (!expectedProperties.isEmpty()) {
            qWarning() << "*** Following properties are not covered: " << expectedProperties.keys();
        }
        QVERIFY(expectedProperties.isEmpty());
    }
    qDebug() << "************************** /End testing element: " << meta_obj->className() << "\n";
    delete obj;
}

// UserType comparison function for POD types (e.g. enums)
template <class T>
static bool customPodCompFn(QVariant one, QVariant theOther)
{
    if (!(one.canConvert<T>() && theOther.canConvert<T>())) {
        qDebug() << "QVariants are not convertible/comparable: " << one << theOther;
        return false;
    }
    return (one.value<T>() == theOther.value<T>());
}

// Dumb UserType comparison function for pointers. 'Dumb' means that the comparator
// is only interested if both are null or non-null pointers. This is useful when you
// set a pointer property, but internally the element just rips some value off it
// (and hence the pointer in read() will remain unchanged).
template <class T>
static bool customPtrCompFn(QVariant one, QVariant theOther)
{
    if (!(one.canConvert<T>() && theOther.canConvert<T>())) {
        qDebug() << "QVariants are not convertible/comparable: " << one << theOther;
        return false;
    }
    return ((one.value<T>() == 0 && theOther.value<T>() == 0) ||
            (one.value<T>() != 0 && theOther.value<T>() != 0));
}


void tst_QDeclarativeApiTest::basicApiTest_data()
{
    QTest::addColumn<QString>("expectedClassName");
    QTest::addColumn<QString>("componentString");
    QTest::addColumn<PropertyMap>("expectedProperties");
    QTest::addColumn<bool>("checkPropertiesAreCovered");
#ifdef API_TEST_DECLARATIVE_LOCATION

    // Some general purpose variables
    QDeclarativeCoordinate* generalDeclarativeCoordinate = new QDeclarativeCoordinate(QGeoCoordinate(10,10), this);

    // Address
    PropertyMap geoAddressPropertyMap;
    PropertyValues country = {"", "newValue",0}; geoAddressPropertyMap.insert("country", country);
    PropertyValues countryCode = {"", "newValue",0}; geoAddressPropertyMap.insert("countryCode", countryCode);
    PropertyValues state = {"", "newValue",0}; geoAddressPropertyMap.insert("state", state);
    PropertyValues county = {"", "newValue",0}; geoAddressPropertyMap.insert("county", county);
    PropertyValues city = {"", "newValue",0}; geoAddressPropertyMap.insert("city", city);
    PropertyValues district = {"", "newValue",0}; geoAddressPropertyMap.insert("district", district);
    PropertyValues street = {"", "newValue",0}; geoAddressPropertyMap.insert("street", street);
    PropertyValues postcode = {"", "newValue",0}; geoAddressPropertyMap.insert("postcode", postcode);
    QTest::newRow("Address") << "QDeclarativeGeoAddress" << "import Qt 4.7 \n import QtMobility.location 1.1 \n Address {}" << geoAddressPropertyMap << true;

    // Map
    PropertyMap mapDefaultPropertyMap;
    PropertyValues size = {QVariant(QSizeF(100.0, 100.0)), QVariant(QSize(150.0, 150.0)), 0};
    mapDefaultPropertyMap.insert("size", size);
    PropertyValues zoomLevel = {QVariant(qreal(8)), QVariant(qreal(9)), 0};
    mapDefaultPropertyMap.insert("zoomLevel", zoomLevel);
    PropertyValues mapType = {QVariant::fromValue(QDeclarativeGraphicsGeoMap::NoMap), QVariant::fromValue(QDeclarativeGraphicsGeoMap::StreetMap), &customPodCompFn<QtMobility::QDeclarativeGraphicsGeoMap::MapType>};
    mapDefaultPropertyMap.insert("mapType", mapType);
    PropertyValues connectivityMode = {QVariant::fromValue(QDeclarativeGraphicsGeoMap::NoConnectivity), QVariant::fromValue(QDeclarativeGraphicsGeoMap::OfflineMode), &customPodCompFn<QtMobility::QDeclarativeGraphicsGeoMap::ConnectivityMode>};
    mapDefaultPropertyMap.insert("connectivityMode", connectivityMode);
    PropertyValues minimumZoomLevel = {qreal(-1.0), QVariant(), 0};  // (is not writable -> no newValue)
    mapDefaultPropertyMap.insert("minimumZoomLevel", minimumZoomLevel);
    PropertyValues maximumZoomLevel = {qreal(-1.0), QVariant(), 0};  // (Is not writable -> no newValue)
    mapDefaultPropertyMap.insert("maximumZoomLevel", maximumZoomLevel);
    // QDeclarativeCoordinate* centerCoordinate = new QDeclarativeCoordinate(QGeoCoordinate(10,10), this);
    PropertyValues center = {QVariant(), QVariant::fromValue(generalDeclarativeCoordinate), &customPtrCompFn<QDeclarativeCoordinate*>};
    mapDefaultPropertyMap.insert("center", center);
    PropertyValues plugin = {QVariant::fromValue((QDeclarativeGeoServiceProvider*)(0)), QVariant(), &customPodCompFn<QDeclarativeGeoServiceProvider*>};
    mapDefaultPropertyMap.insert("plugin", plugin);
    PropertyValues objects = {QVariant(), QVariant(), 0}; // todo, what is the best way to test declarative list
    mapDefaultPropertyMap.insert("objects", objects);
    QTest::newRow("Map") << "QDeclarativeGraphicsGeoMap" << "import Qt 4.7 \n import QtMobility.location 1.1 \n Map {}" << mapDefaultPropertyMap << true;
    // Test separately because setting plugin provider changes the expected values
    PropertyMap mapNokiaPluginPropertyMap;
    QDeclarativeGeoServiceProvider* geoServiceProvider = new QDeclarativeGeoServiceProvider(this);
    geoServiceProvider->setName("nokia");
    PropertyValues nokiaPlugin = {QVariant::fromValue((QDeclarativeGeoServiceProvider*)(0)), QVariant::fromValue(geoServiceProvider), &customPodCompFn<QDeclarativeGeoServiceProvider*>};
    mapNokiaPluginPropertyMap.insert("plugin", nokiaPlugin);
    QTest::newRow("Map (existing plugin)") << "QDeclarativeGraphicsGeoMap" << "import Qt 4.7 \n import QtMobility.location 1.1 \n Map {}" << mapNokiaPluginPropertyMap << false;
    // Must not crash even if there is no plugin
    PropertyMap mapNonexistentPluginPropertyMap;
    QDeclarativeGeoServiceProvider* geoNonexistentServiceProvider = new QDeclarativeGeoServiceProvider(this);
    geoNonexistentServiceProvider->setName("non_existent_plugin");
    PropertyValues nonexistentPlugin = {QVariant::fromValue((QDeclarativeGeoServiceProvider*)(0)), QVariant::fromValue(geoServiceProvider), &customPodCompFn<QDeclarativeGeoServiceProvider*>};
    mapNonexistentPluginPropertyMap.insert("plugin", nonexistentPlugin);
    QTest::newRow("Map (nonexisting plugin)") << "QDeclarativeGraphicsGeoMap" << "import Qt 4.7 \n import QtMobility.location 1.1 \n Map {}" << mapNokiaPluginPropertyMap << false;

    // MapCircle
    PropertyMap mapCircleDefaultPropertyMap;
    // QDeclarativeCoordinate* mapCircleCoordinate = new QDeclarativeCoordinate(QGeoCoordinate(10,10), this);
    PropertyValues mapCircleCenterCoordinate = {QVariant(), QVariant::fromValue(generalDeclarativeCoordinate), &customPtrCompFn<QDeclarativeCoordinate*>};
    mapCircleDefaultPropertyMap.insert("center", mapCircleCenterCoordinate);
    PropertyValues mapCircleColor = {QColor(), QColor(1,2,3), 0};
    mapCircleDefaultPropertyMap.insert("color", mapCircleColor);
    PropertyValues mapCircleBorder = {QVariant(), QVariant(), 0};
    mapCircleDefaultPropertyMap.insert("border", mapCircleBorder);
    QTest::newRow("MapCircle") << "QDeclarativeGeoMapCircleObject" << "import Qt 4.7 \n import QtMobility.location 1.1 \n MapCircle {}" << mapCircleDefaultPropertyMap << true;

    // MapText
    PropertyMap mapTextDefaultPropertyMap;
    PropertyValues mapTextCoordinate = {QVariant(), QVariant::fromValue(generalDeclarativeCoordinate), &customPtrCompFn<QDeclarativeCoordinate*>};
    mapTextDefaultPropertyMap.insert("coordinate", mapTextCoordinate);
    PropertyValues mapTextColor = {QColor(), QColor(1,2,3), 0};
    mapTextDefaultPropertyMap.insert("color", mapTextColor);
    PropertyValues mapTextHorizontalAlignment = {QVariant::fromValue(QDeclarativeGeoMapTextObject::AlignHCenter), QVariant::fromValue(QDeclarativeGeoMapTextObject::AlignLeft), &customPodCompFn<QtMobility::QDeclarativeGeoMapTextObject::HorizontalAlignment>};
    mapTextDefaultPropertyMap.insert("horizontalAlignment", mapTextHorizontalAlignment);
    PropertyValues mapTextVerticalAlignment = {QVariant::fromValue(QDeclarativeGeoMapTextObject::AlignVCenter), QVariant::fromValue(QDeclarativeGeoMapTextObject::AlignBottom), &customPodCompFn<QtMobility::QDeclarativeGeoMapTextObject::VerticalAlignment>};
    mapTextDefaultPropertyMap.insert("verticalAlignment", mapTextVerticalAlignment);
    QTest::newRow("MapText") << "QDeclarativeGeoMapTextObject" << "import Qt 4.7 \n import QtMobility.location 1.1 \n MapText {}" << mapTextDefaultPropertyMap << true;

    // MapRectangle
    PropertyMap mapRectangleDefaultPropertyMap;
    PropertyValues mapRectangleTopLeft = {QVariant(), QVariant::fromValue(generalDeclarativeCoordinate), &customPtrCompFn<QDeclarativeCoordinate*>};
    mapRectangleDefaultPropertyMap.insert("topLeft", mapRectangleTopLeft);
    PropertyValues mapRectangleBottomRight = {QVariant(), QVariant::fromValue(generalDeclarativeCoordinate), &customPtrCompFn<QDeclarativeCoordinate*>};
    mapRectangleDefaultPropertyMap.insert("bottomRight", mapRectangleBottomRight);
    PropertyValues mapRectangleColor = {QColor(), QColor(1,2,3), 0};
    mapRectangleDefaultPropertyMap.insert("color", mapRectangleColor);
    PropertyValues mapRectangleBorder = {QVariant(), QVariant(), 0};
    mapRectangleDefaultPropertyMap.insert("border", mapRectangleBorder);
    QTest::newRow("MapRectangle") << "QDeclarativeGeoMapRectangleObject" << "import Qt 4.7 \n import QtMobility.location 1.1 \n MapRectangle {}" << mapRectangleDefaultPropertyMap << true;

    // MapImage
    PropertyMap mapImageDefaultPropertyMap;
    PropertyValues mapImageCoordinate = {QVariant(), QVariant::fromValue(generalDeclarativeCoordinate), &customPtrCompFn<QDeclarativeCoordinate*>};
    mapImageDefaultPropertyMap.insert("coordinate", mapImageCoordinate);
    PropertyValues mapImageSource = {QUrl(), QUrl("some_url"), 0};
    mapImageDefaultPropertyMap.insert("source", mapImageSource);
    // Error below is due to nonexistent image url
    PropertyValues mapImageStatus = {QVariant::fromValue(QDeclarativeGeoMapPixmapObject::Error), QVariant(), &customPodCompFn<QtMobility::QDeclarativeGeoMapPixmapObject::Status>};
    mapImageDefaultPropertyMap.insert("status", mapImageStatus);
    QTest::newRow("MapImage") << "QDeclarativeGeoMapPixmapObject" << "import Qt 4.7 \n import QtMobility.location 1.1 \n MapImage {}" << mapImageDefaultPropertyMap << true;

#endif // API_TEST_DECLARATIVE_LOCATION
}

/*
    Helper function to create components from given string.
*/
QObject* tst_QDeclarativeApiTest::createComponent(const QString& componentString)
{
    QDeclarativeComponent component(&m_engine);
    component.setData(componentString.toLatin1(), QUrl::fromLocalFile(""));
    QObject* source_obj = component.create();
    Q_ASSERT(source_obj != 0);
    return source_obj;
}

QTEST_MAIN(tst_QDeclarativeApiTest)
#include "tst_qdeclarativeapitest.moc"
