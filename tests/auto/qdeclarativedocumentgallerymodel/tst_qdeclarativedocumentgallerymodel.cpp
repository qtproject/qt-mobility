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
    void construct_data();
    void construct();

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

void tst_QDeclarativeDocumentGalleryModel::construct_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QVariant>("rootItem");
    QTest::addColumn<QGalleryQueryRequest::Scope>("scope");
    QTest::addColumn<QGalleryFilter>("filter");
    QTest::addColumn<QStringList>("propertyNames");
    QTest::addColumn<QStringList>("sortPropertyNames");
    QTest::addColumn<bool>("autoUpdate");
    QTest::addColumn<int>("offset");
    QTest::addColumn<int>("limit");

    QTest::newRow("Default []")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {}\n")
            << "File"
            << QVariant()
            << QGalleryQueryRequest::AllDescendants
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << false
            << 0
            << 0;

    QTest::newRow("Audio [ title, duration | +rating ]")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootType: DocumentGallery.Audio\n"
                        "properties: [ \"title\", \"duration\" ]\n"
                        "sortProperties: [ \"+rating\" ]\n"
                    "}\n")
            << "Audio"
            << QVariant()
            << QGalleryQueryRequest::AllDescendants
            << QGalleryFilter()
            << (QStringList() << QLatin1String("title") << QLatin1String("duration"))
            << (QStringList() << QLatin1String("+rating"))
            << false
            << 0
            << 0;

    QTest::newRow("Video [] offset: 30; limit: 15")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootType: DocumentGallery.Video\n"
                        "offset: 30\n"
                        "limit: 15\n"
                    "}\n")
            << "Video"
            << QVariant()
            << QGalleryQueryRequest::AllDescendants
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << false
            << 30
            << 15;

    QTest::newRow("Image [] autoUpdate: true")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootType: DocumentGallery.Image\n"
                        "autoUpdate: true\n"
                    "}\n")
            << "Image"
            << QVariant()
            << QGalleryQueryRequest::AllDescendants
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << true
            << 0
            << 0;

    QTest::newRow("Playlist [] autoUpdate: false")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootType: DocumentGallery.Playlist\n"
                        "autoUpdate: false\n"
                    "}\n")
            << "Playlist"
            << QVariant()
            << QGalleryQueryRequest::AllDescendants
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << false
            << 0
            << 0;

    QTest::newRow("Audio [] rootItem: folder:///path/to/music")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootType: DocumentGallery.Audio\n"
                        "rootItem: \"folder:///path/to/music\"\n"
                    "}\n")
            << "Audio"
            << QVariant(QLatin1String("folder:///path/to/music"))
            << QGalleryQueryRequest::AllDescendants
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << false
            << 0
            << 0;

    QTest::newRow("Folder [] scope: DirectDescendants")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootType: DocumentGallery.Folder\n"
                        "scope: DocumentGalleryModel.DirectDescendants\n"
                    "}\n")
            << "Folder"
            << QVariant()
            << QGalleryQueryRequest::DirectDescendants
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << false
            << 0
            << 0;

    QTest::newRow("Image [] autoUpdate: keywords = holiday")
            << QByteArray(
                    "import Qt 4.7\n"
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryModel {\n"
                        "rootType: DocumentGallery.Image\n"
                        "filter: GalleryEqualsFilter {\n"
                            "property: \"keywords\"\n"
                            "value: \"holiday\"\n"
                        "}\n"
                    "}\n")
            << "Image"
            << QVariant()
            << QGalleryQueryRequest::AllDescendants
            << QGalleryFilter(QDocumentGallery::keywords == QLatin1String("holiday"))
            << QStringList()
            << QStringList()
            << false
            << 0
            << 0;
}

void tst_QDeclarativeDocumentGalleryModel::construct()
{
    QFETCH(QByteArray, qml);
    QFETCH(QString, rootType);
    QFETCH(QVariant, rootItem);
    QFETCH(QGalleryQueryRequest::Scope, scope);
    QFETCH(QGalleryFilter, filter);
    QFETCH(QStringList, propertyNames);
    QFETCH(QStringList, sortPropertyNames);
    QFETCH(bool, autoUpdate);
    QFETCH(int, offset);
    QFETCH(int, limit);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QCOMPARE(gallery.request()->rootType(), rootType);
    QCOMPARE(gallery.request()->rootItem(), rootItem);
    QCOMPARE(gallery.request()->scope(), scope);
    QCOMPARE(gallery.request()->filter(), filter);
    QCOMPARE(gallery.request()->propertyNames(), propertyNames);
    QCOMPARE(gallery.request()->sortPropertyNames(), sortPropertyNames);
    QCOMPARE(gallery.request()->autoUpdate(), autoUpdate);
    QCOMPARE(gallery.request()->offset(), offset);
    QCOMPARE(gallery.request()->limit(), limit);
}

QTEST_MAIN(tst_QDeclarativeDocumentGalleryModel)

#include "tst_qdeclarativedocumentgallerymodel.moc"
