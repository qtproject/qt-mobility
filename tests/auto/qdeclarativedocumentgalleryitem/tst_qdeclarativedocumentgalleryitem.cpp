/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=plugins/declarative/gallery


#include <qabstractgallery.h>
#include <qgalleryitemrequest.h>
#include <qgalleryresultset.h>

#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeengine.h>

#include <qdeclarativedocumentgallery.h>
#include <qdeclarativegalleryitem.h>

#include <QtTest/QtTest>

QTM_USE_NAMESPACE


class QtGalleryTestResponse : public QGalleryResultSet
{
    Q_OBJECT
public:
    QtGalleryTestResponse(
            const QStringList &propertyNames,
            const QHash<QString, QVariant> &metaData,
            int count,
            QGalleryAbstractRequest::State state,
            int error,
            const QString &errorString)
        : m_count(count)
        , m_currentIndex(-1)
        , m_propertyNames(propertyNames)
    {
        typedef QHash<QString, QVariant>::const_iterator iterator;
        for (iterator it = metaData.begin(), end = metaData.end(); it != end; ++it) {
            int propertyKey = m_propertyNames.indexOf(it.key());

            if (propertyKey >= 0)
                m_metaData.insert(propertyKey, it.value());
        }

        if (error != QGalleryAbstractRequest::NoError)
            QGalleryAbstractResponse::error(error, errorString);
        else if (state == QGalleryAbstractRequest::Finished)
            finish();
        else if (state == QGalleryAbstractRequest::Idle)
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

    QVariant itemId() const { return isValid() ? QVariant(1) : QVariant(); }
    QUrl itemUrl() const { return isValid() ? QUrl("file:///path/to/song.mp3") : QUrl(); }
    QString itemType() const { return isValid() ? QLatin1String("Audio") : QString(); }

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
    using QGalleryResultSet::cancel;
    using QGalleryResultSet::error;
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
        , m_state(QGalleryAbstractRequest::Finished)
        , m_error(QGalleryAbstractRequest::NoError)
    {}

    bool isRequestSupported(QGalleryAbstractRequest::RequestType type) const {
        return type == QGalleryAbstractRequest::TypeRequest; }

    void setState(QGalleryAbstractRequest::State state) { m_state = state; }
    void setError(int error, const QString &errorString) {
        m_error = error; m_errorString = errorString; }

    void setCount(int count) { m_count = count; }

    void setBlackList(const QStringList &propertyNames) { m_blacklist = propertyNames; }
    void setMetaData(const QHash<QString, QVariant> &metaData) { m_metaData = metaData; }

    QGalleryItemRequest *request() const { return m_request.data(); }
    QtGalleryTestResponse *response() const { return m_response.data(); }

    void reset()
    {
        m_count = 0;
        m_state = QGalleryAbstractRequest::Finished;
        m_error = QGalleryItemRequest::NoError;
        m_metaData.clear();
        m_blacklist.clear();
    }

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if ((m_request = qobject_cast<QGalleryItemRequest *>(request))) {
            QStringList propertyNames = m_request.data()->propertyNames();
            foreach (QString propertyName, m_blacklist)
                propertyNames.removeAll(propertyName);

            m_response = new QtGalleryTestResponse(
                    propertyNames,
                    m_metaData,
                    m_count,
                    m_state,
                    m_error,
                    m_errorString);
        } else {
            m_response.clear();
        }
        return m_response.data();
    }

private:
    int m_count;
    QGalleryAbstractRequest::State m_state;
    int m_error;
    QString m_errorString;
    QHash<QString, QVariant> m_metaData;
    QStringList m_blacklist;
    QWeakPointer<QGalleryItemRequest> m_request;
    QWeakPointer<QtGalleryTestResponse> m_response;
};

class tst_QDeclarativeDocumentGalleryItem : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void initTestCase();
    void cleanup();

private Q_SLOTS:
    void itemId_data();
    void itemId();
    void properties_data();
    void properties();
    void autoUpdate_data();
    void autoUpdate();
    void disableAutoUpdateFinished();
    void disableAutoUpdateIdle();
    void disableAutoUpdateActive();
    void asyncResponse();
    void cancelAsyncResponse();
    void cancelIdleResponse();
    void cancelPendingResponse();
    void deferExecuteCanceledResponse();
    void clear();
    void clearPendingResponse();
    void error_data();
    void error();
    void progress_data();
    void progress();
    void available();
    void itemUrl();
    void itemType();
    void metaData();

private:
    QtTestGallery gallery;
    QDeclarativeEngine engine;
};

void tst_QDeclarativeDocumentGalleryItem::initTestCase()
{
    engine.rootContext()->setContextProperty(QLatin1String("qt_testGallery"), &gallery);
}

void tst_QDeclarativeDocumentGalleryItem::cleanup()
{
    gallery.reset();
}

void tst_QDeclarativeDocumentGalleryItem::itemId_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QVariant>("qmlItemId");
    QTest::addColumn<QVariant>("itemId");

    QTest::newRow("Null -> 45")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem {}\n")
            << QVariant()
            << QVariant(45);

    QTest::newRow("45 -> 45")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem { item: 45 }\n")
            << QVariant(45)
            << QVariant(45);

    QTest::newRow("45 -> file:///path/to/file.ext")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem { item: 45 }\n")
            << QVariant(45)
            << QVariant(QLatin1String("file:///path/to/file.ext"));

    QTest::newRow("file:///path/to/file.ext -> Null")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem { item: \"file:///path/to/file.ext\" }\n")
            << QVariant(QLatin1String("file:///path/to/file.ext"))
            << QVariant();
}

void tst_QDeclarativeDocumentGalleryItem::itemId()
{
    QFETCH(QByteArray, qml);
    QFETCH(QVariant, qmlItemId);
    QFETCH(QVariant, itemId);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);

    QSignalSpy spy(object.data(), SIGNAL(itemIdChanged()));

    if (qmlItemId.isValid()) {
        QVERIFY(gallery.request());
        QCOMPARE(gallery.request()->itemId(), qmlItemId);
    } else {
        QVERIFY(!gallery.request());
    }

    QCOMPARE(object->property("item"), qmlItemId);

    QCOMPARE(object->setProperty("item", itemId), true);
    QCOMPARE(object->property("item"), itemId);
    QCOMPARE(spy.count(), itemId != qmlItemId ? 1 : 0);

    QMetaObject::invokeMethod(object.data(), "reload");

    if (itemId.isValid()) {
        QVERIFY(gallery.request());
        QCOMPARE(gallery.request()->itemId(), itemId);
    }
}

void tst_QDeclarativeDocumentGalleryItem::properties_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QStringList>("qmlPropertyNames");
    QTest::addColumn<QStringList>("propertyNames");

    QTest::newRow("[] -> [ title, fileName ]")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem {\n"
                        "item: 0\n"
                    "}\n")
            << QStringList()
            << (QStringList() << QLatin1String("title") << QLatin1String("fileName"));

    QTest::newRow("[ title, fileName ] -> [ title, fileName ]")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem {\n"
                        "item: 0\n"
                        "properties: [ \"title\", \"fileName\" ]"
                    "}\n")
            << (QStringList() << QLatin1String("title") << QLatin1String("fileName"))
            << (QStringList() << QLatin1String("title") << QLatin1String("fileName"));

    QTest::newRow("[ title, fileName ] -> [ count ]")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem {\n"
                        "item: 0\n"
                        "properties: [ \"title\", \"fileName\" ]"
                    "}\n")
            << (QStringList() << QLatin1String("title") << QLatin1String("fileName"))
            << (QStringList() << QLatin1String("count"));
}

void tst_QDeclarativeDocumentGalleryItem::properties()
{
    QFETCH(QByteArray, qml);
    QFETCH(QStringList, qmlPropertyNames);
    QFETCH(QStringList, propertyNames);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());

    QSignalSpy spy(object.data(), SIGNAL(itemIdChanged()));

    QCOMPARE(gallery.request()->propertyNames(), qmlPropertyNames);
    QCOMPARE(object->property("properties"), QVariant(qmlPropertyNames));

    QCOMPARE(object->setProperty("properties", propertyNames), true);
    QCOMPARE(object->property("properties"), QVariant(qmlPropertyNames));
    QCOMPARE(spy.count(), 0);

    QMetaObject::invokeMethod(object.data(), "reload");
    QVERIFY(gallery.request());
    QCOMPARE(gallery.request()->propertyNames(), qmlPropertyNames);

}

void tst_QDeclarativeDocumentGalleryItem::autoUpdate_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<bool>("qmlAutoUpdate");
    QTest::addColumn<bool>("autoUpdate");

    QTest::newRow("Default -> true")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem {\n"
                        "item: 0\n"
                    "}\n")
            << false
            << true;

    QTest::newRow("true -> false")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem {\n"
                        "item: 0\n"
                        "autoUpdate: true\n"
                    "}\n")
            << true
            << false;

    QTest::newRow("false -> true")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem {\n"
                        "item: 0\n"
                        "autoUpdate: false\n"
                    "}\n")
            << false
            << true;

    QTest::newRow("true -> true")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem {\n"
                        "item: 0\n"
                        "autoUpdate: true\n"
                    "}\n")
            << true
            << true;

    QTest::newRow("false -> false")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem {\n"
                        "item: 0\n"
                        "autoUpdate: false\n"
                    "}\n")
            << false
            << false;
}

void tst_QDeclarativeDocumentGalleryItem::autoUpdate()
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

void tst_QDeclarativeDocumentGalleryItem::disableAutoUpdateFinished()
{
    const QByteArray qml(
        "import QtMobility.gallery 1.1\n"
        "DocumentGalleryItem {\n"
            "item: 12\n"
            "autoUpdate: true\n"
        "}\n");

    gallery.setState(QGalleryAbstractRequest::Finished);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("autoUpdate"), QVariant(true));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));

    object->setProperty("autoUpdate", false);
    QCOMPARE(object->property("autoUpdate"), QVariant(false));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));
    QCOMPARE(spy.count(), 0);
}

void tst_QDeclarativeDocumentGalleryItem::disableAutoUpdateIdle()
{
    const QByteArray qml(
        "import QtMobility.gallery 1.1\n"
        "DocumentGalleryItem {\n"
            "item: 12\n"
            "autoUpdate: true\n"
        "}\n");

    gallery.setState(QGalleryAbstractRequest::Idle);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("autoUpdate"), QVariant(true));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Idle));

    object->setProperty("autoUpdate", false);
    QCOMPARE(object->property("autoUpdate"), QVariant(false));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryItem::disableAutoUpdateActive()
{
    const QByteArray qml(
        "import QtMobility.gallery 1.1\n"
        "DocumentGalleryItem {\n"
            "item: 12\n"
            "autoUpdate: true\n"
        "}\n");

    gallery.setState(QGalleryAbstractRequest::Active);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("autoUpdate"), QVariant(true));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Active));

    object->setProperty("autoUpdate", false);
    QCOMPARE(object->property("autoUpdate"), QVariant(false));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Active));
    QCOMPARE(spy.count(), 0);

    QVERIFY(gallery.response());
    gallery.response()->finish(true);
    QCOMPARE(object->property("autoUpdate"), QVariant(false));
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));
    QCOMPARE(spy.count(), 1);
}
void tst_QDeclarativeDocumentGalleryItem::asyncResponse()
{
    const QByteArray qml(
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryItem { item: 12 }\n");

    gallery.setState(QGalleryAbstractRequest::Active);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Active));

    gallery.response()->finish();
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryItem::cancelAsyncResponse()
{
    const QByteArray qml(
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryItem { item: 12 }\n");

    gallery.setState(QGalleryAbstractRequest::Active);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Active));

    QMetaObject::invokeMethod(object.data(), "cancel");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Canceled));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryItem::cancelIdleResponse()
{
    const QByteArray qml(
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryItem { item: 12; autoUpdate: true }\n");

    gallery.setState(QGalleryAbstractRequest::Idle);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Idle));

    QMetaObject::invokeMethod(object.data(), "cancel");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryItem::cancelPendingResponse()
{
    const QByteArray qml(
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryItem { item: 12 }\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));

    object->setProperty("autoUpdate", true);

    QMetaObject::invokeMethod(object.data(), "cancel");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));
    QCOMPARE(spy.count(), 0);

    QCoreApplication::processEvents();
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));
    QCOMPARE(spy.count(), 0);
}

void tst_QDeclarativeDocumentGalleryItem::deferExecuteCanceledResponse()
{
    const QByteArray qml(
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryItem { item: 12 }\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));

    object->setProperty("autoUpdate", true);

    QMetaObject::invokeMethod(object.data(), "cancel");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));
    QCOMPARE(spy.count(), 0);

    object->setProperty("autoUpdate", false);
    object->setProperty("autoUpdate", true);

    QCoreApplication::processEvents();
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryItem::clear()
{
    const QByteArray qml(
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryItem { item: 12 }\n");

    gallery.setCount(1);

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy statusSpy(object.data(), SIGNAL(statusChanged()));
    QSignalSpy availableSpy(object.data(), SIGNAL(availableChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));

    QMetaObject::invokeMethod(object.data(), "clear");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Null));
    QCOMPARE(object->property("available"), QVariant(false));
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(availableSpy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryItem::clearPendingResponse()
{
    const QByteArray qml(
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryItem { item: 12 }\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());

    QSignalSpy spy(object.data(), SIGNAL(statusChanged()));

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Finished));

    object->setProperty("autoUpdate", true);

    QMetaObject::invokeMethod(object.data(), "clear");
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Null));
    QCOMPARE(spy.count(), 1);

    QCoreApplication::processEvents();
    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Null));
    QCOMPARE(spy.count(), 1);
}

void tst_QDeclarativeDocumentGalleryItem::error_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<int>("errorCode");
    QTest::addColumn<QString>("errorMessage");
    QTest::addColumn<QByteArray>("expectedErrorMessage");

    QTest::newRow("Specific error message")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem { item: 0 }\n")
            << int(QDocumentGallery::ConnectionError)
            << "Connection to server failed"
            << QByteArray(
                    "<Unknown File>: QML DocumentGalleryItem: "
                    "Connection to server failed");

    QTest::newRow("Generic connection Error")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem { item: 0 }\n")
            << int(QDocumentGallery::ConnectionError)
            << QString()
            << QByteArray(
                    "<Unknown File>: QML DocumentGalleryItem: "
                    "An error was encountered connecting to the document gallery");

    QTest::newRow("Generic rootItem error")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem { item: 0 }\n")
            << int(QDocumentGallery::ItemIdError)
            << QString()
            << QByteArray(
                    "<Unknown File>: QML DocumentGalleryItem: "
                    "The value of item is not a valid item ID");

    QTest::newRow("Unhandled error code")
            << QByteArray(
                    "import QtMobility.gallery 1.1\n"
                    "DocumentGalleryItem { item: 0 }\n")
            << int(QDocumentGallery::NoGallery)
            << QString()
            << QByteArray();
}

void tst_QDeclarativeDocumentGalleryItem::error()
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

    QCOMPARE(object->property("status"), QVariant(QDeclarativeGalleryItem::Error));
}

void tst_QDeclarativeDocumentGalleryItem::progress_data()
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

void tst_QDeclarativeDocumentGalleryItem::progress()
{
    QFETCH(int, currentProgress);
    QFETCH(int, maximumProgress);
    QFETCH(qreal, normalizedProgress);

    const QByteArray qml(
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryItem { item: 12 }\n");

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

void tst_QDeclarativeDocumentGalleryItem::available()
{
    const QByteArray qml(
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryItem { item: 12 }\n");

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

void tst_QDeclarativeDocumentGalleryItem::itemUrl()
{
    const QByteArray qml(
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryItem { item: 12 }\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());
    QCOMPARE(object->property("itemUrl"), QVariant(QUrl()));

    QSignalSpy spy(object.data(), SIGNAL(availableChanged()));

    gallery.response()->setCount(1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(object->property("itemUrl"), QVariant(QUrl(QLatin1String("file:///path/to/song.mp3"))));

    gallery.response()->setCount(0);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(object->property("itemUrl"), QVariant(QUrl()));
}

void tst_QDeclarativeDocumentGalleryItem::itemType()
{
    const QByteArray qml(
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryItem { item: 12 }\n");

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.request());
    QVERIFY(gallery.response());
    QCOMPARE(object->property("itemType"), QVariant(QDeclarativeDocumentGallery::InvalidType));

    QSignalSpy spy(object.data(), SIGNAL(availableChanged()));

    gallery.response()->setCount(1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(object->property("itemType"), QVariant(QDeclarativeDocumentGallery::Audio));

    gallery.response()->setCount(0);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(object->property("itemType"), QVariant(QDeclarativeDocumentGallery::InvalidType));
}

void tst_QDeclarativeDocumentGalleryItem::metaData()
{
    const QByteArray qml(
            "import QtMobility.gallery 1.1\n"
            "DocumentGalleryItem {\n"
                "item: 12\n"
                "properties: [ \"fileName\", \"title\", \"turtle\" ]\n"
            "}\n");

    {
        QHash<QString, QVariant> metaData;
        metaData.insert(QLatin1String("fileName"), QLatin1String("interlude.mp3"));
        gallery.setMetaData(metaData);
        gallery.setBlackList(QStringList() << QLatin1String("turtle"));
    }

    QDeclarativeComponent component(&engine);
    component.setData(qml, QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY(object);
    QVERIFY(gallery.response());

    QObject *metaData = object->property("metaData").value<QObject *>();
    QVERIFY(metaData);
    QCOMPARE(metaData->property("fileName"), QVariant());
    QCOMPARE(metaData->property("title"), QVariant());
    QCOMPARE(metaData->property("turtle"), QVariant());

    gallery.response()->setCount(1);
    QCOMPARE(metaData->property("fileName"), QVariant(QLatin1String("interlude.mp3")));
    QCOMPARE(metaData->property("title"), QVariant(QVariant::String));
    QCOMPARE(metaData->property("turtle"), QVariant());

    {
        QHash<QString, QVariant> metaData;
        metaData.insert(QLatin1String("title"), QLatin1String("Interlude"));
        gallery.setMetaData(metaData);
        gallery.setCount(1);
    }

    gallery.setBlackList(QStringList() << QLatin1String("turtle") << QLatin1String("fileName"));
    QMetaObject::invokeMethod(object.data(), "reload");

    QCOMPARE(metaData->property("fileName"), QVariant());
    QCOMPARE(metaData->property("title"), QVariant(QLatin1String("Interlude")));
    QCOMPARE(metaData->property("turtle"), QVariant());

    QVERIFY(gallery.response());
    const int titleKey = gallery.response()->propertyKey(QLatin1String("title"));
    QVERIFY(titleKey >= 0);

    metaData->setProperty("title", QLatin1String("An Interlude"));
    QCOMPARE(gallery.response()->metaData(titleKey), QVariant(QLatin1String("An Interlude")));

    gallery.response()->setMetaData(titleKey, QLatin1String("Interlude"));
    QCOMPARE(metaData->property("title"), QVariant(QLatin1String("Interlude")));

    gallery.response()->setCount(0);
    QCOMPARE(metaData->property("fileName"), QVariant());
    QCOMPARE(metaData->property("title"), QVariant());
    QCOMPARE(metaData->property("turtle"), QVariant());
}

QTEST_MAIN(tst_QDeclarativeDocumentGalleryItem)

#include "tst_qdeclarativedocumentgalleryitem.moc"
