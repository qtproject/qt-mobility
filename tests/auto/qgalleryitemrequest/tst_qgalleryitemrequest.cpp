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

//TESTED_COMPONENT=src/documentgallery

#include <qgalleryitemrequest.h>

#include <qabstractgallery.h>
#include <qgalleryabstractresponse.h>
#include <qgalleryresultset.h>
#include <qgalleryresource.h>
#include <qgallerytype.h>

#include <QtTest/QtTest>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QGalleryResultSet*)

class tst_QGalleryItemRequest : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void propertyNames();
    void autoUpdate();
    void itemId();
    void executeSynchronous();
    void executeAsynchronous();
    void noResponse();
    void multipleResults();
};

class QtGalleryTestResponse : public QGalleryResultSet
{
    Q_OBJECT
public:
    QtGalleryTestResponse(
            const QStringList &propertyNames,
            int count,
            QGalleryAbstractRequest::State state,
            int error,
            const QString &errorString)
        : m_count(count)
        , m_currentIndex(-1)
        , m_propertyNames(propertyNames)
    {
        m_propertyNames.removeAll(QLatin1String("turtle"));

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
    QUrl itemUrl() const { return isValid() ? QUrl("http://example.com") : QUrl(); }
    QString itemType() const { return isValid() ? QLatin1String("WebPage") : QString(); }

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

    void setCount(int count) { m_count = count; }

    using QGalleryResultSet::finish;
    using QGalleryResultSet::resume;
    using QGalleryResultSet::cancel;
    using QGalleryResultSet::error;
    using QGalleryResultSet::itemsInserted;
    using QGalleryResultSet::itemsRemoved;
    using QGalleryResultSet::itemsMoved;
    using QGalleryResultSet::metaDataChanged;

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
        , m_state(QGalleryAbstractRequest::Active)
        , m_error(QGalleryAbstractRequest::NoError)
    {}

    bool isRequestSupported(QGalleryAbstractRequest::RequestType type) const {
        return type == QGalleryAbstractRequest::ItemRequest; }

    void setState(QGalleryAbstractRequest::State state) { m_state = state; }
    void setError(int error, const QString &errorString) {
        m_error = error; m_errorString = errorString; }

    void setCount(int count) { m_count = count; }

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if (request->type() == QGalleryAbstractRequest::ItemRequest) {
            return new QtGalleryTestResponse(
                    static_cast<QGalleryItemRequest *>(request)->propertyNames(),
                    m_count,
                    m_state,
                    m_error,
                    m_errorString);
        }
        return 0;
    }

private:
    int m_count;
    QGalleryAbstractRequest::State m_state;
    int m_error;
    QString m_errorString;
};

void tst_QGalleryItemRequest::initTestCase()
{
    qRegisterMetaType<QGalleryResultSet*>();
    qRegisterMetaType<QList<int> >();
}

void tst_QGalleryItemRequest::propertyNames()
{
    const QGalleryProperty titleProperty = {"title", sizeof("title")};
    const QGalleryProperty artistProperty = {"artist", sizeof("artist")};

    const QStringList propertyNames = QStringList()
            << titleProperty
            << artistProperty.name()
            << QLatin1String("album")
            << QLatin1String("trackNumber");

    QGalleryItemRequest request;

    QSignalSpy spy(&request, SIGNAL(propertyNamesChanged()));

    QCOMPARE(request.propertyNames(), QStringList());

    request.setPropertyNames(QStringList());
    QCOMPARE(request.propertyNames(), QStringList());
    QCOMPARE(spy.count(), 0);

    request.setPropertyNames(propertyNames);
    QCOMPARE(request.propertyNames(), propertyNames);
    QCOMPARE(spy.count(), 1);

    request.setPropertyNames(propertyNames);
    QCOMPARE(request.propertyNames(), propertyNames);
    QCOMPARE(spy.count(), 1);

    request.setPropertyNames(QStringList());
    QCOMPARE(request.propertyNames(), QStringList());
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryItemRequest::autoUpdate()
{
    QGalleryItemRequest request;

    QSignalSpy spy(&request, SIGNAL(autoUpdateChanged()));

    QCOMPARE(request.autoUpdate(), false);

    request.setAutoUpdate(false);
    QCOMPARE(request.autoUpdate(), false);
    QCOMPARE(spy.count(), 0);

    request.setAutoUpdate(true);
    QCOMPARE(request.autoUpdate(), true);
    QCOMPARE(spy.count(), 1);

    request.setAutoUpdate(true);
    QCOMPARE(request.autoUpdate(), true);
    QCOMPARE(spy.count(), 1);

    request.setAutoUpdate(false);
    QCOMPARE(request.autoUpdate(), false);
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryItemRequest::itemId()
{
    QGalleryItemRequest request;

    QSignalSpy spy(&request, SIGNAL(itemIdChanged()));

    QCOMPARE(request.itemId(), QVariant());

    request.setItemId(QVariant());
    QCOMPARE(request.itemId(), QVariant());
    QCOMPARE(spy.count(), 0);

    request.setItemId(76);
    QCOMPARE(request.itemId(), QVariant(76));
    QCOMPARE(spy.count(), 1);

    request.setItemId(76);
    QCOMPARE(request.itemId(), QVariant(76));
    QCOMPARE(spy.count(), 1);

    request.setItemId(QLatin1String("65"));
    QCOMPARE(request.itemId(), QVariant(QLatin1String("65")));
    QCOMPARE(spy.count(), 2);

    request.setItemId(QLatin1String("65"));
    QCOMPARE(request.itemId(), QVariant(QLatin1String("65")));
    QCOMPARE(spy.count(), 2);

    request.setItemId(QVariant());
    QCOMPARE(request.itemId(), QVariant());
    QCOMPARE(spy.count(), 3);
}

void tst_QGalleryItemRequest::executeSynchronous()
{
    QtTestGallery gallery;
    gallery.setState(QGalleryAbstractRequest::Finished);
    gallery.setCount(1);
    gallery.setError(80, QString());

    QGalleryItemRequest request(&gallery);
    QVERIFY(request.resultSet() == 0);

    request.setPropertyNames(QStringList()
            << QLatin1String("album")
            << QLatin1String("trackNumber")
            << QLatin1String("turtle"));

    QSignalSpy resultSetSpy(&request, SIGNAL(resultSetChanged(QGalleryResultSet*)));
    QSignalSpy itemChangedSpy(&request, SIGNAL(itemChanged()));
    QSignalSpy metaDataSpy(&request, SIGNAL(metaDataChanged(QList<int>)));

    request.execute();

    QCOMPARE(request.error(), 80);
    QCOMPARE(request.state(), QGalleryAbstractRequest::Error);
    QCOMPARE(resultSetSpy.count(), 0);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) == 0);

    gallery.setError(QGalleryAbstractRequest::NoError, QString());
    request.execute();
    QCOMPARE(request.state(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(resultSetSpy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);
    QCOMPARE(resultSetSpy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());

    QCOMPARE(request.propertyKey(QLatin1String("title")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("album")), 0);
    QCOMPARE(request.propertyKey(QLatin1String("trackNumber")), 1);
    QCOMPARE(request.propertyKey(QLatin1String("turtle")), -1);

    QCOMPARE(request.propertyAttributes(0), QGalleryProperty::CanRead | QGalleryProperty::CanWrite);
    QCOMPARE(request.propertyType(0), QVariant::String);

    const QList<int> propertyKeys = QList<int>()
            << request.propertyKey(QLatin1String("album"))
            << request.propertyKey(QLatin1String("trackNumber"));

    QCOMPARE(itemChangedSpy.count(), 1);
    QCOMPARE(metaDataSpy.count(), 1);
    QCOMPARE(metaDataSpy.last().value(0).value<QList<int> >(), propertyKeys);

    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.itemUrl(), QUrl(QLatin1String("http://example.com")));
    QCOMPARE(request.itemType(), QLatin1String("WebPage"));
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.setMetaData(1, 12), true);
    QCOMPARE(request.metaData(1), QVariant(12));
    QCOMPARE(metaDataSpy.count(), 2);
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant(12));
    QCOMPARE(request.setMetaData(QLatin1String("trackNumber"), 5), true);
    QCOMPARE(metaDataSpy.count(), 3);
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant(5));
    QCOMPARE(request.resources(), QList<QGalleryResource>()
            << QGalleryResource(QUrl(QLatin1String("http://example.com"))));

    request.clear();
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(resultSetSpy.count(), 2);
    QVERIFY(request.resultSet() == 0);
    QCOMPARE(resultSetSpy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());
    QCOMPARE(itemChangedSpy.count(), 2);
    QCOMPARE(metaDataSpy.count(), 3);
}

void tst_QGalleryItemRequest::executeAsynchronous()
{
    QtTestGallery gallery;
    gallery.setState(QGalleryAbstractRequest::Active);

    QGalleryItemRequest request(&gallery);
    QVERIFY(request.resultSet() == 0);

    request.setPropertyNames(QStringList()
            << QLatin1String("album")
            << QLatin1String("trackNumber")
            << QLatin1String("turtle"));

    QSignalSpy resultSetSpy(&request, SIGNAL(resultSetChanged(QGalleryResultSet*)));
    QSignalSpy itemChangedSpy(&request, SIGNAL(itemChanged()));
    QSignalSpy metaDataSpy(&request, SIGNAL(metaDataChanged(QList<int>)));

    request.execute();
    QCOMPARE(request.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(resultSetSpy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);
    QCOMPARE(resultSetSpy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());

    QCOMPARE(request.propertyKey(QLatin1String("title")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("album")), 0);
    QCOMPARE(request.propertyKey(QLatin1String("trackNumber")), 1);
    QCOMPARE(request.propertyKey(QLatin1String("turtle")), -1);

    QCOMPARE(request.propertyAttributes(0), QGalleryProperty::CanRead | QGalleryProperty::CanWrite);
    QCOMPARE(request.propertyType(0), QVariant::String);

    const QList<int> propertyKeys = QList<int>()
            << request.propertyKey(QLatin1String("album"))
            << request.propertyKey(QLatin1String("trackNumber"));

    QCOMPARE(itemChangedSpy.count(), 0);
    QCOMPARE(metaDataSpy.count(), 0);

    QCOMPARE(request.isValid(), false);
    QCOMPARE(request.itemUrl(), QUrl());
    QCOMPARE(request.itemType(), QString());
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.setMetaData(1, 12), false);
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());
    QCOMPARE(request.setMetaData(QLatin1String("trackNumber"), 12), false);
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());
    QCOMPARE(request.resources(), QList<QGalleryResource>());

    QtGalleryTestResponse *resultSet = qobject_cast<QtGalleryTestResponse *>(request.resultSet());
    QVERIFY(resultSet != 0);

    resultSet->setCount(1);
    resultSet->itemsInserted(0, 1);

    QCOMPARE(itemChangedSpy.count(), 1);
    QCOMPARE(metaDataSpy.count(), 1);
    QCOMPARE(metaDataSpy.last().value(0).value<QList<int> >(), propertyKeys);

    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.itemUrl(), QUrl(QLatin1String("http://example.com")));
    QCOMPARE(request.itemType(), QLatin1String("WebPage"));
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.setMetaData(1, 12), true);
    QCOMPARE(metaDataSpy.count(), 2);
    QCOMPARE(request.metaData(1), QVariant(12));
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant(12));
    QCOMPARE(request.setMetaData(QLatin1String("trackNumber"), 5), true);
    QCOMPARE(metaDataSpy.count(), 3);
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant(5));
    QCOMPARE(request.resources(), QList<QGalleryResource>()
            << QGalleryResource(QUrl(QLatin1String("http://example.com"))));

    resultSet->finish(false);

    QCOMPARE(request.state(), QGalleryAbstractRequest::Finished);
    QCOMPARE(resultSetSpy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);

    resultSet->setCount(0);
    resultSet->itemsRemoved(0, 1);

    QCOMPARE(itemChangedSpy.count(), 2);
    QCOMPARE(metaDataSpy.count(), 4);
    QCOMPARE(metaDataSpy.last().value(0).value<QList<int> >(), propertyKeys);

    QCOMPARE(request.isValid(), false);
    QCOMPARE(request.itemUrl(), QUrl());
    QCOMPARE(request.itemType(), QString());
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.setMetaData(1, 12), false);
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());
    QCOMPARE(request.setMetaData(QLatin1String("trackNumber"), 12), false);
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());
    QCOMPARE(request.resources(), QList<QGalleryResource>());

    request.clear();
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(resultSetSpy.count(), 2);
    QVERIFY(request.resultSet() == 0);
    QCOMPARE(resultSetSpy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());

    QCOMPARE(itemChangedSpy.count(), 2);
    QCOMPARE(metaDataSpy.count(), 4);
}

void tst_QGalleryItemRequest::noResponse()
{
    QGalleryItemRequest request;

    QCOMPARE(request.propertyKey(QLatin1String("title")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("album")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("trackNumber")), -1);

    QCOMPARE(request.propertyAttributes(0), QGalleryProperty::Attributes());
    QCOMPARE(request.propertyType(0), QVariant::Invalid);

    QCOMPARE(request.isValid(), false);
    QCOMPARE(request.itemUrl(), QUrl());
    QCOMPARE(request.itemType(), QString());
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.setMetaData(1, QLatin1String("hello")), false);
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.metaData(QLatin1String("title")), QVariant());
    QCOMPARE(request.setMetaData(QLatin1String("title"), QLatin1String("hello")), false);
    QCOMPARE(request.metaData(QLatin1String("title")), QVariant());
    QCOMPARE(request.resources(), QList<QGalleryResource>());
}

void tst_QGalleryItemRequest::multipleResults()
{
    const QList<int> propertyKeys = QList<int>() << 2 << 15;

    QtTestGallery gallery;
    gallery.setCount(1);

    QGalleryItemRequest request(&gallery);

    QSignalSpy itemChangedSpy(&request, SIGNAL(itemChanged()));
    QSignalSpy metaDataSpy(&request, SIGNAL(metaDataChanged(QList<int>)));

    gallery.setState(QGalleryAbstractRequest::Active);
    request.execute();

    QCOMPARE(request.isValid(), true);
    QCOMPARE(itemChangedSpy.count(), 1);
    QCOMPARE(metaDataSpy.count(), 0);

    QtGalleryTestResponse *resultSet = qobject_cast<QtGalleryTestResponse *>(
            request.resultSet());
    QVERIFY(resultSet != 0);

    resultSet->metaDataChanged(0, 1, propertyKeys);
    QCOMPARE(metaDataSpy.count(), 1);
    QCOMPARE(metaDataSpy.last().value(0).value<QList<int> >(), propertyKeys);

    resultSet->setCount(3);
    resultSet->itemsInserted(1, 2);

    QCOMPARE(itemChangedSpy.count(), 1);
    QCOMPARE(metaDataSpy.count(), 1);

    resultSet->metaDataChanged(0, 1, propertyKeys);
    QCOMPARE(metaDataSpy.count(), 2);
    QCOMPARE(metaDataSpy.last().value(0).value<QList<int> >(), propertyKeys);

    resultSet->metaDataChanged(1, 1, propertyKeys);
    QCOMPARE(metaDataSpy.count(), 2);

    resultSet->itemsMoved(0, 1, 1);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(itemChangedSpy.count(), 2);
    QCOMPARE(metaDataSpy.count(), 2);

    resultSet->itemsMoved(2, 0, 1);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(itemChangedSpy.count(), 3);
    QCOMPARE(metaDataSpy.count(), 2);

    resultSet->itemsMoved(1, 2, 1);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(itemChangedSpy.count(), 3);
    QCOMPARE(metaDataSpy.count(), 2);

    resultSet->setCount(1);
    resultSet->itemsRemoved(1, 1);

    QCOMPARE(request.isValid(), true);
    QCOMPARE(itemChangedSpy.count(), 3);
    QCOMPARE(metaDataSpy.count(), 2);

}

QTEST_MAIN(tst_QGalleryItemRequest)

#include "tst_qgalleryitemrequest.moc"
