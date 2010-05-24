/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>

#include <qgalleryurlrequest.h>

#include <qabstractgallery.h>
#include <qgalleryabstractresponse.h>

QTM_USE_NAMESPACE

class tst_QGalleryUrlRequest : public QObject
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
    QtGalleryTestResponse(
            int count, const QVariant &itemId, const QString &itemType, int result, bool idle)
        : m_count(count), m_itemId(itemId), m_itemType(itemType)
    {
        if (result != QGalleryAbstractRequest::NoResult)
            finish(result, idle);
    }

    QtGalleryTestResponse(int result, bool idle)
        : m_count(0)
    {
        if (result != QGalleryAbstractRequest::NoResult)
            finish(result, idle);
    }

    QStringList propertyNames() const { return QStringList(); }
    int propertyKey(const QString &) const { return -1; }
    QGalleryProperty::Attributes propertyAttributes(int) const {
        return QGalleryProperty::Attributes(); }

    int count() const { return m_count; }

    QVariant id(int) const { return m_itemId; }
    QUrl url(int) const { return QUrl(); }
    QString type(int) const { return m_itemType; }
    QString parentId(int) const { return QString(); }
    QList<QGalleryResource> resources(int) const { return QList<QGalleryResource>(); }
    ItemStatus status(int) const { return ItemStatus(); }

    QVariant metaData(int, int) const { return QVariant(); }
    void setMetaData(int, int, const QVariant &) {}

    bool waitForFinished(int) { return false; }

public Q_SLOTS:
    void insert(const QVariant &itemId, const QString &itemType) {
        m_itemId = itemId; m_itemType = itemType; m_count = 1; inserted(0, 1); }

    void doFinish(int result, bool idle) { finish(result, idle); }

private:
    int m_count;
    QVariant m_itemId;
    QString m_itemType;
};

class QtTestGallery : public QAbstractGallery
{
    Q_OBJECT
public:
    QtTestGallery() : m_count(0), m_result(QGalleryAbstractRequest::NoResult), m_idle(false) {}

    bool isRequestSupported(QGalleryAbstractRequest::Type type) const {
        return type == QGalleryAbstractRequest::Url; }

    void setResult(int result) { m_result = result; }
    void setIdle(bool idle) { m_idle = idle; }

    void setItemId(const QVariant &itemId) { m_itemId = itemId; }
    void setItemType(const QString &itemType) { m_itemType = itemType; }
    void setCount(int count) { m_count = count; }

#ifdef Q_MOC_RUN
Q_SIGNALS:
#endif
    void insert(const QVariant &itemId, const QString &itemType);
    void finish(int result, bool idle);

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if (request->type() == QGalleryAbstractRequest::Url) {
            QtGalleryTestResponse *response = new QtGalleryTestResponse(
                    m_count, m_itemId, m_itemType, m_result, m_idle);

            connect(this, SIGNAL(insert(QVariant,QString)),
                    response, SLOT(insert(QVariant,QString)));
            connect(this, SIGNAL(finish(int,bool)), response, SLOT(doFinish(int,bool)));

            return response;
        }
        return 0;
    }

private:
    int m_count;
    QVariant m_itemId;
    QString m_itemType;
    int m_result;
    bool m_idle;
};

void tst_QGalleryUrlRequest::properties()
{
    const QUrl fileUrl(QLatin1String("file:///a/local/file"));
    const QUrl httpUrl(QLatin1String("http://example.com/"));

    QGalleryUrlRequest request;

    QCOMPARE(request.itemUrl(), QUrl());

    request.setItemUrl(fileUrl);
    QCOMPARE(request.itemUrl(), fileUrl);

    request.setItemUrl(httpUrl);
    QCOMPARE(request.itemUrl(), httpUrl);
}

void tst_QGalleryUrlRequest::executeSynchronous()
{
    const QVariant itemId(15);
    const QString itemType(QLatin1String("File"));

    QtTestGallery gallery;
    gallery.setResult(QGalleryAbstractRequest::ConnectionError);
    gallery.setItemId(itemId);
    gallery.setItemType(itemType);
    gallery.setCount(1);

    QGalleryUrlRequest request(&gallery);
    QCOMPARE(request.itemId(), QVariant());
    QCOMPARE(request.itemType(), QString());

    QSignalSpy spy(&request, SIGNAL(itemChanged()));

    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::ConnectionError));
    QCOMPARE(spy.count(), 0);
    QCOMPARE(request.itemId(), QVariant());
    QCOMPARE(request.itemType(), QString());

    gallery.setResult(QGalleryAbstractRequest::Succeeded);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(request.itemId(), itemId);
    QCOMPARE(request.itemType(), itemType);

    request.clear();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(spy.count(), 2);
    QCOMPARE(request.itemId(), QVariant());
    QCOMPARE(request.itemType(), QString());
}

void tst_QGalleryUrlRequest::executeAsynchronous()
{
    const QVariant itemId(15);
    const QString itemType(QLatin1String("File"));

    QtTestGallery gallery;
    gallery.setResult(QGalleryAbstractRequest::NoResult);
    gallery.setItemId(itemId);
    gallery.setItemType(itemType);

    QGalleryUrlRequest request(&gallery);

    QSignalSpy spy(&request, SIGNAL(itemChanged()));

    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(spy.count(), 0);
    QCOMPARE(request.itemId(), QVariant());
    QCOMPARE(request.itemType(), QString());

    gallery.insert(itemId, itemType);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(request.itemId(), itemId);
    QCOMPARE(request.itemType(), itemType);

    gallery.finish(QGalleryAbstractRequest::Succeeded, true);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(request.itemId(), itemId);
    QCOMPARE(request.itemType(), itemType);

    request.clear();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(spy.count(), 2);
    QCOMPARE(request.itemId(), QVariant());
    QCOMPARE(request.itemType(), QString());
}

QTEST_MAIN(tst_QGalleryUrlRequest)

#include "tst_qgalleryurlrequest.moc"
