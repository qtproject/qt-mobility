/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QDebug>
#include <QAudioFormat>
#include <qabstractmediaobject.h>
#include <qabstractmediacontrol.h>
#include <qabstractmediaservice.h>
#include <qmediarecordercontrol.h>
#include <qmediarecorder.h>
#include <qaudiodevicecontrol.h>
#include <qaudioencodecontrol.h>

class MockAudioEncodeProvider : public QAudioEncodeControl
{
    Q_OBJECT
public:
    MockAudioEncodeProvider(QObject *parent):
        QAudioEncodeControl(parent)
    {
        m_codecs << "audio/pcm" << "audio/mpeg";
        m_index = 0;
        m_encodeIdx = 0;
        m_encodeName.append("speex");
        m_encodeOptions.append(QStringList() << "quality" << "bitrate" << "mode" << "vbr" << "vad" << "dtx");
        m_encodeName.append("mp3");
        m_encodeOptions.append(QStringList() << "quality" << "bitrate" << "mode" << "vbr");
        m_bitrate = 128;
        m_quality = 1;
    }

    ~MockAudioEncodeProvider() {}

    QAudioFormat format() const
    {
        return m_format;
    }

    bool isFormatSupported(const QAudioFormat &format) const
    {
        if((qstrcmp(format.codec().toLocal8Bit().constData(),"audio/pcm")!=0)&&
           (qstrcmp(format.codec().toLocal8Bit().constData(),"audio/mpeg")!=0))
            return false;

        if((format.frequency() != 44100) || (format.channels() != 2) ||
           (format.sampleSize() != 16) ||
           (format.sampleType() != QAudioFormat::SignedInt) ||
           (format.byteOrder() != QAudioFormat::LittleEndian))
            return false;
        // mock only supports PCM or MP3 f=44100,stereo, S16LE
        return true;
    }

    bool setFormat(const QAudioFormat &format)
    {
        m_format = format;

		return true;
    }

    QStringList supportedAudioCodecs() const
    {
        return m_codecs;
    }

    QString audioCodec() const
    {
        return m_codecs.at(m_index);
    }

    bool setAudioCodec(const QString &codecName)
    {
        if(qstrcmp(codecName.toLocal8Bit().constData(),"audio/pcm")==0) {
            m_index = 0;
            return true;

        } else if(qstrcmp(codecName.toLocal8Bit().constData(),"audio/mpeg")==0){
            m_index = 1;
            return true;
        }
        return false;
    }

    QString codecDescription(const QString &codecName) const
    {
        if(qstrcmp(codecName.toLocal8Bit().constData(),"audio/pcm")==0)
            return QString("Pulse Code Modulation");
        if(qstrcmp(codecName.toLocal8Bit().constData(),"audio/mpeg")==0)
            return QString("MP3 audio format");

        return QString();
    }

    int bitrate() const
    {
        return m_bitrate;
    }

    void setBitrate(int bitrate)
    {
        m_bitrate = bitrate;
    }

    qreal quality() const
    {
        return m_quality;
    }

    void setQuality(qreal qual)
    {
        m_quality = qual;
    }

    QStringList supportedEncodingOptions() const
    {
        return m_encodeOptions.at(m_encodeIdx);
    }

    QVariant encodingOption(const QString &name) const
    {
        //TODO?
        return QVariant();
    }

    void setEncodingOption(const QString &name, const QVariant &value)
    {
        //TODO?
    }

private:
    QAudioFormat m_format;
    QStringList  m_codecs;
    QStringList  m_codecsDesc;
    int          m_bitrate;
    qreal        m_quality;

    QList<QString>  m_encodeName;
    QList<QStringList> m_encodeOptions;

    int          m_index;
    int          m_encodeIdx;
};

class MockAudioDeviceProvider : public QAudioDeviceControl
{
    Q_OBJECT
public:
    MockAudioDeviceProvider(QObject *parent):
        QAudioDeviceControl(parent)
    {
        m_names << "device1" << "device2" << "device3";
        m_descriptions << "dev1 comment" << "dev2 comment" << "dev3 comment";
        m_index = 0;
        emit devicesChanged();
    }
    ~MockAudioDeviceProvider() {};

    int deviceCount() const
    {
        return m_names.count();
    }

    QString name(int index) const
    {
        return m_names[index];
    }

    QString description(int index) const
    {
        return m_descriptions[index];
    }

    QIcon icon(int index) const
    {
        Q_UNUSED(index)

        return QIcon();
    }

    int defaultDevice() const
    {
        return 1;
    }

    int selectedDevice() const
    {
        return m_index;
    }

public Q_SLOTS:
    void setSelectedDevice(int index)
    {
        m_index = index;
        emit selectedDeviceChanged(m_index);
        emit selectedDeviceChanged(m_names[m_index]);
        emit devicesChanged();
    }

private:
    int m_index;
    QStringList m_names;
    QStringList m_descriptions;
};

class MockProvider : public QMediaRecorderControl
{
    Q_OBJECT

public:
    MockProvider(QObject *parent):
        QMediaRecorderControl(parent),
    m_state(QMediaRecorder::StoppedState),
    m_position(0) {}

    QUrl sink() const
    {
        return m_sink;
    }

    bool setSink(const QUrl &sink)
    {
        m_sink = sink;
        return true;
    }

    QMediaRecorder::State state() const
    {
        return m_state;
    }

    qint64 duration() const
    {
        return m_position;
    }

public slots:
    void record()
    {
        m_state = QMediaRecorder::RecordingState;
        m_position=1;
        emit stateChanged(m_state);
        emit durationChanged(m_position);
    }

    void pause()
    {
        m_state = QMediaRecorder::PausedState;
        emit stateChanged(m_state);
    }

    void stop()
    {
        m_position=0;
        m_state = QMediaRecorder::StoppedState;
        emit stateChanged(m_state);
    }

public:
    QUrl       m_sink;
    QMediaRecorder::State m_state;
    qint64     m_position;
};

class MockService : public QAbstractMediaService
{
    Q_OBJECT
public:
    MockService(QObject *parent, QAbstractMediaControl *control):
        QAbstractMediaService(parent),
        mockControl(control)
    {
        mockAudioDeviceControl = new MockAudioDeviceProvider(parent);
        mockAudioEncodeControl = new MockAudioEncodeProvider(parent);
    }

    QAbstractMediaControl* control(const char * name) const
    {
        if(qstrcmp(name,QAudioEncodeControl_iid) == 0)
            return mockAudioEncodeControl;
        if(qstrcmp(name,QAudioDeviceControl_iid) == 0)
            return mockAudioDeviceControl;
        if(qstrcmp(name,QMediaRecorderControl_iid) == 0)
            return mockControl;

        return 0;
    }

    QAbstractMediaControl   *mockControl;
    QAudioDeviceControl     *mockAudioDeviceControl;
    QAudioEncodeControl     *mockAudioEncodeControl;
};

class MockObject : public QAbstractMediaObject
{
    Q_OBJECT
public:
    MockObject(QObject *parent, QAbstractMediaControl *control):
        QAbstractMediaObject(parent),
        mockService(new MockService(this, control))
    {
    }

    bool isValid() const
    {
        return true;
    }

    QAbstractMediaService* service() const
    {
        return mockService;
    }

    QAbstractMediaService *mockService;
};

class tst_QMediaRecorder: public QObject
{
    Q_OBJECT

public slots:
    void init();
    void cleanup();

private slots:
    void testSink();
    void testRecord();
    void testAudioDeviceControl();
    void testAudioEncodeControl();

private:
    QAudioEncodeControl* encode;
    QAudioDeviceControl* audio;
    MockObject      *object;
    MockProvider    *mock;
    QMediaRecorder  *capture;
};

void tst_QMediaRecorder::init()
{
    mock = new MockProvider(this);
    object = new MockObject(this, mock);
    capture = new QMediaRecorder(object);
    QVERIFY(capture->isValid());
    audio = qobject_cast<QAudioDeviceControl*>(capture->service()->control(QAudioDeviceControl_iid));
    encode = qobject_cast<QAudioEncodeControl*>(capture->service()->control(QAudioEncodeControl_iid));
}

void tst_QMediaRecorder::cleanup()
{
    delete capture;
}

void tst_QMediaRecorder::testSink()
{
    capture->setSink(QUrl("test.tmp"));
    QUrl s = capture->sink();
    QCOMPARE(s.toString(), QString("test.tmp"));
}

void tst_QMediaRecorder::testRecord()
{
    QSignalSpy stateSignal(capture,SIGNAL(stateChanged(QMediaRecorder::State)));
    QSignalSpy progressSignal(capture, SIGNAL(durationChanged(qint64)));
    capture->record();

    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(stateSignal.count() == 1);
    QVERIFY(progressSignal.count() == 1);
    capture->pause();
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(stateSignal.count() == 2);
    capture->stop();
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(stateSignal.count() == 3);
}

void tst_QMediaRecorder::testAudioDeviceControl()
{
    QSignalSpy readSignal(audio,SIGNAL(selectedDeviceChanged(int)));
    QVERIFY(audio->deviceCount() == 3);
    QVERIFY(audio->defaultDevice() == 1);
    audio->setSelectedDevice(1);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(audio->selectedDevice() == 1);
    QVERIFY(readSignal.count() == 1);
    QVERIFY(audio->name(1) == QString("device2"));
    QVERIFY(audio->description(1) == "dev2 comment");
}

void tst_QMediaRecorder::testAudioEncodeControl()
{
    QAudioFormat fmt;
    fmt.setFrequency(44100);
    fmt.setChannels(2);
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleSize(16);
    fmt.setSampleType(QAudioFormat::SignedInt);
    fmt.setCodec("audio/pcm");
    QVERIFY(encode->isFormatSupported(fmt));
    fmt.setFrequency(8000);
    QVERIFY(!encode->isFormatSupported(fmt));
    fmt.setFrequency(44100);
    encode->setFormat(fmt);
    QStringList codecs = encode->supportedAudioCodecs();
    QVERIFY(codecs.count() == 2);
    QVERIFY(encode->setAudioCodec("audio/mpeg"));
    QVERIFY(encode->audioCodec() == QString("audio/mpeg"));
    encode->setBitrate(64);
    QVERIFY(encode->bitrate() == 64);
    encode->setQuality(1.0);
    QVERIFY(encode->quality() == 1.0);
    QStringList options = encode->supportedEncodingOptions();
    QVERIFY(options.count() == 6);
    encode->setEncodingOption("mp3",QStringList() << "bitrate" << "vbr");
}

QTEST_MAIN(tst_QMediaRecorder)

#include "tst_qmediarecorder.moc"
