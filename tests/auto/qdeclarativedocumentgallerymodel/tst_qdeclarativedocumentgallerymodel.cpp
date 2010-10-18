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
#include <qgalleryqueryrequest.h>
#include <qgalleryresultset.h>

#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeengine.h>

#include <qdeclarativedocumentgallery.h>
#include <qdeclarativegalleryfilter.h>
#include <qdeclarativegalleryquerymodel.h>

#include <QtTest/QtTest>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGalleryFilter)
Q_DECLARE_METATYPE(QGalleryFilter::Type)
Q_DECLARE_METATYPE(QGalleryQueryRequest::Scope)
Q_DECLARE_METATYPE(QModelIndex)

class QtTestResultSet : public QGalleryResultSet
{
    Q_OBJECT
public:
    struct Row
    {
        Row() {}
        Row(const QVariant &itemId,
                const QUrl &itemUrl,
                const QString &itemType,
                const QHash<QString, QVariant> &metaData)
            : itemId(itemId)
            , itemUrl(itemUrl)
            , itemType(itemType)
            , metaData(metaData) {}
        QVariant itemId;
        QUrl itemUrl;
        QString itemType;
        QHash<QString, QVariant> metaData;
    };

    QtTestResultSet(
            QGalleryAbstractRequest::Status status,
            int error,
            const QString &errorString,
            const QHash<QString, QGalleryProperty::Attributes> &propertyAttributes,
            const QVector<Row> &rows)
        : m_propertyNames(propertyAttributes.keys())
        , m_propertyAttributes(propertyAttributes)
        , m_rows(rows)
        , m_currentIndex(-1)
        , m_insertIndex(0)
        , m_insertCount(0)
    {
        if (error != QGalleryAbstractRequest::NoError)
            QGalleryAbstractResponse::error(error, errorString);
        else if (status == QGalleryAbstractRequest::Finished)
            finish();
        else if (status == QGalleryAbstractRequest::Idle)
            finish(true);
    }

    QStringList propertyNames() const { return m_propertyNames; }

    int propertyKey(const QString &propertyName) const {
        return  m_propertyNames.indexOf(propertyName); }

    QGalleryProperty::Attributes propertyAttributes(int key) const {
        return m_propertyAttributes.value(m_propertyNames.value(key)); }

    QVariant::Type propertyType(int key) const {
        return key >= 0 && key < m_propertyNames.count() ? QVariant::String : QVariant::Invalid; }

    int itemCount() const { return m_rows.count(); }

    int currentIndex() const { return m_currentIndex; }
    bool fetch(int index)
    {
        emit currentIndexChanged(m_currentIndex = index);
        emit currentItemChanged();

        return isValid();
    }

    QVariant itemId() const { return m_rows.value(m_currentIndex).itemId; }
    QUrl itemUrl() const { return m_rows.value(m_currentIndex).itemUrl; }
    QString itemType() const { return m_rows.value(m_currentIndex).itemType; }

    QVariant metaData(int key) const {
        return m_rows.value(m_currentIndex).metaData.value(m_propertyNames.value(key)); }

    bool setMetaData(int key, const QVariant &value)
    {
        if (isValid() && key >= 0 && key < m_propertyNames.count()) {
            const QString propertyName = m_propertyNames.at(key);

            if (m_propertyAttributes.value(propertyName) & QGalleryProperty::CanWrite) {
                m_rows[m_currentIndex].metaData[propertyName] = value;

                emit metaDataChanged(m_currentIndex, 1, QList<int>() << key);

                return true;
            }
        }
        return false;
    }

    void beginInsertRows(int index) { m_insertIndex = index; m_insertCount = 0; }
    void addRow()
    {
        m_rows.insert(m_insertIndex + m_insertCount++, Row());
    }
    void addRow(
            const QString &itemId,
            const QUrl &itemUrl,
            const QString &rootType,
            const QHash<QString, QVariant> &metaData)
    {
        m_rows.insert(m_insertIndex + m_insertCount++, Row(itemId, itemUrl, rootType, metaData));
    }
    void endInsertRows() { emit itemsInserted(m_insertIndex, m_insertCount); }

    void removeRows(int index, int count) {
        m_rows.remove(index, count); emit itemsRemoved(index, count); }

    using QGalleryResultSet::finish;
    using QGalleryResultSet::metaDataChanged;
    using QGalleryResultSet::itemsMoved;
    using QGalleryResultSet::progressChanged;

private:
    const QStringList m_propertyNames;
    const QHash<QString, QGalleryProperty::Attributes> m_propertyAttributes;
    QVector<Row> m_rows;
    int m_currentIndex;
    int m_insertIndex;
    int m_insertCount;
};

class QtTestGallery : public QAbstractGallery
{
public:
    QtTestGallery()
        : m_status(QGalleryAbstractRequest::Finished)
        , m_error(QGalleryAbstractRequest::NoError)
    {
    }

    bool isRequestSupported(QGalleryAbstractRequest::RequestType) const { return true; }

    void setPropertyAttributes(const QHash<QString, QGalleryProperty::Attributes> &attributes)
    {
        m_propertyAttributes = attributes;
    }

    void setStatus(QGalleryAbstractRequest::Status status) { m_status = status; }
    void setError(int error, const QString &errorString) {
        m_error = error; m_errorString = errorString; }

    void addRow()
    {
        m_rows.append(QtTestResultSet::Row());
    }

    void addRow(
            const QVariant &itemId,
            const QUrl &itemUrl,
            const QString &rootType,
            const QHash<QString, QVariant> &metaData)
    {
        m_rows.append(QtTestResultSet::Row(itemId, itemUrl, rootType, metaData));
    }

    void reset()
    {
        m_rows.clear();
        m_status = QGalleryAbstractRequest::Finished;
        m_error = QGalleryAbstractRequest::NoError;
    }

    QGalleryQueryRequest *request() const { return m_request.data(); }
    QtTestResultSet *response() const { return m_response.data(); }

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if ((m_request = qobject_cast<QGalleryQueryRequest *>(request))) {
            m_response = new QtTestResultSet(
                    m_status, m_error, m_errorString, m_propertyAttributes, m_rows);
        } else {
            m_response.clear();
        }
        return m_response.data();
    }


private:
    QHash<QString, QGalleryProperty::Attributes> m_propertyAttributes;
    QVector<QtTestResultSet::Row> m_rows;
    QGalleryAbstractRequest::Status m_status;
    int m_error;
    QString m_errorString;
    QWeakPointer<QGalleryQueryRequest> m_request;
    QWeakPointer<QtTestResultSet> m_response;
};

class tst_QDeclarativeDocumentGalleryModel : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void initTestCase();
    void cleanup();

private Q_SLOTS:
    void rootType_data();
    void rootType();
    void rootTypeAuto_data() { rootType_data(); }
    void rootTypeAuto();
    void properties_data();
    void properties();
    void propertiesAuto_data() { properties_data(); }
    void propertiesAuto();
    void sortProperties_data();
    void sortProperties();
    void sortPropertiesAuto_data() { sortProperties_data(); }
    void sortPropertiesAuto();
    void autoUpdate_data();
    void autoUpdate();
    void autoUpdateAuto_data() { autoUpdate_data(); }
    void autoUpdateAuto();
    void disableAutoUpdateFinished();
    void disableAutoUpdateIdle();
    void disableAutoUpdateActive();
    void rootItem_data();
    void rootItem();
    void rootItemAuto_data() { rootItem_data(); }
    void rootItemAuto();
    void scope_data();
    void scope();
    void scopeAuto_data() { scope_data(); }
    void scopeAuto();
    void offset_data();
    void offset();
    void offsetAuto_data() { offset_data(); }
    void offsetAuto();
    void limit_data();
    void limit();
    void limitAuto_data() { limit_data(); }
    void limitAuto();
    void filter_data();
    void filter();
    void changeFilter_data();
    void changeFilter();
    void changeFilterAuto_data() { changeFilter_data(); }
    void changeFilterAuto();
    void filterProperties_data();
    void filterProperties();
    void groupFilter_data();
    void groupFilter();
    void progress_data();
    void progress();
    void roleNames();
    void indexes();
    void data_data();
    void data();
    void setData_data();
    void setData();
    void property_data() { data_data(); }
    void property();
    void setProperty_data() { setData_data(); }
    void setProperty();
    void get_data() { data_data(); }
    void get();
    void set_data() { setData_data(); }
    void set();
    void itemsInserted();
    void itemsRemoved();
    void itemsMoved();
    void metaDataChanged();
    void asyncResponse();
    void cancelAsyncResponse();
    void cancelIdleResponse();
    void cancelPendingResponse();
    void deferExecuteCancelledResponse();
    void clear();
    void clearPendingResponse();
    void error_data();
    void error();

private:
    void populateGallery();

    QtTestGallery gallery;
    QDeclarativeEngine engine;
};

void tst_QDeclarativeDocumentGalleryModel::initTestCase()
{
    qRegisterMetaType<QGalleryFilter>();
    qRegisterMetaType<QGalleryFilter::Type>();
    qRegisterMetaType<QGalleryQueryRequest::Scope>();
    qRegisterMetaType<QModelIndex>();
    QHash<QString, QGalleryProperty::Attributes> propertyAttributes;
    propertyAttributes.insert(QLatin1String("title"), QGalleryProperty::CanRead
            | QGalleryProperty::CanWrite
            | QGalleryProperty::CanFilter
            | QGalleryProperty::CanSort);
    propertyAttributes.insert(QLatin1String("fileName"), QGalleryProperty::CanRead
            | QGalleryProperty::CanFilter
            | QGalleryProperty::CanSort);
    propertyAttributes.insert(QLatin1String("keywords"), QGalleryProperty::CanRead
            | QGalleryProperty::CanWrite
            | QGalleryProperty::CanFilter);
    gallery.setPropertyAttributes(propertyAttributes);

    engine.rootContext()->setContextProperty(QLatin1String("qt_testGallery"), &gallery);
}

void tst_QDeclarativeDocumentGalleryModel::cleanup()
{
    gallery.reset();
}

void tst_QDeclarativeDocumentGalleryModel::populateGallery()
{
    QHash<QString, QVariant> metaData;

    metaData.insert(QLatin1String("fileName"), QLatin1String("interlude.mp3"));
    metaData.insert(QLatin1String("title"), QLatin1String("Interlude"));
    gallery.addRow(
            3,
            QUrl(QLatin1String("file:///music/interlude.mp3")),
            QLatin1String("Audio"),
            metaData);

    metaData.insert(QLatin1String("fileName"), QLatin1String("beep.wav"));
    metaData.remove(QLatin1String("title"));
    gallery.addRow(
            45,
            QUrl(QLatin1String("file:///sounds/beep.wav")),
            QLatin1String("Audio"),
            metaData);
}


void tst_QDeclarativeDocumentGalleryModel::rootType_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QString>("qmlRootTypeString");
    QTest::addColumn<int>("qmlRootType");
    QTest::addColumn<int>("rootType");

    QTest::newRow("Default -> Audio")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << "File"
            << int(QDeclarativeDocumentGallery::File)
            << int(QDeclarativeDocumentGallery::Audio);

    QTest::newRow("File -> Image")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootType: DocumentGallery.File\n"
                    "}\n")
            << "File"
            << int(QDeclarativeDocumentGallery::File)
            << int(QDeclarativeDocumentGallery::Image);

    QTest::newRow("Video -> Artist")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootType: DocumentGallery.Video\n"
                    "}\n")
            << "Video"
            << int(QDeclarativeDocumentGallery::Video)
            << int(QDeclarativeDocumentGallery::Artist);

    QTest::newRow("Playlist -> Playlist")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootType: DocumentGallery.Playlist\n"
                    "}\n")
            << "Playlist"
            << int(QDeclarativeDocumentGallery::Playlist)
            << int(QDeclarativeDocumentGallery::Playlist);
}

void tst_QDeclarativeDocumentGalleryModel::rootType()
{
    QFETCH(QByteArray, qml);
    QFETCH(QString, qmlRootTypeString);
    QFETCH(int, qmlRootType);
    QFETCH(int, rootType);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(rootTypeChanged()));

    QCOMPARE(object->property("rootType"), QVariant(qmlRootType));
    QCOMPARE(gallery.request()->rootType(), qmlRootTypeString);

    object->setProperty("rootType", rootType);
    QCOMPARE(object->property("rootType"), QVariant(qmlRootType));
    QCOMPARE(spy.count(), 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->rootType(), qmlRootTypeString);
}

void tst_QDeclarativeDocumentGalleryModel::rootTypeAuto()
{
    QFETCH(QByteArray, qml);
    QFETCH(QString, qmlRootTypeString);
    QFETCH(int, qmlRootType);
    QFETCH(int, rootType);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(rootTypeChanged()));

    QCOMPARE(object->property("rootType"), QVariant(qmlRootType));
    QCOMPARE(gallery.request()->rootType(), qmlRootTypeString);

    object->setProperty("rootType", rootType);
    QCOMPARE(object->property("rootType"), QVariant(qmlRootType));
    QCOMPARE(spy.count(), 0);

    QCoreApplication::processEvents();
    QCOMPARE(gallery.request()->rootType(), qmlRootTypeString);
}

void tst_QDeclarativeDocumentGalleryModel::properties_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QStringList>("qmlPropertyNames");
    QTest::addColumn<QStringList>("propertyNames");

    QTest::newRow("Default -> [ title ]")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << QStringList()
            << (QStringList() << QLatin1String("title"));

    QTest::newRow("[ title, duration ] -> [ fileName ]")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "properties: [ \"title\", \"duration\" ]\n"
                        "sortProperties: [ \"+rating\" ]\n"
                    "}\n")
            << (QStringList() << QLatin1String("title") << QLatin1String("duration"))
            << (QStringList() << QLatin1String("fileName"));
}

void tst_QDeclarativeDocumentGalleryModel::properties()
{
    QFETCH(QByteArray, qml);
    QFETCH(QStringList, qmlPropertyNames);
    QFETCH(QStringList, propertyNames);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(propertyNamesChanged()));

    QCOMPARE(object->property("properties"), QVariant(qmlPropertyNames));
    QCOMPARE(gallery.request()->propertyNames(), qmlPropertyNames);

    object->setProperty("properties", propertyNames);
    QCOMPARE(object->property("properties"), QVariant(qmlPropertyNames));
    QCOMPARE(spy.count(), 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->propertyNames(), qmlPropertyNames);
}

void tst_QDeclarativeDocumentGalleryModel::propertiesAuto()
{
    QFETCH(QByteArray, qml);
    QFETCH(QStringList, qmlPropertyNames);
    QFETCH(QStringList, propertyNames);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(propertyNamesChanged()));

    QCOMPARE(object->property("properties"), QVariant(qmlPropertyNames));
    QCOMPARE(gallery.request()->propertyNames(), qmlPropertyNames);

    object->setProperty("properties", propertyNames);
    QCOMPARE(object->property("properties"), QVariant(qmlPropertyNames));
    QCOMPARE(spy.count(), 0);

    QCoreApplication::processEvents();
    QCOMPARE(gallery.request()->propertyNames(), qmlPropertyNames);
}

void tst_QDeclarativeDocumentGalleryModel::sortProperties_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QStringList>("qmlSortPropertyNames");
    QTest::addColumn<QStringList>("sortPropertyNames");

    QTest::newRow("Default -> [ +rating ]")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << QStringList()
            << (QStringList() << "+rating");

    QTest::newRow("[ +rating ] ->  [ albumArtist, albumTitle, trackNumber ]")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "sortProperties: [ \"+rating\" ]\n"
                    "}\n")
            << (QStringList() << QLatin1String("+rating"))
            << (QStringList()
                << QLatin1String("albumArtist")
                << QLatin1String("albumTitle")
                << QLatin1String("trackNumber"));

    QTest::newRow("[ albumArtist, albumTitle, trackNumber ] ->  [ albumArtist, albumTitle, trackNumber ]")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "sortProperties: [ \"albumArtist\", \"albumTitle\", \"trackNumber\" ]\n"
                    "}\n")
            << (QStringList()
                << QLatin1String("albumArtist")
                << QLatin1String("albumTitle")
                << QLatin1String("trackNumber"))
            << (QStringList()
                << QLatin1String("albumArtist")
                << QLatin1String("albumTitle")
                << QLatin1String("trackNumber"));
}

void tst_QDeclarativeDocumentGalleryModel::sortProperties()
{
    QFETCH(QByteArray, qml);
    QFETCH(QStringList, qmlSortPropertyNames);
    QFETCH(QStringList, sortPropertyNames);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(sortPropertyNamesChanged()));

    QCOMPARE(object->property("sortProperties"), QVariant(qmlSortPropertyNames));
    QCOMPARE(gallery.request()->sortPropertyNames(), qmlSortPropertyNames);

    object->setProperty("sortProperties", sortPropertyNames);
    QCOMPARE(object->property("sortProperties"), QVariant(sortPropertyNames));
    QCOMPARE(spy.count(), qmlSortPropertyNames != sortPropertyNames ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->sortPropertyNames(), sortPropertyNames);

    object->setProperty("sortProperties", qmlSortPropertyNames);
    QCOMPARE(object->property("sortProperties"), QVariant(qmlSortPropertyNames));
    QCOMPARE(spy.count(), qmlSortPropertyNames != sortPropertyNames ? 2 : 0);

    QCoreApplication::processEvents();
    QCOMPARE(gallery.request()->sortPropertyNames(), qmlSortPropertyNames);
}

void tst_QDeclarativeDocumentGalleryModel::sortPropertiesAuto()
{
    QFETCH(QByteArray, qml);
    QFETCH(QStringList, qmlSortPropertyNames);
    QFETCH(QStringList, sortPropertyNames);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(sortPropertyNamesChanged()));

    QCOMPARE(object->property("sortProperties"), QVariant(qmlSortPropertyNames));
    QCOMPARE(gallery.request()->sortPropertyNames(), qmlSortPropertyNames);

    object->setProperty("sortProperties", sortPropertyNames);
    QCOMPARE(object->property("sortProperties"), QVariant(sortPropertyNames));
    QCOMPARE(spy.count(), qmlSortPropertyNames != sortPropertyNames ? 1 : 0);

    QCoreApplication::processEvents();
    QCOMPARE(gallery.request()->sortPropertyNames(), sortPropertyNames);
}

void tst_QDeclarativeDocumentGalleryModel::autoUpdate_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<bool>("qmlAutoUpdate");
    QTest::addColumn<bool>("autoUpdate");

    QTest::newRow("Default -> true")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << false
            << true;

    QTest::newRow("true -> false")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "autoUpdate: true\n"
                    "}\n")
            << true
            << false;

    QTest::newRow("false -> true")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "autoUpdate: false\n"
                    "}\n")
            << false
            << true;

    QTest::newRow("true -> true")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "autoUpdate: true\n"
                    "}\n")
            << true
            << true;

    QTest::newRow("false -> false")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "autoUpdate: false\n"
                    "}\n")
            << false
            << false;
}

void tst_QDeclarativeDocumentGalleryModel::autoUpdate()
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

void tst_QDeclarativeDocumentGalleryModel::autoUpdateAuto()
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

    QCoreApplication::processEvents();
    QCOMPARE(gallery.request()->autoUpdate(), autoUpdate);
}

void tst_QDeclarativeDocumentGalleryModel::disableAutoUpdateFinished()
{
    const QByteArray qml(
        "import Qt 4.7\n"
        "import QtMobility.gallery 1.1\n"
        "DocumentGalleryModel {\n"
            "autoUpdate: true\n"
        "}\n");

    gallery.setStatus(QGalleryAbstractRequest::Finished);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("autoUpdate"), QVariant(true));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));

    object->setProperty("autoUpdate", false);
    QCOMPARE(object->property("autoUpdate"), QVariant(false));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));
    QCOMPARE(spy.count(), 0);
}

void tst_QDeclarativeDocumentGalleryModel::disableAutoUpdateIdle()
{
    const QByteArray qml(
        "import Qt 4.7\n"
        "import QtMobility.gallery 1.1\n"
        "DocumentGalleryModel {\n"
            "autoUpdate: true\n"
        "}\n");

    gallery.setStatus(QGalleryAbstractRequest::Idle);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("autoUpdate"), QVariant(true));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Idle));

    object->setProperty("autoUpdate", false);
    QCOMPARE(object->property("autoUpdate"), QVariant(false));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryModel::disableAutoUpdateActive()
{
    const QByteArray qml(
        "import Qt 4.7\n"
        "import QtMobility.gallery 1.1\n"
        "DocumentGalleryModel {\n"
            "autoUpdate: true\n"
        "}\n");

    gallery.setStatus(QGalleryAbstractRequest::Active);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("autoUpdate"), QVariant(true));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Active));

    object->setProperty("autoUpdate", false);
    QCOMPARE(object->property("autoUpdate"), QVariant(false));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Active));
    QCOMPARE(spy.count(), 0);

    QVERIFY(gallery.response());
    gallery.response()->finish(true);
    QCOMPARE(object->property("autoUpdate"), QVariant(false));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryModel::rootItem_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QVariant>("qmlRootItem");
    QTest::addColumn<QVariant>("rootItem");

    QTest::newRow("Default -> 34")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << QVariant()
            << QVariant(34);

    QTest::newRow("folder:///path/to/music -> folder:///path/to/images")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootItem: \"folder:///path/to/music\"\n"
                    "}\n")
            << QVariant(QLatin1String("folder:///path/to/music"))
            << QVariant(QLatin1String("folder:///path/to/images"));

    QTest::newRow("26 -> folder:///path/to/images")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootItem: 26\n"
                    "}\n")
            << QVariant(26)
            << QVariant(QLatin1String("folder:///path/to/images"));


    QTest::newRow("26 -> 26")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootItem: 26\n"
                    "}\n")
            << QVariant(26)
            << QVariant(26);
}

void tst_QDeclarativeDocumentGalleryModel::rootItem()
{
    QFETCH(QByteArray, qml);
    QFETCH(QVariant, qmlRootItem);
    QFETCH(QVariant, rootItem);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(rootItemChanged()));

    QCOMPARE(object->property("rootItem"), qmlRootItem);
    QCOMPARE(gallery.request()->rootItem(), qmlRootItem);

    object->setProperty("rootItem", rootItem);
    QCOMPARE(object->property("rootItem"), rootItem);
    QCOMPARE(spy.count(), qmlRootItem != rootItem ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->rootItem(), rootItem);
}

void tst_QDeclarativeDocumentGalleryModel::rootItemAuto()
{
    QFETCH(QByteArray, qml);
    QFETCH(QVariant, qmlRootItem);
    QFETCH(QVariant, rootItem);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(rootItemChanged()));

    QCOMPARE(object->property("rootItem"), qmlRootItem);
    QCOMPARE(gallery.request()->rootItem(), qmlRootItem);

    object->setProperty("rootItem", rootItem);
    QCOMPARE(object->property("rootItem"), rootItem);
    QCOMPARE(spy.count(), qmlRootItem != rootItem ? 1 : 0);

    QCoreApplication::processEvents();
    QCOMPARE(gallery.request()->rootItem(), rootItem);
}

void tst_QDeclarativeDocumentGalleryModel::scope_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QGalleryQueryRequest::Scope>("qmlScope");
    QTest::addColumn<QGalleryQueryRequest::Scope>("scope");

    QTest::newRow("Default -> DirectDescendants")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << QGalleryQueryRequest::AllDescendants
            << QGalleryQueryRequest::DirectDescendants;

    QTest::newRow("DirectDescendants -> AllDescendants")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "scope: DocumentGalleryModel.DirectDescendants\n"
                    "}\n")
            << QGalleryQueryRequest::DirectDescendants
            << QGalleryQueryRequest::AllDescendants;

    QTest::newRow("AllDescendants -> DirectDescendants")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "scope: DocumentGalleryModel.AllDescendants\n"
                    "}\n")
            << QGalleryQueryRequest::AllDescendants
            << QGalleryQueryRequest::DirectDescendants;


    QTest::newRow("AllDescendants -> AllDescendants")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "scope: DocumentGalleryModel.AllDescendants\n"
                    "}\n")
            << QGalleryQueryRequest::AllDescendants
            << QGalleryQueryRequest::AllDescendants;

    QTest::newRow("DirectDescendants -> DirectDescendants")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "scope: DocumentGalleryModel.DirectDescendants\n"
                    "}\n")
            << QGalleryQueryRequest::DirectDescendants
            << QGalleryQueryRequest::DirectDescendants;
}

void tst_QDeclarativeDocumentGalleryModel::scope()
{
    QFETCH(QByteArray, qml);
    QFETCH(QGalleryQueryRequest::Scope, qmlScope);
    QFETCH(QGalleryQueryRequest::Scope, scope);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(scopeChanged()));

    QCOMPARE(object->property("scope"), QVariant(int(qmlScope)));
    QCOMPARE(gallery.request()->scope(), qmlScope);

    object->setProperty("scope", scope);
    QCOMPARE(object->property("scope"), QVariant(int(scope)));
    QCOMPARE(spy.count(), qmlScope != scope ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->scope(), scope);
}

void tst_QDeclarativeDocumentGalleryModel::scopeAuto()
{
    QFETCH(QByteArray, qml);
    QFETCH(QGalleryQueryRequest::Scope, qmlScope);
    QFETCH(QGalleryQueryRequest::Scope, scope);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(scopeChanged()));

    QCOMPARE(object->property("scope"), QVariant(int(qmlScope)));
    QCOMPARE(gallery.request()->scope(), qmlScope);

    object->setProperty("scope", scope);
    QCOMPARE(object->property("scope"), QVariant(int(scope)));
    QCOMPARE(spy.count(), qmlScope != scope ? 1 : 0);

    QCoreApplication::processEvents();
    QCOMPARE(gallery.request()->scope(), scope);
}

void tst_QDeclarativeDocumentGalleryModel::offset_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<int>("qmlOffset");
    QTest::addColumn<int>("offset");

    QTest::newRow("Default -> 125")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << 0
            << 125;

    QTest::newRow("30 -> 15")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "offset: 30\n"
                    "}\n")
            << 30
            << 15;

    QTest::newRow("63 -> 63")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "offset: 63\n"
                    "}\n")
            << 63
            << 63;
}

void tst_QDeclarativeDocumentGalleryModel::offset()
{
    QFETCH(QByteArray, qml);
    QFETCH(int, qmlOffset);
    QFETCH(int, offset);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(offsetChanged()));

    QCOMPARE(object->property("offset"), QVariant(qmlOffset));
    QCOMPARE(gallery.request()->offset(), qmlOffset);

    object->setProperty("offset", offset);
    QCOMPARE(object->property("offset"), QVariant(offset));
    QCOMPARE(spy.count(), qmlOffset != offset ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->offset(), offset);
}

void tst_QDeclarativeDocumentGalleryModel::offsetAuto()
{
    QFETCH(QByteArray, qml);
    QFETCH(int, qmlOffset);
    QFETCH(int, offset);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(offsetChanged()));

    QCOMPARE(object->property("offset"), QVariant(qmlOffset));
    QCOMPARE(gallery.request()->offset(), qmlOffset);

    object->setProperty("offset", offset);
    QCOMPARE(object->property("offset"), QVariant(offset));
    QCOMPARE(spy.count(), qmlOffset != offset ? 1 : 0);

    QCoreApplication::processEvents();
    QCOMPARE(gallery.request()->offset(), offset);
}

void tst_QDeclarativeDocumentGalleryModel::limit_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<int>("qmlLimit");
    QTest::addColumn<int>("limit");

    QTest::newRow("Default -> 125")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << 0
            << 125;

    QTest::newRow("30 -> 15")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "limit: 30\n"
                    "}\n")
            << 30
            << 15;

    QTest::newRow("63 -> 63")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "limit: 63\n"
                    "}\n")
            << 63
            << 63;
}

void tst_QDeclarativeDocumentGalleryModel::limit()
{
    QFETCH(QByteArray, qml);
    QFETCH(int, qmlLimit);
    QFETCH(int, limit);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(limitChanged()));

    QCOMPARE(object->property("limit"), QVariant(qmlLimit));
    QCOMPARE(gallery.request()->limit(), qmlLimit);

    object->setProperty("limit", limit);
    QCOMPARE(object->property("limit"), QVariant(limit));
    QCOMPARE(spy.count(), qmlLimit != limit ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->limit(), limit);
}

void tst_QDeclarativeDocumentGalleryModel::limitAuto()
{
    QFETCH(QByteArray, qml);
    QFETCH(int, qmlLimit);
    QFETCH(int, limit);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(limitChanged()));

    QCOMPARE(object->property("limit"), QVariant(qmlLimit));
    QCOMPARE(gallery.request()->limit(), qmlLimit);

    object->setProperty("limit", limit);
    QCOMPARE(object->property("limit"), QVariant(limit));
    QCOMPARE(spy.count(), qmlLimit != limit ? 1 : 0);

    QCoreApplication::processEvents();
    QCOMPARE(gallery.request()->limit(), limit);
}

void tst_QDeclarativeDocumentGalleryModel::filter_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QGalleryFilter>("filter");

    QTest::newRow("Default")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << QGalleryFilter();


    QTest::newRow("width == 1920")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryEqualsFilter {\n"
                            "property: \"width\"\n"
                            "value: 1920\n"
                            "negated: false\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::width == 1920);

    QTest::newRow("width != 1920")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryEqualsFilter {\n"
                            "property: \"width\"\n"
                            "value: 1920\n"
                            "negated: true\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(!(QDocumentGallery::width == 1920));

    QTest::newRow("width < 1920")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryLessThanFilter {\n"
                            "property: \"width\"\n"
                            "value: 1920\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::width < 1920);

    QTest::newRow("width <= 1920")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryLessThanEqualsFilter {\n"
                            "property: \"width\"\n"
                            "value: 1920\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::width <= 1920);

    QTest::newRow("width > 1920")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryGreaterThanFilter {\n"
                            "property: \"width\"\n"
                            "value: 1920\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::width > 1920);

    QTest::newRow("width >= 1920")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryGreaterThanEqualsFilter {\n"
                            "property: \"width\"\n"
                            "value: 1920\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::width >= 1920);

    QTest::newRow("fileName.startsWith(p)")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryStartsWithFilter {\n"
                            "property: \"fileName\"\n"
                            "value: \"p\"\n"
                            "negated: false\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::fileName.startsWith(QLatin1String("p")));

    QTest::newRow("!fileName.startsWith(p)")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryStartsWithFilter {\n"
                            "property: \"fileName\"\n"
                            "value: \"p\"\n"
                            "negated: true\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(!QDocumentGallery::fileName.startsWith(QLatin1String("p")));

    QTest::newRow("fileName.endsWith(.jpg)")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryEndsWithFilter {\n"
                            "property: \"fileName\"\n"
                            "value: \".jpg\"\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::fileName.endsWith(QLatin1String(".jpg")));

    QTest::newRow("fileName.contains(p)")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryContainsFilter {\n"
                            "property: \"fileName\"\n"
                            "value: \"p\"\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::fileName.contains(QLatin1String("p")));

    QTest::newRow("fileName.wildcard(p*.jpg)")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryWildcardFilter {\n"
                            "property: \"fileName\"\n"
                            "value: \"p*.jpg\"\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::fileName.wildcard(QLatin1String("p*.jpg")));

    QTest::newRow("fileName.regExp(p\\d{6}\\.(jpg|jpeg))")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryEqualsFilter {\n"
                            "property: \"fileName\"\n"
                            "value: /p\\d{6}\\.(jpg|jpeg)/\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::fileName.regExp(QRegExp(
                    QLatin1String("p\\d{6}\\.(jpg|jpeg)"), Qt::CaseSensitive, QRegExp::RegExp2)));


    {
        QGalleryIntersectionFilter filter;
        filter.append(QDocumentGallery::fileName.startsWith(QLatin1String("p")));
        filter.append(QDocumentGallery::fileName.endsWith(QLatin1String(".jpg")));

        QTest::newRow("fileName.startsWith(p) && fileName.endsWith(.jpg")
                << QByteArray(
                        "import Qt 4.7\n"
                        "import QtMobility.gallery 1.1\n"
                        "DocumentGalleryModel {\n"
                            "filter: GalleryFilterIntersection {\n"
                                "GalleryStartsWithFilter {\n"
                                    "property: \"fileName\"\n"
                                    "value: \"p\"\n"
                                "}\n"
                                "GalleryEndsWithFilter {\n"
                                    "property: \"fileName\"\n"
                                    "value: \".jpg\"\n"
                                "}\n"
                            "}\n"
                        "}\n")
                << QGalleryFilter(filter);
    } {
        QGalleryUnionFilter filter;
        filter.append(QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String(".jpg"), QGalleryFilter::EndsWith));
        filter.append(QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String(".jpeg"), QGalleryFilter::EndsWith));

        QTest::newRow("fileName.endsWith(.jpg) || fileName.endsWith(.jpeg")
                << QByteArray(
                        "import Qt 4.7\n"
                        "import QtMobility.gallery 1.1\n"
                        "DocumentGalleryModel {\n"
                            "filter: GalleryFilterUnion {\n"
                                "GalleryEndsWithFilter {\n"
                                    "property: \"fileName\"\n"
                                    "value: \".jpg\"\n"
                                "}\n"
                                "GalleryEndsWithFilter {\n"
                                    "property: \"fileName\"\n"
                                    "value: \".jpeg\"\n"
                                "}\n"
                            "}\n"
                        "}\n")
                << QGalleryFilter(filter);
    } {
        QGalleryUnionFilter unionFilter;
        unionFilter.append(QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String(".jpg"), QGalleryFilter::EndsWith));
        unionFilter.append(QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String(".jpeg"), QGalleryFilter::EndsWith));

        QGalleryIntersectionFilter filter;
        filter.append(QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String("p"), QGalleryFilter::StartsWith));
        filter.append(unionFilter);

        QTest::newRow("fileName.startsWith(p) && (fileName.endsWith(.jpg) || fileName.endsWith(.jpeg)")
                << QByteArray(
                        "import Qt 4.7\n"
                        "import QtMobility.gallery 1.1\n"
                        "DocumentGalleryModel {\n"
                            "filter: GalleryFilterIntersection {\n"
                                "GalleryStartsWithFilter {\n"
                                    "property: \"fileName\"\n"
                                    "value: \"p\"\n"
                                "}\n"
                                "GalleryFilterUnion {\n"
                                    "GalleryEndsWithFilter {\n"
                                        "property: \"fileName\"\n"
                                        "value: \".jpg\"\n"
                                    "}\n"
                                    "GalleryEndsWithFilter {\n"
                                        "property: \"fileName\"\n"
                                        "value: \".jpeg\"\n"
                                    "}\n"
                                "}\n"
                            "}\n"
                        "}\n")
                << QGalleryFilter(filter);
    } {
        QGalleryUnionFilter filter;
        {
            QGalleryIntersectionFilter intersectionFilter;
            intersectionFilter.append(QDocumentGallery::fileName.startsWith(QLatin1String("p")));
            intersectionFilter.append(QDocumentGallery::fileName.endsWith(QLatin1String(".jpg")));
            filter.append(intersectionFilter);
        } {
            QGalleryIntersectionFilter intersectionFilter;
            intersectionFilter.append(QDocumentGallery::fileName.startsWith(QLatin1String("p")));
            intersectionFilter.append(QDocumentGallery::fileName.endsWith(QLatin1String(".jpeg")));
            filter.append(intersectionFilter);
        }

        QTest::newRow("(fileName.startsWith(p) && fileName.endsWith(.jpg) || (fileName.startsWith(p) && fileName.endsWith(.jpeg)")
                << QByteArray(
                        "import Qt 4.7\n"
                        "import QtMobility.gallery 1.1\n"
                        "DocumentGalleryModel {\n"
                            "filter: GalleryFilterUnion {\n"
                                "GalleryFilterIntersection {\n"
                                    "GalleryStartsWithFilter {\n"
                                        "property: \"fileName\"\n"
                                        "value: \"p\"\n"
                                    "}\n"
                                    "GalleryEndsWithFilter {\n"
                                        "property: \"fileName\"\n"
                                        "value: \".jpg\"\n"
                                    "}\n"
                                "}\n"
                                "GalleryFilterIntersection {\n"
                                    "GalleryStartsWithFilter {\n"
                                        "property: \"fileName\"\n"
                                        "value: \"p\"\n"
                                    "}\n"
                                    "GalleryEndsWithFilter {\n"
                                        "property: \"fileName\"\n"
                                        "value: \".jpeg\"\n"
                                    "}\n"
                                "}\n"
                            "}\n"
                        "}\n")
                << QGalleryFilter(filter);
    } {
        QGalleryIntersectionFilter filter;
        filter.append(QDocumentGallery::fileName.startsWith(QLatin1String("p")));
        filter.append(QDocumentGallery::fileName.endsWith(QLatin1String(".jpg")));

        QTest::newRow("fileName.startsWith(p) && (fileName.endsWith(.jpg)")
                << QByteArray(
                        "import Qt 4.7\n"
                        "import QtMobility.gallery 1.1\n"
                        "DocumentGalleryModel {\n"
                            "filter: GalleryFilterIntersection {\n"
                                "GalleryStartsWithFilter {\n"
                                    "property: \"fileName\"\n"
                                    "value: \"p\"\n"
                                "}\n"
                                "GalleryFilterIntersection {\n"
                                    "GalleryEndsWithFilter {\n"
                                        "property: \"fileName\"\n"
                                        "value: \".jpg\"\n"
                                    "}\n"
                                "}\n"
                            "}\n"
                        "}\n")
                << QGalleryFilter(filter);
    } {
        QGalleryUnionFilter filter;
        filter.append(QDocumentGallery::fileName.endsWith(QLatin1String(".jpg")));
        filter.append(QDocumentGallery::fileName.endsWith(QLatin1String(".jpeg")));

        QTest::newRow("fileName.endsWith(.jpg) || (fileName.endsWith(.jpeg)")
                << QByteArray(
                        "import Qt 4.7\n"
                        "import QtMobility.gallery 1.1\n"
                        "DocumentGalleryModel {\n"
                            "filter: GalleryFilterUnion {\n"
                                "GalleryEndsWithFilter {\n"
                                    "property: \"fileName\"\n"
                                    "value: \".jpg\"\n"
                                "}\n"
                                "GalleryFilterUnion {\n"
                                    "GalleryEndsWithFilter {\n"
                                        "property: \"fileName\"\n"
                                        "value: \".jpeg\"\n"
                                    "}\n"
                                "}\n"
                            "}\n"
                        "}\n")
                << QGalleryFilter(filter);
    }
}

void tst_QDeclarativeDocumentGalleryModel::filter()
{
    QFETCH(QByteArray, qml);
    QFETCH(QGalleryFilter, filter);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QCOMPARE(gallery.request()->filter(), filter);

    if (filter.isValid()) {
        QVERIFY(object->property("filter").value<QDeclarativeGalleryFilterBase *>());
    } else {
        QVERIFY(!object->property("filter").value<QDeclarativeGalleryFilterBase *>());
    }
}

void tst_QDeclarativeDocumentGalleryModel::changeFilter_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QGalleryFilter>("qmlFilter");
    QTest::addColumn<QByteArray>("filterQml");
    QTest::addColumn<QGalleryFilter>("filter");

    QTest::newRow("undefined -> width == 1920")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << QGalleryFilter()
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryEqualsFilter {\n"
                        "property: \"width\"\n"
                        "value: 1920\n"
                        "negated: false\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::width == 1920);

    QTest::newRow("width == 1920 -> width == 1920")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryEqualsFilter {\n"
                            "property: \"width\"\n"
                            "value: 1920\n"
                            "negated: false\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::width == 1920)
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryEqualsFilter {\n"
                        "property: \"width\"\n"
                        "value: 1920\n"
                        "negated: false\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::width == 1920);

    QTest::newRow("width == 1920 -> width != 1920")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryEqualsFilter {\n"
                            "property: \"width\"\n"
                            "value: 1920\n"
                            "negated: false\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::width == 1920)
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryEqualsFilter {\n"
                        "property: \"width\"\n"
                        "value: 1920\n"
                        "negated: true\n"
                    "}\n")
            << QGalleryFilter(!(QDocumentGallery::width == 1920));

    QTest::newRow("width == 1920 -> undefined")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryEqualsFilter {\n"
                            "property: \"width\"\n"
                            "value: 1920\n"
                            "negated: false\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QDocumentGallery::width == 1920)
            << QByteArray()
            << QGalleryFilter();
}

void tst_QDeclarativeDocumentGalleryModel::changeFilter()
{
    QFETCH(QByteArray, qml);
    QFETCH(QGalleryFilter, qmlFilter);
    QFETCH(QByteArray, filterQml);
    QFETCH(QGalleryFilter, filter);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QCOMPARE(gallery.request()->filter(), qmlFilter);

    QSignalSpy spy(object.data(), SIGNAL(filterChanged()));

    if (!filterQml.isEmpty()) {
        QDeclarativeComponent filterComponent(&engine);
        filterComponent.setData(filterQml, QUrl());

        QScopedPointer<QObject> filterObject(filterComponent.create());
        QVERIFY(filterObject);

        QDeclarativeGalleryFilterBase *declarativeFilter
                = static_cast<QDeclarativeGalleryFilterBase *>(filterObject.data());
        void *argv[] = { &declarativeFilter };

        int propertyIndex = object->metaObject()->indexOfProperty("filter");
        QVERIFY(propertyIndex != -1);
        object->qt_metacall(QMetaObject::WriteProperty, propertyIndex, argv);
        QCOMPARE(spy.count(), 1);

        QMetaObject::invokeMethod(object.data(), "reload");
    } else {
        QDeclarativeGalleryFilterBase *declarativeFilter = 0;
        void *argv[] = { &declarativeFilter };

        int propertyIndex = object->metaObject()->indexOfProperty("filter");
        QVERIFY(propertyIndex != -1);
        object->qt_metacall(QMetaObject::WriteProperty, propertyIndex, argv);
        QCOMPARE(spy.count(), 1);

        QMetaObject::invokeMethod(object.data(), "reload");
    }
    QVERIFY(gallery.request());
    QCOMPARE(gallery.request()->filter(), filter);
}

void tst_QDeclarativeDocumentGalleryModel::changeFilterAuto()
{
    QFETCH(QByteArray, qml);
    QFETCH(QGalleryFilter, qmlFilter);
    QFETCH(QByteArray, filterQml);
    QFETCH(QGalleryFilter, filter);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QCOMPARE(gallery.request()->filter(), qmlFilter);

    QSignalSpy spy(object.data(), SIGNAL(filterChanged()));

    if (!filterQml.isEmpty()) {
        QDeclarativeComponent filterComponent(&engine);
        filterComponent.setData(filterQml, QUrl());

        QScopedPointer<QObject> filterObject(filterComponent.create());
        QVERIFY(filterObject);

        QDeclarativeGalleryFilterBase *declarativeFilter
                = static_cast<QDeclarativeGalleryFilterBase *>(filterObject.data());
        void *argv[] = { &declarativeFilter };

        int propertyIndex = object->metaObject()->indexOfProperty("filter");
        QVERIFY(propertyIndex != -1);
        object->qt_metacall(QMetaObject::WriteProperty, propertyIndex, argv);
        QCOMPARE(spy.count(), 1);

        QCoreApplication::processEvents();
    } else {
        QDeclarativeGalleryFilterBase *declarativeFilter = 0;
        void *argv[] = { &declarativeFilter };

        int propertyIndex = object->metaObject()->indexOfProperty("filter");
        QVERIFY(propertyIndex != -1);
        object->qt_metacall(QMetaObject::WriteProperty, propertyIndex, argv);
        QCOMPARE(spy.count(), 1);

        QCoreApplication::processEvents();
    }
    QVERIFY(gallery.request());
    QCOMPARE(gallery.request()->filter(), filter);
}

void tst_QDeclarativeDocumentGalleryModel::filterProperties_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QByteArray>("property");
    QTest::addColumn<QByteArray>("changeSignal");
    QTest::addColumn<QVariant>("qmlValue");
    QTest::addColumn<QVariant>("value");

    QTest::newRow("Equals.property: Default -> title")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryEqualsFilter {}\n")
            << QByteArray("property")
            << QByteArray(SIGNAL(propertyNameChanged()))
            << QVariant(QVariant::String)
            << QVariant(QLatin1String("title"));

    QTest::newRow("Equals.property: title -> title")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryEqualsFilter { property: \"title\" }\n")
            << QByteArray("property")
            << QByteArray(SIGNAL(propertyNameChanged()))
            << QVariant(QLatin1String("title"))
            << QVariant(QLatin1String("title"));

    QTest::newRow("Equals.property: title -> duration")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryEqualsFilter { property: \"title\" }\n")
            << QByteArray("property")
            << QByteArray(SIGNAL(propertyNameChanged()))
            << QVariant(QLatin1String("title"))
            << QVariant(QLatin1String("duration"));

    QTest::newRow("Equals.value: Default -> Interlude")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryEqualsFilter {}\n")
            << QByteArray("value")
            << QByteArray(SIGNAL(valueChanged()))
            << QVariant()
            << QVariant(QLatin1String("Interlude"));

    QTest::newRow("Equals.value: Interlude -> Interlude")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryEqualsFilter { value: \"Interlude\" }\n")
            << QByteArray("value")
            << QByteArray(SIGNAL(valueChanged()))
            << QVariant(QLatin1String("Interlude"))
            << QVariant(QLatin1String("Interlude"));

    QTest::newRow("Equals.value: Interlude -> 45000")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryEqualsFilter { value: \"Interlude\" }\n")
            << QByteArray("value")
            << QByteArray(SIGNAL(valueChanged()))
            << QVariant(QLatin1String("Interlude"))
            << QVariant(45000);

    QTest::newRow("Equals.negated: Default -> true")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryEqualsFilter {}\n")
            << QByteArray("negated")
            << QByteArray(SIGNAL(negatedChanged()))
            << QVariant(false)
            << QVariant(true);

    QTest::newRow("Equals.negated: true -> true")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryEqualsFilter { negated: true }\n")
            << QByteArray("negated")
            << QByteArray(SIGNAL(negatedChanged()))
            << QVariant(true)
            << QVariant(true);

    QTest::newRow("Equals.negated: true -> false")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryEqualsFilter { negated: true }\n")
            << QByteArray("negated")
            << QByteArray(SIGNAL(negatedChanged()))
            << QVariant(true)
            << QVariant(false);

    QTest::newRow("Wildcard.property: Default -> title")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryWildcardFilter {}\n")
            << QByteArray("property")
            << QByteArray(SIGNAL(propertyNameChanged()))
            << QVariant(QVariant::String)
            << QVariant(QLatin1String("title"));

    QTest::newRow("Wildcard.property: title -> title")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryWildcardFilter { property: \"title\" }\n")
            << QByteArray("property")
            << QByteArray(SIGNAL(propertyNameChanged()))
            << QVariant(QLatin1String("title"))
            << QVariant(QLatin1String("title"));

    QTest::newRow("Wildcard.property: title -> duration")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryWildcardFilter { property: \"title\" }\n")
            << QByteArray("property")
            << QByteArray(SIGNAL(propertyNameChanged()))
            << QVariant(QLatin1String("title"))
            << QVariant(QLatin1String("duration"));

    QTest::newRow("Wildcard.value: Default -> *Interlude")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryWildcardFilter {}\n")
            << QByteArray("value")
            << QByteArray(SIGNAL(valueChanged()))
            << QVariant(QVariant::String)
            << QVariant(QLatin1String("*Interlude"));

    QTest::newRow("Wildcard.value: *Interlude -> *Interlude")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryWildcardFilter { value: \"*Interlude\" }\n")
            << QByteArray("value")
            << QByteArray(SIGNAL(valueChanged()))
            << QVariant(QLatin1String("*Interlude"))
            << QVariant(QLatin1String("*Interlude"));

    QTest::newRow("Wildcard.value: *Interlude -> interlude.*")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryWildcardFilter { value: \"*Interlude\" }\n")
            << QByteArray("value")
            << QByteArray(SIGNAL(valueChanged()))
            << QVariant(QLatin1String("*Interlude"))
            << QVariant(QLatin1String("interlude.*"));

    QTest::newRow("Wildcard.negated: Default -> true")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryWildcardFilter {}\n")
            << QByteArray("negated")
            << QByteArray(SIGNAL(negatedChanged()))
            << QVariant(false)
            << QVariant(true);

    QTest::newRow("Wildcard.negated: false -> false")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryWildcardFilter { negated: true }\n")
            << QByteArray("negated")
            << QByteArray(SIGNAL(negatedChanged()))
            << QVariant(true)
            << QVariant(true);

    QTest::newRow("Wildcard.negated: false -> true")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "GalleryWildcardFilter { negated: true }\n")
            << QByteArray("negated")
            << QByteArray(SIGNAL(negatedChanged()))
            << QVariant(true)
            << QVariant(false);
}

void tst_QDeclarativeDocumentGalleryModel::filterProperties()
{
    QFETCH(QByteArray, qml);
    QFETCH(QByteArray, property);
    QFETCH(QByteArray, changeSignal);
    QFETCH(QVariant, qmlValue);
    QFETCH(QVariant, value);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);

    QSignalSpy spy(object.data(), changeSignal.constData());

    QCOMPARE(object->property(property.constData()), qmlValue);

    QCOMPARE(object->setProperty(property.constData(), value), true);
    QCOMPARE(object->property(property.constData()), value);
    QCOMPARE(spy.count(), qmlValue != value ? 1 : 0);
}

void tst_QDeclarativeDocumentGalleryModel::groupFilter_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QGalleryFilter::Type>("type");

    QTest::newRow("Intersection")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryFilterIntersection {}\n"
                    "}\n")
            << QGalleryFilter::Intersection;

    QTest::newRow("Union")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryFilterUnion {}\n"
                    "}\n")
            << QGalleryFilter::Union;
}

void tst_QDeclarativeDocumentGalleryModel::groupFilter()
{
    QFETCH(QByteArray, qml);
    QFETCH(QGalleryFilter::Type, type);

    QGalleryFilter filter;
    QList<QGalleryFilter> actualFilters;
    QList<QGalleryFilter> expectedFilters;

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);

    QObject *declarativeFilter = object->property("filter").value<QDeclarativeGalleryFilterBase *>();
    QVERIFY(declarativeFilter);

    QSignalSpy spy(declarativeFilter, SIGNAL(filterChanged()));

    QDeclarativeListProperty<QDeclarativeGalleryFilterBase> filterList
            = declarativeFilter->property("filters")
                    .value<QDeclarativeListProperty<QDeclarativeGalleryFilterBase> >();

    QVERIFY(filterList.append);
    QVERIFY(filterList.count);
    QVERIFY(filterList.at);
    QVERIFY(filterList.clear);

    QCOMPARE(filterList.count(&filterList), 0);

    {
        const QByteArray filterQml(
                "import Qt 4.7\n"
                "import QtMobility.gallery 1.1\n"
                "GalleryEqualsFilter { property: \"fileName\"; value: \"filename.ext\" }\n");

        QDeclarativeComponent filterComponent(&engine);
        filterComponent.setData(filterQml, QUrl());

        QScopedPointer<QObject> filterObject(filterComponent.create());
        QVERIFY(filterObject);

        filterList.append(
                &filterList, static_cast<QDeclarativeGalleryFilterBase *>(filterObject.take()));
    }
    QCOMPARE(filterList.count(&filterList), 1);
    QVERIFY(filterList.at(&filterList, 0));
    QCOMPARE(filterList.at(&filterList, 0)->filter(),
             QGalleryFilter(QDocumentGallery::fileName == QLatin1String("filename.ext")));
    QCOMPARE(spy.count(), 1);

    QCoreApplication::processEvents();
    QVERIFY(gallery.request());

    filter = gallery.request()->filter();
    QCOMPARE(filter.type(), type);

    actualFilters = type == QGalleryFilter::Intersection
            ? filter.toIntersectionFilter().filters()
            : filter.toUnionFilter().filters();
    expectedFilters.append(QDocumentGallery::fileName == QLatin1String("filename.ext"));
    QCOMPARE(actualFilters, expectedFilters);

    {
        const QByteArray filterQml(
                "import Qt 4.7\n"
                "import QtMobility.gallery 1.1\n"
                "GalleryLessThanFilter { property: \"trackNumber\"; value: 12 }\n");

        QDeclarativeComponent filterComponent(&engine);
        filterComponent.setData(filterQml, QUrl());

        QScopedPointer<QObject> filterObject(filterComponent.create());
        QVERIFY(filterObject);

        filterList.append(
                &filterList, static_cast<QDeclarativeGalleryFilterBase *>(filterObject.take()));
    }
    QCOMPARE(filterList.count(&filterList), 2);
    QVERIFY(filterList.at(&filterList, 1));
    QCOMPARE(filterList.at(&filterList, 1)->filter(),
             QGalleryFilter(QDocumentGallery::trackNumber < 12));
    QCOMPARE(spy.count(), 2);

    QCoreApplication::processEvents();
    QVERIFY(gallery.request());

    filter = gallery.request()->filter();
    QCOMPARE(filter.type(), type);

    actualFilters = type == QGalleryFilter::Intersection
            ? filter.toIntersectionFilter().filters()
            : filter.toUnionFilter().filters();
    expectedFilters.append(QDocumentGallery::trackNumber < 12);
    QCOMPARE(actualFilters, expectedFilters);

    filterList.clear(&filterList);
    QCOMPARE(filterList.count(&filterList), 0);
    QCOMPARE(spy.count(), 3);

    QCoreApplication::processEvents();
    QVERIFY(gallery.request());

    filter = gallery.request()->filter();
    QCOMPARE(filter.type(), type);

    actualFilters = type == QGalleryFilter::Intersection
            ? filter.toIntersectionFilter().filters()
            : filter.toUnionFilter().filters();
    expectedFilters.clear();
    QCOMPARE(actualFilters, expectedFilters);
}

void tst_QDeclarativeDocumentGalleryModel::progress_data()
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

void tst_QDeclarativeDocumentGalleryModel::progress()
{
    QFETCH(int, currentProgress);
    QFETCH(int, maximumProgress);
    QFETCH(qreal, normalizedProgress);

    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

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

void tst_QDeclarativeDocumentGalleryModel::roleNames()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(object.data());
    QVERIFY(model);

    QHash<int, QByteArray> roleNames = model->roleNames();

    QCOMPARE(roleNames.value(QDeclarativeGalleryQueryModel::ItemId), QByteArray("itemId"));
    QCOMPARE(roleNames.value(QDeclarativeGalleryQueryModel::ItemType), QByteArray("itemType"));

    QCOMPARE(roleNames.key("fileName", -1), gallery.response()->propertyKey(QLatin1String("fileName")) + 2);
    QCOMPARE(roleNames.key("title", -1), gallery.response()->propertyKey(QLatin1String("title")) + 2);
    QCOMPARE(roleNames.key("turtle", -1), -1);
}

void tst_QDeclarativeDocumentGalleryModel::indexes()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    gallery.addRow();
    gallery.addRow();

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(object.data());
    QVERIFY(model);

    QCOMPARE(model->rowCount(), 2);
    QCOMPARE(model->columnCount(), 1);

    QCOMPARE(model->index(-1, -1).isValid(), false);
    QCOMPARE(model->index(-1,  0).isValid(), false);
    QCOMPARE(model->index(-1,  0).isValid(), false);
    QCOMPARE(model->index(-1,  1).isValid(), false);

    QCOMPARE(model->index( 0, -1).isValid(), false);
    QCOMPARE(model->index( 0,  0).isValid(), true);
    QCOMPARE(model->index( 0,  0).isValid(), true);
    QCOMPARE(model->index( 0,  1).isValid(), false);

    QCOMPARE(model->index( 1, -1).isValid(), false);
    QCOMPARE(model->index( 1,  0).isValid(), true);
    QCOMPARE(model->index( 1,  0).isValid(), true);
    QCOMPARE(model->index( 1,  1).isValid(), false);

    QCOMPARE(model->index( 2, -1).isValid(), false);
    QCOMPARE(model->index( 2,  0).isValid(), false);
    QCOMPARE(model->index( 2,  0).isValid(), false);
    QCOMPARE(model->index( 2,  1).isValid(), false);

    QModelIndex parent = model->index(0, 0);

    QCOMPARE(model->rowCount(parent), 0);
    QCOMPARE(model->columnCount(parent), 0);

    QCOMPARE(model->index(-1, -1, parent).isValid(), false);
    QCOMPARE(model->index(-1,  0, parent).isValid(), false);
    QCOMPARE(model->index(-1,  0, parent).isValid(), false);
    QCOMPARE(model->index(-1,  1, parent).isValid(), false);

    QCOMPARE(model->index( 0, -1, parent).isValid(), false);
    QCOMPARE(model->index( 0,  0, parent).isValid(), false);
    QCOMPARE(model->index( 0,  0, parent).isValid(), false);
    QCOMPARE(model->index( 0,  1, parent).isValid(), false);

    QCOMPARE(model->index( 1, -1, parent).isValid(), false);
    QCOMPARE(model->index( 1,  0, parent).isValid(), false);
    QCOMPARE(model->index( 1,  0, parent).isValid(), false);
    QCOMPARE(model->index( 1,  1, parent).isValid(), false);

    QCOMPARE(model->index( 2, -1, parent).isValid(), false);
    QCOMPARE(model->index( 2,  0, parent).isValid(), false);
    QCOMPARE(model->index( 2,  0, parent).isValid(), false);
    QCOMPARE(model->index( 2,  1, parent).isValid(), false);

}

void tst_QDeclarativeDocumentGalleryModel::data_data()
{
    QTest::addColumn<int>("index");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QVariant>("itemId");
    QTest::addColumn<QVariant>("itemType");
    QTest::addColumn<QVariant>("fileName");
    QTest::addColumn<QVariant>("title");

    QTest::newRow("-1")
            << -1
            << false
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant();

    QTest::newRow("0")
            << 0
            << true
            << QVariant(3)
            << QVariant::fromValue(QDeclarativeDocumentGallery::Audio)
            << QVariant(QLatin1String("interlude.mp3"))
            << QVariant(QLatin1String("Interlude"));

    QTest::newRow("1")
            << 1
            << true
            << QVariant(45)
            << QVariant::fromValue(QDeclarativeDocumentGallery::Audio)
            << QVariant(QLatin1String("beep.wav"))
            << QVariant(QVariant::String);

    QTest::newRow("2")
            << 2
            << false
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant();
}

void tst_QDeclarativeDocumentGalleryModel::data()
{
    QFETCH(int, index);
    QFETCH(bool, isValid);
    QFETCH(QVariant, itemId);
    QFETCH(QVariant, itemType);
    QFETCH(QVariant, fileName);
    QFETCH(QVariant, title);

    populateGallery();

    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(object.data());
    QVERIFY(model);

    const int itemIdRole = QDeclarativeGalleryQueryModel::ItemId;
    const int itemTypeRole = QDeclarativeGalleryQueryModel::ItemType;
    const int fileNameRole = model->roleNames().key("fileName");
    const int titleRole = model->roleNames().key("title");

    QCOMPARE(model->rowCount(), 2);
    QCOMPARE(model->columnCount(), 1);

    QModelIndex modelIndex = model->index(index, 0);
    QCOMPARE(modelIndex.isValid(), isValid);

    QCOMPARE(model->data(modelIndex, itemIdRole), itemId);
    QCOMPARE(model->data(modelIndex, itemTypeRole).toInt(), itemType.toInt());
    QCOMPARE(model->data(modelIndex, fileNameRole), fileName);
    QCOMPARE(model->data(modelIndex, titleRole), title);
}

void tst_QDeclarativeDocumentGalleryModel::setData_data()
{
    QTest::addColumn<int>("index");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QVariant>("itemId");
    QTest::addColumn<QVariant>("newItemId");
    QTest::addColumn<QVariant>("itemType");
    QTest::addColumn<QVariant>("newItemType");
    QTest::addColumn<QVariant>("fileName");
    QTest::addColumn<QVariant>("newFileName");
    QTest::addColumn<QVariant>("title");
    QTest::addColumn<QVariant>("newTitle");

    QTest::newRow("-1")
            << -1
            << false
            << QVariant()
            << QVariant(9)
            << QVariant()
            << QVariant::fromValue(QDeclarativeDocumentGallery::File)
            << QVariant()
            << QVariant(QLatin1String("a.file"))
            << QVariant()
            << QVariant(QLatin1String("Something"));

    QTest::newRow("0")
            << 0
            << true
            << QVariant(3)
            << QVariant(76)
            << QVariant::fromValue(QDeclarativeDocumentGallery::Audio)
            << QVariant::fromValue(QDeclarativeDocumentGallery::Video)
            << QVariant(QLatin1String("interlude.mp3"))
            << QVariant(QLatin1String("03-interlude.mp3"))
            << QVariant(QLatin1String("Interlude"))
            << QVariant(QLatin1String("An Interlude"));

    QTest::newRow("1")
            << 1
            << true
            << QVariant(45)
            << QVariant(21)
            << QVariant::fromValue(QDeclarativeDocumentGallery::Audio)
            << QVariant::fromValue(QDeclarativeDocumentGallery::Image)
            << QVariant(QLatin1String("beep.wav"))
            << QVariant(QLatin1String("beep.mp3"))
            << QVariant(QVariant::String)
            << QVariant(QLatin1String("Beep Sound"));

    QTest::newRow("2")
            << 2
            << false
            << QVariant()
            << QVariant(9)
            << QVariant()
            << QVariant::fromValue(QDeclarativeDocumentGallery::File)
            << QVariant()
            << QVariant(QLatin1String("a.file"))
            << QVariant()
            << QVariant(QLatin1String("Something"));
}

void tst_QDeclarativeDocumentGalleryModel::setData()
{
    QFETCH(int, index);
    QFETCH(bool, isValid);
    QFETCH(QVariant, itemId);
    QFETCH(QVariant, newItemId);
    QFETCH(QVariant, itemType);
    QFETCH(QVariant, newItemType);
    QFETCH(QVariant, fileName);
    QFETCH(QVariant, newFileName);
    QFETCH(QVariant, title);
    QFETCH(QVariant, newTitle);

    populateGallery();

    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(object.data());
    QVERIFY(model);

    QSignalSpy spy(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

    const int itemIdRole = QDeclarativeGalleryQueryModel::ItemId;
    const int itemTypeRole = QDeclarativeGalleryQueryModel::ItemType;
    const int fileNameRole = model->roleNames().key("fileName");
    const int titleRole = model->roleNames().key("title");

    QCOMPARE(model->rowCount(), 2);
    QCOMPARE(model->columnCount(), 1);

    QModelIndex modelIndex = model->index(index, 0);
    QCOMPARE(modelIndex.isValid(), isValid);

    QCOMPARE(model->setData(modelIndex, newTitle, titleRole), isValid);
    QCOMPARE(model->data(modelIndex, titleRole), isValid ? newTitle : title);
    QCOMPARE(spy.count(), isValid ? 1 : 0);

    QCOMPARE(model->setData(modelIndex, newItemId, itemIdRole), false);
    QCOMPARE(model->data(modelIndex, itemIdRole), itemId);
    QCOMPARE(spy.count(), isValid ? 1 : 0);

    QCOMPARE(model->setData(modelIndex, newItemType, itemTypeRole), false);
    QCOMPARE(model->data(modelIndex, itemTypeRole).toInt(), itemType.toInt());
    QCOMPARE(spy.count(), isValid ? 1 : 0);

    QCOMPARE(model->setData(modelIndex, newFileName, fileNameRole), false);
    QCOMPARE(model->data(modelIndex, fileNameRole), fileName);
    QCOMPARE(spy.count(), isValid ? 1 : 0);

}

#define QT_GALLERY_MODEL_CALL_PROPERTY(index, property) \
    QMetaObject::invokeMethod( \
            object.data(), \
            "property", \
            Q_RETURN_ARG(QVariant, value), \
            Q_ARG(int, index), \
            Q_ARG(QString, QLatin1String(property)))

void tst_QDeclarativeDocumentGalleryModel::property()
{
    QFETCH(int, index);
    QFETCH(QVariant, itemId);
    QFETCH(QVariant, itemType);
    QFETCH(QVariant, fileName);
    QFETCH(QVariant, title);

    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    populateGallery();

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QCOMPARE(object->property("count"), QVariant(2));

    QVariant value;

    QCOMPARE(QT_GALLERY_MODEL_CALL_PROPERTY(index, "itemId"), true);
    QCOMPARE(value, itemId);
    QCOMPARE(QT_GALLERY_MODEL_CALL_PROPERTY(index, "itemType"), true);
    QCOMPARE(value.toInt(), itemType.toInt());
    QCOMPARE(QT_GALLERY_MODEL_CALL_PROPERTY(index, "fileName"), true);
    QCOMPARE(value, fileName);
    QCOMPARE(QT_GALLERY_MODEL_CALL_PROPERTY(index, "title"), true);
    QCOMPARE(value, title);
    QCOMPARE(QT_GALLERY_MODEL_CALL_PROPERTY(index, "turtle"), true);
    QCOMPARE(value, QVariant());
}

#define QT_GALLERY_MODEL_CALL_SET_PROPERTY(index, property, value) \
    QMetaObject::invokeMethod( \
            object.data(), \
            "setProperty", \
            Q_ARG(int, index), \
            Q_ARG(QString, QLatin1String(property)), \
            Q_ARG(QVariant, QVariant(value)))

void tst_QDeclarativeDocumentGalleryModel::setProperty()
{
    QFETCH(int, index);
    QFETCH(bool, isValid);
    QFETCH(QVariant, itemId);
    QFETCH(QVariant, newItemId);
    QFETCH(QVariant, itemType);
    QFETCH(QVariant, newItemType);
    QFETCH(QVariant, fileName);
    QFETCH(QVariant, newFileName);
    QFETCH(QVariant, title);
    QFETCH(QVariant, newTitle);

    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    populateGallery();

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(dataChanged(QModelIndex,QModelIndex)));

    QCOMPARE(object->property("count"), QVariant(2));

    QVariant value;

    QCOMPARE(QT_GALLERY_MODEL_CALL_SET_PROPERTY(index, "itemId", newItemId), true);
    QCOMPARE(QT_GALLERY_MODEL_CALL_PROPERTY(index, "itemId"), true);
    QCOMPARE(value, itemId);
    QCOMPARE(spy.count(), 0);

    QCOMPARE(QT_GALLERY_MODEL_CALL_SET_PROPERTY(index, "itemType", newItemType), true);
    QCOMPARE(QT_GALLERY_MODEL_CALL_PROPERTY(index, "itemType"), true);
    QCOMPARE(value.toInt(), itemType.toInt());
    QCOMPARE(spy.count(), 0);

    QCOMPARE(QT_GALLERY_MODEL_CALL_SET_PROPERTY(index, "fileName", newFileName), true);
    QCOMPARE(QT_GALLERY_MODEL_CALL_PROPERTY(index, "fileName"), true);
    QCOMPARE(value, fileName);
    QCOMPARE(spy.count(), 0);

    QCOMPARE(QT_GALLERY_MODEL_CALL_SET_PROPERTY(index, "title", newTitle), true);
    QCOMPARE(QT_GALLERY_MODEL_CALL_PROPERTY(index, "title"), true);
    QCOMPARE(value, isValid ? newTitle : title);
    QCOMPARE(spy.count(), isValid ? 1 : 0);

    QCOMPARE(QT_GALLERY_MODEL_CALL_SET_PROPERTY(index, "turtle", QLatin1String("It's a turtle")), true);
    QCOMPARE(QT_GALLERY_MODEL_CALL_PROPERTY(index, "turtle"), true);
    QCOMPARE(value, QVariant());
    QCOMPARE(spy.count(), isValid ? 1 : 0);
}

void tst_QDeclarativeDocumentGalleryModel::get()
{
    QFETCH(int, index);
    QFETCH(QVariant, itemId);
    QFETCH(QVariant, itemType);
    QFETCH(QVariant, fileName);
    QFETCH(QVariant, title);

    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    populateGallery();

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QCOMPARE(object->property("count"), QVariant(2));

    QScriptEngine scriptEngine;
    QScriptValue values;
    QVERIFY(QMetaObject::invokeMethod(
            object.data(),
            "get",
            Q_RETURN_ARG(QScriptValue, values),
            Q_ARG(QScriptValue, QScriptValue(&scriptEngine, index))));

    QCOMPARE(values.property(QLatin1String("itemId")).toVariant(), itemId);
    QCOMPARE(values.property(QLatin1String("itemType")).toInt32(), itemType.toInt());
    QCOMPARE(values.property(QLatin1String("fileName")).toVariant(), fileName);
    QCOMPARE(values.property(QLatin1String("title")).toVariant(), title);
    QCOMPARE(values.property(QLatin1String("turtle")).toVariant(), QVariant());
}

#define QT_GALLERY_MODEL_CALL_SET(index) \
    QMetaObject::invokeMethod( \
            object.data(), \
            "setProperty", \
            Q_ARG(int, index), \
            Q_ARG(QString, QLatin1String(property)), \
            Q_ARG(QVariant, QVariant(value)))

void tst_QDeclarativeDocumentGalleryModel::set()
{
    QFETCH(int, index);
    QFETCH(bool, isValid);
    QFETCH(QVariant, itemId);
    QFETCH(QVariant, newItemId);
    QFETCH(QVariant, itemType);
    QFETCH(QVariant, newItemType);
    QFETCH(QVariant, fileName);
    QFETCH(QVariant, newFileName);
    QFETCH(QVariant, title);
    QFETCH(QVariant, newTitle);

    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    populateGallery();

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(dataChanged(QModelIndex,QModelIndex)));

    QCOMPARE(object->property("count"), QVariant(2));

    QScriptEngine scriptEngine;
    QScriptValue values = scriptEngine.newObject();

    values.setProperty(QLatin1String("itemId"), qScriptValueFromValue(&scriptEngine, newItemId));
    values.setProperty(QLatin1String("itemUrl"), qScriptValueFromValue(&scriptEngine, newItemType));
    values.setProperty(QLatin1String("fileName"), qScriptValueFromValue(&scriptEngine, newFileName));
    values.setProperty(QLatin1String("title"), qScriptValueFromValue(&scriptEngine, newTitle));
    values.setProperty(
            QLatin1String("turtle"), QScriptValue(&scriptEngine, QLatin1String("It's a turtle")));

    QVERIFY(QMetaObject::invokeMethod(
            object.data(), "set", Q_ARG(int, index), Q_ARG(QScriptValue, values)));

    QVERIFY(QMetaObject::invokeMethod(
            object.data(),
            "get",
            Q_RETURN_ARG(QScriptValue, values),
            Q_ARG(QScriptValue, QScriptValue(&scriptEngine, index))));

    QCOMPARE(values.property(QLatin1String("itemId")).toVariant(), itemId);
    QCOMPARE(values.property(QLatin1String("itemType")).toInt32(), itemType.toInt());
    QCOMPARE(values.property(QLatin1String("fileName")).toVariant(), fileName);
    QCOMPARE(values.property(QLatin1String("title")).toVariant(), isValid ? newTitle : title);
    QCOMPARE(values.property(QLatin1String("turtle")).toVariant(), QVariant());

    QCOMPARE(spy.count(), isValid ? 1 : 0);

    values.setProperty(QLatin1String("itemId"), qScriptValueFromValue(&scriptEngine, itemId));
    values.setProperty(QLatin1String("itemUrl"), qScriptValueFromValue(&scriptEngine, itemType));
    values.setProperty(QLatin1String("fileName"), qScriptValueFromValue(&scriptEngine, fileName));
    values.setProperty(QLatin1String("title"), qScriptValueFromValue(&scriptEngine, title));
    values.setProperty(
            QLatin1String("turtle"), QScriptValue(&scriptEngine, QLatin1String("It's a turtle")));

    QVERIFY(QMetaObject::invokeMethod(
            object.data(), "set", Q_ARG(int, index), Q_ARG(QScriptValue, values)));

    QVERIFY(QMetaObject::invokeMethod(
            object.data(),
            "get",
            Q_RETURN_ARG(QScriptValue, values),
            Q_ARG(QScriptValue, QScriptValue(&scriptEngine, index))));

    QCOMPARE(values.property(QLatin1String("itemId")).toVariant(), itemId);
    QCOMPARE(values.property(QLatin1String("itemType")).toInt32(), itemType.toInt());
    QCOMPARE(values.property(QLatin1String("fileName")).toVariant(), fileName);
    QCOMPARE(values.property(QLatin1String("title")).toVariant(), title);
    QCOMPARE(values.property(QLatin1String("turtle")).toVariant(), QVariant());

    QCOMPARE(spy.count(), isValid ? 2 : 0);
}

void tst_QDeclarativeDocumentGalleryModel::itemsInserted()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(object.data());
    QVERIFY(model);

    QSignalSpy countSpy(model, SIGNAL(countChanged()));
    QSignalSpy beforeSpy(model, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    QSignalSpy afterSpy(model, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QCOMPARE(model->rowCount(), 0);
    QCOMPARE(model->columnCount(), 1);

    gallery.response()->beginInsertRows(0);
    gallery.response()->addRow();
    gallery.response()->addRow();
    gallery.response()->addRow();
    gallery.response()->endInsertRows();

    QCOMPARE(model->rowCount(), 3);
    QCOMPARE(model->property("count"), QVariant(3));
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(beforeSpy.count(), 1);
    QCOMPARE(beforeSpy.last().value(1).toInt(), 0);
    QCOMPARE(beforeSpy.last().value(2).toInt(), 2);
    QCOMPARE(afterSpy.count(), 1);
    QCOMPARE(afterSpy.last().value(1).toInt(), 0);
    QCOMPARE(afterSpy.last().value(2).toInt(), 2);

    gallery.response()->beginInsertRows(1);
    gallery.response()->addRow();
    gallery.response()->addRow();
    gallery.response()->endInsertRows();

    QCOMPARE(model->rowCount(), 5);
    QCOMPARE(model->property("count"), QVariant(5));
    QCOMPARE(countSpy.count(), 2);
    QCOMPARE(beforeSpy.count(), 2);
    QCOMPARE(beforeSpy.last().value(1).toInt(), 1);
    QCOMPARE(beforeSpy.last().value(2).toInt(), 2);
    QCOMPARE(afterSpy.count(), 2);
    QCOMPARE(afterSpy.last().value(1).toInt(), 1);
    QCOMPARE(afterSpy.last().value(2).toInt(), 2);
}

void tst_QDeclarativeDocumentGalleryModel::itemsRemoved()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    gallery.addRow();
    gallery.addRow();
    gallery.addRow();
    gallery.addRow();
    gallery.addRow();

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(object.data());
    QVERIFY(model);

    QSignalSpy countSpy(model, SIGNAL(countChanged()));
    QSignalSpy beforeSpy(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
    QSignalSpy afterSpy(model, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QCOMPARE(model->rowCount(), 5);
    QCOMPARE(model->columnCount(), 1);
    QCOMPARE(model->property("count"), QVariant(5));

    gallery.response()->removeRows(1, 2);

    QCOMPARE(model->rowCount(), 3);
    QCOMPARE(model->property("count"), QVariant(3));
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(beforeSpy.count(), 1);
    QCOMPARE(beforeSpy.last().value(1).toInt(), 1);
    QCOMPARE(beforeSpy.last().value(2).toInt(), 2);
    QCOMPARE(afterSpy.count(), 1);
    QCOMPARE(afterSpy.last().value(1).toInt(), 1);
    QCOMPARE(afterSpy.last().value(2).toInt(), 2);

    gallery.response()->removeRows(0, 3);

    QCOMPARE(model->rowCount(), 0);
    QCOMPARE(model->property("count"), QVariant(0));
    QCOMPARE(countSpy.count(), 2);
    QCOMPARE(beforeSpy.count(), 2);
    QCOMPARE(beforeSpy.last().value(1).toInt(), 0);
    QCOMPARE(beforeSpy.last().value(2).toInt(), 2);
    QCOMPARE(afterSpy.count(), 2);
    QCOMPARE(afterSpy.last().value(1).toInt(), 0);
    QCOMPARE(afterSpy.last().value(2).toInt(), 2);
}

void tst_QDeclarativeDocumentGalleryModel::itemsMoved()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    gallery.addRow();
    gallery.addRow();
    gallery.addRow();
    gallery.addRow();
    gallery.addRow();

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(object.data());
    QVERIFY(model);

    QSignalSpy countSpy(model, SIGNAL(countChanged()));
    QSignalSpy beforeSpy(model, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)));
    QSignalSpy afterSpy(model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)));

    QCOMPARE(model->rowCount(), 5);
    QCOMPARE(model->columnCount(), 1);
    QCOMPARE(model->property("count"), QVariant(5));

    gallery.response()->itemsMoved(0, 3, 2);

    QCOMPARE(model->rowCount(), 5);
    QCOMPARE(model->property("count"), QVariant(5));
    QCOMPARE(countSpy.count(), 0);
    QCOMPARE(beforeSpy.count(), 1);
    QCOMPARE(beforeSpy.last().value(1).toInt(), 0);
    QCOMPARE(beforeSpy.last().value(2).toInt(), 1);
    QCOMPARE(beforeSpy.last().value(4).toInt(), 3);
    QCOMPARE(afterSpy.count(), 1);
    QCOMPARE(afterSpy.last().value(1).toInt(), 0);
    QCOMPARE(afterSpy.last().value(2).toInt(), 1);
    QCOMPARE(afterSpy.last().value(4).toInt(), 3);

    gallery.response()->itemsMoved(2, 0, 3);

    QCOMPARE(model->rowCount(), 5);
    QCOMPARE(model->property("count"), QVariant(5));
    QCOMPARE(countSpy.count(), 0);
    QCOMPARE(beforeSpy.count(), 2);
    QCOMPARE(beforeSpy.last().value(1).toInt(), 2);
    QCOMPARE(beforeSpy.last().value(2).toInt(), 4);
    QCOMPARE(beforeSpy.last().value(4).toInt(), 0);
    QCOMPARE(afterSpy.count(), 2);
    QCOMPARE(afterSpy.last().value(1).toInt(), 2);
    QCOMPARE(afterSpy.last().value(2).toInt(), 4);
    QCOMPARE(afterSpy.last().value(4).toInt(), 0);
}

void tst_QDeclarativeDocumentGalleryModel::metaDataChanged()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    gallery.addRow();
    gallery.addRow();
    gallery.addRow();
    gallery.addRow();
    gallery.addRow();

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(object.data());
    QVERIFY(model);

    QSignalSpy spy(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

    QCOMPARE(model->rowCount(), 5);
    QCOMPARE(model->columnCount(), 1);

    QList<int> keys;
    keys.append(model->roleNames().key("fileName"));
    keys.append(model->roleNames().key("title"));

    gallery.response()->metaDataChanged(1, 1, keys);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.last().value(0).value<QModelIndex>(), model->index(1, 0));
    QCOMPARE(spy.last().value(1).value<QModelIndex>(), model->index(1, 0));

    gallery.response()->metaDataChanged(2, 3, QList<int>());

    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.last().value(0).value<QModelIndex>(), model->index(2, 0));
    QCOMPARE(spy.last().value(1).value<QModelIndex>(), model->index(4, 0));
}

void tst_QDeclarativeDocumentGalleryModel::asyncResponse()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {}\n");

    gallery.setStatus(QGalleryAbstractRequest::Active);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Active));

    gallery.response()->finish();
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryModel::cancelAsyncResponse()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {}\n");

    gallery.setStatus(QGalleryAbstractRequest::Active);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Active));

    QMetaObject::invokeMethod(object.data(), "cancel");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Cancelled));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryModel::cancelIdleResponse()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel { autoUpdate: true }\n");

    gallery.setStatus(QGalleryAbstractRequest::Idle);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Idle));

    QMetaObject::invokeMethod(object.data(), "cancel");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryModel::cancelPendingResponse()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {}\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));

    object->setProperty("rootItem", 95);

    QMetaObject::invokeMethod(object.data(), "cancel");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));
    QCOMPARE(spy.count(), 0);

    QCoreApplication::processEvents();
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));
    QCOMPARE(spy.count(), 0);
}

void tst_QDeclarativeDocumentGalleryModel::deferExecuteCancelledResponse()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {}\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));

    object->setProperty("rootItem", 95);

    QMetaObject::invokeMethod(object.data(), "cancel");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));
    QCOMPARE(spy.count(), 0);

    object->setProperty("rootItem", 21);

    QCoreApplication::processEvents();
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryModel::clear()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {}\n");

    gallery.addRow();
    gallery.addRow();
    gallery.addRow();

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy statusSpy(object.data(), SIGNAL(statusChanged()));
    QSignalSpy removeSpy(object.data(), SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));

    QMetaObject::invokeMethod(object.data(), "clear");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Null));
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(removeSpy.last().value(1).toInt(), 0);
    QCOMPARE(removeSpy.last().value(2).toInt(), 2);
}

void tst_QDeclarativeDocumentGalleryModel::clearPendingResponse()
{
    const QByteArray qml(
            "import Qt 4.7\n"
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryModel {}\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Finished));

    object->setProperty("rootItem", 95);

    QMetaObject::invokeMethod(object.data(), "clear");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Null));
    QCOMPARE(spy.count(), 1);

    QCoreApplication::processEvents();
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Null));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryModel::error_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<int>("errorCode");
    QTest::addColumn<QString>("errorMessage");
    QTest::addColumn<QByteArray>("expectedErrorMessage");

    QTest::newRow("Specific error message")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << int(QDocumentGallery::ConnectionError)
            << "Connection to server failed"
            << QByteArray(
                    "<Unknown File>: QML DocumentGalleryModel: "
                    "Connection to server failed");

    QTest::newRow("Generic connection Error")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << int(QDocumentGallery::ConnectionError)
            << QString()
            << QByteArray(
                    "<Unknown File>: QML DocumentGalleryModel: "
                    "An error was encountered connecting to the document gallery");

    QTest::newRow("Generic rootType Error")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel { rootType: DocumentGallery.Video }\n")
            << int(QDocumentGallery::ItemTypeError)
            << QString()
            << QByteArray(
                    "<Unknown File>: QML DocumentGalleryModel: "
                    "DocumentGallery.Video is not a supported item type");

    QTest::newRow("Invalid rootType Error")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel { rootType: DocumentGallery.InvalidType }\n")
            << int(QDocumentGallery::ItemTypeError)
            << QString()
            << QByteArray(
                    "<Unknown File>: QML DocumentGalleryModel: "
                    "DocumentGallery.InvalidType is not a supported item type");

    QTest::newRow("Generic rootItem error")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << int(QDocumentGallery::ItemIdError)
            << QString()
            << QByteArray(
                    "<Unknown File>: QML DocumentGalleryModel: "
                    "The value of rootItem is not a valid item ID");

    QTest::newRow("Generic filter Error")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << int(QDocumentGallery::FilterError)
            << QString()
            << QByteArray(
                    "<Unknown File>: QML DocumentGalleryModel: "
                    "The value of filter is unsupported");

    QTest::newRow("Unhandled error code")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << int(QDocumentGallery::NoGallery)
            << QString()
            << QByteArray();
}

void tst_QDeclarativeDocumentGalleryModel::error()
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

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryQueryModel::Error));
}

QTEST_MAIN(tst_QDeclarativeDocumentGalleryModel)

#include "tst_qdeclarativedocumentgallerymodel.moc"
