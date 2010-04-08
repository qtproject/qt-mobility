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

#include <qgalleryrequest.h>
#include <qgallery.h>

class tst_QGalleryAbstractRequest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void type();
    void isSupported();

    void executeNoGallery();
    void executeUnsupported();
    void executeSync();
    void executeAsync();
    void cancelActive();
    void cancelIdle();
    void clear();
    void waitForFinished();
    void setGallery();
    void clearGallery();
    void deleteGallery();
};

class QtGalleryTestResponse : public QGalleryAbstractResponse
{
public:
    QtGalleryTestResponse(int result, bool idle)
        : m_ignoreCancel(false)
        , m_finishInWait(false)
    {
        if (result != QGalleryAbstractRequest::NoResult)
            finish(result, idle);
    }

    QStringList propertyNames() const { return QStringList(); }
    int propertyKey(const QString &) const { return -1; }

    int count() const { return 0; }

    QString id(int) const { return QString(); }
    QUrl url(int) const { return QUrl(); }
    QString type(int) const { return QString(); }
    QString parentId(int) const { return QString(); }
    QList<QGalleryResource> resources(int) const { return QList<QGalleryResource>(); }

    QVariant metaData(int, int) const { return QVariant(); }
    void setMetaData(int, int, const QVariant &) {}

    MetaDataFlags metaDataFlags(int, int) const { return MetaDataFlags(); }

    bool waitForFinished(int)
    {
        if (m_finishInWait) {
            finish(QGalleryAbstractRequest::Succeeded, true);
            return true;
        } else {
            return false;
        }
    }

    void cancel() { if (!m_ignoreCancel) QGalleryAbstractResponse::cancel(); }

    void doFinish(int result, bool idle) { finish(result, idle); }
    void setIgnoreCancel(bool ignore) { m_ignoreCancel = ignore; }
    void setFinishInWait(bool finish) { m_finishInWait = finish; }

private:
    bool m_ignoreCancel;
    bool m_finishInWait;
};

class QtGalleryTestRequest : public QGalleryAbstractRequest
{
public:
    QtGalleryTestRequest(QGalleryAbstractRequest::Type type)
        : QGalleryAbstractRequest(type)
        , m_response(0)
    {
    }

    QtGalleryTestRequest(
            QAbstractGallery *gallery, QGalleryAbstractRequest::Type type)
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
public:
    QtTestGallery() : m_result(QGalleryAbstractRequest::NoResult), m_idle(false) {}

    bool isRequestSupported(QGalleryAbstractRequest::Type type) const {
        return m_supportedRequests.contains(type); }

    void setResult(int result) { m_result = result; }
    void setIdle(bool idle) { m_idle = idle; }

    void setSupportedRequests(const QList<QGalleryAbstractRequest::Type> &requests) {
        m_supportedRequests = requests; }

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if (m_supportedRequests.contains(request->type()))
            return new QtGalleryTestResponse(m_result, m_idle);
        return 0;
    }

private:
    QList<QGalleryAbstractRequest::Type> m_supportedRequests;
    int m_result;
    bool m_idle;
};

void tst_QGalleryAbstractRequest::type()
{
    QCOMPARE(QtGalleryTestRequest(QGalleryAbstractRequest::Item).type(),
             QGalleryAbstractRequest::Item);

    QCOMPARE(QtGalleryTestRequest(QGalleryAbstractRequest::Move).type(),
             QGalleryAbstractRequest::Move);
}

void tst_QGalleryAbstractRequest::isSupported()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy
            << QGalleryAbstractRequest::Move);

    QCOMPARE(QtGalleryTestRequest(&gallery, QGalleryAbstractRequest::Item).isSupported(), false);
    QCOMPARE(QtGalleryTestRequest(&gallery, QGalleryAbstractRequest::Insert).isSupported(), false);
    QCOMPARE(QtGalleryTestRequest(&gallery, QGalleryAbstractRequest::Move).isSupported(), true);
    QCOMPARE(QtGalleryTestRequest(&gallery, QGalleryAbstractRequest::Copy).isSupported(), true);
}

void tst_QGalleryAbstractRequest::executeNoGallery()
{
    QtGalleryTestRequest request(QGalleryAbstractRequest::Copy);

    QSignalSpy succeededSpy(&request, SIGNAL(succeeded()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy failedSpy(&request, SIGNAL(failed(int)));
    QSignalSpy finishedSpy(&request, SIGNAL(finished(int)));
    QSignalSpy resultSpy(&request, SIGNAL(resultChanged()));

    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoGallery));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);
    QCOMPARE(failedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::NoGallery));
    QCOMPARE(finishedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::NoGallery));
}

void tst_QGalleryAbstractRequest::executeUnsupported()
{
    QtTestGallery gallery;
    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::Copy);

    QSignalSpy succeededSpy(&request, SIGNAL(succeeded()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy failedSpy(&request, SIGNAL(failed(int)));
    QSignalSpy finishedSpy(&request, SIGNAL(finished(int)));
    QSignalSpy resultSpy(&request, SIGNAL(resultChanged()));

    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NotSupported));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);
    QCOMPARE(failedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::NotSupported));
    QCOMPARE(finishedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::NotSupported));
}

void tst_QGalleryAbstractRequest::executeSync()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::Copy);

    QSignalSpy succeededSpy(&request, SIGNAL(succeeded()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy failedSpy(&request, SIGNAL(failed(int)));
    QSignalSpy finishedSpy(&request, SIGNAL(finished(int)));
    QSignalSpy resultSpy(&request, SIGNAL(resultChanged()));

    // Successful execution.
    gallery.setResult(QGalleryAbstractRequest::Succeeded);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);
    QCOMPARE(finishedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::Succeeded));

    // Unsuccessful execution.
    gallery.setResult(QGalleryAbstractRequest::RequestError);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::RequestError));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 2);
    QCOMPARE(failedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::RequestError));
    QCOMPARE(finishedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::RequestError));

    // Successful execution, to idle.
    gallery.setResult(QGalleryAbstractRequest::Succeeded);
    gallery.setIdle(true);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Idle);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 3);
    QCOMPARE(resultSpy.count(), 3);
    QCOMPARE(finishedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::Succeeded));

}
void tst_QGalleryAbstractRequest::executeAsync()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::Copy);

    QSignalSpy succeededSpy(&request, SIGNAL(succeeded()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy failedSpy(&request, SIGNAL(failed(int)));
    QSignalSpy finishedSpy(&request, SIGNAL(finished(int)));
    QSignalSpy resultSpy(&request, SIGNAL(resultChanged()));

    // Successful execution.
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(resultSpy.count(), 0);

    request.response()->doFinish(QGalleryAbstractRequest::Succeeded, false);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);
    QCOMPARE(finishedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::Succeeded));

    // Unsuccessful execution.
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 2);

    request.response()->doFinish(QGalleryAbstractRequest::RequestError, false);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::RequestError));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 3);
    QCOMPARE(failedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::RequestError));
    QCOMPARE(finishedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::RequestError));

    // Successful execution, to idle.
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 4);

    request.response()->doFinish(QGalleryAbstractRequest::Succeeded, true);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Idle);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 3);
    QCOMPARE(resultSpy.count(), 5);
    QCOMPARE(finishedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::Succeeded));
}

void tst_QGalleryAbstractRequest::cancelActive()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::Copy);

    QSignalSpy succeededSpy(&request, SIGNAL(succeeded()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy failedSpy(&request, SIGNAL(failed(int)));
    QSignalSpy finishedSpy(&request, SIGNAL(finished(int)));
    QSignalSpy resultSpy(&request, SIGNAL(resultChanged()));

    // Cancel synchronously.
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(resultSpy.count(), 0);

    request.cancel();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Cancelled));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);
    QCOMPARE(finishedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::Cancelled));

    // Cancel asynchronously.
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 2);

    request.response()->setIgnoreCancel(true);
    request.cancel();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Cancelling);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 2);

    request.response()->setIgnoreCancel(false);
    request.response()->cancel();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Cancelled));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 2);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 3);
    QCOMPARE(finishedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::Cancelled));
}

void tst_QGalleryAbstractRequest::cancelIdle()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy);
    gallery.setResult(QGalleryAbstractRequest::Succeeded);
    gallery.setIdle(true);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::Copy);

    QSignalSpy succeededSpy(&request, SIGNAL(succeeded()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy failedSpy(&request, SIGNAL(failed(int)));
    QSignalSpy finishedSpy(&request, SIGNAL(finished(int)));
    QSignalSpy resultSpy(&request, SIGNAL(resultChanged()));

    // Cancel synchronously.
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Idle);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);
    QCOMPARE(finishedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::Succeeded));

    request.cancel();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);

    // Cancel asynchronously.
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Idle);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 1);
    QCOMPARE(finishedSpy.last().value(0).toInt(), int(QGalleryAbstractRequest::Succeeded));

    request.response()->setIgnoreCancel(true);
    request.cancel();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Idle);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 1);

    request.response()->setIgnoreCancel(false);
    request.response()->cancel();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 1);
}

void tst_QGalleryAbstractRequest::clear()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::Copy);

    QSignalSpy succeededSpy(&request, SIGNAL(succeeded()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy failedSpy(&request, SIGNAL(failed(int)));
    QSignalSpy finishedSpy(&request, SIGNAL(finished(int)));
    QSignalSpy resultSpy(&request, SIGNAL(resultChanged()));
    
    // Clear no response.
    request.clear();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(resultSpy.count(), 0);

    // Clear finished response.
    gallery.setResult(QGalleryAbstractRequest::Succeeded);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);

    request.clear();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 2);

    // Clear idle response.
    gallery.setIdle(true);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Idle);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 3);

    request.clear();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 4);

    // Clear active response.
    gallery.setResult(QGalleryAbstractRequest::NoResult);
    gallery.setIdle(false);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 4);

    request.clear();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 4);
}

void tst_QGalleryAbstractRequest::waitForFinished()
{
    QtTestGallery gallery;
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::Copy);

    QSignalSpy succeededSpy(&request, SIGNAL(succeeded()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy failedSpy(&request, SIGNAL(failed(int)));
    QSignalSpy finishedSpy(&request, SIGNAL(finished(int)));
    QSignalSpy resultSpy(&request, SIGNAL(resultChanged()));

    // Wait while inactive, no result.
    QCOMPARE(request.waitForFinished(300), true);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(resultSpy.count(), 0);

    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(resultSpy.count(), 0);

    // Timeout while waiting.
    QCOMPARE(request.waitForFinished(300), false);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(resultSpy.count(), 0);

    // Finish while waiting.
    request.response()->setFinishInWait(true);
    QCOMPARE(request.waitForFinished(300), true);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Idle);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);

    // Wait while idle.
    request.response()->setFinishInWait(false);
    QCOMPARE(request.waitForFinished(300), true);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Idle);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);

    // Wait while inactive, with result.
    request.cancel();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);

    QCOMPARE(request.waitForFinished(300), true);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);
}

void tst_QGalleryAbstractRequest::setGallery()
{
    QtTestGallery copyGallery;
    copyGallery.setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy);

    QtTestGallery moveGallery;
    moveGallery.setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Move);

    QtGalleryTestRequest copyRequest(QGalleryAbstractRequest::Copy);
    QtGalleryTestRequest moveRequest(QGalleryAbstractRequest::Move);

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
    gallery.setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy);

    QtGalleryTestRequest request(&gallery, QGalleryAbstractRequest::Copy);

    QSignalSpy succeededSpy(&request, SIGNAL(succeeded()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy failedSpy(&request, SIGNAL(failed(int)));
    QSignalSpy finishedSpy(&request, SIGNAL(finished(int)));
    QSignalSpy resultSpy(&request, SIGNAL(resultChanged()));

    // Clear no response.
    request.setGallery(0);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(resultSpy.count(), 0);

    // Clear finished response.
    gallery.setResult(QGalleryAbstractRequest::Succeeded);
    request.setGallery(&gallery);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);

    request.setGallery(0);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 2);

    // Clear idle response.
    gallery.setIdle(true);
    request.setGallery(&gallery);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Idle);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 3);

    request.setGallery(0);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 4);

    // Clear active response.
    gallery.setResult(QGalleryAbstractRequest::NoResult);
    gallery.setIdle(false);
    request.setGallery(&gallery);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 4);

    request.setGallery(0);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 4);
}

void tst_QGalleryAbstractRequest::deleteGallery()
{
    QtTestGallery *gallery = 0;

    QtGalleryTestRequest request(QGalleryAbstractRequest::Copy);

    QSignalSpy succeededSpy(&request, SIGNAL(succeeded()));
    QSignalSpy cancelledSpy(&request, SIGNAL(cancelled()));
    QSignalSpy failedSpy(&request, SIGNAL(failed(int)));
    QSignalSpy finishedSpy(&request, SIGNAL(finished(int)));
    QSignalSpy resultSpy(&request, SIGNAL(resultChanged()));

    // No response.
    gallery = new QtTestGallery;
    gallery->setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy);

    request.setGallery(gallery);
    delete gallery;
    QVERIFY(request.gallery() == 0);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 0);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 0);
    QCOMPARE(resultSpy.count(), 0);

    // Finished response.
    gallery = new QtTestGallery;
    gallery->setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy);
    gallery->setResult(QGalleryAbstractRequest::Succeeded);

    request.setGallery(gallery);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 1);

    delete gallery;
    QVERIFY(request.gallery() == 0);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 1);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(resultSpy.count(), 2);

    // Idle response.
    gallery = new QtTestGallery;
    gallery->setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy);
    gallery->setResult(QGalleryAbstractRequest::Succeeded);
    gallery->setIdle(true);

    request.setGallery(gallery);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Idle);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 3);

    delete gallery;
    QVERIFY(request.gallery() == 0);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 4);

    // Clear active response.
    gallery = new QtTestGallery;
    gallery->setSupportedRequests(QList<QGalleryAbstractRequest::Type>()
            << QGalleryAbstractRequest::Copy);

    request.setGallery(gallery);
    request.execute();
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 4);

    delete gallery;
    QVERIFY(request.gallery() == 0);
    QCOMPARE(request.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(request.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(succeededSpy.count(), 2);
    QCOMPARE(cancelledSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 2);
    QCOMPARE(resultSpy.count(), 4);
}

QTEST_MAIN(tst_QGalleryAbstractRequest)

#include "tst_qgalleryabstractrequest.moc"
