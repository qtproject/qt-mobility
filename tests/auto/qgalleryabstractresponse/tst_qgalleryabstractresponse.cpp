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

#include <qgalleryabstractresponse.h>

QTM_USE_NAMESPACE

class tst_QGalleryAbstractResponse : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void finish();
    void cancel();
    void idle();
};

class QtGalleryTestResponse : public QGalleryAbstractResponse
{
public:
    QStringList propertyNames() const { return QStringList(); }
    int propertyKey(const QString &) const { return -1; }
    QGalleryProperty::Attributes propertyAttributes(int) const {
        return QGalleryProperty::Attributes(); }

    int count() const { return 0; }

    QString id(int) const { return QString(); }
    QUrl url(int) const { return QUrl(); }
    QString type(int) const { return QString(); }
    QString parentId(int) const { return QString(); }
    QList<QGalleryResource> resources(int) const { return QList<QGalleryResource>(); }
    ItemStatus status(int index) const { return ItemStatus(); }

    QVariant metaData(int, int) const { return QVariant(); }
    void setMetaData(int, int, const QVariant &) {}

    bool waitForFinished(int) { return true; }

    void doFinish(int result, bool idle) { finish(result, idle); }
};

void tst_QGalleryAbstractResponse::finish()
{
    QtGalleryTestResponse response;

    QSignalSpy spy(&response, SIGNAL(finished()));

    QCOMPARE(response.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(response.isIdle(), false);

    // Calling finish with no result does nothing.
    response.doFinish(QGalleryAbstractRequest::NoResult, false);
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 0);

    response.doFinish(QGalleryAbstractRequest::NoResult, true);
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 0);

    // The first call to finish with a valid result emits finished.
    response.doFinish(QGalleryAbstractRequest::Succeeded, false);
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 1);

    // Subsequent calls to finish do nothing.
    response.doFinish(QGalleryAbstractRequest::Succeeded, true);
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 1);

    response.doFinish(QGalleryAbstractRequest::Cancelled, false);
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 1);

    // Subsequent call to cancel do nothing.
    response.cancel();
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 1);
}

void tst_QGalleryAbstractResponse::cancel()
{
    QtGalleryTestResponse response;

    QSignalSpy spy(&response, SIGNAL(finished()));

    QCOMPARE(response.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(response.isIdle(), false);

    // Calling cancel on an unfinished will set the result to Cancelled, and
    //emit finished().
    response.cancel();
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Cancelled));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 1);

    // Subsequent calls to finish do nothing.
    response.doFinish(QGalleryAbstractRequest::RequestError, true);
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Cancelled));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 1);

    response.doFinish(QGalleryAbstractRequest::Cancelled, false);
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Cancelled));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 1);

    // Subsequent call to cancel do nothing.
    response.cancel();
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Cancelled));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 1);
}

void tst_QGalleryAbstractResponse::idle()
{
    QtGalleryTestResponse response;

    QSignalSpy spy(&response, SIGNAL(finished()));

    QCOMPARE(response.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(response.isIdle(), false);

    response.doFinish(QGalleryAbstractRequest::Succeeded, true);
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(response.isIdle(), true);
    QCOMPARE(spy.count(), 1);

    // Subsequent calls to finish do nothing.
    response.doFinish(QGalleryAbstractRequest::Succeeded, false);
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(response.isIdle(), true);
    QCOMPARE(spy.count(), 1);

    response.doFinish(QGalleryAbstractRequest::RequestError, true);
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(response.isIdle(), true);
    QCOMPARE(spy.count(), 1);

    // Cancel exits the idle state.
    response.cancel();
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 2);

    // Subsequent calls to finish do nothing.
    response.doFinish(QGalleryAbstractRequest::Succeeded, false);
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 2);

    response.doFinish(QGalleryAbstractRequest::RequestError, true);
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 2);

    // Subsequent call to cancel do nothing.
    response.cancel();
    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(spy.count(), 2);
}

#include "tst_qgalleryabstractresponse.moc"

QTEST_MAIN(tst_QGalleryAbstractResponse)
