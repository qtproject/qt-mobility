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
#include <qaudioencodercontrol.h>
#include <qmediacontainercontrol.h>
#include <QtMultimediaKit/qaudioformat.h>
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

QT_USE_NAMESPACE
class tst_QMediaRecorder: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:

    // Positive test cases
    void testMediaRecorderObject();
    void testAudioSink();
    void testAudioEndPointSelector();
    void testDefaultAudioEncodingSettings();
    void testAudioWav();
    void testAudioWavSr8kHz();
    void testAudioWavSr16kHz();
    void testAudioWavSr32kHz();
    void testAudioWavSr96kHz();
    void testAudioWavSr8kHzStereo();
    void testAudioWavSr16kHzStereo();
    void testAudioWavSr32kHzStereo();
    void testAudioWavSr96kHzStereo();
    void testAudioWavQualityVeryLow();
    void testAudioWavQualityLow();
    void testAudioWavQualityNormal();
    void testAudioWavQualityHigh();
    void testAudioWavQualityVeryHigh();
    // Negetive test cases
    void testAudioWavSr4kHz();
    void testAudioWavSr8kHz5Channel();

    // Positive test cases
    void testAudioAmr();
    void testAudioAmrBr4750bps();
    void testAudioAmrBr5150bps();
    void testAudioAmrBr5900bps();
    void testAudioAmrBr6700bps();
    void testAudioAmrBr7400bps();
    void testAudioAmrBr7950bps();
    void testAudioAmrBr10200bps();
    void testAudioAmrBr12200bps();
    void testAudioAmrQualityVeryLow();
    void testAudioAmrQualityLow();
    void testAudioAmrQualityNormal();
    void testAudioAmrQualityHigh();
    void testAudioAmrQualityVeryHigh();
    // Negetive test cases
    void testAudioAmrSr8kHzMono();
    void testAudioAmrStereo();
    void testAudioAmrSr16kHz();

    // Positive test cases
    void testAudioMpeg();
    void testAudioMpegBr32kSr8kHz();
    void testAudioMpegBr32kbpsSr8kHzStereo();
    void testAudioMpegBr128kbpsSr16kHz();
    void testAudioMpegBr128kbpsSr16kHzStereo();
    void testAudioMpegBr256kbpsSr48kHz();
    void testAudioMpegBr256kbpsSr48kHzStereo();
    void testAudioMpegQualityVeryLow();
    void testAudioMpegQualityLow();
    void testAudioMpegQualityNormal();
    void testAudioMpegQualityHigh();
    void testAudioMpegQualityVeryHigh();
    // Negetive test cases
    void testAudioMpegBr16kSr8kHz();


private:

    QUrl nextFileName(QDir outputDir, QString appendName, QString ext);

    QAudioEncoderControl *audioEncoder;
    QAudioEndpointSelector *audioEndpoint;
    QMediaRecorder  *audiocapture;
    QAudioCaptureSource *captureSource;
};

void tst_QMediaRecorder::initTestCase()
{
    qRegisterMetaType<QMediaRecorder::State>("QMediaRecorder::State");
    qRegisterMetaType<QMediaRecorder::Error>("QMediaRecorder::Error");

    captureSource = new QAudioCaptureSource;
    audiocapture = new QMediaRecorder(captureSource);

    audioEndpoint = qobject_cast<QAudioEndpointSelector*>(audiocapture->service()->control(QAudioEndpointSelector_iid));
    audioEncoder = qobject_cast<QAudioEncoderControl*>(audiocapture->service()->control(QAudioEncoderControl_iid));
}

void tst_QMediaRecorder::cleanupTestCase()
{
    delete audiocapture;
    delete captureSource;
}

void tst_QMediaRecorder::testMediaRecorderObject()
{
    //audioocontainer types
    QCOMPARE(audiocapture->audioCodecDescription("pcm"), QString("pcm"));
    QCOMPARE(audiocapture->audioCodecDescription("amr"), QString("amr"));
    QCOMPARE(audiocapture->audioCodecDescription("aac"), QString("aac"));
    QCOMPARE(audiocapture->containerDescription("audio/wav"), QString("wav container"));
    QCOMPARE(audiocapture->containerDescription("audio/amr"), QString("amr File format"));
    QCOMPARE(audiocapture->containerDescription("audio/mpeg"), QString("mpeg container"));
    QCOMPARE(audiocapture->containerMimeType(), QString("audio/wav"));
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(audiocapture->outputLocation().toLocalFile(), QString());
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(audiocapture->supportedAudioCodecs().count(), 3); // "pcm", "amr", "aac");
    QAudioEncoderSettings settings;
    settings.setCodec("pcm");
    QCOMPARE(audiocapture->supportedAudioSampleRates(settings).count(), 12);
    bool isContinuous;
    audiocapture->supportedAudioSampleRates(settings, &isContinuous);
    QCOMPARE(isContinuous, false);
    QCOMPARE(audiocapture->supportedContainers().count(), 3); // "audio/wav", "audio/amr", "audio/mpeg");
}

void tst_QMediaRecorder::testAudioSink()
{
    audiocapture->setOutputLocation(QUrl("test.wav"));
    QUrl s = audiocapture->outputLocation();
    QCOMPARE(s.toString(), QString("test.wav"));
}

void tst_QMediaRecorder::testAudioEndPointSelector()
{
    QSignalSpy audioSignal(audioEndpoint,SIGNAL(activeEndpointChanged(QString)));
    QVERIFY(audioEndpoint->availableEndpoints().count() == 1);
    QVERIFY(audioEndpoint->defaultEndpoint().compare("Default Mic") == 0);
    audioEndpoint->setActiveEndpoint("Default Mic");
    QVERIFY(audioEndpoint->activeEndpoint().compare("Default Mic") == 0);
    QVERIFY(audioSignal.count() == 1);
    QVERIFY(audioEndpoint->endpointDescription("Default Mic").compare("Default Mic") == 0);
}

void tst_QMediaRecorder::testDefaultAudioEncodingSettings()
{
    QAudioEncoderSettings audioSettings = audiocapture->audioSettings();
    QCOMPARE(audioSettings.codec(), QString("pcm"));
    QCOMPARE(audiocapture->containerMimeType(), QString("audio/wav"));
    QCOMPARE(audioSettings.bitRate(), 0);
    QCOMPARE(audioSettings.channelCount(), 1);
    QCOMPARE(audioSettings.encodingMode(), QtMultimediaKit::ConstantQualityEncoding);
    QCOMPARE(audioSettings.quality(), QtMultimediaKit::VeryLowQuality);
    QCOMPARE(audioSettings.sampleRate(), 8000);
}

void tst_QMediaRecorder::testAudioWav()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "", "wav"));
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavSr8kHz()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Sr8kHz", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setSampleRate(8000);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavSr16kHz()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Sr16kHz", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setSampleRate(16000);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavSr32kHz()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Sr32kHz", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setSampleRate(32000);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavSr96kHz()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Sr32kHz", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setSampleRate(96000);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavSr8kHzStereo()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Sr8kHzStereo", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setSampleRate(8000);
    audioSettings.setChannelCount(2);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavSr16kHzStereo()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Sr16kHzStereo", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setSampleRate(16000);
    audioSettings.setChannelCount(2);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavSr32kHzStereo()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Sr32kHzStereo", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setSampleRate(32000);
    audioSettings.setChannelCount(2);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavSr96kHzStereo()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Sr32kHzStereo", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setSampleRate(96000);
    audioSettings.setChannelCount(2);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavQualityVeryLow()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "VeryLowQuality", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::VeryLowQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavQualityLow()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "LowQuality", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::LowQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavQualityNormal()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "NormalQuality", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::NormalQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavQualityHigh()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "HighQuality", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::HighQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavQualityVeryHigh()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "VeryHighQuality", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::VeryHighQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioWavSr4kHz()
{
    QSignalSpy errorSignal(audiocapture,SIGNAL(error(QMediaRecorder::Error)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Sr441kHz", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setSampleRate(4000);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(errorSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->error(), QMediaRecorder::ResourceError);
    QCOMPARE(audiocapture->errorString(), QString("Generic error"));
}

void tst_QMediaRecorder::testAudioWavSr8kHz5Channel()
{
    QSignalSpy errorSignal(audiocapture,SIGNAL(error(QMediaRecorder::Error)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Sr441kHz", "wav"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("pcm");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setSampleRate(8000);
    audioSettings.setChannelCount(6);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/wav"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(errorSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->error(), QMediaRecorder::ResourceError);
    QCOMPARE(audiocapture->errorString(), QString("Generic error"));
}

void tst_QMediaRecorder::testAudioAmr()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setSampleRate(-1);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrBr4750bps()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br4750bps", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(4750);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrBr5150bps()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br4750bps", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(5150);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrBr5900bps()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br4750bps", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(5900);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrBr6700bps()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br4750bps", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(6700);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrBr7400bps()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br4750bps", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(7400);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrBr7950bps()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br4750bps", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(7950);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrBr10200bps()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br4750bps", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(10200);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrBr12200bps()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br4750bps", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(12200);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrQualityVeryLow()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "VeryLowQuality", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::VeryLowQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrQualityLow()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "LowQuality", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::LowQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrQualityNormal()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "NormalQuality", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::NormalQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrQualityHigh()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "HighQuality", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::HighQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrQualityVeryHigh()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "LowQuality", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::VeryHighQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrSr8kHzMono()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Sr8kHzMono", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setSampleRate(8000);
    audioSettings.setChannelCount(1);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->pause();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::PausedState);
    QCOMPARE(stateSignal.count(), 2);
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 3); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 3);
}

void tst_QMediaRecorder::testAudioAmrStereo()
{
    QSignalSpy errorSignal(audiocapture,SIGNAL(error(QMediaRecorder::Error)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Stereo", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setSampleRate(-1);
    audioSettings.setChannelCount(2);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(errorSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->error(), QMediaRecorder::ResourceError);
    QCOMPARE(audiocapture->errorString(), QString("Generic error"));
}

void tst_QMediaRecorder::testAudioAmrSr16kHz()
{
    QSignalSpy errorSignal(audiocapture,SIGNAL(error(QMediaRecorder::Error)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Sr16kHz", "amr"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("amr");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setSampleRate(16000);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/amr"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(errorSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->error(), QMediaRecorder::ResourceError);
    QCOMPARE(audiocapture->errorString(), QString("Generic error"));
}

void tst_QMediaRecorder::testAudioMpeg()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setSampleRate(-1);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 2);
}

void tst_QMediaRecorder::testAudioMpegBr32kSr8kHz()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br32kSr8kHz", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(32000);
    audioSettings.setSampleRate(8000);
    audioSettings.setChannelCount(1);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 2);
}

void tst_QMediaRecorder::testAudioMpegBr32kbpsSr8kHzStereo()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br32kSr8kHzStereo", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(32000);
    audioSettings.setSampleRate(8000);
    audioSettings.setChannelCount(2);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 2);
}

void tst_QMediaRecorder::testAudioMpegBr128kbpsSr16kHz()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br128kSr16kHz", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(128000);
    audioSettings.setSampleRate(16000);
    audioSettings.setChannelCount(1);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 2);
}

void tst_QMediaRecorder::testAudioMpegBr128kbpsSr16kHzStereo()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br128kSr16kHzStereo", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(128000);
    audioSettings.setSampleRate(16000);
    audioSettings.setChannelCount(2);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 2);
}

void tst_QMediaRecorder::testAudioMpegBr256kbpsSr48kHz()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br256kSr48kHz", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(256000);
    audioSettings.setSampleRate(48000);
    audioSettings.setChannelCount(1);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 2);
}

void tst_QMediaRecorder::testAudioMpegBr256kbpsSr48kHzStereo()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br256kSr48kHzStereo", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(256000);
    audioSettings.setSampleRate(48000);
    audioSettings.setChannelCount(2);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 2);
}

void tst_QMediaRecorder::testAudioMpegQualityVeryLow()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "VeryLowQuality", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::VeryLowQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 2);
}

void tst_QMediaRecorder::testAudioMpegQualityLow()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "LowQuality", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::LowQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 2);
}

void tst_QMediaRecorder::testAudioMpegQualityNormal()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "NormalQuality", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::NormalQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 2);
}

void tst_QMediaRecorder::testAudioMpegQualityHigh()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "HighQuality", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::HighQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 2);
}

void tst_QMediaRecorder::testAudioMpegQualityVeryHigh()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "VeryHighQuality", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    audioSettings.setQuality(QtMultimediaKit::VeryHighQuality);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(stateSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(audiocapture->error(), QMediaRecorder::NoError);
    QCOMPARE(audiocapture->errorString(), QString());
    QCOMPARE(stateSignal.count(), 1);
    QTest::qWait(5000);  // wait for 5 seconds
    audiocapture->stop();
    QTRY_COMPARE(stateSignal.count(), 2); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QCOMPARE(stateSignal.count(), 2);
}

void tst_QMediaRecorder::testAudioMpegBr16kSr8kHz()
{
    QSignalSpy errorSignal(audiocapture,SIGNAL(error(QMediaRecorder::Error)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br16kHzSr8kHz", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(16000);
    audioSettings.setSampleRate(8000);
    QVideoEncoderSettings videoSettings;
    audiocapture->setEncodingSettings(audioSettings, videoSettings, QString("audio/mpeg"));
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);
    QTest::qWait(500);  // wait for recorder to initialize itself
    audiocapture->record();
    QTRY_COMPARE(errorSignal.count(), 1); // wait for callbacks to complete in symbian API
    QCOMPARE(audiocapture->error(), QMediaRecorder::ResourceError);
    QCOMPARE(audiocapture->errorString(), QString("Generic error"));
}


QUrl tst_QMediaRecorder::nextFileName(QDir outputDir, QString appendName, QString ext)
{
    int lastImage = 0;
    int fileCount = 0;
    foreach( QString fileName, outputDir.entryList(QStringList() << "testclip_*." + ext) ) {
        int imgNumber = fileName.mid(5, fileName.size()-9).toInt();
        lastImage = qMax(lastImage, imgNumber);
        if (outputDir.exists(fileName))
            fileCount+=1;
    }
    lastImage+=fileCount;

    QUrl location(QDir::toNativeSeparators(outputDir.canonicalPath() + QString("/testclip_%1").arg(lastImage+1 , 4, 10, QLatin1Char('0')) + appendName + "." + ext));
    return location;
}

QTEST_MAIN(tst_QMediaRecorder)

#include "tst_qmediarecorder_xa.moc"
