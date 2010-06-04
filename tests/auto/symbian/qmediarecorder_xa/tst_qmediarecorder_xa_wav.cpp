/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tst_qmediarecorder_xa.h"
#include "tst_qmediarecorder_xa_macros.h"

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
