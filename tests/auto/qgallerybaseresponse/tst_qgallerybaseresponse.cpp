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

#include <qgallerybaseresponse_p.h>

QTM_USE_NAMESPACE

class tst_QGalleryBaseResponse : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void noResult();
    void successfulResult();
    void unsuccessfulResult();
};

void tst_QGalleryBaseResponse::noResult()
{
    QGalleryBaseResponse response;

    QCOMPARE(response.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(response.waitForFinished(300), true);
}

void tst_QGalleryBaseResponse::successfulResult()
{

    QGalleryBaseResponse response(QGalleryAbstractRequest::Succeeded);

    QCOMPARE(response.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(response.waitForFinished(300), true);
}

void tst_QGalleryBaseResponse::unsuccessfulResult()
{
    QGalleryBaseResponse response(QGalleryAbstractRequest::ItemTypeError);

    QCOMPARE(response.result(), int(QGalleryAbstractRequest::ItemTypeError));
    QCOMPARE(response.isIdle(), false);
    QCOMPARE(response.waitForFinished(300), true);
}

#include "tst_qgallerybaseresponse.moc"

QTEST_MAIN(tst_QGalleryBaseResponse)
