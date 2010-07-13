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

#include <qgallerycountrequest.h>

#include <qabstractgallery.h>
#include <qgalleryabstractresponse.h>
#include <qgallerytype.h>

QTM_USE_NAMESPACE

class tst_QGalleryCountRequest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void properties();
    void executeSynchronous();
    void executeAsynchronous();
};

class QtGalleryTestResponse : public QGalleryAbstractResponse
{
    Q_OBJECT
public:
    QtGalleryTestResponse(int count, int result, bool idle)
        : m_count(count)
    {
        if (result != QGalleryAbstractRequest::NoResult)
            finish(result, idle);
    }

    QStringList propertyNames() const { return QStringList(); }
    int propertyKey(const QString &) const { return -1; }
    QGalleryProperty::Attributes propertyAttributes(int) const {
        return QGalleryProperty::Attributes(); }
    QVariant::Type propertyType(int) const { return QVariant::Invalid; }

    int count() const { return m_count; }

    QVariant id(int) const { return QVariant(); }
    QUrl url(int) const { return QUrl(); }
    QString type(int) const { return QString(); }
    QString parentId(int) const { return QString(); }
    QList<QGalleryResource> resources(int) const { return QList<QGalleryResource>(); }
    ItemStatus status(int) const { return ItemStatus(); }

    QVariant metaData(int, int) const { return QVariant(); }
    void setMetaData(int, int, const QVariant &) {}

    bool waitForFinished(int) { return false; }

public Q_SLOTS:

    void doFinish(int result, bool idle) { finish(result, idle); }

    void insert(int count) { m_count += count; inserted(0, count); }
    void remove(int count) { m_count -= count; removed(0, count); }

private:
    int m_count;
};

class QtTestGallery : public QAbstractGallery
{
    Q_OBJECT
public:
    QtTestGallery() : m_count(0), m_result(QGalleryAbstractRequest::NoResult), m_idle(false) {}

    bool isRequestSupported(QGalleryAbstractRequest::Type type) const {
        return type == QGalleryAbstractRequest::Count; }

    void setCount(int count) { m_count = count; }
    void setResult(int result) { m_result = result; }
    void setIdle(bool idle) { m_idle = idle; }

#ifdef Q_MOC_RUN
Q_SIGNALS:
#endif
    void insert(int count);
    void remove(int count);
    void finish(int result, bool idle);

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if (request->type() == QGalleryAbstractRequest::Count) {
            QtGalleryTestResponse *response = new QtGalleryTestResponse(m_count, m_result, m_idle);

            connect(this, SIGNAL(finish(int,bool)), response, SLOT(doFinish(int,bool)));
            connect(this, SIGNAL(insert(int)), response, SLOT(insert(int)));
            connect(this, SIGNAL(remove(int)), response, SLOT(remove(int)));

            return response;
        }
        return 0;
    }

private:
    int m_count;
    int m_result;
    bool m_idle;
};

void tst_QGalleryCountRequest::properties()
{
    const QGalleryType audioType("Audio");
    const QGalleryType videoType("Video");

    const QGalleryFilter filter = QGalleryMetaDataFilter(QLatin1String("trackNumber"), 12);

    QGalleryCountRequest request;

    QCOMPARE(request.isLive(), false);
    QCOMPARE(request.itemType(), QString());
    QCOMPARE(request.scope(), QGalleryAbstractRequest::AllDescendants);
    QCOMPARE(request.scopeItemId(), QVariant());
    QCOMPARE(request.filter(), QGalleryFilter());

     request.setLive(true);
    QCOMPARE(request.isLive(), true);

    request.setItemType(audioType);
    QCOMPARE(request.itemType(), QString::fromLatin1("Audio"));

    request.setItemType(videoType.name());
    QCOMPARE(request.itemType(), QString::fromLatin1("Video"));

    request.setScope(QGalleryAbstractRequest::DirectDescendants);
    QCOMPARE(request.scope(), QGalleryAbstractRequest::DirectDescendants);

    request.setScopeItemId(QVariant(76));
    QCOMPARE(request.scopeItemId(), QVariant(76));

    request.setScopeItemId(QVariant(QLatin1String("65")));
    QCOMPARE(request.scopeItemId(), QVariant(QLatin1String("65")));


    request.setFilter(filter);
    QCOMPARE(request.filter(), filter);
}

void tst_QGalleryCountRequest::executeSynchronous()
{
    QtTestGallery gallery;
    gallery.setResult(QGalleryAbstractRequest::ConnectionError);

    QGalleryCountRequest request(&gallery);

    QSignalSpy spy(&request, SIGNAL(countChanged()));

    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::ConnectionError));
    QCOMPARE(spy.count(), 0);
    QCOMPARE(request.count(), 0);

    gallery.setCount(40);
    gallery.setResult(QGalleryAbstractRequest::Succeeded);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(request.count(), 40);

    request.clear();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(spy.count(), 2);
    QCOMPARE(request.count(), 0);
}

void tst_QGalleryCountRequest::executeAsynchronous()
{
    QtTestGallery gallery;
    gallery.setResult(QGalleryAbstractRequest::NoResult);

    QGalleryCountRequest request(&gallery);

    QSignalSpy spy(&request, SIGNAL(countChanged()));

    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(spy.count(), 0);
    QCOMPARE(request.count(), 0);

    gallery.insert(12);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(request.count(), 12);

    gallery.finish(QGalleryAbstractRequest::Succeeded, true);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(request.count(), 12);

    gallery.remove(5);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(request.count(), 7);

    gallery.finish(QGalleryAbstractRequest::Succeeded, false);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(spy.count(), 2);
    QCOMPARE(request.count(), 7);

    request.clear();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(spy.count(), 3);
    QCOMPARE(request.count(), 0);
}

QTEST_MAIN(tst_QGalleryCountRequest)

#include "tst_qgallerycountrequest.moc"
