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

void tst_QMediaRecorder::testAudioMpegBr192kbpsSr32kHz()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br192kSr32kHz", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(192000);
    audioSettings.setSampleRate(32000);
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

void tst_QMediaRecorder::testAudioMpegBr192kbpsSr32kHzStereo()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));
    audiocapture->setOutputLocation(nextFileName(QDir::rootPath(), "Br192kSr32kHzStereo", "mp4"));
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("aac");
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantBitRateEncoding);
    audioSettings.setBitRate(192000);
    audioSettings.setSampleRate(32000);
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
