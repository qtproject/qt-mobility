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

//TESTED_COMPONENT=src/documentgallery

#include <qgalleryqueryrequest.h>

#include <qabstractgallery.h>
#include <qgalleryabstractresponse.h>
#include <qgalleryresultset.h>
#include <qgalleryresource.h>
#include <qgallerytype.h>

#include <QtTest/QtTest>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGalleryResultSet*)

class tst_QGalleryQueryRequest : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void properties();
    void executeSynchronous();
    void executeAsynchronous();
    void noResponse();
};


class QtGalleryTestResponse : public QGalleryResultSet
{
    Q_OBJECT
public:
    QtGalleryTestResponse(const QStringList &propertyNames, int count, int result, bool idle)
        : m_count(count)
        , m_currentIndex(-1)
        , m_propertyNames(propertyNames)
    {
        if (result != QGalleryAbstractRequest::NoResult)
            finish(result, idle);
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

    void doFinish(int result, bool idle) { finish(result, idle); }

    void setCount(int count) { m_count = count; }

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
    QtTestGallery() : m_count(1), m_result(QGalleryAbstractRequest::NoResult), m_idle(false) {}

    bool isRequestSupported(QGalleryAbstractRequest::RequestType type) const {
        return type == QGalleryAbstractRequest::QueryRequest; }

    void setResult(int result) { m_result = result; }
    void setIdle(bool idle) { m_idle = idle; }

    void setCount(int count) { m_count = count; }

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if (request->type() == QGalleryAbstractRequest::QueryRequest) {
            return new QtGalleryTestResponse(
                    static_cast<QGalleryQueryRequest *>(request)->propertyNames(),
                    m_count,
                    m_result,
                    m_idle);
        }
        return 0;
    }

private:
    int m_count;
    int m_result;
    bool m_idle;
};


void tst_QGalleryQueryRequest::initTestCase()
{
    qRegisterMetaType<QGalleryResultSet*>();
}

void tst_QGalleryQueryRequest::properties()
{
    const QGalleryProperty titleProperty("title");
    const QGalleryProperty artistProperty("artist");
    const QGalleryProperty albumProperty("album");
    const QGalleryType audioType("Audio");
    const QGalleryType videoType("Video");

    const QGalleryFilter filter = QGalleryMetaDataFilter(QLatin1String("trackNumber"), 12);

    QGalleryQueryRequest request;

    QCOMPARE(request.propertyNames(), QStringList());
    QCOMPARE(request.sortPropertyNames(), QStringList());
    QCOMPARE(request.isLive(), false);
    QCOMPARE(request.offset(), 0);
    QCOMPARE(request.limit(), 0);
    QCOMPARE(request.rootType(), QString());
    QCOMPARE(request.scope(), QGalleryQueryRequest::AllDescendants);
    QCOMPARE(request.rootItem(), QVariant());
    QCOMPARE(request.filter(), QGalleryFilter());

    request.setPropertyNames(QStringList()
            << titleProperty
            << artistProperty.name()
            << QLatin1String("album")
            << QString::fromLatin1("trackNumber"));
    QCOMPARE(request.propertyNames(), QStringList()
            << QLatin1String("title")
            << QLatin1String("artist")
            << QLatin1String("album")
            << QLatin1String("trackNumber"));

    request.setSortPropertyNames(QStringList()
            << artistProperty.ascending()
            << albumProperty.descending()
            << QLatin1String("-trackNumber")
            << titleProperty);
    QCOMPARE(request.sortPropertyNames(), QStringList()
             << QLatin1String("+artist")
             << QLatin1String("-album")
             << QLatin1String("-trackNumber")
             << QLatin1String("title"));

    request.setLive(true);
    QCOMPARE(request.isLive(), true);

    request.setOffset(140);
    QCOMPARE(request.offset(), 140);

    request.setOffset(-93);
    QCOMPARE(request.offset(), 0);

    request.setLimit(45);
    QCOMPARE(request.limit(), 45);

    request.setLimit(-3);
    QCOMPARE(request.limit(), 0);

    request.setRootType(audioType);
    QCOMPARE(request.rootType(), QString::fromLatin1("Audio"));

    request.setRootType(videoType.name());
    QCOMPARE(request.rootType(), QString::fromLatin1("Video"));

    request.setScope(QGalleryQueryRequest::DirectDescendants);
    QCOMPARE(request.scope(), QGalleryQueryRequest::DirectDescendants);

    request.setRootItem(QVariant(76));
    QCOMPARE(request.rootItem(), QVariant(76));

    request.setRootItem(QVariant(QLatin1String("65")));
    QCOMPARE(request.rootItem(), QVariant(QLatin1String("65")));

    request.setFilter(filter);
    QCOMPARE(request.filter(), filter);
}

void tst_QGalleryQueryRequest::executeSynchronous()
{
    QtTestGallery gallery;
    gallery.setResult(QGalleryAbstractRequest::ConnectionError);

    QGalleryQueryRequest request(&gallery);
    QVERIFY(request.resultSet() == 0);

    request.setPropertyNames(QStringList()
            << QLatin1String("album")
            << QLatin1String("trackNumber"));

    QSignalSpy spy(&request, SIGNAL(resultSetChanged(QGalleryResultSet*)));

    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::ConnectionError));
    QCOMPARE(spy.count(), 0);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) == 0);

    gallery.setResult(QGalleryAbstractRequest::Succeeded);
    gallery.setCount(10);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(spy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);
    QCOMPARE(spy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());

    QCOMPARE(request.propertyKey(QLatin1String("title")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("album")), 0);
    QCOMPARE(request.propertyKey(QLatin1String("trackNumber")), 1);

    QCOMPARE(request.propertyAttributes(0), QGalleryProperty::CanRead | QGalleryProperty::CanWrite);
    QCOMPARE(request.propertyType(0), QVariant::String);

    QCOMPARE(request.itemCount(), 10);
    QCOMPARE(request.currentIndex(), -1);
    QCOMPARE(request.itemId(), QVariant());
    QCOMPARE(request.itemUrl(), QUrl());
    QCOMPARE(request.itemType(), QString());
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.setMetaData(1, 12), false);
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());
    QCOMPARE(request.setMetaData(QLatin1String("trackNumber"), 12), false);
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());
    QCOMPARE(request.resources(), QList<QGalleryResource>());

    QCOMPARE(request.seek(0, false), true);
    QCOMPARE(request.currentIndex(), 0);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.seek(1, true), true);
    QCOMPARE(request.currentIndex(), 1);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.seek(-1, true), true);
    QCOMPARE(request.currentIndex(), 0);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.seek(-1, false), false);
    QCOMPARE(request.currentIndex(), -1);
    QCOMPARE(request.isValid(), false);
    QCOMPARE(request.last(), true);
    QCOMPARE(request.currentIndex(), 9);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.next(), false);
    QCOMPARE(request.currentIndex(), 10);
    QCOMPARE(request.isValid(), false);
    QCOMPARE(request.previous(), true);
    QCOMPARE(request.currentIndex(), 9);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.first(), true);
    QCOMPARE(request.currentIndex(), 0);
    QCOMPARE(request.previous(), false);
    QCOMPARE(request.currentIndex(), -1);
    QCOMPARE(request.isValid(), false);
    QCOMPARE(request.next(), true);
    QCOMPARE(request.currentIndex(), 0);
    QCOMPARE(request.isValid(), true);

    QCOMPARE(request.itemId(), QVariant(1));
    QCOMPARE(request.itemUrl(), QUrl(QLatin1String("http://example.com")));
    QCOMPARE(request.itemType(), QLatin1String("WebPage"));
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.setMetaData(1, 12), true);
    QCOMPARE(request.metaData(1), QVariant(12));
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant(12));
    QCOMPARE(request.setMetaData(QLatin1String("trackNumber"), 5), true);
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant(5));
    QCOMPARE(request.resources(), QList<QGalleryResource>()
            << QGalleryResource(QUrl(QLatin1String("http://example.com"))));

    QCOMPARE(request.previous(), false);
    QCOMPARE(request.currentIndex(), -1);

    QCOMPARE(request.itemId(), QVariant());
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
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(spy.count(), 2);
    QVERIFY(request.resultSet() == 0);
    QCOMPARE(spy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());
}

void tst_QGalleryQueryRequest::executeAsynchronous()
{
    QtTestGallery gallery;
    gallery.setResult(QGalleryAbstractRequest::NoResult);

    QGalleryQueryRequest request(&gallery);
    QVERIFY(request.resultSet() == 0);

    QSignalSpy spy(&request, SIGNAL(resultSetChanged(QGalleryResultSet*)));

    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(spy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);
    QCOMPARE(spy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());

    qobject_cast<QtGalleryTestResponse *>(request.resultSet())->doFinish(
            QGalleryAbstractRequest::Succeeded, false);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(spy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);

    request.clear();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(spy.count(), 2);
    QVERIFY(request.resultSet() == 0);
    QCOMPARE(spy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());
}

void tst_QGalleryQueryRequest::noResponse()
{
    QGalleryQueryRequest request;

    QCOMPARE(request.propertyKey(QLatin1String("title")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("album")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("trackNumber")), -1);

    QCOMPARE(request.propertyAttributes(0), QGalleryProperty::Attributes());
    QCOMPARE(request.propertyType(0), QVariant::Invalid);

    QCOMPARE(request.itemCount(), 0);
    QCOMPARE(request.currentIndex(), -1);
    QCOMPARE(request.itemId(), QVariant());
    QCOMPARE(request.itemUrl(), QUrl());
    QCOMPARE(request.itemType(), QString());
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.setMetaData(1, QLatin1String("hello")), false);
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.metaData(QLatin1String("title")), QVariant());
    QCOMPARE(request.setMetaData(QLatin1String("title"), QLatin1String("hello")), false);
    QCOMPARE(request.metaData(QLatin1String("title")), QVariant());
    QCOMPARE(request.resources(), QList<QGalleryResource>());
    QCOMPARE(request.seek(0, false), false);
    QCOMPARE(request.seek(1, true), false);
    QCOMPARE(request.seek(-1, true), false);
    QCOMPARE(request.next(), false);
    QCOMPARE(request.previous(), false);
}

QTEST_MAIN(tst_QGalleryQueryRequest)

#include "tst_qgalleryqueryrequest.moc"
