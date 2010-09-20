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

#include <QtTest/QtTest>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGalleryFilter);
Q_DECLARE_METATYPE(QGalleryQueryRequest::Scope);

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

    QVariant::Type propertyType(int) const { return QVariant::Invalid; }

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
        return m_rows.value(m_currentIndex).metaData.value(m_propertyNames.at(key)); }

    bool setMetaData(int key, const QVariant &value)
    {
        if (isValid() && key >= 0) {
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

    using QGalleryResultSet::metaDataChanged;
    using QGalleryResultSet::itemsMoved;

private:
    QStringList m_propertyNames;
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

private Q_SLOTS:
    void rootType_data();
    void rootType();
    void properties_data();
    void properties();
    void sortProperties_data();
    void sortProperties();
    void autoUpdate_data();
    void autoUpdate();
    void rootItem_data();
    void rootItem();
    void scope_data();
    void scope();
    void offset_data();
    void offset();
    void limit_data();
    void limit();
    void filter_data();
    void filter();

private:
    QtTestGallery gallery;
    QDeclarativeEngine engine;
};

void tst_QDeclarativeDocumentGalleryModel::initTestCase()
{
    qRegisterMetaType<QGalleryFilter>();
    qRegisterMetaType<QGalleryQueryRequest::Scope>();

    engine.rootContext()->setContextProperty(QLatin1String("qt_testGallery"), &gallery);
}

void tst_QDeclarativeDocumentGalleryModel::rootType_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QString>("initialRootTypeString");
    QTest::addColumn<int>("initialRootType");
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
    QFETCH(QString, initialRootTypeString);
    QFETCH(int, initialRootType);
    QFETCH(int, rootType);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(rootTypeChanged()));

    QCOMPARE(object->property("rootType"), QVariant(initialRootType));
    QCOMPARE(gallery.request()->rootType(), initialRootTypeString);

    object->setProperty("rootType", rootType);
    QCOMPARE(object->property("rootType"), QVariant(initialRootType));
    QCOMPARE(spy.count(), 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->rootType(), initialRootTypeString);
}

void tst_QDeclarativeDocumentGalleryModel::properties_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QStringList>("initialPropertyNames");
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
    QFETCH(QStringList, initialPropertyNames);
    QFETCH(QStringList, propertyNames);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(propertyNamesChanged()));

    QCOMPARE(object->property("properties"), QVariant(initialPropertyNames));
    QCOMPARE(gallery.request()->propertyNames(), initialPropertyNames);

    object->setProperty("properties", propertyNames);
    QCOMPARE(object->property("properties"), QVariant(initialPropertyNames));
    QCOMPARE(spy.count(), 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->propertyNames(), initialPropertyNames);
}

void tst_QDeclarativeDocumentGalleryModel::sortProperties_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QStringList>("initialSortPropertyNames");
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
    QFETCH(QStringList, initialSortPropertyNames);
    QFETCH(QStringList, sortPropertyNames);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(sortPropertyNamesChanged()));

    QCOMPARE(object->property("sortProperties"), QVariant(initialSortPropertyNames));
    QCOMPARE(gallery.request()->sortPropertyNames(), initialSortPropertyNames);

    object->setProperty("sortProperties", sortPropertyNames);
    QCOMPARE(object->property("sortProperties"), QVariant(sortPropertyNames));
    QCOMPARE(spy.count(), initialSortPropertyNames != sortPropertyNames ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->sortPropertyNames(), sortPropertyNames);
}

void tst_QDeclarativeDocumentGalleryModel::autoUpdate_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<bool>("initialAutoUpdate");
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
    QFETCH(bool, initialAutoUpdate);
    QFETCH(bool, autoUpdate);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(autoUpdateChanged()));

    QCOMPARE(object->property("autoUpdate"), QVariant(initialAutoUpdate));
    QCOMPARE(gallery.request()->autoUpdate(), initialAutoUpdate);

    object->setProperty("autoUpdate", autoUpdate);
    QCOMPARE(object->property("autoUpdate"), QVariant(autoUpdate));
    QCOMPARE(spy.count(), initialAutoUpdate != autoUpdate ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->autoUpdate(), autoUpdate);
}

void tst_QDeclarativeDocumentGalleryModel::rootItem_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QVariant>("initialRootItem");
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
}

void tst_QDeclarativeDocumentGalleryModel::rootItem()
{
    QFETCH(QByteArray, qml);
    QFETCH(QVariant, initialRootItem);
    QFETCH(QVariant, rootItem);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(rootItemChanged()));

    QCOMPARE(object->property("rootItem"), initialRootItem);
    QCOMPARE(gallery.request()->rootItem(), initialRootItem);

    object->setProperty("rootItem", rootItem);
    QCOMPARE(object->property("rootItem"), rootItem);
    QCOMPARE(spy.count(), initialRootItem != rootItem ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->rootItem(), rootItem);
}

void tst_QDeclarativeDocumentGalleryModel::scope_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QGalleryQueryRequest::Scope>("initialScope");
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
    QFETCH(QGalleryQueryRequest::Scope, initialScope);
    QFETCH(QGalleryQueryRequest::Scope, scope);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(scopeChanged()));

    QCOMPARE(object->property("scope"), QVariant(int(initialScope)));
    QCOMPARE(gallery.request()->scope(), initialScope);

    object->setProperty("scope", scope);
    QCOMPARE(object->property("scope"), QVariant(int(scope)));
    QCOMPARE(spy.count(), initialScope != scope ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->scope(), scope);
}

void tst_QDeclarativeDocumentGalleryModel::offset_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<int>("initialOffset");
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
    QFETCH(int, initialOffset);
    QFETCH(int, offset);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(offsetChanged()));

    QCOMPARE(object->property("offset"), QVariant(initialOffset));
    QCOMPARE(gallery.request()->offset(), initialOffset);

    object->setProperty("offset", offset);
    QCOMPARE(object->property("offset"), QVariant(offset));
    QCOMPARE(spy.count(), initialOffset != offset ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QCOMPARE(gallery.request()->offset(), offset);
}

void tst_QDeclarativeDocumentGalleryModel::limit_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<int>("initialLimit");
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
    QFETCH(int, initialLimit);
    QFETCH(int, limit);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(limitChanged()));

    QCOMPARE(object->property("limit"), QVariant(initialLimit));
    QCOMPARE(gallery.request()->limit(), initialLimit);

    object->setProperty("limit", limit);
    QCOMPARE(object->property("limit"), QVariant(limit));
    QCOMPARE(spy.count(), initialLimit != limit ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");
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
                            "inverted: false\n"
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
                            "inverted: true\n"
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
                            "inverted: false\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(QGalleryMetaDataFilter(
                    QDocumentGallery::fileName, QLatin1String("p"), QGalleryFilter::StartsWith));

    QTest::newRow("!fileName.startsWith(p)")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "filter: GalleryStartsWithFilter {\n"
                            "property: \"fileName\"\n"
                            "value: \"p\"\n"
                            "inverted: true\n"
                        "}\n"
                    "}\n")
            << QGalleryFilter(!QGalleryMetaDataFilter(
                    QDocumentGallery::fileName, QLatin1String("p"), QGalleryFilter::StartsWith));
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
            << QGalleryFilter(QGalleryMetaDataFilter(
                    QDocumentGallery::fileName, QLatin1String(".jpg"), QGalleryFilter::EndsWith));

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
            << QGalleryFilter(QGalleryMetaDataFilter(
                    QDocumentGallery::fileName, QLatin1String("p"), QGalleryFilter::Contains));

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
            << QGalleryFilter(QGalleryMetaDataFilter(
                    QDocumentGallery::fileName, QLatin1String("p*.jpg"), QGalleryFilter::Wildcard));

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
            << QGalleryFilter(QGalleryMetaDataFilter(
                    QDocumentGallery::fileName,
                    QRegExp(QLatin1String("p\\d{6}\\.(jpg|jpeg)"), Qt::CaseSensitive, QRegExp::RegExp2),
                    QGalleryFilter::RegExp));

    {
        QGalleryIntersectionFilter filter;
        filter.append(QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String("p"), QGalleryFilter::StartsWith));
        filter.append(QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String(".jpg"), QGalleryFilter::EndsWith));

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
            intersectionFilter.append(QGalleryMetaDataFilter(
                    QDocumentGallery::fileName, QLatin1String("p"), QGalleryFilter::StartsWith));
            intersectionFilter.append(QGalleryMetaDataFilter(
                    QDocumentGallery::fileName, QLatin1String(".jpg"), QGalleryFilter::EndsWith));
            filter.append(intersectionFilter);
        } {
            QGalleryIntersectionFilter intersectionFilter;
            intersectionFilter.append(QGalleryMetaDataFilter(
                    QDocumentGallery::fileName, QLatin1String("p"), QGalleryFilter::StartsWith));
            intersectionFilter.append(QGalleryMetaDataFilter(
                    QDocumentGallery::fileName, QLatin1String(".jpeg"), QGalleryFilter::EndsWith));
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
        filter.append(QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String("p"), QGalleryFilter::StartsWith));
        filter.append(QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String(".jpg"), QGalleryFilter::EndsWith));

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
        filter.append(QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String(".jpg"), QGalleryFilter::EndsWith));
        filter.append(QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String(".jpeg"), QGalleryFilter::EndsWith));

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
    QVERIFY(gallery.response());

    QCOMPARE(gallery.request()->filter(), filter);
}

QTEST_MAIN(tst_QDeclarativeDocumentGalleryModel)

#include "tst_qdeclarativedocumentgallerymodel.moc"
