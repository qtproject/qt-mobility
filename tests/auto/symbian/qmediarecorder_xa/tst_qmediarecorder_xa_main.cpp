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

void tst_QMediaRecorder::initTestCase()
{
    qRegisterMetaType<QMediaRecorder::State>("QMediaRecorder::State");
    qRegisterMetaType<QMediaRecorder::Error>("QMediaRecorder::Error");

    captureSource = new QAudioCaptureSource;
    audiocapture = new QMediaRecorder(captureSource);

    //FIXME audioEndpoint = qobject_cast<QAudioEndpointSelector*>(audiocapture->service()->requestControl(QAudioEndpointSelector_iid));
    //FIXME audioEncoder = qobject_cast<QAudioEncoderControl*>(audiocapture->service()->requestControl(QAudioEncoderControl_iid));
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
    QCOMPARE(audioSettings.encodingMode(), QtMultimedia::ConstantQualityEncoding);
    QCOMPARE(audioSettings.quality(), QtMultimedia::VeryLowQuality);
    QCOMPARE(audioSettings.sampleRate(), 8000);
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

QTEST_MAIN_S60(tst_QMediaRecorder)

#include "moc_tst_qmediarecorder_xa.cpp"
