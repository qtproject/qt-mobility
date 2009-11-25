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

#include <qmediacontent.h>

QTM_USE_NAMESPACE
class tst_QMediaContent : public QObject
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
    void testCoverArt();
    void testThumbnails();
    void testPoster();
};

void tst_QMediaContent::testNull()
{
    QMediaContent media;

    QCOMPARE(media.isNull(), true);
    QCOMPARE(media.canonicalUri(), QUrl());
    QCOMPARE(media.canonicalResource(), QMediaResource());
    QCOMPARE(media.resources(), QMediaResourceList());

    media.setCoverArtUriSmall(QUrl("file:///cover/art/small.jpg"));
    media.setCoverArtUriLarge(QUrl("file:///cover/art/large.jpg"));
    media.setThumbnailUriSmall(QUrl("file:///thumbnails/small.jpg"));
    media.setThumbnailUriLarge(QUrl("file:///thumbnails/large.jpg"));
    media.setPosterUri(QUrl("file:///videos/poster.jpg"));

    QCOMPARE(media.coverArtUriSmall(), QUrl());
    QCOMPARE(media.coverArtUriLarge(), QUrl());
    QCOMPARE(media.thumbnailUriSmall(), QUrl());
    QCOMPARE(media.thumbnailUriLarge(), QUrl());
    QCOMPARE(media.posterUri(), QUrl());
}

void tst_QMediaContent::testUriCtor()
{
    QMediaContent media(QUrl("http://example.com/movie.mov"));

    QCOMPARE(media.canonicalUri(), QUrl("http://example.com/movie.mov"));
    QCOMPARE(media.canonicalResource().uri(), QUrl("http://example.com/movie.mov"));
}

void tst_QMediaContent::testResourceCtor()
{
    QMediaContent media(QMediaResource(QUrl("http://example.com/movie.mov")));

    QCOMPARE(media.canonicalResource(), QMediaResource(QUrl("http://example.com/movie.mov")));
}

void tst_QMediaContent::testResourceListCtor()
{
    QMediaResourceList  resourceList;
    resourceList << QMediaResource(QUrl("http://example.com/movie.mov"));

    QMediaContent media(resourceList);

    QCOMPARE(media.canonicalUri(), QUrl("http://example.com/movie.mov"));
    QCOMPARE(media.canonicalResource().uri(), QUrl("http://example.com/movie.mov"));
}

void tst_QMediaContent::testCopy()
{
    QMediaContent media1(QMediaResource(QUrl("http://example.com/movie.mov")));
    QMediaContent media2(media1);

    QVERIFY(media1 == media2);

    media1.setCoverArtUriLarge(QUrl("file:///cover/art/large.jpg"));
    QCOMPARE(media1.coverArtUriLarge(), QUrl("file:///cover/art/large.jpg"));
    QCOMPARE(media2.coverArtUriLarge(), QUrl());

    media2.setCoverArtUriSmall(QUrl("file:///cover/art/small.jpg"));
    QCOMPARE(media1.coverArtUriSmall(), QUrl());
    QCOMPARE(media2.coverArtUriSmall(), QUrl("file:///cover/art/small.jpg"));
}

void tst_QMediaContent::testAssignment()
{
    QMediaContent media1(QMediaResource(QUrl("http://example.com/movie.mov")));
    QMediaContent media2;
    QMediaContent media3;

    media2 = media1;
    QVERIFY(media2 == media1);

    media1.setCoverArtUriLarge(QUrl("file:///cover/art/large.jpg"));
    QCOMPARE(media1.coverArtUriLarge(), QUrl("file:///cover/art/large.jpg"));
    QCOMPARE(media2.coverArtUriLarge(), QUrl());

    media2.setCoverArtUriSmall(QUrl("file:///cover/art/small.jpg"));
    QCOMPARE(media1.coverArtUriSmall(), QUrl());
    QCOMPARE(media2.coverArtUriSmall(), QUrl("file:///cover/art/small.jpg"));

    media2 = media3;
    QVERIFY(media2 == media3);
}

void tst_QMediaContent::testEquality()
{
    QMediaContent media1;
    QMediaContent media2;
    QMediaContent media3(QMediaResource(QUrl("http://example.com/movie.mov")));
    QMediaContent media4(QMediaResource(QUrl("http://example.com/movie.mov")));
    QMediaContent media5(QMediaResource(QUrl("file:///some/where/over/the/rainbow.mp3")));

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

    media3.setCoverArtUriSmall(QUrl("file:///cover/art/small.jpg"));
    QCOMPARE(media3 == media4, false);
    QCOMPARE(media3 != media4, true);

    media4.setCoverArtUriSmall(QUrl("file:///cover/art/small.jpg"));
    QCOMPARE(media3 == media4, true);
    QCOMPARE(media3 != media4, false);

    media3.setCoverArtUriLarge(QUrl("file:///cover/art/large.jpg"));
    QCOMPARE(media3 == media4, false);
    QCOMPARE(media3 != media4, true);

    media4.setCoverArtUriLarge(QUrl("file:///cover/art/large.jpg"));
    QCOMPARE(media3 == media4, true);
    QCOMPARE(media3 != media4, false);

    media3.setThumbnailUriSmall(QUrl("file:///thumbnails/small.jpg"));
    QCOMPARE(media3 == media4, false);
    QCOMPARE(media3 != media4, true);

    media4.setThumbnailUriSmall(QUrl("file:///thumbnails/small.jpg"));
    QCOMPARE(media3 == media4, true);
    QCOMPARE(media3 != media4, false);

    media3.setThumbnailUriLarge(QUrl("file:///thumbnails/large.jpg"));
    QCOMPARE(media3 == media4, false);
    QCOMPARE(media3 != media4, true);

    media4.setThumbnailUriLarge(QUrl("file:///thumbnails/large.jpg"));
    QCOMPARE(media3 == media4, true);
    QCOMPARE(media3 != media4, false);

    media3.setPosterUri(QUrl("file:///videos/poster.jpg"));
    QCOMPARE(media3 == media4, false);
    QCOMPARE(media3 != media4, true);

    media4.setPosterUri(QUrl("file:///videos/poster.jpg"));
    QCOMPARE(media3 == media4, true);
    QCOMPARE(media3 != media4, false);
}

void tst_QMediaContent::testResources()
{
    QMediaResourceList  resourceList;

    resourceList << QMediaResource(QUrl("http://example.com/movie-main.mov"));
    resourceList << QMediaResource(QUrl("http://example.com/movie-big.mov"));
    QMediaContent    media(resourceList);

    QMediaResourceList  res = media.resources();
    QCOMPARE(res.size(), 2);
    QCOMPARE(res[0], QMediaResource(QUrl("http://example.com/movie-main.mov")));
    QCOMPARE(res[1], QMediaResource(QUrl("http://example.com/movie-big.mov")));
}

void tst_QMediaContent::testCoverArt()
{
    QMediaContent content(QUrl("http://example.com/song.mp3"));

    QCOMPARE(content.coverArtUriSmall(), QUrl());
    QCOMPARE(content.coverArtUriLarge(), QUrl());

    content.setCoverArtUriSmall(QUrl("http://example.com/covers/small.jpg"));
    content.setCoverArtUriLarge(QUrl("http://example.com/covers/large.jpg"));

    QCOMPARE(content.coverArtUriSmall(), QUrl("http://example.com/covers/small.jpg"));
    QCOMPARE(content.coverArtUriLarge(), QUrl("http://example.com/covers/large.jpg"));
}

void tst_QMediaContent::testThumbnails()
{
    QMediaContent content(QUrl("http://example.com/photo.jpg"));

    QCOMPARE(content.thumbnailUriSmall(), QUrl());
    QCOMPARE(content.thumbnailUriLarge(), QUrl());

    content.setThumbnailUriSmall(QUrl("http://example.com/thumbnails/small.jpg"));
    content.setThumbnailUriLarge(QUrl("http://example.com/thumbnails/large.jpg"));

    QCOMPARE(content.thumbnailUriSmall(), QUrl("http://example.com/thumbnails/small.jpg"));
    QCOMPARE(content.thumbnailUriLarge(), QUrl("http://example.com/thumbnails/large.jpg"));
}

void tst_QMediaContent::testPoster()
{
    QMediaContent content(QUrl("http://example.com/video.mp4"));

    QCOMPARE(content.posterUri(), QUrl());

    content.setPosterUri(QUrl("http://example.com/poster.jpg"));
    QCOMPARE(content.posterUri(), QUrl("http://example.com/poster.jpg"));
}

QTEST_MAIN(tst_QMediaContent)

#include "tst_qmediacontent.moc"
