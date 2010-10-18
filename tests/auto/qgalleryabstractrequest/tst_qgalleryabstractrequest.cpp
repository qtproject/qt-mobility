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

#include <qgalleryabstractrequest.h>

#include <qabstractgallery.h>
#include <qgalleryabstractresponse.h>

#include <QtTest/QtTest>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGalleryAbstractRequest::Status)

class tst_QGalleryAbstractRequest : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void type();
    void isSupported();
    void executeNoGallery();
    void executeUnsupported();
    void executeSync();
    void executeAsync();
    void resume();
    void cancelActive();
    void cancelIdle();
    void clear();
    void waitForFinished();
    void progress();
    void setGallery();
    void clearGallery();
    void deleteGallery();
};

class QtGalleryTestResponse : public QGalleryAbstractResponse
{
    Q_OBJECT
public:
    QtGalleryTestResponse(
            QGalleryAbstractRequest::Status status, int error, const QString &errorString)
        : m_ignoreCancel(false)
        , m_finishInWait(false)
    {
        if (error != QGalleryAbstractRequest::NoError)
            QGalleryAbstractResponse::error(error, errorString);
        else if (status == QGalleryAbstractRequest::Finished)
            finish();
        else if (status == QGalleryAbstractRequest::Idle)
            finish(true);
    }

    bool waitForFinished(int)
    {
        if (m_finishInWait) {
            finish(true);
            return true;
        } else {
            return false;
        }
    }

    void cancel() { if (!m_ignoreCancel) QGalleryAbstractResponse::cancel(); }

    using QGalleryAbstractResponse::finished;
    using QGalleryAbstractResponse::finish;
    using QGalleryAbstractResponse::error;
    using QGalleryAbstractResponse::resume;
    using QGalleryAbstractResponse::resumed;
    using QGalleryAbstractResponse::cancelled;
    using QGalleryAbstractResponse::progressChanged;

    void setIgnoreCancel(bool ignore) { m_ignoreCancel = ignore; }
    void setFinishInWait(bool finish) { m_finishInWait = finish; }

private:
    bool m_ignoreCancel;
    bool m_finishInWait;
};

class QtGalleryTestRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
public:
    QtGalleryTestRequest(QGalleryAbstractRequest::RequestType type)
        : QGalleryAbstractRequest(type)
        , m_response(0)
    {
    }

    QtGalleryTestRequest(
            QAbstractGallery *gallery, QGalleryAbstractRequest::RequestType type)
        : QGalleryAbstractRequest(gallery, type)
        , m_response(0)
    {
    }

    QtGalleryTestResponse *response() const { return m_response; }

protected:
    void setResponse(QGalleryAbstractResponse *response) {
        m_response = static_cast<QtGalleryTestResponse *>(response); }

private:
    QtGalleryTestResponse *m_response;
};

class QtTestGallery : public QAbstractGallery
{
    Q_OBJECT
public:
    QtTestGallery()
        : m_status(QGalleryAbstractRequest::Active)
        , m_error(QGalleryAbstractRequest::NoError)
    {}

    bool isRequestSupported(QGalleryAbstractRequest::RequestType type) const {
        return m_supportedRequests.contains(type); }

    void setStatus(QGalleryAbstractRequest::Status status) { m_status = status; }
    void setError(int error, const QString &errorString) {
        m_error = error; m_errorString = errorString; }

    void setSupportedRequests(const QList<QGalleryAbstractRequest::RequestType> &requests) {
        m_supportedRequests = requests; }

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if (m_supportedRequests.contains(request->type())) {
            return new QtGalleryTestResponse(m_status, m_error, m_errorString);
        }
        return 0;
    }

private:
    QList<QGalleryAbstractRequest::RequestType> m_supportedRequests;
    QGalleryAbstractRequest::Status m_status;
    int m_error;
    QString m_errorString;
};


void tst_QGalleryAbstractRequest::type()
{
    QCOMPARE(QtGalleryTestRequest(QGalleryAbstractRequest::QueryRequest).type(),
             QGalleryAbstractRequest::QueryRequest);

    QCOMPARE(QtGalleryTestRequest(QGalleryAbstractRequest::ItemRequest).type(),
             QGalleryAbstractRequest::ItemRequest);
}

void tst_QGalleryAbstractRequest::initTestCase()
{
    qRegisterMetaType<QGalleryAbstractRequest::Status>();
}

void tst_QGalleryAbstractRequest::isSupported()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);

    QCOMPARE(QtGalleryTestRequest(&gallery, QGalleryAbstractRequest::QueryRequest).isSupported(), false);
    QCOMPARE(QtGalleryTestRequest(&gallery, QGalleryAbstractRequest::ItemRequest).isSupported(), true);
}

void tst_QGalleryAbstractRequest::executeNoGallery()
{
    QtGalleryTestRequest request(QGalleryAbstractRequest::ItemRequest);

    QSignalSpy finishedSpy(&request, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy errorSpy(&request, SIGNAL(error(int,QString)));
    QSignalSpy statusSpy(&request, SIGNAL(statusChanged(QGalleryAbstractRequest::Status)));

    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Error);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoGallery));
    QCOMPARE(request.errorString(), QLatin1String("No gallery has been set on the QtGalleryTestRequest."));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(errorSpy.last().value(0).toInt(), request.error());
    QCOMPARE(errorSpy.last().value(1).toString(), request.errorString());
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());
}

void tst_QGalleryAbstractRequest::executeUnsupported()
{
    QtTestGallery gallery;
    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::ItemRequest);

    QSignalSpy finishedSpy(&request, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy errorSpy(&request, SIGNAL(error(int,QString)));
    QSignalSpy statusSpy(&request, SIGNAL(statusChanged(QGalleryAbstractRequest::Status)));

    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Error);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NotSupported));
    QCOMPARE(request.errorString(), QLatin1String("QtGalleryTestRequest is not supported by QtTestGallery."));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(errorSpy.last().value(0).toInt(), request.error());
    QCOMPARE(errorSpy.last().value(1).toString(), request.errorString());
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());
}

void tst_QGalleryAbstractRequest::executeSync()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::ItemRequest);

    QSignalSpy finishedSpy(&request, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy errorSpy(&request, SIGNAL(error(int,QString)));
    QSignalSpy statusSpy(&request, SIGNAL(statusChanged(QGalleryAbstractRequest::Status)));

    // Successful execution.
    gallery.setStatus(QGalleryAbstractRequest::Finished);
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Unsuccessful execution.
    gallery.setError(121, QLatin1String("bad input"));
    request.execute();
    QVERIFY(request.response() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Error);
    QCOMPARE(request.error(), 121);
    QCOMPARE(request.errorString(), QLatin1String("bad input"));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 2);
    QCOMPARE(errorSpy.last().value(0).toInt(), request.error());
    QCOMPARE(errorSpy.last().value(1).toString(), request.errorString());
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    gallery.setError(65, QLatin1String("bad connection"));
    request.execute();
    QVERIFY(request.response() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Error);
    QCOMPARE(request.error(), 65);
    QCOMPARE(request.errorString(), QLatin1String("bad connection"));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 2);
    QCOMPARE(statusSpy.count(), 3);
    QCOMPARE(errorSpy.last().value(0).toInt(), request.error());
    QCOMPARE(errorSpy.last().value(1).toString(), request.errorString());
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Successful execution, to idle.
    gallery.setError(QGalleryAbstractRequest::NoError, QString());
    gallery.setStatus(QGalleryAbstractRequest::Idle);
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 2);
    QCOMPARE(statusSpy.count(), 4);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Execute unsupported.
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>());
    request.execute();
    QVERIFY(request.response() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Error);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NotSupported));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 3);
    QCOMPARE(statusSpy.count(), 5);
    QCOMPARE(errorSpy.last().value(0).toInt(), request.error());
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());
}
void tst_QGalleryAbstractRequest::executeAsync()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::ItemRequest);

    QSignalSpy finishedSpy(&request, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy errorSpy(&request, SIGNAL(error(int,QString)));
    QSignalSpy statusSpy(&request, SIGNAL(statusChanged(QGalleryAbstractRequest::Status)));

    // Successful execution.
    gallery.setStatus(QGalleryAbstractRequest::Active);
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.response()->finish(false);
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Unsuccessful execution.
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 3);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.response()->error(80, false);
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Error);
    QCOMPARE(request.error(), 80);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 4);
    QCOMPARE(errorSpy.last().value(0).toInt(), request.error());
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Successful execution, to active.
    gallery.setError(QGalleryAbstractRequest::NoError, QString());
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 5);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Response emits finish without changing the error.
    request.response()->finished();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 5);

    // Response finishes correctly.
    request.response()->finish(true);
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 6);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Response emits finish without changing the idle status.
    request.response()->finished();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 6);

    // Response cancels itself.
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 6);

    // Response exits idle status correctly.
    request.response()->finish(false);
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 7);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Response emits finish from finished status.
    request.response()->finished();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 7);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());
}

void tst_QGalleryAbstractRequest::resume()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);
    gallery.setStatus(QGalleryAbstractRequest::Idle);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::ItemRequest);

    QSignalSpy finishedSpy(&request, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy errorSpy(&request, SIGNAL(error(int,QString)));
    QSignalSpy statusSpy(&request, SIGNAL(statusChanged(QGalleryAbstractRequest::Status)));

    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Response emits resumed without changing idle.
    request.response()->resumed();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);

    // Response resumes correctly.
    request.response()->resume();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Resuming while active does nothing.
    request.response()->resume();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);

    // Emitting resumed while active does nothing.
    request.response()->resumed();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);

    // Returning to idle again doesn't emit finished.
    request.response()->finish(true);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 3);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Finishing  doesn't emit finished.
    request.response()->resume();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 4);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.response()->finish(false);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 5);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Emitting resumed when finished does nothing
    request.response()->resumed();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 5);
}

void tst_QGalleryAbstractRequest::cancelActive()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::ItemRequest);

    QSignalSpy finishedSpy(&request, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy errorSpy(&request, SIGNAL(error(int,QString)));
    QSignalSpy statusSpy(&request, SIGNAL(statusChanged(QGalleryAbstractRequest::Status)));

    // Cancel synchronously.
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.cancel();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Cancelled);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Cancel asynchronously.
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 3);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Response emits cancelled unprovoked.
    request.response()->cancelled();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 3);

    request.response()->setIgnoreCancel(true);
    request.cancel();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Cancelling);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 4);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // No status changes while cancel in pending
    request.cancel();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Cancelling);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 4);

    request.response()->setIgnoreCancel(false);
    request.response()->cancel();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Cancelled);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 2);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 5);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // No status changes after cancel is finished.
    request.cancel();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Cancelled);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 2);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 5);

    // Finish as normal if response finishes after being cancelled.
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 2);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 6);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.response()->setIgnoreCancel(true);
    request.cancel();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Cancelling);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 2);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 7);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.response()->finish(false);
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 2);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 8);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());
}

void tst_QGalleryAbstractRequest::cancelIdle()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);
    gallery.setStatus(QGalleryAbstractRequest::Idle);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::ItemRequest);

    QSignalSpy finishedSpy(&request, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy errorSpy(&request, SIGNAL(error(int,QString)));
    QSignalSpy statusSpy(&request, SIGNAL(statusChanged(QGalleryAbstractRequest::Status)));

    // Cancel synchronously.
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.cancel();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Cancel asynchronously.
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 3);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.response()->setIgnoreCancel(true);
    request.cancel();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Cancelling);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 4);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.response()->setIgnoreCancel(false);
    request.response()->cancel();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 5);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());
}

void tst_QGalleryAbstractRequest::clear()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::ItemRequest);

    QSignalSpy finishedSpy(&request, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy errorSpy(&request, SIGNAL(error(int,QString)));
    QSignalSpy statusSpy(&request, SIGNAL(statusChanged(QGalleryAbstractRequest::Status)));
    
    // Clear no response.
    request.clear();
    QVERIFY(request.response() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 0);

    // Clear finished response.
    gallery.setStatus(QGalleryAbstractRequest::Finished);
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.clear();
    QVERIFY(request.response() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Clear idle response.
    gallery.setStatus(QGalleryAbstractRequest::Idle);
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 3);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.clear();
    QVERIFY(request.response() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 4);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Clear active response.
    gallery.setStatus(QGalleryAbstractRequest::Active);
    request.execute();
    QVERIFY(request.response() != 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 5);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.clear();
    QVERIFY(request.response() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 6);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Clear error response.
    gallery.setError(65, QLatin1String("bad connection"));
    request.execute();
    QVERIFY(request.response() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Error);
    QCOMPARE(request.error(), 65);
    QCOMPARE(request.errorString(), QLatin1String("bad connection"));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 7);
    QCOMPARE(errorSpy.last().value(0).toInt(), request.error());
    QCOMPARE(errorSpy.last().value(1).toString(), request.errorString());
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.clear();
    QVERIFY(request.response() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(request.errorString(), QString());
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 8);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());
}

void tst_QGalleryAbstractRequest::waitForFinished()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::ItemRequest);

    QSignalSpy finishedSpy(&request, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy errorSpy(&request, SIGNAL(error(int,QString)));
    QSignalSpy statusSpy(&request, SIGNAL(statusChanged(QGalleryAbstractRequest::Status)));

    // Wait while inactive, no error.
    QCOMPARE(request.waitForFinished(300), true);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 0);

    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Timeout while waiting.
    QCOMPARE(request.waitForFinished(300), false);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);

    // Finish while waiting.
    request.response()->setFinishInWait(true);
    QCOMPARE(request.waitForFinished(300), true);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    // Wait while idle.
    request.response()->setFinishInWait(false);
    QCOMPARE(request.waitForFinished(300), true);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);

    // Wait while finished.
    request.response()->finish(false);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 3);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    QCOMPARE(request.waitForFinished(300), true);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 3);

    // Wait while cancelled.
    gallery.setStatus(QGalleryAbstractRequest::Active);
    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 4);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.cancel();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Cancelled);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 5);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    QCOMPARE(request.waitForFinished(300), true);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Cancelled);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 5);
}

void tst_QGalleryAbstractRequest::progress()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::ItemRequest);

    QSignalSpy spy(&request, SIGNAL(progressChanged(int,int)));

    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(request.maximumProgress(), 0);

    request.execute();
    QCOMPARE(spy.count(), 0);
    QVERIFY(request.response() != 0);

    request.response()->progressChanged(120, 1000);
    QCOMPARE(request.currentProgress(), 120);
    QCOMPARE(request.maximumProgress(), 1000);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.last().value(0).toInt(), 120);
    QCOMPARE(spy.last().value(1).toInt(), 1000);

    request.response()->progressChanged(5600, 95);
    QCOMPARE(request.currentProgress(), 5600);
    QCOMPARE(request.maximumProgress(), 95);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.last().value(0).toInt(), 5600);
    QCOMPARE(spy.last().value(1).toInt(), 95);

    request.cancel();
    QCOMPARE(request.currentProgress(), 5600);
    QCOMPARE(request.maximumProgress(), 95);
    QCOMPARE(spy.count(), 2);

    request.execute();
    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.last().value(0).toInt(), 0);
    QCOMPARE(spy.last().value(1).toInt(), 0);

    request.response()->progressChanged(-2, 10);
    QCOMPARE(request.currentProgress(), -2);
    QCOMPARE(request.maximumProgress(), 10);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy.last().value(0).toInt(), -2);
    QCOMPARE(spy.last().value(1).toInt(), 10);

    request.clear();
    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(spy.count(), 5);
    QCOMPARE(spy.last().value(0).toInt(), 0);
    QCOMPARE(spy.last().value(1).toInt(), 0);

    request.execute();
    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(spy.count(), 5);

    request.response()->progressChanged(0, 75);
    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(request.maximumProgress(), 75);
    QCOMPARE(spy.count(), 6);
    QCOMPARE(spy.last().value(0).toInt(), 0);
    QCOMPARE(spy.last().value(1).toInt(), 75);

    request.execute();
    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(spy.count(), 7);
    QCOMPARE(spy.last().value(0).toInt(), 0);
    QCOMPARE(spy.last().value(1).toInt(), 0);

    request.response()->progressChanged(0, -23);
    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(request.maximumProgress(), -23);
    QCOMPARE(spy.count(), 8);
    QCOMPARE(spy.last().value(0).toInt(), 0);
    QCOMPARE(spy.last().value(1).toInt(), -23);

    request.clear();
    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(request.currentProgress(), 0);
    QCOMPARE(spy.count(), 9);
    QCOMPARE(spy.last().value(0).toInt(), 0);
    QCOMPARE(spy.last().value(1).toInt(), 0);
}

void tst_QGalleryAbstractRequest::setGallery()
{
    QtTestGallery copyGallery;
    copyGallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);

    QtTestGallery moveGallery;
    moveGallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::QueryRequest);

    QtGalleryTestRequest copyRequest(QGalleryAbstractRequest::ItemRequest);
    QtGalleryTestRequest moveRequest(QGalleryAbstractRequest::QueryRequest);

    QSignalSpy copySpy(&copyRequest, SIGNAL(supportedChanged()));
    QSignalSpy moveSpy(&moveRequest, SIGNAL(supportedChanged()));

    QVERIFY(copyRequest.gallery() == 0);
    QVERIFY(moveRequest.gallery() == 0);
    QCOMPARE(copyRequest.isSupported(), false);
    QCOMPARE(moveRequest.isSupported(), false);

    copyRequest.setGallery(&copyGallery);
    moveRequest.setGallery(&moveGallery);

    QVERIFY(copyRequest.gallery()  == &copyGallery);
    QVERIFY(moveRequest.gallery()  == &moveGallery);
    QCOMPARE(copyRequest.isSupported(), true);
    QCOMPARE(moveRequest.isSupported(), true);
    QCOMPARE(copySpy.count(), 1);
    QCOMPARE(moveSpy.count(), 1);

    copyRequest.setGallery(&copyGallery);
    moveRequest.setGallery(&moveGallery);
    QVERIFY(copyRequest.gallery()  == &copyGallery);
    QVERIFY(moveRequest.gallery()  == &moveGallery);
    QCOMPARE(copySpy.count(), 1);
    QCOMPARE(moveSpy.count(), 1);

    copyRequest.setGallery(&moveGallery);
    moveRequest.setGallery(&copyGallery);

    QVERIFY(copyRequest.gallery()  == &moveGallery);
    QVERIFY(moveRequest.gallery()  == &copyGallery);
    QCOMPARE(copyRequest.isSupported(), false);
    QCOMPARE(moveRequest.isSupported(), false);
    QCOMPARE(copySpy.count(), 2);
    QCOMPARE(moveSpy.count(), 2);

    copyRequest.setGallery(0);
    moveRequest.setGallery(0);

    QVERIFY(copyRequest.gallery()  == 0);
    QVERIFY(moveRequest.gallery()  == 0);
    QCOMPARE(copyRequest.isSupported(), false);
    QCOMPARE(moveRequest.isSupported(), false);
    QCOMPARE(copySpy.count(), 3);
    QCOMPARE(moveSpy.count(), 3);
}

void tst_QGalleryAbstractRequest::clearGallery()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::ItemRequest);

    QSignalSpy finishedSpy(&request, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy errorSpy(&request, SIGNAL(error(int,QString)));
    QSignalSpy statusSpy(&request, SIGNAL(statusChanged(QGalleryAbstractRequest::Status)));

    // Clear no response.
    request.setGallery(0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 0);

    // Clear finished response.
    gallery.setStatus(QGalleryAbstractRequest::Finished);
    request.setGallery(&gallery);
    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.setGallery(0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);

    // Clear idle response.
    gallery.setStatus(QGalleryAbstractRequest::Idle);
    request.setGallery(&gallery);
    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.setGallery(0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);

    // Clear active response.
    gallery.setStatus(QGalleryAbstractRequest::Active);
    request.setGallery(&gallery);
    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 3);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    request.setGallery(0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 3);

    // execute after clearing gallery.
    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Error);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoGallery));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 4);
    QCOMPARE(errorSpy.last().value(0).toInt(), request.error());
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());
}

void tst_QGalleryAbstractRequest::deleteGallery()
{
    QtTestGallery *gallery = 0;

    QtGalleryTestRequest request(QGalleryAbstractRequest::ItemRequest);

    QSignalSpy finishedSpy(&request, SIGNAL(finished()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy errorSpy(&request, SIGNAL(error(int,QString)));
    QSignalSpy statusSpy(&request, SIGNAL(statusChanged(QGalleryAbstractRequest::Status)));

    // No response.
    gallery = new QtTestGallery;
    gallery->setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);

    request.setGallery(gallery);
    delete gallery;
    QVERIFY(request.gallery() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 0);

    // Finished response.
    gallery = new QtTestGallery;
    gallery->setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);
    gallery->setStatus(QGalleryAbstractRequest::Finished);

    request.setGallery(gallery);
    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    delete gallery;
    QVERIFY(request.gallery() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 1);

    // Idle response.
    gallery = new QtTestGallery;
    gallery->setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);
    gallery->setStatus(QGalleryAbstractRequest::Idle);

    request.setGallery(gallery);
    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    delete gallery;
    QVERIFY(request.gallery() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Idle);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 2);

    // Clear active response.
    gallery = new QtTestGallery;
    gallery->setSupportedRequests(QList<QGalleryAbstractRequest::RequestType>()
            << QGalleryAbstractRequest::ItemRequest);

    request.setGallery(gallery);
    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 3);
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());

    delete gallery;
    QVERIFY(request.gallery() == 0);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(statusSpy.count(), 3);

    // execute after deleting gallery.
    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Error);
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoGallery));
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 4);
    QCOMPARE(errorSpy.last().value(0).toInt(), request.error());
    QCOMPARE(statusSpy.last().value(0).value<QGalleryAbstractRequest::Status>(), request.status());
}

QTEST_MAIN(tst_QGalleryAbstractRequest)

#include "tst_qgalleryabstractrequest.moc"
