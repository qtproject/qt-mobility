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
#include <experimental/qcamera.h>
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
    void testVideoSink();
    void testAudioSink();
    void testVideoRecord();
    void testAudioRecord();
    void testVideoDeviceControl();
    void testAudioEndPointSelector();
    void testAudioEncoderControl();
    void testMediaFormatsControl();
    void testDefaultVideoEncodingSettings();
    void testDefaultAudioEncodingSettings();
    void testVideoEncoderControl();
    void testEncodingSettings();   

private:
    QUrl recordPathVideo(QDir outputDir);
    QUrl recordPathAudio(QDir outputDir);

    QAudioEncoderControl *audioEncoder;
    QAudioEncoderControl *videoAudioEncoder;
    QAudioEndpointSelector *audioEndpoint;
    QVideoDeviceControl *videoDevice;
    QCamera *camera;
    QMediaRecorder  *capture;
    QMediaRecorder  *audiocapture;
    QVideoEncoderControl *videoEncoder;
    QAudioCaptureSource *captureSource;
};

void tst_QMediaRecorder::initTestCase()
{
    qRegisterMetaType<QtMobility::QMediaRecorder::State>("QMediaRecorder::State");
    qRegisterMetaType<QtMobility::QMediaRecorder::Error>("QMediaRecorder::Error");
    qRegisterMetaType<QCamera::State>("QCamera::State");

    camera = new QCamera;
    capture = new QMediaRecorder(camera);
    captureSource = new QAudioCaptureSource;
    audiocapture = new QMediaRecorder(captureSource);          
    
    videoAudioEncoder = qobject_cast<QAudioEncoderControl*>(capture->service()->control(QAudioEncoderControl_iid));
    videoDevice = qobject_cast<QVideoDeviceControl*>(capture->service()->control(QVideoDeviceControl_iid));
    videoEncoder = qobject_cast<QVideoEncoderControl*>(capture->service()->control(QVideoEncoderControl_iid));
    audioEndpoint = qobject_cast<QAudioEndpointSelector*>(audiocapture->service()->control(QAudioEndpointSelector_iid));    
    audioEncoder = qobject_cast<QAudioEncoderControl*>(audiocapture->service()->control(QAudioEncoderControl_iid));
}

void tst_QMediaRecorder::cleanupTestCase()
{    
    delete audiocapture;
    delete captureSource;
    delete capture;
    delete camera;
}

QUrl tst_QMediaRecorder::recordPathVideo(QDir outputDir)
{
    int lastImage = 0;    
    foreach( QString fileName, outputDir.entryList(QStringList() << "testclip_*.mpg") ) {
        int imgNumber = fileName.mid(5, fileName.size()-9).toInt();
        lastImage = qMax(lastImage, imgNumber);
    }

    QUrl location(QDir::toNativeSeparators(outputDir.canonicalPath()+QString("/testclip_%1.mpg").arg(lastImage+1,4,10,QLatin1Char('0'))));
    return location;
}

QUrl tst_QMediaRecorder::recordPathAudio(QDir outputDir)
{
    int lastImage = 0;    
    foreach( QString fileName, outputDir.entryList(QStringList() << "testclip_*.amr") ) {
        int imgNumber = fileName.mid(5, fileName.size()-9).toInt();
        lastImage = qMax(lastImage, imgNumber);
    }

    QUrl location(QDir::toNativeSeparators(outputDir.canonicalPath()+QString("/testclip_%1.amr").arg(lastImage+1,4,10,QLatin1Char('0'))));
    return location;
}


void tst_QMediaRecorder::testVideoSink()
{   
    capture->setOutputLocation(QUrl("test.tmp"));
    QUrl s = capture->outputLocation();
    QCOMPARE(s.toString(), QString("test.tmp"));    
}

void tst_QMediaRecorder::testAudioSink()
{   
    audiocapture->setOutputLocation(QUrl("test.tmp"));
    QUrl s = audiocapture->outputLocation();
    QCOMPARE(s.toString(), QString("test.tmp"));    
}

void tst_QMediaRecorder::testVideoRecord()
{
    QSignalSpy stateSignal(capture,SIGNAL(stateChanged(QMediaRecorder::State)));
    QSignalSpy cameraState(camera, SIGNAL(stateChanged(QCamera::State)));

    QCOMPARE(capture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(camera->state(), QCamera::StoppedState);
    camera->start();
    QTRY_COMPARE(cameraState.count(), 1); // wait for camera to initialize itself
    QCOMPARE(camera->state(), QCamera::ActiveState);
    QTest::qWait(500);  // wait for recorder to initialize itself 
    capture->setOutputLocation(recordPathVideo(QDir::rootPath()));
    capture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(capture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(capture->error(), QMediaRecorder::NoError);
    QCOMPARE(capture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    capture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(capture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    capture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(capture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
    camera->stop();
    QTRY_COMPARE(cameraState.count(), 2); // wait for camera to uninitialize itself
    QCOMPARE(camera->state(), QCamera::StoppedState);
}

void tst_QMediaRecorder::testAudioRecord()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));    
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);        
    audiocapture->setOutputLocation(recordPathAudio(QDir::rootPath()));        
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);    
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API    
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);        
}

void tst_QMediaRecorder::testVideoDeviceControl()
{
    QSignalSpy readSignal(videoDevice,SIGNAL(selectedDeviceChanged(QString)));
    QVERIFY(videoDevice->deviceCount() == 2);
    QVERIFY(videoDevice->defaultDevice() == 0);
    videoDevice->setSelectedDevice(1);    
    QVERIFY(videoDevice->selectedDevice() == 1);
    QVERIFY(readSignal.count() == 1);
    QVERIFY(videoDevice->deviceDescription(0).compare("Back camera") == 0);
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
    QVERIFY(codecs.count() == 3);    
    QVERIFY(audiocapture->audioCodecDescription("PCM") == "Pulse code modulation");
    QStringList options = audioEncoder->supportedEncodingOptions("PCM");
    QCOMPARE(options.count(), 0);    
    QVERIFY(audioEncoder->encodingOption("PCM","bitrate").toInt() == -1);
    audioEncoder->setEncodingOption("PCM", "bitrate", QString("vbr"));
    QCOMPARE(audioEncoder->encodingOption("PCM","bitrate").toInt(), -1);
    QCOMPARE(audiocapture->supportedAudioSampleRates().count(), 12);
}

void tst_QMediaRecorder::testMediaFormatsControl()
{
    //videocontainer types
    QCOMPARE(capture->supportedContainers(), QStringList() << "video/3gpp" << "video/mp4" << "video/3gpp2");
    QCOMPARE(capture->containerDescription("video/3gpp"), QString("3GPP File Format"));
    QCOMPARE(capture->containerDescription("video/mp4"), QString("MPEG-4 File Format"));
    QCOMPARE(capture->containerDescription("video/3gpp2"), QString("3GPP2 File Format"));
    //audioocontainer types
    QCOMPARE(audiocapture->supportedContainers(), QStringList() << "audio/wav" << "audio/basic" << "audio/amr");   
    QCOMPARE(audiocapture->containerDescription("audio/wav"), QString("WAV Write Format"));    
    QCOMPARE(audiocapture->containerDescription("audio/amr"), QString("AMR Write Format"));
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
  
void tst_QMediaRecorder::testDefaultAudioEncodingSettings()
{
    QAudioEncoderSettings audioSettings = audiocapture->audioSettings();
    QCOMPARE(audioSettings.codec(), QString("AMR"));
    QString format = audiocapture->containerMimeType();
    QCOMPARE(format, QString("audio/amr"));
/*    QCOMPARE(audioSettings.bitRate(), 128*1024);
    QCOMPARE(audioSettings.sampleRate(), -1);
    QCOMPARE(audioSettings.quality(), QtMedia::NormalQuality);
    QCOMPARE(audioSettings.channelCount(), -1);

    QCOMPARE(audioSettings.encodingMode(), QtMedia::ConstantQualityEncoding);*/
   
}

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
    QVERIFY(vCodecs.count() == 30);    

    QStringList options = videoEncoder->supportedEncodingOptions("video/mp4");
    QCOMPARE(options.count(), 4);
    
    QVERIFY(videoEncoder->encodingOption("video/mp4","bitrate") == QVariant(-1));
    videoEncoder->setEncodingOption("video/mp4", "bitrate", QVariant(128000));
    QCOMPARE(videoEncoder->encodingOption("video/mp4","bitrate"), QVariant(128000));
    videoEncoder->setEncodingOption("video/mp4", "bitrate", QVariant(-1));
    
    QStringList aCodecs = capture->supportedAudioCodecs();    
    QVERIFY(aCodecs.count() == 2);    
    QVERIFY(capture->audioCodecDescription("audio/aac") == "Advanced Audio Coding");
    QVERIFY(capture->audioCodecDescription("audio/amr") == "Adaptive Multi-Rate audio codec");
}

void tst_QMediaRecorder::testEncodingSettings()
{
    QAudioEncoderSettings audioSettings;
    QVideoEncoderSettings videoSettings;
    QString format;

    audioSettings.setCodec("audio/aac");
/*    audioSettings.setSampleRate(44100);
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
