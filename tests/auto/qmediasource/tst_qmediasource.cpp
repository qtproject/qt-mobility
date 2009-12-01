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

#include "qmediasource.h"

QTM_USE_NAMESPACE
class tst_QMediaSource : public QObject
{
    Q_OBJECT

private slots:
    void testNull();
    void testUriCtor();
    void testResourceCtor();
    void testResourceListCtor();
    void testCopy();
    void testAssignment();
    void testEquality();
    void testResources();
};

void tst_QMediaSource::testNull()
{
    QMediaSource media;

    QCOMPARE(media.isNull(), true);
    QCOMPARE(media.contentUri(), QUrl());
    QCOMPARE(media.contentResource(), QMediaResource());
}

void tst_QMediaSource::testUriCtor()
{
    QMediaSource media(QUrl("http://example.com/movie.mov"));

    QCOMPARE(media.contentUri(), QUrl("http://example.com/movie.mov"));
    QCOMPARE(media.contentResource().uri(), QUrl("http://example.com/movie.mov"));
}

void tst_QMediaSource::testResourceCtor()
{
    QMediaSource media(QMediaResource(QUrl("http://example.com/movie.mov")));

    QCOMPARE(media.contentResource(), QMediaResource(QUrl("http://example.com/movie.mov")));
}

void tst_QMediaSource::testResourceListCtor()
{
    QMediaResourceList  resourceList;
    resourceList << QMediaResource(QUrl("http://example.com/movie.mov"));

    QMediaSource        media(resourceList);

    QCOMPARE(media.contentUri(), QUrl("http://example.com/movie.mov"));
    QCOMPARE(media.contentResource().uri(), QUrl("http://example.com/movie.mov"));
}

void tst_QMediaSource::testCopy()
{
    QMediaSource media1(QMediaResource(QUrl("http://example.com/movie.mov")));
    QMediaSource media2(media1);

    QVERIFY(media1 == media2);
}

void tst_QMediaSource::testAssignment()
{
    QMediaSource media1(QMediaResource(QUrl("http://example.com/movie.mov")));
    QMediaSource media2;
    QMediaSource media3;

    media2 = media1;
    QVERIFY(media2 == media1);

    media2 = media3;
    QVERIFY(media2 == media3);
}

void tst_QMediaSource::testEquality()
{
    QMediaSource media1;
    QMediaSource media2;
    QMediaSource media3(QMediaResource(QUrl("http://example.com/movie.mov")));
    QMediaSource media4(QMediaResource(QUrl("http://example.com/movie.mov")));
    QMediaSource media5(QMediaResource(QUrl("file:///some/where/over/the/rainbow.mp3")));

    // null == null
    QCOMPARE(media1 == media2, true);
    QCOMPARE(media1 != media2, false);

    // null != something
    QCOMPARE(media1 == media3, false);
    QCOMPARE(media1 != media3, true);

    // equiv
    QCOMPARE(media3 == media4, true);
    QCOMPARE(media3 != media4, false);

    // not equiv
    QCOMPARE(media4 == media5, false);
    QCOMPARE(media4 != media5, true);
}

void tst_QMediaSource::testResources()
{
    QMediaResourceList  resourceList;

    resourceList << QMediaResource(QUrl("http://example.com/movie-main.mov"));
    resourceList << QMediaResource(QUrl("http://example.com/movie-big.mov"));
    resourceList << QMediaResource(QUrl("http://example.com/movie-little.mov"), QMediaResource::PreviewRole);
    resourceList << QMediaResource(QUrl("http://example.com/movie-poster.jpg"), QMediaResource::PosterRole);

    QMediaSource    media(resourceList);

    QMediaResourceList  res = media.resources(QMediaResource::ContentRole);
    QCOMPARE(res.size(), 2);
    QCOMPARE(res[0], QMediaResource(QUrl("http://example.com/movie-main.mov")));
    QCOMPARE(res[1], QMediaResource(QUrl("http://example.com/movie-big.mov")));

    QCOMPARE(media.resources(QMediaResource::PreviewRole)[0].uri(), QUrl("http://example.com/movie-little.mov"));
    QCOMPARE(media.resources(QMediaResource::PosterRole)[0].uri(), QUrl("http://example.com/movie-poster.jpg"));

    QCOMPARE(media.resources(QMediaResource::CoverArtRole).size(), 0);
    QCOMPARE(media.resources(QMediaResource::ThumbnailRole).size(), 0);
}

QTEST_MAIN(tst_QMediaSource)

#include "tst_qmediasource.moc"
