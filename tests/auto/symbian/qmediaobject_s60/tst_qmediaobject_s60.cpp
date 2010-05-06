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

#include <QtCore/qtimer.h>

#include <qmediaobject.h>
#include <qmediaservice.h>
#include <qmetadatacontrol.h>
#include <qmediaplayer.h>

QT_USE_NAMESPACE
class tst_QMediaObject : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase_data();
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void isMetaDataAvailable();
    void isWritable();
    void metaData();
    void availableMetaData();
    void setMetaData();
    void extendedMetaData();
    void availableExtendedMetaData();
    void setExtendedMetaData();

private:
    QString metaDataKeyAsString(QtMediaServices::MetaData key) const;
};

void tst_QMediaObject::initTestCase_data()
{
    QTest::addColumn<bool>("valid");
    //QTest::addColumn<QMediaPlayer::State>("state");
    //QTest::addColumn<QMediaPlayer::MediaStatus>("status");
    QTest::addColumn<QMediaContent>("mediaContent");
    QTest::addColumn<bool>("metaDataAvailable");
    QTest::addColumn<bool>("metaDataWritable"); // Is this needed ???
    //QTest::addColumn<int>("bufferStatus");
    //QTest::addColumn<qreal>("playbackRate");
    //QTest::addColumn<QMediaPlayer::Error>("error");
    //QTest::addColumn<QString>("errorString");

    QTest::newRow("TestDataNull")
    << false // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent() // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    /*
    QTest::newRow("test_3gp.3gp")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_3gp.3gp")) // mediaContent
    << qint64(-1) // duration
    << qint64(0) // position
    << false // seekable
    << 0 // volume
    << false // muted
    << false; // videoAvailable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString
    */

    QTest::newRow("test_amr.amr")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_amr.amr")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_flash_video.flv")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_flash_video.flv")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_invalid_extension_mp4.xyz")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_invalid_extension_mp4.xyz")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_invalid_extension_wav.xyz")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_invalid_extension_wav.xyz")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_mp3.mp3")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_mp3.mp3")) // mediaContent
#if !defined(__WINS__) || !defined(__WINSCW__)
    << true // metaDataAvailable
#else
    << false // metaDataAvailable
#endif // !defined(__WINS__) || defined(__WINSCW__)
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_mp3_no_metadata.mp3")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_mp3_no_metadata.mp3")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_mp4.mp4")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_mp4.mp4")) // mediaContent
#if defined(__WINS__) || defined(__WINSCW__)
    << true // metaDataAvailable
#else
    << false // metaDataAvailable
#endif // !defined(__WINS__) || defined(__WINSCW__)
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_wav.wav")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_wav.wav")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

    QTest::newRow("test_wmv9.wmv")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("file:///C:/data/testfiles/test_wmv9.wmv")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString


    QTest::newRow("test youtube stream")
    << true // valid
    //<< QMediaPlayer::StoppedState // state
    //<< QMediaPlayer::UnknownMediaStatus // status
    << QMediaContent(QUrl("rtsp://v3.cache4.c.youtube.com/CkgLENy73wIaPwlU2rm7yu8PFhMYESARFEIJbXYtZ29vZ2xlSARSB3JlbGF0ZWRaDkNsaWNrVGh1bWJuYWlsYPi6_IXT2rvpSgw=/0/0/0/video.3gp")) // mediaContent
    << false // metaDataAvailable
    << false; // metaDataWritable
    //<< 0 // bufferStatus
    //<< qreal(0) // playbackRate
    //<< QMediaPlayer::NoError // error
    //<< QString(); // errorString

}

void tst_QMediaObject::initTestCase()
{
}

void tst_QMediaObject::cleanupTestCase()
{
}

void tst_QMediaObject::init()
{
    qRegisterMetaType<QMediaContent>("QMediaContent");
}

void tst_QMediaObject::cleanup()
{
}

void tst_QMediaObject::isMetaDataAvailable()
{
    QFETCH_GLOBAL(bool, metaDataAvailable);
    QFETCH_GLOBAL(QMediaContent, mediaContent);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700); 
    QVERIFY(player.isMetaDataAvailable() == metaDataAvailable);
}

void tst_QMediaObject::isWritable()
{
    QFETCH_GLOBAL(bool, metaDataWritable);
    QFETCH_GLOBAL(QMediaContent, mediaContent);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700);
    QVERIFY(player.isMetaDataWritable() == metaDataWritable);
}

void tst_QMediaObject::metaData()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(bool, metaDataAvailable);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700);
    const QString artist(QLatin1String("Artist"));
    const QString title(QLatin1String("Title"));

    if (player.isMetaDataAvailable()) {
        QEXPECT_FAIL("", "player.metaData(QtMediaServices::AlbumArtist) failed: ", Continue);
        QCOMPARE(player.metaData(QtMediaServices::AlbumArtist).toString(), artist);
        QEXPECT_FAIL("", "player.metaData(QtMediaServices::Title) failed: ", Continue);
        QCOMPARE(player.metaData(QtMediaServices::Title).toString(), title);
    }
}

void tst_QMediaObject::availableMetaData()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(bool, metaDataAvailable);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700);    

    if (player.isMetaDataAvailable()) {
        QList<QtMediaServices::MetaData> metaDataKeys = player.availableMetaData();
        QEXPECT_FAIL("", "metaDataKeys.count() failed: ", Continue);
        QVERIFY(metaDataKeys.count() > 0);
//        qWarning() << "metaDataKeys.count: " << metaDataKeys.count();
        QEXPECT_FAIL("", "metaDataKeys.contains(QtMediaServices::AlbumArtist) failed: ", Continue);
        QVERIFY(metaDataKeys.contains(QtMediaServices::AlbumArtist));
        QEXPECT_FAIL("", "metaDataKeys.contains(QtMediaServices::Title) failed: ", Continue);
        QVERIFY(metaDataKeys.contains(QtMediaServices::Title));
    }
}

void tst_QMediaObject::setMetaData()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700); 

    QString title("Titletest");
    if (player.isMetaDataWritable()) {
        player.setMetaData(QtMediaServices::Title, title); 
        QCOMPARE(player.metaData(QtMediaServices::Title).toString(), title);
    }
}

void tst_QMediaObject::extendedMetaData()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700);
    const QString artist(QLatin1String("Artist"));
    const QString title(QLatin1String("Title"));

    if (player.isMetaDataAvailable()) {  
        QEXPECT_FAIL("", "player.extendedMetaData(QtMediaServices::AlbumArtist) failed: ", Continue);
        QCOMPARE(player.extendedMetaData(metaDataKeyAsString(QtMediaServices::AlbumArtist)).toString(), artist);
        QEXPECT_FAIL("", "player.extendedMetaData(QtMediaServices::Title) failed: ", Continue);
        QCOMPARE(player.extendedMetaData(metaDataKeyAsString(QtMediaServices::Title)).toString(), title);
    }
}

void tst_QMediaObject::availableExtendedMetaData()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700);
    const QString artist(QLatin1String("Artist"));
    const QString title(QLatin1String("Title"));
    
    if (player.isMetaDataAvailable()) {
        QStringList metaDataKeys = player.availableExtendedMetaData();
        QVERIFY(metaDataKeys.count() > 0);
/*        qWarning() << "metaDataKeys.count: " << metaDataKeys.count();
        int count = metaDataKeys.count();
        count = count-1;
        int i = 0;
        while(count >= i)
            {            
            qWarning() << "metaDataKeys " << i <<". " << metaDataKeys.at(i);
            i++;
            }*/
        QEXPECT_FAIL("", "metaDataKeys.contains(QtMediaServices::AlbumArtist) failed: ", Continue);
        QVERIFY(metaDataKeys.contains(metaDataKeyAsString(QtMediaServices::AlbumArtist)));
        QEXPECT_FAIL("", "metaDataKeys.contains(QtMediaServices::AlbumArtist) failed: ", Continue);
        QVERIFY(metaDataKeys.contains(metaDataKeyAsString(QtMediaServices::Title)));
    }
}

void tst_QMediaObject::setExtendedMetaData()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
//    qWarning() << mediaContent.canonicalUrl();
    QMediaPlayer player;

    player.setMedia(mediaContent);
    QTest::qWait(700);
    const QString title(QLatin1String("Titletest"));

    if (player.isMetaDataWritable()) {
        player.setExtendedMetaData(metaDataKeyAsString(QtMediaServices::Title), title);  
        QCOMPARE(player.metaData(QtMediaServices::Title).toString(), title);
    }
}

QString tst_QMediaObject::metaDataKeyAsString(QtMediaServices::MetaData key) const
{
    switch(key) {
        case QtMediaServices::Title: return "title";
        case QtMediaServices::AlbumArtist: return "artist";
        case QtMediaServices::Comment: return "comment";
        case QtMediaServices::Genre: return "genre";
        case QtMediaServices::Year: return "year";
        case QtMediaServices::Copyright: return "copyright";
        case QtMediaServices::AlbumTitle: return "album";
        case QtMediaServices::Composer: return "composer";
        case QtMediaServices::TrackNumber: return "albumtrack";
        case QtMediaServices::AudioBitRate: return "audiobitrate";
        case QtMediaServices::VideoBitRate: return "videobitrate";
        case QtMediaServices::Duration: return "duration";
        case QtMediaServices::MediaType: return "contenttype";
        case QtMediaServices::SubTitle: // TODO: Find the matching metadata keys
        case QtMediaServices::Description:
        case QtMediaServices::Category:
        case QtMediaServices::Date:
        case QtMediaServices::UserRating:
        case QtMediaServices::Keywords:
        case QtMediaServices::Language:
        case QtMediaServices::Publisher:
        case QtMediaServices::ParentalRating:
        case QtMediaServices::RatingOrganisation:
        case QtMediaServices::Size:
        case QtMediaServices::AudioCodec:
        case QtMediaServices::AverageLevel:
        case QtMediaServices::ChannelCount:
        case QtMediaServices::PeakValue:
        case QtMediaServices::SampleRate:
        case QtMediaServices::Author:
        case QtMediaServices::ContributingArtist:
        case QtMediaServices::Conductor:
        case QtMediaServices::Lyrics:
        case QtMediaServices::Mood:
        case QtMediaServices::TrackCount:
        case QtMediaServices::CoverArtUrlSmall:
        case QtMediaServices::CoverArtUrlLarge:
        case QtMediaServices::Resolution:
        case QtMediaServices::PixelAspectRatio:
        case QtMediaServices::VideoFrameRate:
        case QtMediaServices::VideoCodec:
        case QtMediaServices::PosterUrl:
        case QtMediaServices::ChapterNumber:
        case QtMediaServices::Director:
        case QtMediaServices::LeadPerformer:
        case QtMediaServices::Writer:
        case QtMediaServices::CameraManufacturer:
        case QtMediaServices::CameraModel:
        case QtMediaServices::Event:
        case QtMediaServices::Subject:
        default:
            break;
    }

    return QString();
}

QTEST_MAIN(tst_QMediaObject)

#include "tst_qmediaobject_s60.moc"
