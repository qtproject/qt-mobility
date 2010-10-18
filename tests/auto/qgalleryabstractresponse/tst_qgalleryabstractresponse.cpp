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

#include <qgalleryabstractresponse.h>

#include <QtTest/QtTest>

QTM_USE_NAMESPACE

class tst_QGalleryAbstractResponse : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void finish();
    void cancel();
    void cancelIdle();
    void finishIdle();
    void errorActive();
    void errorIdle();
    void errorFinished();
    void errorCancelled();
    void immediateError();
};

class QtGalleryTestResponse : public QGalleryAbstractResponse
{
public:
    using QGalleryAbstractResponse::error;
    using QGalleryAbstractResponse::finish;
    using QGalleryAbstractResponse::resume;
};

void tst_QGalleryAbstractResponse::finish()
{
    QtGalleryTestResponse response;

    QSignalSpy finishedSpy(&response, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&response, SIGNAL(cancelled()));
    QSignalSpy resumedSpy(&response, SIGNAL(resumed()));

    QCOMPARE(response.isActive(), true);
    QCOMPARE(response.isIdle(), false);

    // The first call to finish with a valid result emits finished.
    response.finish(false);
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);

    // Subsequent calls to finish do nothing.
    response.finish(false);
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);

    response.finish(true);
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);

    // Subsequent calls to cancel do nothing.
    response.cancel();
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);

    // Subsequent calls to resume do nothing.
    response.resume();
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);
}

void tst_QGalleryAbstractResponse::cancel()
{
    QtGalleryTestResponse response;

    QSignalSpy finishedSpy(&response, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&response, SIGNAL(cancelled()));
    QSignalSpy resumedSpy(&response, SIGNAL(resumed()));

    QCOMPARE(response.isActive(), true);
    QCOMPARE(response.isIdle(), false);

    // Calling cancel on an unfinished will emit cancelled, and set isActive false.
    response.cancel();
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(resumedSpy.count(), 0);

    // Subsequent calls to finish do nothing.
    response.finish(false);
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(resumedSpy.count(), 0);

    response.finish(true);
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(resumedSpy.count(), 0);

    // Subsequent call to cancel do nothing.
    response.cancel();
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(resumedSpy.count(), 0);

    // Subsequent call to resume do nothing.
    response.resume();
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(resumedSpy.count(), 0);
}

void tst_QGalleryAbstractResponse::cancelIdle()
{
    QtGalleryTestResponse response;

    QSignalSpy finishedSpy(&response, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&response, SIGNAL(cancelled()));
    QSignalSpy resumedSpy(&response, SIGNAL(resumed()));

    QCOMPARE(response.isActive(), true);
    QCOMPARE(response.isIdle(), false);

    // Calling resume while active does nothing.
    response.resume();
    QCOMPARE(response.isActive(), true);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);

    // The first call to finish with a valid result emits finished.
    response.finish(true);
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), true);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);

    // Calling resume set isActive to true and emits resumed.
    response.resume();
    QCOMPARE(response.isActive(), true);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 1);

    // Calling resume while active does nothing.
    response.resume();
    QCOMPARE(response.isActive(), true);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 1);

    // Finishing again works as before.
    response.finish(true);
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), true);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 1);


    // Cancelling sets isIdle to false and emits cancelled.
    response.cancel();
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(resumedSpy.count(), 1);
}

void tst_QGalleryAbstractResponse::finishIdle()
{
    QtGalleryTestResponse response;

    QSignalSpy finishedSpy(&response, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&response, SIGNAL(cancelled()));
    QSignalSpy resumedSpy(&response, SIGNAL(resumed()));

    QCOMPARE(response.isActive(), true);
    QCOMPARE(response.isIdle(), false);

    // Calling resume while active does nothing.
    response.resume();
    QCOMPARE(response.isActive(), true);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);

    // The first call to finish with a valid result emits finished.
    response.finish(true);
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), true);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);

    // Calling resume set isActive to true and emits resumed.
    response.resume();
    QCOMPARE(response.isActive(), true);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 1);

    // Calling resume while active does nothing.
    response.resume();
    QCOMPARE(response.isActive(), true);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 1);

    // Finishing again works as before.
    response.finish(true);
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), true);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 1);

    // Finishing with idle = true does nothing while idle.
    response.finish(true);
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), true);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 1);

    // Finishing with idle = false sets isIdle to false and emits finished.
    response.finish(false);
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 3);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 1);
}

void tst_QGalleryAbstractResponse::errorActive()
{
    QtGalleryTestResponse response;

    QSignalSpy finishedSpy(&response, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&response, SIGNAL(cancelled()));
    QSignalSpy resumedSpy(&response, SIGNAL(resumed()));

    QCOMPARE(response.isActive(), true);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(response.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(response.errorString(), QString());

    response.error(300, QLatin1String("error"));

    QCOMPARE(response.error(), 300);
    QCOMPARE(response.errorString(), QLatin1String("error"));
    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);
}

void tst_QGalleryAbstractResponse::errorIdle()
{
    QtGalleryTestResponse response;
    response.finish(true);

    QSignalSpy finishedSpy(&response, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&response, SIGNAL(cancelled()));
    QSignalSpy resumedSpy(&response, SIGNAL(resumed()));

    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), true);
    QCOMPARE(response.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(response.errorString(), QString());

    response.error(300, QLatin1String("error"));

    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(response.error(), 300);
    QCOMPARE(response.errorString(), QLatin1String("error"));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);
}

void tst_QGalleryAbstractResponse::errorFinished()
{
    QtGalleryTestResponse response;
    response.finish(false);

    QSignalSpy finishedSpy(&response, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&response, SIGNAL(cancelled()));
    QSignalSpy resumedSpy(&response, SIGNAL(resumed()));

    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(response.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(response.errorString(), QString());

    response.error(300, QLatin1String("error"));

    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(response.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(response.errorString(), QString());
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);
}

void tst_QGalleryAbstractResponse::errorCancelled()
{
    QtGalleryTestResponse response;
    response.cancel();

    QSignalSpy finishedSpy(&response, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&response, SIGNAL(cancelled()));
    QSignalSpy resumedSpy(&response, SIGNAL(resumed()));

    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(response.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(response.errorString(), QString());

    response.error(300, QLatin1String("error"));

    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(response.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(response.errorString(), QString());
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(resumedSpy.count(), 0);
}

void tst_QGalleryAbstractResponse::immediateError()
{
    QGalleryAbstractResponse response(300, QLatin1String("error"));

    QCOMPARE(response.isActive(), false);
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(response.error(), 300);
    QCOMPARE(response.errorString(), QLatin1String("error"));
    QCOMPARE(response.waitForFinished(300), true);
}

#include "tst_qgalleryabstractresponse.moc"

QTEST_MAIN(tst_QGalleryAbstractResponse)
