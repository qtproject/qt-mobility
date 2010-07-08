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

//TESTED_COMPONENT=src/multimedia

#include "tst_qmediarecorder_xa.h"
#include "tst_qmediarecorder_xa_macros.h"

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
