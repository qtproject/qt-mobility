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

//TESTED_COMPONENT=plugins/declarative/gallery

#include <qdocumentgallery.h>
#include <qgalleryresultset.h>
#include <qgallerytyperequest.h>

#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeengine.h>

#include <qdeclarativedocumentgallery.h>
#include <qdeclarativegallerytype.h>

#include <QtTest/QtTest>

QTM_USE_NAMESPACE

class QtGalleryTestResponse : public QGalleryResultSet
{
    Q_OBJECT
public:
    QtGalleryTestResponse(
            const QStringList &propertyNames,
            int count,
            QGalleryAbstractRequest::Status status,
            int error,
            const QString &errorString)
        : m_count(count)
        , m_currentIndex(-1)
        , m_propertyNames(propertyNames)
    {
        if (error != QGalleryAbstractRequest::NoError)
            QGalleryAbstractResponse::error(error, errorString);
        else if (status == QGalleryAbstractRequest::Finished)
            finish();
        else if (status == QGalleryAbstractRequest::Idle)
            finish(true);
    }

    int propertyKey(const QString &propertyName) const {
        return m_propertyNames.indexOf(propertyName); }
    QGalleryProperty::Attributes propertyAttributes(int) const {
        return QGalleryProperty::CanRead | QGalleryProperty::CanWrite; }
    QVariant::Type propertyType(int) const { return QVariant::String; }

    int itemCount() const { return m_count; }

    int currentIndex() const { return m_currentIndex; }

    bool fetch(int index)
    {
        emit currentIndexChanged(m_currentIndex = index);
        emit currentItemChanged();

        return isValid();
    }

    QVariant itemId() const { return QVariant(); }
    QUrl itemUrl() const { return QUrl(); }
    QString itemType() const { return QString(); }

    QVariant metaData(int key) const { return isValid() ? m_metaData.value(key) : QVariant(); }
    bool setMetaData(int key, const QVariant &value)
    {
        if (isValid()) {
            m_metaData.insert(key, value);
            emit metaDataChanged(m_currentIndex, 1, QList<int>() << key);
            return true;
        } else {
            return false;
        }
    }

    void setCount(int count)
    {
        qSwap(m_count, count);

        if (m_count == 0) {
            m_currentIndex = -1;

            emit itemsRemoved(0, count);
        } else if (count == 0) {
            emit itemsInserted(0, count);
        }
    }

    using QGalleryResultSet::finish;
    using QGalleryResultSet::resume;
    using QGalleryResultSet::itemsInserted;
    using QGalleryResultSet::itemsRemoved;
    using QGalleryResultSet::itemsMoved;
    using QGalleryResultSet::metaDataChanged;
    using QGalleryResultSet::progressChanged;

private:
    int m_count;
    int m_currentIndex;
    QStringList m_propertyNames;
    QHash<int, QVariant> m_metaData;
};

class QtTestGallery : public QAbstractGallery
{
public:
    QtTestGallery()
        : m_count(0)
        , m_status(QGalleryAbstractRequest::Finished)
        , m_error(QGalleryAbstractRequest::NoError)
    {}

    bool isRequestSupported(QGalleryAbstractRequest::RequestType type) const {
        return type == QGalleryAbstractRequest::TypeRequest; }

    void setStatus(QGalleryAbstractRequest::Status status) { m_status = status; }
    void setError(int error, const QString &errorString) {
        m_error = error; m_errorString = errorString; }

    void setCount(int count) { m_count = count; }

    QGalleryTypeRequest *request() const { return m_request.data(); }
    QtGalleryTestResponse *response() const { return m_response.data(); }

    void reset()
    {
        m_count = 0;
        m_status = QGalleryAbstractRequest::Finished;
        m_error = QGalleryTypeRequest::NoError;
    }

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if ((m_request = qobject_cast<QGalleryTypeRequest *>(request))) {
            m_response = new QtGalleryTestResponse(
                    m_request.data()->propertyNames(),
                    m_count,
                    m_status,
                    m_error,
                    m_errorString);
        } else {
            m_response.clear();
        }
        return m_response.data();
    }

private:
    int m_count;
    QGalleryAbstractRequest::Status m_status;
    int m_error;
    QString m_errorString;
    QWeakPointer<QGalleryTypeRequest> m_request;
    QWeakPointer<QtGalleryTestResponse> m_response;
};

class tst_QDeclarativeDocumentGalleryType : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void initTestCase();
    void cleanup();

private Q_SLOTS:
    void itemType_data();
    void itemType();
    void properties_data();
    void properties();
    void autoUpdate_data();
    void autoUpdate();
    void asyncResponse();
    void cancelAsyncResponse();
    void cancelIdleResponse();
    void clear();
    void error_data();
    void error();
    void progress_data();
    void progress();
    void available();

private:
    QtTestGallery gallery;
    QDeclarativeEngine engine;
};

void tst_QDeclarativeDocumentGalleryType::initTestCase()
{
    engine.rootContext()->setContextProperty(QLatin1String("qt_testGallery"), &gallery);
}

void tst_QDeclarativeDocumentGalleryType::cleanup()
{
    gallery.reset();
}

void tst_QDeclarativeDocumentGalleryType::itemType_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<int>("qmlItemType");
    QTest::addColumn<QString>("qmlItemTypeString");
    QTest::addColumn<int>("itemType");
    QTest::addColumn<QString>("itemTypeString");

    QTest::newRow("Null -> Audio")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType {}\n")
            << int(QDeclarativeDocumentGallery::InvalidType)
            << QString()
            << int(QDeclarativeDocumentGallery::Audio)
            << "Audio";

    QTest::newRow("Audio -> Audio")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType { itemType: DocumentGallery.Audio }\n")
            << int(QDeclarativeDocumentGallery::Audio)
            << "Audio"
            << int(QDeclarativeDocumentGallery::Audio)
            << "Audio";

    QTest::newRow("Audio -> PhotoAlbum")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType { itemType: DocumentGallery.Audio }\n")
            << int(QDeclarativeDocumentGallery::Audio)
            << "Audio"
            << int(QDeclarativeDocumentGallery::PhotoAlbum)
            << "PhotoAlbum";

    QTest::newRow("PhotoAlbum -> Null")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType { itemType: DocumentGallery.PhotoAlbum }\n")
            << int(QDeclarativeDocumentGallery::PhotoAlbum)
            << "PhotoAlbum"
            << int(QDeclarativeDocumentGallery::InvalidType)
            << QString();
}

void tst_QDeclarativeDocumentGalleryType::itemType()
{
    QFETCH(QByteArray, qml);
    QFETCH(int, qmlItemType);
    QFETCH(QString, qmlItemTypeString);
    QFETCH(int, itemType);
    QFETCH(QString, itemTypeString);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);

    QSignalSpy spy(object.data(), SIGNAL(itemTypeChanged()));

    if (qmlItemType != QDeclarativeDocumentGallery::InvalidType) {
        QVERIFY(gallery.request());
        QCOMPARE(gallery.request()->itemType(), qmlItemTypeString);
    } else {
        QVERIFY(!gallery.request());
    }

    QCOMPARE(object->property("itemType"), QVariant(qmlItemType));

    QCOMPARE(object->setProperty("itemType", itemType), true);
    QCOMPARE(object->property("itemType"), QVariant(itemType));
    QCOMPARE(spy.count(), itemType != qmlItemType ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");

    if (itemType != QDeclarativeDocumentGallery::InvalidType) {
        QVERIFY(gallery.request());
        QCOMPARE(gallery.request()->itemType(), itemTypeString);
    }
}

void tst_QDeclarativeDocumentGalleryType::properties_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QStringList>("qmlPropertyNames");
    QTest::addColumn<QStringList>("propertyNames");

    QTest::newRow("[] -> [ title, fileName ]")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType {\n"
                        "itemType: DocumentGallery.File\n"
                    "}\n")
            << QStringList()
            << (QStringList() << QLatin1String("title") << QLatin1String("fileName"));

    QTest::newRow("[ title, fileName ] -> [ title, fileName ]")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType {\n"
                        "itemType: DocumentGallery.File\n"
                        "properties: [ \"title\", \"fileName\" ]"
                    "}\n")
            << (QStringList() << QLatin1String("title") << QLatin1String("fileName"))
            << (QStringList() << QLatin1String("title") << QLatin1String("fileName"));

    QTest::newRow("[ title, fileName ] -> [ count ]")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType {\n"
                        "itemType: DocumentGallery.File\n"
                        "properties: [ \"title\", \"fileName\" ]"
                    "}\n")
            << (QStringList() << QLatin1String("title") << QLatin1String("fileName"))
            << (QStringList() << QLatin1String("count"));
}

void tst_QDeclarativeDocumentGalleryType::properties()
{
    QFETCH(QByteArray, qml);
    QFETCH(QStringList, qmlPropertyNames);
    QFETCH(QStringList, propertyNames);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());

    QSignalSpy spy(object.data(), SIGNAL(itemTypeChanged()));

    QCOMPARE(gallery.request()->propertyNames(), qmlPropertyNames);
    QCOMPARE(object->property("properties"), QVariant(qmlPropertyNames));

    QCOMPARE(object->setProperty("properties", propertyNames), true);
    QCOMPARE(object->property("properties"), QVariant(qmlPropertyNames));
    QCOMPARE(spy.count(), 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QVERIFY(gallery.request());
    QCOMPARE(gallery.request()->propertyNames(), qmlPropertyNames);

}

void tst_QDeclarativeDocumentGalleryType::autoUpdate_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<bool>("qmlAutoUpdate");
    QTest::addColumn<bool>("autoUpdate");

    QTest::newRow("Default -> true")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType {\n"
                        "itemType: DocumentGallery.File\n"
                    "}\n")
            << false
            << true;

    QTest::newRow("true -> false")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType {\n"
                        "itemType: DocumentGallery.File\n"
                        "autoUpdate: true\n"
                    "}\n")
            << true
            << false;

    QTest::newRow("false -> true")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType {\n"
                        "itemType: DocumentGallery.File\n"
                        "autoUpdate: false\n"
                    "}\n")
            << false
            << true;

    QTest::newRow("true -> true")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType {\n"
                        "itemType: DocumentGallery.File\n"
                        "autoUpdate: true\n"
                    "}\n")
            << true
            << true;

    QTest::newRow("false -> false")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType {\n"
                        "itemType: DocumentGallery.File\n"
                        "autoUpdate: false\n"
                    "}\n")
            << false
            << false;
}

void tst_QDeclarativeDocumentGalleryType::autoUpdate()
{
    QFETCH(QByteArray, qml);
    QFETCH(bool, qmlAutoUpdate);
    QFETCH(bool, autoUpdate);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(autoUpdateChanged()));

    QCOMPARE(object->property("autoUpdate"), QVariant(qmlAutoUpdate));
    QCOMPARE(gallery.request()->autoUpdate(), qmlAutoUpdate);

    object->setProperty("autoUpdate", autoUpdate);
    QCOMPARE(object->property("autoUpdate"), QVariant(autoUpdate));
    QCOMPARE(spy.count(), qmlAutoUpdate != autoUpdate ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->autoUpdate(), autoUpdate);
}


void tst_QDeclarativeDocumentGalleryType::asyncResponse()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryType { itemType: DocumentGallery.File }\n");

    gallery.setStatus(QGalleryAbstractRequest::Active);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryType::Active));

    gallery.response()->finish();
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryType::Finished));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryType::cancelAsyncResponse()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryType { itemType: DocumentGallery.File }\n");

    gallery.setStatus(QGalleryAbstractRequest::Active);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryType::Active));

    QMetaObject::invokeMethod(object.data(), "cancel");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryType::Cancelled));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryType::cancelIdleResponse()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryType { itemType: DocumentGallery.File }\n");

    gallery.setStatus(QGalleryAbstractRequest::Idle);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryType::Idle));

    QMetaObject::invokeMethod(object.data(), "cancel");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryType::Finished));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryType::clear()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryType { itemType: DocumentGallery.File }\n");

    gallery.setCount(1);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy statusSpy(object.data(), SIGNAL(statusChanged()));
    QSignalSpy availableSpy(object.data(), SIGNAL(availableChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryType::Finished));

    QMetaObject::invokeMethod(object.data(), "clear");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryType::Null));
    QCOMPARE(object->property("available"), QVariant(false));
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(availableSpy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryType::error_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<int>("errorCode");
    QTest::addColumn<QString>("errorMessage");
    QTest::addColumn<QByteArray>("expectedErrorMessage");

    QTest::newRow("Specific error message")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType { itemType: DocumentGallery.File }\n")
            << int(QDocumentGallery::ConnectionError)
            << "Connection to server failed"
            << QByteArray(
                    "<Unknown File>: QML DocumentGalleryType: "
                    "Connection to server failed");

    QTest::newRow("Generic connection Error")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType { itemType: DocumentGallery.File }\n")
            << int(QDocumentGallery::ConnectionError)
            << QString()
            << QByteArray(
                    "<Unknown File>: QML DocumentGalleryType: "
                    "An error was encountered connecting to the document gallery");

    QTest::newRow("Generic itemType Error")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType { itemType: DocumentGallery.File }\n")
            << int(QDocumentGallery::ItemTypeError)
            << QString()
            << QByteArray(
                    "<Unknown File>: QML DocumentGalleryType: "
                    "DocumentGallery.File is not a supported item type");

    QTest::newRow("Unhandled error code")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryType { itemType: DocumentGallery.File }\n")
            << int(QDocumentGallery::NoGallery)
            << QString()
            << QByteArray();
}

void tst_QDeclarativeDocumentGalleryType::error()
{
    QFETCH(QByteArray, qml);
    QFETCH(int, errorCode);
    QFETCH(QString, errorMessage);
    QFETCH(QByteArray, expectedErrorMessage);

    gallery.setError(errorCode, errorMessage);

    if (!expectedErrorMessage.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, expectedErrorMessage.constData());

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryType::Error));
}

void tst_QDeclarativeDocumentGalleryType::progress_data()
{
    QTest::addColumn<int>("currentProgress");
    QTest::addColumn<int>("maximumProgress");
    QTest::addColumn<qreal>("normalizedProgress");

    QTest::newRow("25%")
            << 25
            << 100
            << qreal(0.25);

    QTest::newRow("50%")
            << 3
            << 6
            << qreal(0.5);

    QTest::newRow("75%")
            << 60
            << 80
            << qreal(0.75);

    QTest::newRow("100%")
            << 32
            << 32
            << qreal(1.0);
}

void tst_QDeclarativeDocumentGalleryType::progress()
{
    QFETCH(int, currentProgress);
    QFETCH(int, maximumProgress);
    QFETCH(qreal, normalizedProgress);

    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryType { itemType: DocumentGallery.File }\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(progressChanged()));

    QCOMPARE(object->property("progress"), QVariant(qreal(0.0)));

    gallery.response()->progressChanged(currentProgress, maximumProgress);
    QCOMPARE(object->property("progress"), QVariant(normalizedProgress));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryType::available()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryType { itemType: DocumentGallery.File }\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());
    QCOMPARE(object->property("available"), QVariant(false));

    QSignalSpy spy(object.data(), SIGNAL(availableChanged()));

    gallery.response()->setCount(1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(object->property("available"), QVariant(true));

    gallery.response()->setCount(0);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(object->property("available"), QVariant(false));
}

QTEST_MAIN(tst_QDeclarativeDocumentGalleryType)

#include "tst_qdeclarativedocumentgallerytype.moc"
