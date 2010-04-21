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

QT_USE_NAMESPACE
class tst_QMediaRecorder: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testAudioSink();
    void testAudioRecord();
    void testAudioRecordWithAMR();
    void testAudioEndPointSelector();
    void testAudioEncoderControl();
    void testMediaFormatsControl();
    void testDefaultAudioEncodingSettings();

private:   
    QUrl recordPathAudio(QDir outputDir);

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

QUrl tst_QMediaRecorder::recordPathAudio(QDir outputDir)
{
    int lastImage = 0;  
    int fileCount = 0;
    foreach( QString fileName, outputDir.entryList(QStringList() << "testclip_*.amr") ) {
        int imgNumber = fileName.mid(5, fileName.size()-9).toInt();
        lastImage = qMax(lastImage, imgNumber);
        if (outputDir.exists(fileName))             
            fileCount+=1;        
    }    
    lastImage+=fileCount;

    QUrl location(QDir::toNativeSeparators(outputDir.canonicalPath()+QString("/testclip_%1.amr").arg(lastImage+1,4,10,QLatin1Char('0'))));
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
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));    
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);     
    QTest::qWait(500);  // wait for recorder to initialize itself 
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

void tst_QMediaRecorder::testAudioRecordWithAMR()
{
    QSignalSpy stateSignal(audiocapture,SIGNAL(stateChanged(QMediaRecorder::State)));    
    QCOMPARE(audiocapture->state(), QMediaRecorder::StoppedState);        
    audiocapture->setOutputLocation(recordPathAudio(QDir::rootPath()));
    QAudioEncoderSettings audioSettings;
    QVideoEncoderSettings videoSettings;
    audioSettings.setCodec("AMR");
    QString format = audiocapture->containerMimeType();
    format = QString("audio/amr");    
    audiocapture->setEncodingSettings(audioSettings,videoSettings,format);
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

void tst_QMediaRecorder::testAudioEndPointSelector()
{
    QSignalSpy audioSignal(audioEndpoint,SIGNAL(activeEndpointChanged(QString)));
    QVERIFY(audioEndpoint->availableEndpoints().count() == 1);
    QVERIFY(audioEndpoint->defaultEndpoint().compare("") == 0);    
    audioEndpoint->setActiveEndpoint("device2");
    QVERIFY(audioEndpoint->activeEndpoint().compare("device2") == 0);
    QVERIFY(audioSignal.count() == 1);
    QVERIFY(audioEndpoint->endpointDescription("device2").compare("") == 0);
}

void tst_QMediaRecorder::testAudioEncoderControl()
{
    QStringList codecs = audiocapture->supportedAudioCodecs();    
    QVERIFY(codecs.count() == 2);    
    QVERIFY(audiocapture->audioCodecDescription("PCM") == "Pulse code modulation");
    QStringList options = audioEncoder->supportedEncodingOptions("PCM");
    QCOMPARE(options.count(), 3);
    QCOMPARE(audiocapture->supportedAudioSampleRates().count(), 12);
    audioEncoder->setEncodingOption("PCM", "channels", QVariant(2));
    QCOMPARE(audioEncoder->encodingOption("PCM","channels").toInt(), 2);
    audioEncoder->setEncodingOption("PCM", "quality", QVariant(int(QtMediaServices::NormalQuality)));
    QCOMPARE(audioEncoder->encodingOption("PCM","quality").toInt(), int(QtMediaServices::NormalQuality));    
    audioEncoder->setEncodingOption("PCM", "samplerate", QVariant(44100));
    QCOMPARE(audioEncoder->encodingOption("PCM","samplerate").toInt(), 44100);    
}

void tst_QMediaRecorder::testMediaFormatsControl()
{
    //audioocontainer types
    QCOMPARE(audiocapture->supportedContainers(), QStringList() << "audio/wav" << "audio/amr");   
    QCOMPARE(audiocapture->containerDescription("audio/wav"), QString("WAV Write Format"));    
    QCOMPARE(audiocapture->containerDescription("audio/amr"), QString("AMR Write Format"));
}

void tst_QMediaRecorder::testDefaultAudioEncodingSettings()
{
    QAudioEncoderSettings audioSettings = audiocapture->audioSettings();
    QCOMPARE(audioSettings.codec(), QString("AMR"));
    QString format = audiocapture->containerMimeType();
    QCOMPARE(format, QString("audio/amr"));
}

QTEST_MAIN(tst_QMediaRecorder)

#include "tst_qmediarecorder_s60.moc"
