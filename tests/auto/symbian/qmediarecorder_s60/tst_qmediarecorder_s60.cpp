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
#include <QDebug>
#include <qmediaobject.h>
#include <qmediacontrol.h>
#include <qmediaservice.h>
#include <qmediarecordercontrol.h>
#include <qmediarecorder.h>
#include <qaudioendpointselector.h>
#include <qvideodevicecontrol.h>
#include <qaudioencodercontrol.h>
#include <qmediacontainercontrol.h>
#include <qvideoencodercontrol.h>
#include <QtMultimedia/qaudioformat.h>
#include <qaudiocapturesource.h>

#define QTRY_COMPARE(a,e)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if ((a) == (e)) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QCOMPARE(a, e)

#define QTRY_VERIFY(a)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if (a) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QVERIFY(a)

QTM_USE_NAMESPACE
class tst_QMediaRecorder: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testAudioSink();
    void testAudioRecord();
    void testAudioEndPointSelector();
    void testAudioEncoderControl();
    void testMediaFormatsControl();
    void testDefaultVideoEncodingSettings();
//    void testDefaultAudioEncodingSettings();
    void testVideoEncoderControl();
    void testEncodingSettings();

private:
    QUrl recordPath(QDir outputDir);

    QAudioEncoderControl *audioEncoder;
    QAudioEndpointSelector *audioEndpoint;
    QMediaRecorder  *capture;
    QMediaRecorder  *audiocapture;
    QAudioCaptureSource *captureSource;
};

void tst_QMediaRecorder::initTestCase()
{
    qRegisterMetaType<QtMobility::QMediaRecorder::State>("QMediaRecorder::State");
    qRegisterMetaType<QtMobility::QMediaRecorder::Error>("QMediaRecorder::Error");

    captureSource = new QAudioCaptureSource;
    audiocapture = new QMediaRecorder(captureSource);

    audioEndpoint = qobject_cast<QAudioEndpointSelector*>(audiocapture->service()->control(QAudioEndpointSelector_iid));    
    audioEncoder = qobject_cast<QAudioEncoderControl*>(audiocapture->service()->control(QAudioEncoderControl_iid));
}

void tst_QMediaRecorder::cleanupTestCase()
{
    delete audiocapture;
    delete captureSource;
    delete capture;
}

QUrl tst_QMediaRecorder::recordPath(QDir outputDir)
{
    int lastImage = 0;
    foreach( QString fileName, outputDir.entryList(QStringList() << "testclip_*.mpg") ) {
        int imgNumber = fileName.mid(5, fileName.size()-9).toInt();
        lastImage = qMax(lastImage, imgNumber);
    }

    QUrl location(QDir::toNativeSeparators(outputDir.canonicalPath()+QString("/testclip_%1.mpg").arg(lastImage+1,4,10,QLatin1Char('0'))));
    return location;
}

void tst_QMediaRecorder::testAudioSink()
{
    audiocapture->setOutputLocation(QUrl("test.tmp"));
    QUrl s = audiocapture->outputLocation();
    QCOMPARE(s.toString(), QString("test.tmp"));
}

void tst_QMediaRecorder::testAudioRecord()
{
}

void tst_QMediaRecorder::testAudioEndPointSelector()
{
    QSignalSpy audioSignal(audioEndpoint,SIGNAL(activeEndpointChanged(QString)));        
    QVERIFY(audioEndpoint->availableEndpoints().count() == 1);
    QVERIFY(audioEndpoint->defaultEndpoint().compare("MMF") == 0);    
    audioEndpoint->setActiveEndpoint("device2");    
    QVERIFY(audioEndpoint->activeEndpoint().compare("device2") == 0);    
    QVERIFY(audioSignal.count() == 1);    
    QVERIFY(audioEndpoint->endpointDescription("device2").compare("") == 0);    
}

void tst_QMediaRecorder::testAudioEncoderControl()
{
    QStringList codecs = audiocapture->supportedAudioCodecs();    
    QVERIFY(codecs.count() == 1);    
    QVERIFY(audiocapture->audioCodecDescription("audio/wav") == "WAV Write Format");
    QStringList options = audioEncoder->supportedEncodingOptions("audio/wav");
    QCOMPARE(options.count(), 0);    
    QVERIFY(audioEncoder->encodingOption("audio/wav","bitrate").toInt() == -1);
    audioEncoder->setEncodingOption("audio/wav", "bitrate", QString("vbr"));
    QCOMPARE(audioEncoder->encodingOption("audio/wav","bitrate").toInt(), -1);
    QCOMPARE(audiocapture->supportedAudioSampleRates(), QList<int>() << 44100);
}

void tst_QMediaRecorder::testMediaFormatsControl()
{
    //videocontainer types
    QCOMPARE(capture->supportedContainers(), QStringList() << "video/3gpp" << "video/mp4" << "video/3gpp2");
    QCOMPARE(capture->containerDescription("video/3gpp"), QString("3GPP File Format"));
    QCOMPARE(capture->containerDescription("video/mp4"), QString("MPEG-4 File Format"));
    QCOMPARE(capture->containerDescription("video/3gpp2"), QString("3GPP2 File Format"));
    //audioocontainer types
    QCOMPARE(audiocapture->supportedContainers(), QStringList() << "audio/wav" << "audio/pcm");
    QCOMPARE(audiocapture->containerDescription("audio/pcm"), QString("WAV file format"));
    QCOMPARE(audiocapture->containerDescription("audio/x-wav"), QString("RAW file format"));
//    QCOMPARE(capture->containerDescription("mp3"), QString("MP3 format"));
//    QCOMPARE(capture->containerDescription("ogg"), QString());
}

void tst_QMediaRecorder::testDefaultVideoEncodingSettings()
{
    QVideoEncoderSettings videoSettings = capture->videoSettings();
    QCOMPARE(videoSettings.codec(), QString("video/H263-2000"));
    QCOMPARE(videoSettings.bitRate(), -1);
    QCOMPARE(videoSettings.resolution(), QSize(176, 144));
    QCOMPARE(videoSettings.frameRate(), 15.0);
    QCOMPARE(videoSettings.quality(), QtMedia::LowQuality);
//    QCOMPARE(videoSettings.encodingMode(), QtMedia::ConstantQualityEncoding);

    QString format = capture->containerMimeType();
    QCOMPARE(format, QString("video/3gpp"));
}
/*  
void tst_QMediaRecorder::testDefaultAudioEncodingSettings()
{
  QAudioEncoderSettings audioSettings = capture->audioSettings();
    QCOMPARE(audioSettings.codec(), QString("audio/pcm"));
    QCOMPARE(audioSettings.bitRate(), 128*1024);
    QCOMPARE(audioSettings.sampleRate(), -1);
    QCOMPARE(audioSettings.quality(), QtMedia::NormalQuality);
    QCOMPARE(audioSettings.channelCount(), -1);

    QCOMPARE(audioSettings.encodingMode(), QtMedia::ConstantQualityEncoding);
   
}*/ 

void tst_QMediaRecorder::testVideoEncoderControl()
{
    bool continuous = false;
    QList<QSize> sizes = capture->supportedResolutions(QVideoEncoderSettings(), &continuous);
    QCOMPARE(sizes.count(), 3);
    QCOMPARE(continuous, false);

    QList<qreal> rates = capture->supportedFrameRates(QVideoEncoderSettings(), &continuous);
    QCOMPARE(rates.count(), 1);
    QCOMPARE(continuous, false);

    QStringList vCodecs = capture->supportedVideoCodecs();
    QVERIFY(vCodecs.count() == 3);
    QCOMPARE(capture->videoCodecDescription("video/mp4"), QString("MPEG-4 File Format"));
    QCOMPARE(capture->videoCodecDescription("video/3gpp"), QString("3GPP File Format"));
    QCOMPARE(capture->videoCodecDescription("video/3ggp2"), QString("3GPP2 File Format"));

    QStringList options = videoEncoder->supportedEncodingOptions("video/mp4");
    QCOMPARE(options.count(), 4);
    
    QVERIFY(videoEncoder->encodingOption("video/mp4","bitrate") == QVariant(-1));
    videoEncoder->setEncodingOption("video/mp4", "bitrate", QVariant(128000));
    QCOMPARE(videoEncoder->encodingOption("video/mp4","bitrate"), QVariant(128000));
    videoEncoder->setEncodingOption("video/mp4", "bitrate", QVariant(-1));
}

void tst_QMediaRecorder::testEncodingSettings()
{
    QAudioEncoderSettings audioSettings;
    QVideoEncoderSettings videoSettings;
    QString format;
/*
    audioSettings.setCodec("audio/mpeg");
    audioSettings.setSampleRate(44100);
    audioSettings.setBitRate(256*1024);
    audioSettings.setQuality(QtMedia::HighQuality);
    audioSettings.setEncodingMode(QtMedia::AverageBitRateEncoding);*/

    videoSettings.setCodec("video/3gpp");
    videoSettings.setBitRate(800);
    videoSettings.setFrameRate(24*1024);
    videoSettings.setResolution(QSize(800,600));
    videoSettings.setQuality(QtMedia::HighQuality);
//    audioSettings.setEncodingMode(QtMedia::TwoPassEncoding);
    
    format = QString("mov");
    
    capture->setEncodingSettings(audioSettings,videoSettings,format);

    QCOMPARE(capture->audioSettings(), audioSettings);
    QCOMPARE(capture->videoSettings(), videoSettings);
    QCOMPARE(capture->containerMimeType(), format);
}

QTEST_MAIN(tst_QMediaRecorder)

#include "tst_qmediarecorder_s60.moc"
