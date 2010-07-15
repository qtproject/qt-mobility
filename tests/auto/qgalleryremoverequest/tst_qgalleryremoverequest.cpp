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

#include <QtTest/QtTest>

#include <qgalleryremoverequest.h>

#include <qabstractgallery.h>
#include <qgalleryabstractresponse.h>

QTM_USE_NAMESPACE

class tst_QGalleryRemoveRequest : public QObject
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
    QtGalleryTestResponse(int result, bool idle)
    {
        if (result != QGalleryAbstractRequest::NoResult)
            finish(result, idle);
    }

    QStringList propertyNames() const { return QStringList(); }
    int propertyKey(const QString &) const { return -1; }
    QGalleryProperty::Attributes propertyAttributes(int) const {
        return QGalleryProperty::Attributes(); }
    QVariant::Type propertyType(int) const { return QVariant::Invalid; }

    int count() const { return 0; }

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

private:
};

class QtTestGallery : public QAbstractGallery
{
    Q_OBJECT
public:
    QtTestGallery() : m_result(QGalleryAbstractRequest::NoResult), m_idle(false) {}

    bool isRequestSupported(QGalleryAbstractRequest::Type type) const {
        return type == QGalleryAbstractRequest::Remove; }

    void setResult(int result) { m_result = result; }
    void setIdle(bool idle) { m_idle = idle; }

#ifdef Q_MOC_RUN
Q_SIGNALS:
#endif
    void finish(int result, bool idle);

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if (request->type() == QGalleryAbstractRequest::Remove) {
            QtGalleryTestResponse *response = new QtGalleryTestResponse(m_result, m_idle);

            connect(this, SIGNAL(finish(int,bool)), response, SLOT(doFinish(int,bool)));

            return response;
        }
        return 0;
    }

private:
    int m_result;
    bool m_idle;
};

void tst_QGalleryRemoveRequest::properties()
{
    QGalleryRemoveRequest request;

    request.setItemId(QVariant(76));
    QCOMPARE(request.itemId(), QVariant(76));

    request.setItemId(QVariant(QLatin1String("65")));
    QCOMPARE(request.itemId(), QVariant(QLatin1String("65")));
}

void tst_QGalleryRemoveRequest::executeSynchronous()
{
    QtTestGallery gallery;
    gallery.setResult(QGalleryAbstractRequest::ConnectionError);

    QGalleryRemoveRequest request(&gallery);

    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::ConnectionError));

    gallery.setResult(QGalleryAbstractRequest::Succeeded);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));

    request.clear();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
}

void tst_QGalleryRemoveRequest::executeAsynchronous()
{
    QtTestGallery gallery;
    gallery.setResult(QGalleryAbstractRequest::NoResult);

    QGalleryRemoveRequest request(&gallery);

    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));

    gallery.finish(QGalleryAbstractRequest::Succeeded, false);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));

    request.clear();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
}

QTEST_MAIN(tst_QGalleryRemoveRequest)

#include "tst_qgalleryremoverequest.moc"
