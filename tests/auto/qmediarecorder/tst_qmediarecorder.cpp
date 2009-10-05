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
#include <multimedia/qmediaobject.h>
#include <multimedia/qmediacontrol.h>
#include <multimedia/qmediaservice.h>
#include <multimedia/qmediarecordercontrol.h>
#include <multimedia/qmediarecorder.h>
#include <multimedia/qaudiodevicecontrol.h>
#include <multimedia/qaudioencodercontrol.h>
#include <multimedia/qmediaformatcontrol.h>
#include <multimedia/qvideoencodercontrol.h>

#ifndef QT_NO_MULTIMEDIA
#include <QtMultimedia/qaudioformat.h>
#else
#include <multimedia/qaudioformat.h>
#endif

class MockMediaFormatControl : public QMediaFormatControl
{
    Q_OBJECT
public:
    MockMediaFormatControl(QObject *parent):
        QMediaFormatControl(parent)
    {
        m_supportedFormats.append("wav");
        m_supportedFormats.append("mp3");

        m_descriptions.insert("wav", "WAV format");
        m_descriptions.insert("mp3", "MP3 format");
    }

    virtual ~MockMediaFormatControl() {};

    QStringList supportedFormats() const
    {
        return m_supportedFormats;
    }

    QString format() const
    {
        return m_format;
    }

    void setFormat(const QString &formatMimeType)
    {
        if (m_supportedFormats.contains(formatMimeType))
            m_format = formatMimeType;
    }

    QString formatDescription(const QString &formatMimeType) const
    {
        return m_descriptions.value(formatMimeType);
    }

private:
    QStringList m_supportedFormats;
    QMap<QString, QString> m_descriptions;
    QString m_format;
};

class MockVideoEncodeProvider : public QVideoEncoderControl
{
    Q_OBJECT
public:
    MockVideoEncodeProvider(QObject *parent):
        QVideoEncoderControl(parent)
    {
        m_bitrate = 0;
        m_quality = 0;
        m_options << "option1" << "option2";
        m_videoCodecs << "video/3gpp" << "video/H264";
        m_sizes << QSize(320,240) << QSize(640,480);
        m_framerates << qMakePair<int,int>(30,1) << qMakePair<int,int>(15,1) << qMakePair<int,int>(1,1);
    }
    ~MockVideoEncodeProvider() {}

    QSize resolution() const { return m_size; }
    QSize minimumResolution() const { return QSize(320,240); }
    QSize maximumResolution() const { return QSize(640,480); }
    QList<QSize> supportedResolutions() const { return m_sizes; }
    void setResolution(const QSize &sz) { m_size = sz; }
    QMediaRecorder::FrameRate frameRate() const { return m_framerate; }
    QMediaRecorder::FrameRate minimumFrameRate() const { return qMakePair<int,int>(1,1); }
    QMediaRecorder::FrameRate maximumFrameRate() const { return qMakePair<int,int>(30,1); }
    QList<QMediaRecorder::FrameRate> supportedFrameRates() const { return m_framerates; }
    void setFrameRate(const QMediaRecorder::FrameRate &rate) { m_framerate = rate; }
    QStringList supportedVideoCodecs() const { return m_videoCodecs; }
    QString videoCodec() const { return m_vCodec; }
    bool setVideoCodec(const QString &codecName) { m_vCodec = codecName; return true; }
    QString videoCodecDescription(const QString &codecName) const { return QString("no comment"); }
    int bitrate() const { return m_bitrate; }
    void setBitrate(int bitrate) { m_bitrate = bitrate; }
    int quality() const { return m_quality; }
    void setQuality(int quality) { m_quality = quality; }
    QStringList supportedEncodingOptions() const { return m_options; }
    QVariant encodingOption(const QString &name) const { return m_option; }
    void setEncodingOption(const QString &name, const QVariant &value) { m_option = value; }

private:
    int m_bitrate;
    int m_quality;
    QStringList m_options;
    QVariant m_option;
    QStringList m_videoCodecs;
    QString m_vCodec;
    QSize m_size;
    QList<QSize> m_sizes;
    QMediaRecorder::FrameRate m_framerate;
    QList<QMediaRecorder::FrameRate> m_framerates;
};

class MockAudioEncodeProvider : public QAudioEncoderControl
{
    Q_OBJECT
public:
    MockAudioEncodeProvider(QObject *parent):
        QAudioEncoderControl(parent)
    {
        m_codecs << "audio/pcm" << "audio/mpeg";
        m_index = 0;
        m_encodeIdx = 0;
        m_encodeName.append("speex");
        m_encodeOptions.append(QStringList() << "quality" << "bitrate" << "mode" << "vbr" << "vad" << "dtx");
        m_encodeName.append("mp3");
        m_encodeOptions.append(QStringList() << "quality" << "bitrate" << "mode" << "vbr");
        m_bitrate = 128;
        m_quality = 50;
        m_frequency = -1;
        m_sampleSize = -1;
        m_channels = -1;
    }

    ~MockAudioEncodeProvider() {}

    int frequency() const { return m_frequency; }
    void setFrequency(int frequency) { m_frequency = frequency; }
    QList<int> supportedFrequencies() const { return QList<int>() << 44100; }
    QPair<int,int> supportedFrequencyRange() const { return qMakePair<int,int>(44100,44100); }

    int channels() const { return m_channels; }
    void setChannels(int channels) { m_channels = channels; }
    QList<int> supportedChannelCounts() const { return QList<int>() << 2; }

    int sampleSize() const { return m_sampleSize; }
    void setSampleSize(int sampleSize) { m_sampleSize = sampleSize; }
    QList<int> supportedSampleSizes() const { return QList<int>() << 16; }

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

    int quality() const
    {
        return m_quality;
    }

    void setQuality(int qual)
    {
        m_quality = qual;
    }

    QStringList supportedEncodingOptions() const
    {
        return m_encodeOptions.at(m_encodeIdx);
    }

    QVariant encodingOption(const QString &name) const
    {
        Q_UNUSED(name);
        //TODO?
        return QVariant();
    }

    void setEncodingOption(const QString &name, const QVariant &value)
    {
        //TODO?
        Q_UNUSED(name);
        Q_UNUSED(value);
    }

private:
    int m_frequency;
    int m_channels;
    int m_sampleSize;

    QStringList  m_codecs;
    QStringList  m_codecsDesc;
    int          m_bitrate;
    int        m_quality;

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

class MockService : public QMediaService
{
    Q_OBJECT
public:
    MockService(QObject *parent, QMediaControl *control):
        QMediaService(parent),
        mockControl(control)
    {
        mockAudioDeviceControl = new MockAudioDeviceProvider(parent);
        mockAudioEncodeControl = new MockAudioEncodeProvider(parent);
        mockFormatControl = new MockMediaFormatControl(parent);
        mockVideoEncodeControl = new MockVideoEncodeProvider(parent);
    }

	QMediaControl* control(const char *name) const
    {
        if(qstrcmp(name,QAudioEncoderControl_iid) == 0)
            return mockAudioEncodeControl;
        if(qstrcmp(name,QAudioDeviceControl_iid) == 0)
            return mockAudioDeviceControl;
        if(qstrcmp(name,QMediaRecorderControl_iid) == 0)
            return mockControl;
        if(qstrcmp(name,QMediaFormatControl_iid) == 0)
            return mockFormatControl;
        if(qstrcmp(name,QVideoEncoderControl_iid) == 0)
            return mockVideoEncodeControl;

        return 0;
    }

    QMediaControl   *mockControl;
    QAudioDeviceControl     *mockAudioDeviceControl;
    QAudioEncoderControl    *mockAudioEncodeControl;
    QMediaFormatControl     *mockFormatControl;
    QVideoEncoderControl    *mockVideoEncodeControl;
};

class MockObject : public QMediaObject
{
    Q_OBJECT
public:
    MockObject(QObject *parent, MockService *service):
        QMediaObject(parent, service)
    {
    }
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
    void testMediaFormatsControl();
    void testVideoEncodeControl();

private:
    QAudioEncoderControl* encode;
    QAudioDeviceControl* audio;
    MockObject      *object;
	MockService		*service;
    MockProvider    *mock;
    QMediaRecorder  *capture;
    QVideoEncoderControl* videoEncode;
};

void tst_QMediaRecorder::init()
{
    qRegisterMetaType<QMediaRecorder::State>("QMediaRecorder::State");

    mock = new MockProvider(this);
	service = new MockService(this, mock);
    object = new MockObject(this, service);
    capture = new QMediaRecorder(object);

    audio = qobject_cast<QAudioDeviceControl*>(capture->service()->control(QAudioDeviceControl_iid));
    encode = qobject_cast<QAudioEncoderControl*>(capture->service()->control(QAudioEncoderControl_iid));
    videoEncode = qobject_cast<QVideoEncoderControl*>(capture->service()->control(QVideoEncoderControl_iid));
}

void tst_QMediaRecorder::cleanup()
{
    delete mock;
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
    QVERIFY(capture->state() == QMediaRecorder::RecordingState);
    QVERIFY(capture->error() == QMediaRecorder::NoError);
    QVERIFY(capture->errorString() == QString());
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(stateSignal.count() == 1);
    QVERIFY(progressSignal.count() == 1);
    capture->pause();
    QVERIFY(capture->state() == QMediaRecorder::PausedState);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(stateSignal.count() == 2);
    capture->stop();
    QVERIFY(capture->state() == QMediaRecorder::StoppedState);
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
    QStringList codecs = capture->supportedAudioCodecs();
    QVERIFY(codecs.count() == 2);
    QVERIFY(capture->setAudioCodec("audio/mpeg"));
    QVERIFY(capture->audioCodec() == QString("audio/mpeg"));
    QVERIFY(capture->audioCodecDescription("audio/pcm") == "Pulse Code Modulation");
    QCOMPARE(capture->audioQuality(), 50);
    capture->setAudioQuality(1);
    QCOMPARE(capture->audioQuality(), 1);
    capture->setAudioBitrate(64);
    QVERIFY(capture->audioBitrate() == 64);
    QStringList options = encode->supportedEncodingOptions();
    QCOMPARE(options.count(), 6);
    encode->setEncodingOption("mp3",QStringList() << "bitrate" << "vbr");
}

void tst_QMediaRecorder::testMediaFormatsControl()
{
    QCOMPARE(capture->supportedFormats(), QStringList() << "wav" << "mp3");
    capture->setFormat("wav");
    QCOMPARE(capture->format(), QString("wav"));
    capture->setFormat("mp3");
    QCOMPARE(capture->format(), QString("mp3"));
    capture->setFormat("ogg");
    QCOMPARE(capture->format(), QString("mp3"));

    QCOMPARE(capture->formatDescription("wav"), QString("WAV format"));
    QCOMPARE(capture->formatDescription("mp3"), QString("MP3 format"));
    QCOMPARE(capture->formatDescription("ogg"), QString());
}

void tst_QMediaRecorder::testVideoEncodeControl()
{
    QList<QSize> sizes = capture->supportedResolutions();
    QVERIFY(sizes.count() == 2);
    capture->setResolution(sizes.first());
    QVERIFY(capture->resolution() == QSize(320,240));
    QVERIFY(capture->minimumResolution() == QSize(320,240));
    QVERIFY(capture->maximumResolution() == QSize(640,480));

    QList<QMediaRecorder::FrameRate> rates = capture->supportedFrameRates();
    QVERIFY(rates.count() == 3);
    capture->setFrameRate(rates.first());
    QVERIFY(capture->frameRate().first == 30);
    QVERIFY(capture->minimumFrameRate().first == 1);
    QVERIFY(capture->maximumFrameRate().first == 30);

    QStringList vCodecs = capture->supportedVideoCodecs();
    QVERIFY(vCodecs.count() == 2);
    capture->setVideoCodec(vCodecs.first());
    QVERIFY(capture->videoCodec().compare("video/3gpp") == 0);
    QVERIFY(capture->videoCodecDescription("video/3gpp").compare("no comment") == 0);

    capture->setAudioBitrate(8000);
    QVERIFY(capture->audioBitrate() == 8000);

    capture->setAudioQuality(1);
    QVERIFY(capture->audioQuality() == 1);
}

QTEST_MAIN(tst_QMediaRecorder)

#include "tst_qmediarecorder.moc"
