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

#include <qgalleryremoverequest.h>

#include <qabstractgallery.h>
#include <qgalleryabstractresponse.h>

#include <QtTest/QtTest>

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
    QtGalleryTestResponse(
            QGalleryAbstractRequest::Status status, int error, const QString &errorString)
    {
        if (error != QGalleryAbstractRequest::NoError)
            QGalleryAbstractResponse::error(error, errorString);
        else if (status == QGalleryAbstractRequest::Finished)
            finish();
        else if (status == QGalleryAbstractRequest::Idle)
            finish(true);
    }

    bool waitForFinished(int) { return false; }

public Q_SLOTS:
    void doFinish(bool idle) { finish(idle); }
    void doError(int error, const QString &errorString) {
        QGalleryAbstractResponse::error(error, errorString); }
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
        return type == QGalleryAbstractRequest::RemoveRequest; }

    void setStatus(QGalleryAbstractRequest::Status status) { m_status = status; }
    void setError(int error, const QString &errorString) {
        m_error = error; m_errorString = errorString; }


#ifdef Q_MOC_RUN
Q_SIGNALS:
#endif
    void finish(bool idle);
    void error(int error, const QString &errorString);

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        if (request->type() == QGalleryAbstractRequest::RemoveRequest) {
            QtGalleryTestResponse *response = new QtGalleryTestResponse(
                    m_status, m_error, m_errorString);

            connect(this, SIGNAL(finish(bool)), response, SLOT(doFinish(bool)));
            connect(this, SIGNAL(error(int,QString)), response, SLOT(doError(int,QString)));

            return response;
        }
        return 0;
    }

private:
    QGalleryAbstractRequest::Status m_status;
    int m_error;
    QString m_errorString;
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
    gallery.setStatus(QGalleryAbstractRequest::Finished);
    gallery.setError(80, QString());

    QGalleryRemoveRequest request(&gallery);

    request.execute();
    QCOMPARE(request.error(), 80);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Error);

    gallery.setError(QGalleryAbstractRequest::NoError, QString());
    request.execute();
    QCOMPARE(request.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);

    request.clear();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Inactive);
}

void tst_QGalleryRemoveRequest::executeAsynchronous()
{
    QtTestGallery gallery;
    gallery.setStatus(QGalleryAbstractRequest::Active);

    QGalleryRemoveRequest request(&gallery);

    request.execute();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Active);

    gallery.finish(false);
    QCOMPARE(request.status(), QGalleryAbstractRequest::Finished);

    request.clear();
    QCOMPARE(request.status(), QGalleryAbstractRequest::Inactive);
}

QTEST_MAIN(tst_QGalleryRemoveRequest)

#include "tst_qgalleryremoverequest.moc"
