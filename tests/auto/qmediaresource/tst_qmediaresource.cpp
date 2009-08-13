/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include "multimedia/qmediaresource.h"

class tst_QMediaResource : public QObject
{
    Q_OBJECT
private slots:
    void constructNull();
    void construct_data();
    void construct();
    void equality();
    void copy();
    void assign();
};

void tst_QMediaResource::constructNull()
{
    QMediaResource resource;

    QCOMPARE(resource.isNull(), true);
    QCOMPARE(resource.uri(), QUrl());
    QCOMPARE(resource.mimeType(), QString());
    QCOMPARE(resource.role(), QMediaResource::ContentRole);
    QCOMPARE(resource.audioCodec(), QString());
    QCOMPARE(resource.videoCodec(), QString());
    QCOMPARE(resource.size(), qint64(0));
    QCOMPARE(resource.duration(), qint64(0));
    QCOMPARE(resource.bitRate(), 0);
    QCOMPARE(resource.sampleSize(), 0);
    QCOMPARE(resource.frequency(), 0);
    QCOMPARE(resource.channels(), 0);
    QCOMPARE(resource.resolution(), QSize());
    QCOMPARE(resource.bitsPerPixel(), 0);
}

void tst_QMediaResource::construct_data()
{
    QTest::addColumn<QUrl>("uri");
    QTest::addColumn<QString>("mimeType");
    QTest::addColumn<QMediaResource::ResourceRole>("role");
    QTest::addColumn<QString>("audioCodec");
    QTest::addColumn<QString>("videoCodec");
    QTest::addColumn<qint64>("size");
    QTest::addColumn<qint64>("duration");
    QTest::addColumn<int>("bitRate");
    QTest::addColumn<int>("sampleSize");
    QTest::addColumn<int>("frequency");
    QTest::addColumn<int>("channels");
    QTest::addColumn<QSize>("resolution");
    QTest::addColumn<int>("bitsPerPixel");

    QTest::newRow("audio content")
            << QUrl(QString::fromLatin1("http:://test.com/test.mp3"))
            << QString::fromLatin1("audio/mpeg")
            << QMediaResource::ContentRole
            << QString::fromLatin1("mp3")
            << QString()
            << qint64(5465433)
            << qint64(17533)
            << 128000
            << 8
            << 44100
            << 2
            << QSize()
            << 0;
    QTest::newRow("image content")
            << QUrl(QString::fromLatin1("http:://test.com/test.jpg"))
            << QString::fromLatin1("image/jpeg")
            << QMediaResource::ContentRole
            << QString()
            << QString()
            << qint64(23600)
            << qint64(0)
            << 0
            << 0
            << 0
            << 0
            << QSize(640, 480)
            << 12;
    QTest::newRow("video content")
            << QUrl(QString::fromLatin1("http:://test.com/test.mp4"))
            << QString::fromLatin1("video/mp4")
            << QMediaResource::ContentRole
            << QString::fromLatin1("aac")
            << QString::fromLatin1("h264")
            << qint64(36245851)
            << qint64(36214)
            << 750000
            << 16
            << 44000
            << 5
            << QSize(720, 576)
            << 16;
    QTest::newRow("thumbnail")
            << QUrl(QString::fromLatin1("file::///thumbs/test.png"))
            << QString::fromLatin1("image/png")
            << QMediaResource::ThumbnailRole
            << QString()
            << QString()
            << qint64(2360)
            << qint64(0)
            << 0
            << 0
            << 0
            << 0
            << QSize(128, 128)
            << 12;
}

void tst_QMediaResource::construct()
{
    QFETCH(QUrl, uri);
    QFETCH(QString, mimeType);
    QFETCH(QMediaResource::ResourceRole, role);
    QFETCH(QString, audioCodec);
    QFETCH(QString, videoCodec);
    QFETCH(qint64, size);
    QFETCH(qint64, duration);
    QFETCH(int, bitRate);
    QFETCH(int, sampleSize);
    QFETCH(int, frequency);
    QFETCH(int, channels);
    QFETCH(QSize, resolution);
    QFETCH(int, bitsPerPixel);

    {
        QMediaResource resource(uri, role);

        QCOMPARE(resource.isNull(), false);
        QCOMPARE(resource.uri(), uri);
        QCOMPARE(resource.mimeType(), QString());
        QCOMPARE(resource.role(), role);
        QCOMPARE(resource.audioCodec(), QString());
        QCOMPARE(resource.videoCodec(), QString());
        QCOMPARE(resource.size(), qint64(0));
        QCOMPARE(resource.duration(), qint64(0));
        QCOMPARE(resource.bitRate(), 0);
        QCOMPARE(resource.sampleSize(), 0);
        QCOMPARE(resource.frequency(), 0);
        QCOMPARE(resource.channels(), 0);
        QCOMPARE(resource.resolution(), QSize());
        QCOMPARE(resource.bitsPerPixel(), 0);
    }
    {
        QMediaResource resource(uri, mimeType, role);

        QCOMPARE(resource.isNull(), false);
        QCOMPARE(resource.uri(), uri);
        QCOMPARE(resource.mimeType(), mimeType);
        QCOMPARE(resource.role(), role);
        QCOMPARE(resource.audioCodec(), QString());
        QCOMPARE(resource.videoCodec(), QString());
        QCOMPARE(resource.size(), qint64(0));
        QCOMPARE(resource.duration(), qint64(0));
        QCOMPARE(resource.bitRate(), 0);
        QCOMPARE(resource.sampleSize(), 0);
        QCOMPARE(resource.frequency(), 0);
        QCOMPARE(resource.channels(), 0);
        QCOMPARE(resource.resolution(), QSize());
        QCOMPARE(resource.bitsPerPixel(), 0);

        resource.setAudioCodec(audioCodec);
        resource.setVideoCodec(videoCodec);
        resource.setSize(size);
        resource.setDuration(duration);
        resource.setBitRate(bitRate);
        resource.setSampleSize(sampleSize);
        resource.setFrequency(frequency);
        resource.setChannels(channels);
        resource.setResolution(resolution);
        resource.setBitsPerPixel(bitsPerPixel);

        QCOMPARE(resource.audioCodec(), audioCodec);
        QCOMPARE(resource.videoCodec(), videoCodec);
        QCOMPARE(resource.size(), size);
        QCOMPARE(resource.duration(), duration);
        QCOMPARE(resource.bitRate(), bitRate);
        QCOMPARE(resource.sampleSize(), sampleSize);
        QCOMPARE(resource.frequency(), frequency);
        QCOMPARE(resource.channels(), channels);
        QCOMPARE(resource.resolution(), resolution);
        QCOMPARE(resource.bitsPerPixel(), bitsPerPixel);
    }
}

void tst_QMediaResource::equality()
{
    QMediaResource resource1(
            QUrl(QString::fromLatin1("http://test.com/test.mp4")),
            QString::fromLatin1("video/mp4"),
            QMediaResource::ContentRole);
    QMediaResource resource2(
            QUrl(QString::fromLatin1("http://test.com/test.mp4")),
            QString::fromLatin1("video/mp4"),
            QMediaResource::ContentRole);
    QMediaResource resource3(
            QUrl(QString::fromLatin1("file:///thumbs/test.jpg")), QMediaResource::ThumbnailRole);
    QMediaResource resource4(
            QUrl(QString::fromLatin1("file:///thumbs/test.jpg")), QMediaResource::ThumbnailRole);
    QMediaResource resource5(
            QUrl(QString::fromLatin1("http://test.com/test.mp3")),
            QString::fromLatin1("audio/mpeg"),
            QMediaResource::ContentRole);

    QCOMPARE(resource1 == resource2, true);
    QCOMPARE(resource1 != resource2, false);

    QCOMPARE(resource3 == resource4, true);
    QCOMPARE(resource3 != resource4, false);

    QCOMPARE(resource1 == resource3, false);
    QCOMPARE(resource1 != resource3, true);

    QCOMPARE(resource1 == resource5, false);
    QCOMPARE(resource1 != resource5, true);

    resource1.setAudioCodec(QString::fromLatin1("mp3"));
    resource2.setAudioCodec(QString::fromLatin1("aac"));

    // Not equal differing audio codecs.
    QCOMPARE(resource1 == resource2, false);
    QCOMPARE(resource1 != resource2, true);

    resource1.setAudioCodec(QString::fromLatin1("aac"));

    // Equal.
    QCOMPARE(resource1 == resource2, true);
    QCOMPARE(resource1 != resource2, false);

    resource1.setVideoCodec(QString());

    // Equal.
    QCOMPARE(resource1 == resource2, true);
    QCOMPARE(resource1 != resource2, false);

    resource1.setVideoCodec(QString::fromLatin1("h264"));

    // Not equal differing video codecs.
    QCOMPARE(resource1 == resource2, false);
    QCOMPARE(resource1 != resource2, true);

    resource2.setVideoCodec(QString::fromLatin1("h264"));

    // Equal.
    QCOMPARE(resource1 == resource2, true);
    QCOMPARE(resource1 != resource2, false);

    resource2.setSize(0);

    // Equal.
    QCOMPARE(resource1 == resource2, true);
    QCOMPARE(resource1 != resource2, false);

    resource1.setSize(546423);

    // Not equal differing video codecs.
    QCOMPARE(resource1 == resource2, false);
    QCOMPARE(resource1 != resource2, true);

    resource2.setSize(546423);

    // Equal.
    QCOMPARE(resource1 == resource2, true);
    QCOMPARE(resource1 != resource2, false);

    resource1.setDuration(54335);
    resource2.setDuration(443233);
    resource1.setBitRate(543);
    resource2.setSampleSize(16);
    resource1.setFrequency(48000);
    resource2.setFrequency(44100);
    resource1.setChannels(0);
    resource2.setBitsPerPixel(24);

    // Not equal, differing duration, bit rate, sample size, frequency, and bits per pixel.
    QCOMPARE(resource1 == resource2, false);
    QCOMPARE(resource1 != resource2, true);

    resource1.setDuration(443233);
    resource2.setSampleSize(0);

    // Not equal, differing bit rate, frequency, and bits per pixel.
    QCOMPARE(resource1 == resource2, false);
    QCOMPARE(resource1 != resource2, true);

    resource2.setBitRate(543);
    resource1.setFrequency(44100);

    // Not equal, differing bits per pixel.
    QCOMPARE(resource1 == resource2, false);
    QCOMPARE(resource1 != resource2, true);

    resource1.setBitsPerPixel(24);

    // Equal
    QCOMPARE(resource1 == resource2, true);
    QCOMPARE(resource1 != resource2, false);

    resource1.setResolution(QSize());

    // Equal
    QCOMPARE(resource1 == resource2, true);
    QCOMPARE(resource1 != resource2, false);

    resource1.setResolution(QSize(640, 480));
    resource2.setResolution(QSize(800, 600));

    // Not equal, differing resolution.
    QCOMPARE(resource1 == resource2, false);
    QCOMPARE(resource1 != resource2, true);

    resource1.setResolution(800, 600);

    // Equal
    QCOMPARE(resource1 == resource2, true);
    QCOMPARE(resource1 != resource2, false);
}

void tst_QMediaResource::copy()
{
    const QUrl uri(QString::fromLatin1("http://test.com/test.mp4"));
    const QString mimeType(QLatin1String("video/mp4"));
    const QMediaResource::ResourceRole role = QMediaResource::PreviewRole;
    const QString amrCodec(QLatin1String("amr"));
    const QString mp3Codec(QLatin1String("mp3"));
    const QString aacCodec(QLatin1String("aac"));
    const QString h264Codec(QLatin1String("h264"));

    QMediaResource original(uri, mimeType, role);
    original.setAudioCodec(amrCodec);

    QMediaResource copy(original);

    QCOMPARE(copy.uri(), uri);
    QCOMPARE(copy.mimeType(), mimeType);
    QCOMPARE(copy.role(), role);
    QCOMPARE(copy.audioCodec(), amrCodec);

    QCOMPARE(original == copy, true);
    QCOMPARE(original != copy, false);

    original.setAudioCodec(mp3Codec);

    QCOMPARE(copy.audioCodec(), amrCodec);
    QCOMPARE(original == copy, false);
    QCOMPARE(original != copy, true);

    copy.setAudioCodec(aacCodec);
    copy.setVideoCodec(h264Codec);

    QCOMPARE(copy.uri(), uri);
    QCOMPARE(copy.mimeType(), mimeType);
    QCOMPARE(copy.role(), role);

    QCOMPARE(original.audioCodec(), mp3Codec);
}

void tst_QMediaResource::assign()
{
    const QUrl uri(QString::fromLatin1("http://test.com/test.mp4"));
    const QString mimeType(QLatin1String("video/mp4"));
    const QMediaResource::ResourceRole role = QMediaResource::PreviewRole;
    const QString amrCodec(QLatin1String("amr"));
    const QString mp3Codec(QLatin1String("mp3"));
    const QString aacCodec(QLatin1String("aac"));
    const QString h264Codec(QLatin1String("h264"));

    QMediaResource copy(QUrl(QString::fromLatin1("file:///thumbs/test.jpg")));

    QMediaResource original(uri, mimeType, role);
    original.setAudioCodec(amrCodec);

    copy = original;

    QCOMPARE(copy.uri(), uri);
    QCOMPARE(copy.mimeType(), mimeType);
    QCOMPARE(copy.role(), role);
    QCOMPARE(copy.audioCodec(), amrCodec);

    QCOMPARE(original == copy, true);
    QCOMPARE(original != copy, false);

    original.setAudioCodec(mp3Codec);

    QCOMPARE(copy.audioCodec(), amrCodec);
    QCOMPARE(original == copy, false);
    QCOMPARE(original != copy, true);

    copy.setAudioCodec(aacCodec);
    copy.setVideoCodec(h264Codec);

    QCOMPARE(copy.uri(), uri);
    QCOMPARE(copy.mimeType(), mimeType);
    QCOMPARE(copy.role(), role);

    QCOMPARE(original.audioCodec(), mp3Codec);
}

QTEST_MAIN(tst_QMediaResource)

#include "tst_qmediaresource.moc"
