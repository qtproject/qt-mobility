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

#include <qgallerytyperequest.h>

#include <qabstractgallery.h>
#include <qgalleryabstractresponse.h>
#include <qgalleryresultset.h>
#include <qgalleryresource.h>
#include <qgallerytype.h>

#include <QtTest/QtTest>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QGalleryResultSet*)

class tst_QGalleryTypeRequest : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void properties();
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
            QGalleryAbstractRequest::Status status,
            int error,
            const QString &errorString)
        : m_count(count)
        , m_currentIndex(-1)
        , m_propertyNames(propertyNames)
    {
        m_propertyNames.removeAll(QLatin1String("turtle"));

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
        , m_status(QGalleryAbstractRequest::Active)
        , m_error(QGalleryAbstractRequest::NoError)
    {}

    bool isRequestSupported(QGalleryAbstractRequest::RequestType type) const {
        return type == QGalleryAbstractRequest::TypeRequest; }

    void setStatus(QGalleryAbstractRequest::Status status) { m_status = status; }
    void setError(int error, const QString &errorString) {
        m_error = error; m_errorString = errorString; }

    void setCount(int count) { m_count = count; }

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if (request->type() == QGalleryAbstractRequest::TypeRequest) {
            return new QtGalleryTestResponse(
                    static_cast<QGalleryTypeRequest *>(request)->propertyNames(),
                    m_count,
                    m_status,
                    m_error,
                    m_errorString);
        }
        return 0;
    }

private:
    int m_count;
    QGalleryAbstractRequest::Status m_status;
    int m_error;
    QString m_errorString;
};

void tst_QGalleryTypeRequest::initTestCase()
{
    qRegisterMetaType<QGalleryResultSet*>();
    qRegisterMetaType<QList<int> >();
}

void tst_QGalleryTypeRequest::properties()
{
    const QGalleryProperty titleProperty("title");
    const QGalleryProperty artistProperty("artist");

    const QGalleryType audioType("Audio");
    const QGalleryType videoType("Video");

    const QGalleryFilter filter = QGalleryMetaDataFilter(QLatin1String("trackNumber"), 12);

    QGalleryTypeRequest request;

    QCOMPARE(request.propertyNames(), QStringList());
    QCOMPARE(request.autoUpdate(), false);
    QCOMPARE(request.itemType(), QString());

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

    request.setAutoUpdate(true);
    QCOMPARE(request.autoUpdate(), true);

    request.setItemType(audioType);
    QCOMPARE(request.itemType(), QString::fromLatin1("Audio"));

    request.setItemType(videoType.name());
    QCOMPARE(request.itemType(), QString::fromLatin1("Video"));
}

void tst_QGalleryTypeRequest::executeSynchronous()
{
    QtTestGallery gallery;
    gallery.setStatus(QGalleryAbstractRequest::Finished);
    gallery.setCount(1);
    gallery.setError(80, QString());

    QGalleryTypeRequest request(&gallery);
    QVERIFY(request.resultSet() == 0);

    request.setPropertyNames(QStringList()
            << QLatin1String("album")
            << QLatin1String("trackNumber")
            << QLatin1String("turtle"));

    QSignalSpy resultSetSpy(&request, SIGNAL(resultSetChanged(QGalleryResultSet*)));
    QSignalSpy typeChangedSpy(&request, SIGNAL(typeChanged()));
    QSignalSpy metaDataSpy(&request, SIGNAL(metaDataChanged(QList<int>)));

    request.execute();

    QCOMPARE(request.error(), 80);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Error);
    QCOMPARE(resultSetSpy.count(), 0);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) == 0);

    gallery.setError(QGalleryAbstractRequest::NoError, QString());
    request.execute();
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(resultSetSpy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);
    QCOMPARE(resultSetSpy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());

    QtGalleryTestResponse *resultSet = qobject_cast<QtGalleryTestResponse *>(
            request.resultSet());
    QVERIFY(resultSet != 0);

    QCOMPARE(request.propertyKey(QLatin1String("title")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("album")), 0);
    QCOMPARE(request.propertyKey(QLatin1String("trackNumber")), 1);
    QCOMPARE(request.propertyKey(QLatin1String("turtle")), -1);

    QCOMPARE(request.propertyAttributes(0), QGalleryProperty::CanRead | QGalleryProperty::CanWrite);
    QCOMPARE(request.propertyType(0), QVariant::String);

    const QList<int> propertyKeys = QList<int>()
            << request.propertyKey(QLatin1String("album"))
            << request.propertyKey(QLatin1String("trackNumber"));

    QCOMPARE(typeChangedSpy.count(), 1);
    QCOMPARE(metaDataSpy.count(), 1);
    QCOMPARE(metaDataSpy.last().value(0).value<QList<int> >(), propertyKeys);

    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());
    QCOMPARE(resultSet->setMetaData(1, 12), true);
    QCOMPARE(request.metaData(1), QVariant(12));
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant(12));
    QCOMPARE(metaDataSpy.count(), 2);

    request.clear();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(resultSetSpy.count(), 2);
    QVERIFY(request.resultSet() == 0);
    QCOMPARE(resultSetSpy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());
    QCOMPARE(typeChangedSpy.count(), 2);
    QCOMPARE(metaDataSpy.count(), 2);
}

void tst_QGalleryTypeRequest::executeAsynchronous()
{
    QtTestGallery gallery;
    gallery.setStatus(QGalleryAbstractRequest::Active);

    QGalleryTypeRequest request(&gallery);
    QVERIFY(request.resultSet() == 0);

    request.setPropertyNames(QStringList()
            << QLatin1String("album")
            << QLatin1String("trackNumber")
            << QLatin1String("turtle"));

    QSignalSpy resultSetSpy(&request, SIGNAL(resultSetChanged(QGalleryResultSet*)));
    QSignalSpy typeChangedSpy(&request, SIGNAL(typeChanged()));
    QSignalSpy metaDataSpy(&request, SIGNAL(metaDataChanged(QList<int>)));

    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(resultSetSpy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);
    QCOMPARE(resultSetSpy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());

    QtGalleryTestResponse *resultSet = qobject_cast<QtGalleryTestResponse *>(
            request.resultSet());
    QVERIFY(resultSet != 0);

    QCOMPARE(request.propertyKey(QLatin1String("title")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("album")), 0);
    QCOMPARE(request.propertyKey(QLatin1String("trackNumber")), 1);
    QCOMPARE(request.propertyKey(QLatin1String("turtle")), -1);

    QCOMPARE(request.propertyAttributes(0), QGalleryProperty::CanRead | QGalleryProperty::CanWrite);
    QCOMPARE(request.propertyType(0), QVariant::String);

    const QList<int> propertyKeys = QList<int>()
            << request.propertyKey(QLatin1String("album"))
            << request.propertyKey(QLatin1String("trackNumber"));

    QCOMPARE(typeChangedSpy.count(), 0);
    QCOMPARE(metaDataSpy.count(), 0);

    QCOMPARE(request.isValid(), false);
    QCOMPARE(request.metaData(1), QVariant());

    resultSet->setCount(1);
    resultSet->itemsInserted(0, 1);

    QCOMPARE(typeChangedSpy.count(), 1);
    QCOMPARE(metaDataSpy.count(), 1);
    QCOMPARE(metaDataSpy.last().value(0).value<QList<int> >(), propertyKeys);

    QCOMPARE(request.isValid(), true);
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());
    QCOMPARE(resultSet->setMetaData(1, 12), true);
    QCOMPARE(request.metaData(1), QVariant(12));
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant(12));
    QCOMPARE(metaDataSpy.count(), 2);

    resultSet->finish(false);

    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(resultSetSpy.count(), 1);
    QVERIFY(qobject_cast<QtGalleryTestResponse *>(request.resultSet()) != 0);

    resultSet->setCount(0);
    resultSet->itemsRemoved(0, 1);

    QCOMPARE(typeChangedSpy.count(), 2);
    QCOMPARE(metaDataSpy.count(), 3);
    QCOMPARE(metaDataSpy.last().value(0).value<QList<int> >(), propertyKeys);

    QCOMPARE(request.isValid(), false);
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.metaData(QLatin1String("trackNumber")), QVariant());


    request.clear();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(resultSetSpy.count(), 2);
    QVERIFY(request.resultSet() == 0);
    QCOMPARE(resultSetSpy.last().at(0).value<QGalleryResultSet*>(), request.resultSet());

    QCOMPARE(typeChangedSpy.count(), 2);
    QCOMPARE(metaDataSpy.count(), 3);
}

void tst_QGalleryTypeRequest::noResponse()
{
    QGalleryTypeRequest request;

    QCOMPARE(request.propertyKey(QLatin1String("title")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("album")), -1);
    QCOMPARE(request.propertyKey(QLatin1String("trackNumber")), -1);

    QCOMPARE(request.propertyAttributes(0), QGalleryProperty::Attributes());
    QCOMPARE(request.propertyType(0), QVariant::Invalid);

    QCOMPARE(request.isValid(), false);
    QCOMPARE(request.metaData(1), QVariant());
    QCOMPARE(request.metaData(QLatin1String("title")), QVariant());
}

void tst_QGalleryTypeRequest::multipleResults()
{
    const QList<int> propertyKeys = QList<int>() << 2 << 15;

    QtTestGallery gallery;
    gallery.setCount(1);

    QGalleryTypeRequest request(&gallery);

    QSignalSpy typeChangedSpy(&request, SIGNAL(typeChanged()));
    QSignalSpy metaDataSpy(&request, SIGNAL(metaDataChanged(QList<int>)));

    request.execute();

    QCOMPARE(request.isValid(), true);
    QCOMPARE(typeChangedSpy.count(), 1);
    QCOMPARE(metaDataSpy.count(), 0);

    QtGalleryTestResponse *resultSet = qobject_cast<QtGalleryTestResponse *>(
            request.resultSet());
    QVERIFY(resultSet != 0);

    resultSet->metaDataChanged(0, 1, propertyKeys);
    QCOMPARE(metaDataSpy.count(), 1);
    QCOMPARE(metaDataSpy.last().value(0).value<QList<int> >(), propertyKeys);

    resultSet->setCount(3);
    resultSet->itemsInserted(1, 2);

    QCOMPARE(typeChangedSpy.count(), 1);
    QCOMPARE(metaDataSpy.count(), 1);

    resultSet->metaDataChanged(0, 1, propertyKeys);
    QCOMPARE(metaDataSpy.count(), 2);
    QCOMPARE(metaDataSpy.last().value(0).value<QList<int> >(), propertyKeys);

    resultSet->metaDataChanged(1, 1, propertyKeys);
    QCOMPARE(metaDataSpy.count(), 2);

    resultSet->itemsMoved(0, 1, 1);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(typeChangedSpy.count(), 2);
    QCOMPARE(metaDataSpy.count(), 2);

    resultSet->itemsMoved(2, 0, 1);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(typeChangedSpy.count(), 3);
    QCOMPARE(metaDataSpy.count(), 2);

    resultSet->itemsMoved(1, 2, 1);
    QCOMPARE(request.isValid(), true);
    QCOMPARE(typeChangedSpy.count(), 3);
    QCOMPARE(metaDataSpy.count(), 2);

    resultSet->setCount(1);
    resultSet->itemsRemoved(1, 1);

    QCOMPARE(request.isValid(), true);
    QCOMPARE(typeChangedSpy.count(), 3);
    QCOMPARE(metaDataSpy.count(), 2);
}

QTEST_MAIN(tst_QGalleryTypeRequest)

#include "tst_qgallerytyperequest.moc"
