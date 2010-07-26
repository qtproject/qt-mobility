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

#include <QtTest/QtTest>

#include <qgalleryqueryrequest.h>

#include <qabstractgallery.h>
#include <qgalleryabstractresponse.h>
#include <qgalleryresultset.h>
#include <qgalleryresource.h>
#include <qgallerytype.h>

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
};

class QtGalleryTestResponse : public QGalleryResultSet
{
    Q_OBJECT
public:
    QtGalleryTestResponse(int result, bool idle)
    {
        if (result != QGalleryAbstractRequest::NoResult)
            finish(result, idle);
    }

    int propertyKey(const QString &) const { return -1; }
    QGalleryProperty::Attributes propertyAttributes(int) const {
        return QGalleryProperty::Attributes(); }
    QVariant::Type propertyType(int) const { return QVariant::Invalid; }

    int itemCount() const { return 0; }

    int currentIndex() const { return m_currentIndex; }

    bool seek(int index, bool relative) {
        m_currentIndex = relative ? m_currentIndex + index : index; return false; }

    QVariant itemId() const { return QVariant(); }
    QUrl itemUrl() const { return QUrl(); }
    QString itemType() const { return QString(); }
    QList<QGalleryResource> resources() const { return QList<QGalleryResource>(); }

    QVariant metaData(int) const { return QVariant(); }
    bool setMetaData(int, const QVariant &) { return false; }

    bool waitForFinished(int) { return false; }

    void doFinish(int result, bool idle) { finish(result, idle); }

private:
    int m_currentIndex;
};

class QtTestGallery : public QAbstractGallery
{
public:
    QtTestGallery() : m_result(QGalleryAbstractRequest::NoResult), m_idle(false) {}

    bool isRequestSupported(QGalleryAbstractRequest::Type type) const {
        return type == QGalleryAbstractRequest::Query; }

    void setResult(int result) { m_result = result; }
    void setIdle(bool idle) { m_idle = idle; }

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if (request->type() == QGalleryAbstractRequest::Query)
            return new QtGalleryTestResponse(m_result, m_idle);
        return 0;
    }

private:
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
    QCOMPARE(request.scope(), QGalleryAbstractRequest::AllDescendants);
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

    request.setScope(QGalleryAbstractRequest::DirectDescendants);
    QCOMPARE(request.scope(), QGalleryAbstractRequest::DirectDescendants);

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

    QSignalSpy spy(&request, SIGNAL(resultSetChanged(QGalleryResultSet*)));

    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::ConnectionError));
    QCOMPARE(spy.count(), 0);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) == 0);

    gallery.setResult(QGalleryAbstractRequest::Succeeded);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(spy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);
    QCOMPARE(spy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());

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

QTEST_MAIN(tst_QGalleryQueryRequest)

#include "tst_qgalleryqueryrequest.moc"
