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
#include <qmetadatawritercontrol.h>
#include <qaudioendpointselector.h>
#include <qaudioencodercontrol.h>
#include <qmediacontainercontrol.h>
#include <qvideoencodercontrol.h>

#include <QtMultimedia/qaudioformat.h>

QT_USE_NAMESPACE
class MockMediaContainerControl : public QMediaContainerControl
{
    Q_OBJECT
public:
    MockMediaContainerControl(QObject *parent):
        QMediaContainerControl(parent)
    {
        m_supportedContainers.append("wav");
        m_supportedContainers.append("mp3");
        m_supportedContainers.append("mov");

        m_descriptions.insert("wav", "WAV format");
        m_descriptions.insert("mp3", "MP3 format");
        m_descriptions.insert("mov", "MOV format");
    }

    virtual ~MockMediaContainerControl() {};

    QStringList supportedContainers() const
    {
        return m_supportedContainers;
    }

    QString containerMimeType() const
    {
        return m_format;
    }

    void setContainerMimeType(const QString &formatMimeType)
    {
        if (m_supportedContainers.contains(formatMimeType))
            m_format = formatMimeType;
    }

    QString containerDescription(const QString &formatMimeType) const
    {
        return m_descriptions.value(formatMimeType);
    }

private:
    QStringList m_supportedContainers;
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
        m_supportedEncodeOptions.insert("video/3gpp", QStringList() << "quantizer" << "me");
        m_supportedEncodeOptions.insert("video/H264", QStringList() << "quantizer" << "me" << "bframes");
        m_videoCodecs << "video/3gpp" << "video/H264";
        m_sizes << QSize(320,240) << QSize(640,480);
        m_framerates << 30 << 15 << 1;
    }
    ~MockVideoEncodeProvider() {}

    QVideoEncoderSettings videoSettings() const { return m_videoSettings; }
    void setVideoSettings(const QVideoEncoderSettings &settings) { m_videoSettings = settings; };

    QList<QSize> supportedResolutions(const QVideoEncoderSettings & = QVideoEncoderSettings(),
                                      bool *continuous = 0) const
    {
        if (continuous)
            *continuous = true;

        return m_sizes;
    }

    QList<qreal> supportedFrameRates(const QVideoEncoderSettings & = QVideoEncoderSettings(),
                                     bool *continuous = 0) const
    {
        if (continuous)
            *continuous = false;

        return m_framerates;
    }

    QStringList supportedVideoCodecs() const { return m_videoCodecs; }
    QString videoCodecDescription(const QString &codecName) const { return codecName; }

    QStringList supportedEncodingOptions(const QString &codec) const
    {
        return m_supportedEncodeOptions.value(codec);
    }

    QVariant encodingOption(const QString &codec, const QString &name) const
    {
        return m_encodeOptions[codec].value(name);
    }

    void setEncodingOption(const QString &codec, const QString &name, const QVariant &value)
    {
        m_encodeOptions[codec][name] = value;
    }

private:
    QVideoEncoderSettings m_videoSettings;

    QMap<QString, QStringList> m_supportedEncodeOptions;
    QMap< QString, QMap<QString, QVariant> > m_encodeOptions;

    QStringList m_videoCodecs;
    QList<QSize> m_sizes;
    QList<qreal> m_framerates;
};

class MockAudioEncodeProvider : public QAudioEncoderControl
{
    Q_OBJECT
public:
    MockAudioEncodeProvider(QObject *parent):
        QAudioEncoderControl(parent)
    {
        m_codecs << "audio/pcm" << "audio/mpeg";
        m_supportedEncodeOptions.insert("audio/pcm", QStringList());
        m_supportedEncodeOptions.insert("audio/mpeg", QStringList() << "quality" << "bitrate" << "mode" << "vbr");
        m_audioSettings.setCodec("audio/pcm");
        m_audioSettings.setBitRate(128*1024);
    }

    ~MockAudioEncodeProvider() {}

    QAudioEncoderSettings audioSettings() const { return m_audioSettings; }
    void setAudioSettings(const QAudioEncoderSettings &settings) { m_audioSettings = settings; }

    QList<int> supportedSampleRates(const QAudioEncoderSettings & = QAudioEncoderSettings(), bool *continuous = 0) const
    {
        if (continuous)
            *continuous = false;

        return QList<int>() << 44100;
    }

    QStringList supportedAudioCodecs() const
    {
        return m_codecs;
    }

    QString codecDescription(const QString &codecName) const
    {
        if (codecName == "audio/pcm")
            return QString("Pulse Code Modulation");

        if (codecName == "audio/mpeg")
            return QString("MP3 audio format");

        return QString();
    }


    QStringList supportedEncodingOptions(const QString &codec) const
    {
        return m_supportedEncodeOptions.value(codec);
    }

    QVariant encodingOption(const QString &codec, const QString &name) const
    {
        return m_encodeOptions[codec].value(name);
    }

    void setEncodingOption(const QString &codec, const QString &name, const QVariant &value)
    {
        m_encodeOptions[codec][name] = value;
    }

private:
    QAudioEncoderSettings m_audioSettings;

    QStringList  m_codecs;
    QStringList  m_codecsDesc;

    QMap<QString, QStringList> m_supportedEncodeOptions;
    QMap< QString, QMap<QString, QVariant> > m_encodeOptions;

};

class MockAudioEndpointSelectorProvider : public QAudioEndpointSelector
{
    Q_OBJECT
public:
    MockAudioEndpointSelectorProvider(QObject *parent):
        QAudioEndpointSelector(parent)
    {
        m_names << "device1" << "device2" << "device3";
        m_descriptions << "dev1 comment" << "dev2 comment" << "dev3 comment";
        emit availableEndpointsChanged();
    }
    ~MockAudioEndpointSelectorProvider() {};

    QList<QString> availableEndpoints() const
    {
        return m_names;
    }

    QString endpointDescription(const QString& name) const
    {
        QString desc;

        for(int i = 0; i < m_names.count(); i++) {
            if (m_names.at(i).compare(name) == 0) {
                desc = m_descriptions.at(i);
                break;
            }
        }
        return desc;
    }

    QString defaultEndpoint() const
    {
        return m_names.at(0);
    }

    QString activeEndpoint() const
    {
        return m_audioInput;
    }

public Q_SLOTS:

    void setActiveEndpoint(const QString& name)
    {
        m_audioInput = name;
        emit activeEndpointChanged(name);
    }

private:
    QString         m_audioInput;
    QList<QString>  m_names;
    QList<QString>  m_descriptions;
};

class MockProvider : public QMediaRecorderControl
{
    Q_OBJECT

public:
    MockProvider(QObject *parent):
        QMediaRecorderControl(parent),
    m_state(QMediaRecorder::StoppedState),
    m_position(0),
    m_muted(false) {}

    QUrl outputLocation() const
    {
        return m_sink;
    }

    bool setOutputLocation(const QUrl &sink)
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

    bool isMuted() const
    {
        return m_muted;
    }

    void applySettings() {}

    using QMediaRecorderControl::error;

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

    void setMuted(bool muted)
    {
        if (m_muted != muted)
            emit mutedChanged(m_muted = muted);
    }

public:
    QUrl       m_sink;
    QMediaRecorder::State m_state;
    qint64     m_position;
    bool m_muted;
};


class QtTestMetaDataProvider : public QMetaDataWriterControl
{
    Q_OBJECT
public:
    QtTestMetaDataProvider(QObject *parent = 0)
        : QMetaDataWriterControl(parent)
        , m_available(false)
        , m_writable(false)
    {
    }

    bool isMetaDataAvailable() const { return m_available; }
    void setMetaDataAvailable(bool available) {
        if (m_available != available)
            emit metaDataAvailableChanged(m_available = available);
    }
    QList<QtMediaServices::MetaData> availableMetaData() const { return m_data.keys(); }

    bool isWritable() const { return m_writable; }
    void setWritable(bool writable) { emit writableChanged(m_writable = writable); }

    QVariant metaData(QtMediaServices::MetaData key) const { return m_data.value(key); }
    void setMetaData(QtMediaServices::MetaData key, const QVariant &value) {
        m_data.insert(key, value); }

    QVariant extendedMetaData(const QString &key) const { return m_extendedData.value(key); }
    void setExtendedMetaData(const QString &key, const QVariant &value) {
        m_extendedData.insert(key, value); }

    QStringList availableExtendedMetaData() const { return m_extendedData.keys(); }

    using QMetaDataWriterControl::metaDataChanged;

    void populateMetaData()
    {
        m_available = true;
    }

    bool m_available;
    bool m_writable;
    QMap<QtMediaServices::MetaData, QVariant> m_data;
    QMap<QString, QVariant> m_extendedData;
};

class MockService : public QMediaService
{
    Q_OBJECT
public:
    MockService(QObject *parent, QMediaControl *control):
        QMediaService(parent),
        mockControl(control),
        hasControls(true)
    {
        mockAudioEndpointSelector = new MockAudioEndpointSelectorProvider(parent);
        mockAudioEncodeControl = new MockAudioEncodeProvider(parent);
        mockFormatControl = new MockMediaContainerControl(parent);
        mockVideoEncodeControl = new MockVideoEncodeProvider(parent);
        mockMetaDataControl = new QtTestMetaDataProvider(parent);
    }

    QMediaControl* requestControl(const char *name)
    {
        if(hasControls && qstrcmp(name,QAudioEncoderControl_iid) == 0)
            return mockAudioEncodeControl;
        if(hasControls && qstrcmp(name,QAudioEndpointSelector_iid) == 0)
            return mockAudioEndpointSelector;
        if(hasControls && qstrcmp(name,QMediaRecorderControl_iid) == 0)
            return mockControl;
        if(hasControls && qstrcmp(name,QMediaContainerControl_iid) == 0)
            return mockFormatControl;
        if(hasControls && qstrcmp(name,QVideoEncoderControl_iid) == 0)
            return mockVideoEncodeControl;
        if (hasControls && qstrcmp(name, QMetaDataWriterControl_iid) == 0)
            return mockMetaDataControl;

        return 0;
    }

    void releaseControl(QMediaControl*) {}

    QMediaControl   *mockControl;
    QAudioEndpointSelector  *mockAudioEndpointSelector;
    QAudioEncoderControl    *mockAudioEncodeControl;
    QMediaContainerControl     *mockFormatControl;
    QVideoEncoderControl    *mockVideoEncodeControl;
    QtTestMetaDataProvider *mockMetaDataControl;
    bool hasControls;
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
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testNullService();
    void testNullControls();
    void testError();
    void testSink();
    void testRecord();
    void testMute();
    void testAudioDeviceControl();
    void testAudioEncodeControl();
    void testMediaFormatsControl();
    void testVideoEncodeControl();
    void testEncodingSettings();
    void testAudioSettings();
    void testVideoSettings();

    void nullMetaDataControl();
    void isMetaDataAvailable();
    void isWritable();
    void metaDataChanged();
    void metaData_data();
    void metaData();
    void setMetaData_data();
    void setMetaData();
    void extendedMetaData_data() { metaData_data(); }
    void extendedMetaData();
    void setExtendedMetaData_data() { extendedMetaData_data(); }
    void setExtendedMetaData();

private:
    QAudioEncoderControl* encode;
    QAudioEndpointSelector* audio;
    MockObject      *object;
    MockService		*service;
    MockProvider    *mock;
    QMediaRecorder  *capture;
    QVideoEncoderControl* videoEncode;
};

void tst_QMediaRecorder::initTestCase()
{
    qRegisterMetaType<QMediaRecorder::State>("QMediaRecorder::State");
    qRegisterMetaType<QMediaRecorder::Error>("QMediaRecorder::Error");

    mock = new MockProvider(this);
    service = new MockService(this, mock);
    object = new MockObject(this, service);
    capture = new QMediaRecorder(object);

    audio = qobject_cast<QAudioEndpointSelector*>(service->requestControl(QAudioEndpointSelector_iid));
    encode = qobject_cast<QAudioEncoderControl*>(service->requestControl(QAudioEncoderControl_iid));
    videoEncode = qobject_cast<QVideoEncoderControl*>(service->requestControl(QVideoEncoderControl_iid));
}

void tst_QMediaRecorder::cleanupTestCase()
{
    delete capture;
    delete object;
    delete service;
    delete mock;
}

void tst_QMediaRecorder::testNullService()
{
    const QString id(QLatin1String("application/x-format"));

    MockObject object(0, 0);
    QMediaRecorder recorder(&object);

    QCOMPARE(recorder.outputLocation(), QUrl());
    QCOMPARE(recorder.state(), QMediaRecorder::StoppedState);
    QCOMPARE(recorder.error(), QMediaRecorder::NoError);
    QCOMPARE(recorder.duration(), qint64(0));
    QCOMPARE(recorder.supportedContainers(), QStringList());
    QCOMPARE(recorder.containerDescription(id), QString());
    QCOMPARE(recorder.supportedAudioCodecs(), QStringList());
    QCOMPARE(recorder.audioCodecDescription(id), QString());
    QCOMPARE(recorder.supportedAudioSampleRates(), QList<int>());
    QCOMPARE(recorder.supportedVideoCodecs(), QStringList());
    QCOMPARE(recorder.videoCodecDescription(id), QString());
    bool continuous = true;
    QCOMPARE(recorder.supportedResolutions(QVideoEncoderSettings(), &continuous), QList<QSize>());
    QCOMPARE(continuous, false);
    continuous = true;
    QCOMPARE(recorder.supportedFrameRates(QVideoEncoderSettings(), &continuous), QList<qreal>());
    QCOMPARE(continuous, false);
    QCOMPARE(recorder.audioSettings(), QAudioEncoderSettings());
    QCOMPARE(recorder.videoSettings(), QVideoEncoderSettings());
    QCOMPARE(recorder.containerMimeType(), QString());
    QVERIFY(!recorder.isMuted());
    recorder.setMuted(true);
    QVERIFY(!recorder.isMuted());
}

void tst_QMediaRecorder::testNullControls()
{
    const QString id(QLatin1String("application/x-format"));

    MockService service(0, 0);
    service.hasControls = false;
    MockObject object(0, &service);
    QMediaRecorder recorder(&object);

    QCOMPARE(recorder.outputLocation(), QUrl());
    QCOMPARE(recorder.state(), QMediaRecorder::StoppedState);
    QCOMPARE(recorder.error(), QMediaRecorder::NoError);
    QCOMPARE(recorder.duration(), qint64(0));
    QCOMPARE(recorder.supportedContainers(), QStringList());
    QCOMPARE(recorder.containerDescription(id), QString());
    QCOMPARE(recorder.supportedAudioCodecs(), QStringList());
    QCOMPARE(recorder.audioCodecDescription(id), QString());
    QCOMPARE(recorder.supportedAudioSampleRates(), QList<int>());
    QCOMPARE(recorder.supportedVideoCodecs(), QStringList());
    QCOMPARE(recorder.videoCodecDescription(id), QString());
    bool continuous = true;
    QCOMPARE(recorder.supportedResolutions(QVideoEncoderSettings(), &continuous), QList<QSize>());
    QCOMPARE(continuous, false);
    continuous = true;
    QCOMPARE(recorder.supportedFrameRates(QVideoEncoderSettings(), &continuous), QList<qreal>());
    QCOMPARE(continuous, false);
    QCOMPARE(recorder.audioSettings(), QAudioEncoderSettings());
    QCOMPARE(recorder.videoSettings(), QVideoEncoderSettings());
    QCOMPARE(recorder.containerMimeType(), QString());

    recorder.setOutputLocation(QUrl("file://test/save/file.mp4"));
    QCOMPARE(recorder.outputLocation(), QUrl());

    QAudioEncoderSettings audio;
    audio.setCodec(id);
    audio.setQuality(QtMediaServices::LowQuality);

    QVideoEncoderSettings video;
    video.setCodec(id);
    video.setResolution(640, 480);

    recorder.setEncodingSettings(audio, video, id);

    QCOMPARE(recorder.audioSettings(), QAudioEncoderSettings());
    QCOMPARE(recorder.videoSettings(), QVideoEncoderSettings());
    QCOMPARE(recorder.containerMimeType(), QString());

    QSignalSpy spy(&recorder, SIGNAL(stateChanged(QMediaRecorder::State)));

    recorder.record();
    QCOMPARE(recorder.state(), QMediaRecorder::StoppedState);
    QCOMPARE(recorder.error(), QMediaRecorder::NoError);
    QCOMPARE(spy.count(), 0);

    recorder.pause();
    QCOMPARE(recorder.state(), QMediaRecorder::StoppedState);
    QCOMPARE(recorder.error(), QMediaRecorder::NoError);
    QCOMPARE(spy.count(), 0);

    recorder.stop();
    QCOMPARE(recorder.state(), QMediaRecorder::StoppedState);
    QCOMPARE(recorder.error(), QMediaRecorder::NoError);
    QCOMPARE(spy.count(), 0);
}

void tst_QMediaRecorder::testError()
{
    const QString errorString(QLatin1String("format error"));

    QSignalSpy spy(capture, SIGNAL(error(QMediaRecorder::Error)));

    QCOMPARE(capture->error(), QMediaRecorder::NoError);
    QCOMPARE(capture->errorString(), QString());

    mock->error(QMediaRecorder::FormatError, errorString);
    QCOMPARE(capture->error(), QMediaRecorder::FormatError);
    QCOMPARE(capture->errorString(), errorString);
    QCOMPARE(spy.count(), 1);

    QCOMPARE(spy.last()[0].value<QMediaRecorder::Error>(), QMediaRecorder::FormatError);
}

void tst_QMediaRecorder::testSink()
{
    capture->setOutputLocation(QUrl("test.tmp"));
    QUrl s = capture->outputLocation();
    QCOMPARE(s.toString(), QString("test.tmp"));
}

void tst_QMediaRecorder::testRecord()
{
    QSignalSpy stateSignal(capture,SIGNAL(stateChanged(QMediaRecorder::State)));
    QSignalSpy progressSignal(capture, SIGNAL(durationChanged(qint64)));
    capture->record();
    QCOMPARE(capture->state(), QMediaRecorder::RecordingState);
    QCOMPARE(capture->error(), QMediaRecorder::NoError);
    QCOMPARE(capture->errorString(), QString());
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(stateSignal.count(), 1);
    QCOMPARE(stateSignal.last()[0].value<QMediaRecorder::State>(), QMediaRecorder::RecordingState);
    QVERIFY(progressSignal.count() > 0);
    capture->pause();
    QCOMPARE(capture->state(), QMediaRecorder::PausedState);
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(stateSignal.count(), 2);
    capture->stop();
    QCOMPARE(capture->state(), QMediaRecorder::StoppedState);
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(stateSignal.count(), 3);
    mock->stop();
    QCOMPARE(stateSignal.count(), 3);

}

void tst_QMediaRecorder::testMute()
{
    QSignalSpy mutedChanged(capture, SIGNAL(mutedChanged(bool)));
    QVERIFY(!capture->isMuted());
    capture->setMuted(true);

    QCOMPARE(mutedChanged.size(), 1);
    QCOMPARE(mutedChanged[0][0].toBool(), true);
    QVERIFY(capture->isMuted());

    capture->setMuted(false);

    QCOMPARE(mutedChanged.size(), 2);
    QCOMPARE(mutedChanged[1][0].toBool(), false);
    QVERIFY(!capture->isMuted());

    capture->setMuted(false);
    QCOMPARE(mutedChanged.size(), 2);
}

void tst_QMediaRecorder::testAudioDeviceControl()
{
    QSignalSpy readSignal(audio,SIGNAL(activeEndpointChanged(QString)));
    QVERIFY(audio->availableEndpoints().size() == 3);
    QVERIFY(audio->defaultEndpoint().compare("device1") == 0);
    audio->setActiveEndpoint("device2");
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(audio->activeEndpoint().compare("device2") == 0);
    QVERIFY(readSignal.count() == 1);
    QVERIFY(audio->endpointDescription("device2").compare("dev2 comment") == 0);
}

void tst_QMediaRecorder::testAudioEncodeControl()
{
    QStringList codecs = capture->supportedAudioCodecs();
    QVERIFY(codecs.count() == 2);
    QVERIFY(capture->audioCodecDescription("audio/pcm") == "Pulse Code Modulation");
    QStringList options = encode->supportedEncodingOptions("audio/mpeg");
    QCOMPARE(options.count(), 4);
    QVERIFY(encode->encodingOption("audio/mpeg","bitrate").isNull());
    encode->setEncodingOption("audio/mpeg", "bitrate", QString("vbr"));
    QCOMPARE(encode->encodingOption("audio/mpeg","bitrate").toString(), QString("vbr"));
    QCOMPARE(capture->supportedAudioSampleRates(), QList<int>() << 44100);
}

void tst_QMediaRecorder::testMediaFormatsControl()
{
    QCOMPARE(capture->supportedContainers(), QStringList() << "wav" << "mp3" << "mov");

    QCOMPARE(capture->containerDescription("wav"), QString("WAV format"));
    QCOMPARE(capture->containerDescription("mp3"), QString("MP3 format"));
    QCOMPARE(capture->containerDescription("ogg"), QString());
}

void tst_QMediaRecorder::testVideoEncodeControl()
{
    bool continuous = false;
    QList<QSize> sizes = capture->supportedResolutions(QVideoEncoderSettings(), &continuous);
    QCOMPARE(sizes.count(), 2);
    QCOMPARE(continuous, true);

    QList<qreal> rates = capture->supportedFrameRates(QVideoEncoderSettings(), &continuous);
    QCOMPARE(rates.count(), 3);
    QCOMPARE(continuous, false);

    QStringList vCodecs = capture->supportedVideoCodecs();
    QVERIFY(vCodecs.count() == 2);
    QCOMPARE(capture->videoCodecDescription("video/3gpp"), QString("video/3gpp"));

    QStringList options = videoEncode->supportedEncodingOptions("video/3gpp");
    QCOMPARE(options.count(), 2);

    QVERIFY(encode->encodingOption("video/3gpp","me").isNull());
    encode->setEncodingOption("video/3gpp", "me", QString("dia"));
    QCOMPARE(encode->encodingOption("video/3gpp","me").toString(), QString("dia"));

}

void tst_QMediaRecorder::testEncodingSettings()
{
    QAudioEncoderSettings audioSettings = capture->audioSettings();
    QCOMPARE(audioSettings.codec(), QString("audio/pcm"));
    QCOMPARE(audioSettings.bitRate(), 128*1024);
    QCOMPARE(audioSettings.sampleRate(), -1);
    QCOMPARE(audioSettings.quality(), QtMediaServices::NormalQuality);
    QCOMPARE(audioSettings.channelCount(), -1);

    QCOMPARE(audioSettings.encodingMode(), QtMediaServices::ConstantQualityEncoding);

    QVideoEncoderSettings videoSettings = capture->videoSettings();
    QCOMPARE(videoSettings.codec(), QString());
    QCOMPARE(videoSettings.bitRate(), -1);
    QCOMPARE(videoSettings.resolution(), QSize());
    QCOMPARE(videoSettings.frameRate(), 0.0);
    QCOMPARE(videoSettings.quality(), QtMediaServices::NormalQuality);
    QCOMPARE(videoSettings.encodingMode(), QtMediaServices::ConstantQualityEncoding);

    QString format = capture->containerMimeType();
    QCOMPARE(format, QString());

    audioSettings.setCodec("audio/mpeg");
    audioSettings.setSampleRate(44100);
    audioSettings.setBitRate(256*1024);
    audioSettings.setQuality(QtMediaServices::HighQuality);
    audioSettings.setEncodingMode(QtMediaServices::AverageBitRateEncoding);

    videoSettings.setCodec("video/3gpp");
    videoSettings.setBitRate(800);
    videoSettings.setFrameRate(24*1024);
    videoSettings.setResolution(QSize(800,600));
    videoSettings.setQuality(QtMediaServices::HighQuality);
    audioSettings.setEncodingMode(QtMediaServices::TwoPassEncoding);

    format = QString("mov");

    capture->setEncodingSettings(audioSettings,videoSettings,format);

    QCOMPARE(capture->audioSettings(), audioSettings);
    QCOMPARE(capture->videoSettings(), videoSettings);
    QCOMPARE(capture->containerMimeType(), format);
}

void tst_QMediaRecorder::testAudioSettings()
{
    QAudioEncoderSettings settings;
    QVERIFY(settings.isNull());
    QVERIFY(settings == QAudioEncoderSettings());

    QCOMPARE(settings.codec(), QString());
    settings.setCodec(QLatin1String("codecName"));
    QCOMPARE(settings.codec(), QLatin1String("codecName"));
    QVERIFY(!settings.isNull());
    QVERIFY(settings != QAudioEncoderSettings());

    settings = QAudioEncoderSettings();
    QCOMPARE(settings.bitRate(), -1);
    settings.setBitRate(128000);
    QCOMPARE(settings.bitRate(), 128000);
    QVERIFY(!settings.isNull());

    settings = QAudioEncoderSettings();
    QCOMPARE(settings.quality(), QtMediaServices::NormalQuality);
    settings.setQuality(QtMediaServices::HighQuality);
    QCOMPARE(settings.quality(), QtMediaServices::HighQuality);
    QVERIFY(!settings.isNull());

    settings = QAudioEncoderSettings();
    QCOMPARE(settings.sampleRate(), -1);
    settings.setSampleRate(44100);
    QCOMPARE(settings.sampleRate(), 44100);
    QVERIFY(!settings.isNull());

    settings = QAudioEncoderSettings();
    QCOMPARE(settings.channelCount(), -1);
    settings.setChannelCount(2);
    QCOMPARE(settings.channelCount(), 2);
    QVERIFY(!settings.isNull());

    settings = QAudioEncoderSettings();
    QVERIFY(settings.isNull());
    QCOMPARE(settings.codec(), QString());
    QCOMPARE(settings.bitRate(), -1);
    QCOMPARE(settings.quality(), QtMediaServices::NormalQuality);
    QCOMPARE(settings.sampleRate(), -1);

    {
        QAudioEncoderSettings settings1;
        QAudioEncoderSettings settings2;
        QCOMPARE(settings2, settings1);

        settings2 = settings1;
        QCOMPARE(settings2, settings1);
        QVERIFY(settings2.isNull());

        settings1.setQuality(QtMediaServices::HighQuality);

        QVERIFY(settings2.isNull());
        QVERIFY(!settings1.isNull());
        QVERIFY(settings1 != settings2);
    }

    {
        QAudioEncoderSettings settings1;
        QAudioEncoderSettings settings2(settings1);
        QCOMPARE(settings2, settings1);

        settings2 = settings1;
        QCOMPARE(settings2, settings1);
        QVERIFY(settings2.isNull());

        settings1.setQuality(QtMediaServices::HighQuality);

        QVERIFY(settings2.isNull());
        QVERIFY(!settings1.isNull());
        QVERIFY(settings1 != settings2);
    }

    QAudioEncoderSettings settings1;
    settings1.setBitRate(1);
    QAudioEncoderSettings settings2;
    settings2.setBitRate(1);
    QVERIFY(settings1 == settings2);
    settings2.setBitRate(2);
    QVERIFY(settings1 != settings2);

    settings1 = QAudioEncoderSettings();
    settings1.setChannelCount(1);
    settings2 = QAudioEncoderSettings();
    settings2.setChannelCount(1);
    QVERIFY(settings1 == settings2);
    settings2.setChannelCount(2);
    QVERIFY(settings1 != settings2);

    settings1 = QAudioEncoderSettings();
    settings1.setCodec("codec1");
    settings2 = QAudioEncoderSettings();
    settings2.setCodec("codec1");
    QVERIFY(settings1 == settings2);
    settings2.setCodec("codec2");
    QVERIFY(settings1 != settings2);

    settings1 = QAudioEncoderSettings();
    settings1.setEncodingMode(QtMediaServices::ConstantBitRateEncoding);
    settings2 = QAudioEncoderSettings();
    settings2.setEncodingMode(QtMediaServices::ConstantBitRateEncoding);
    QVERIFY(settings1 == settings2);
    settings2.setEncodingMode(QtMediaServices::TwoPassEncoding);
    QVERIFY(settings1 != settings2);

    settings1 = QAudioEncoderSettings();
    settings1.setQuality(QtMediaServices::NormalQuality);
    settings2 = QAudioEncoderSettings();
    settings2.setQuality(QtMediaServices::NormalQuality);
    QVERIFY(settings1 == settings2);
    settings2.setQuality(QtMediaServices::LowQuality);
    QVERIFY(settings1 != settings2);

    settings1 = QAudioEncoderSettings();
    settings1.setSampleRate(1);
    settings2 = QAudioEncoderSettings();
    settings2.setSampleRate(1);
    QVERIFY(settings1 == settings2);
    settings2.setSampleRate(2);
    QVERIFY(settings1 != settings2);
}

void tst_QMediaRecorder::testVideoSettings()
{
    QVideoEncoderSettings settings;
    QVERIFY(settings.isNull());
    QVERIFY(settings == QVideoEncoderSettings());

    QCOMPARE(settings.codec(), QString());
    settings.setCodec(QLatin1String("codecName"));
    QCOMPARE(settings.codec(), QLatin1String("codecName"));
    QVERIFY(!settings.isNull());
    QVERIFY(settings != QVideoEncoderSettings());

    settings = QVideoEncoderSettings();
    QCOMPARE(settings.bitRate(), -1);
    settings.setBitRate(128000);
    QCOMPARE(settings.bitRate(), 128000);
    QVERIFY(!settings.isNull());

    settings = QVideoEncoderSettings();
    QCOMPARE(settings.quality(), QtMediaServices::NormalQuality);
    settings.setQuality(QtMediaServices::HighQuality);
    QCOMPARE(settings.quality(), QtMediaServices::HighQuality);
    QVERIFY(!settings.isNull());

    settings = QVideoEncoderSettings();
    QCOMPARE(settings.frameRate(), qreal());
    settings.setFrameRate(30000.0/10001);
    QVERIFY(qFuzzyCompare(settings.frameRate(), qreal(30000.0/10001)));
    settings.setFrameRate(24.0);
    QVERIFY(qFuzzyCompare(settings.frameRate(), qreal(24.0)));
    QVERIFY(!settings.isNull());

    settings = QVideoEncoderSettings();
    QCOMPARE(settings.resolution(), QSize());
    settings.setResolution(QSize(320,240));
    QCOMPARE(settings.resolution(), QSize(320,240));
    settings.setResolution(800,600);
    QCOMPARE(settings.resolution(), QSize(800,600));
    QVERIFY(!settings.isNull());

    settings = QVideoEncoderSettings();
    QVERIFY(settings.isNull());
    QCOMPARE(settings.codec(), QString());
    QCOMPARE(settings.bitRate(), -1);
    QCOMPARE(settings.quality(), QtMediaServices::NormalQuality);
    QCOMPARE(settings.frameRate(), qreal());
    QCOMPARE(settings.resolution(), QSize());

    {
        QVideoEncoderSettings settings1;
        QVideoEncoderSettings settings2;
        QCOMPARE(settings2, settings1);

        settings2 = settings1;
        QCOMPARE(settings2, settings1);
        QVERIFY(settings2.isNull());

        settings1.setQuality(QtMediaServices::HighQuality);

        QVERIFY(settings2.isNull());
        QVERIFY(!settings1.isNull());
        QVERIFY(settings1 != settings2);
    }

    {
        QVideoEncoderSettings settings1;
        QVideoEncoderSettings settings2(settings1);
        QCOMPARE(settings2, settings1);

        settings2 = settings1;
        QCOMPARE(settings2, settings1);
        QVERIFY(settings2.isNull());

        settings1.setQuality(QtMediaServices::HighQuality);

        QVERIFY(settings2.isNull());
        QVERIFY(!settings1.isNull());
        QVERIFY(settings1 != settings2);
    }

    QVideoEncoderSettings settings1;
    settings1.setBitRate(1);
    QVideoEncoderSettings settings2;
    settings2.setBitRate(1);
    QVERIFY(settings1 == settings2);
    settings2.setBitRate(2);
    QVERIFY(settings1 != settings2);

    settings1 = QVideoEncoderSettings();
    settings1.setResolution(800,600);
    settings2 = QVideoEncoderSettings();
    settings2.setResolution(QSize(800,600));
    QVERIFY(settings1 == settings2);
    settings2.setResolution(QSize(400,300));
    QVERIFY(settings1 != settings2);

    settings1 = QVideoEncoderSettings();
    settings1.setCodec("codec1");
    settings2 = QVideoEncoderSettings();
    settings2.setCodec("codec1");
    QVERIFY(settings1 == settings2);
    settings2.setCodec("codec2");
    QVERIFY(settings1 != settings2);

    settings1 = QVideoEncoderSettings();
    settings1.setEncodingMode(QtMediaServices::ConstantBitRateEncoding);
    settings2 = QVideoEncoderSettings();
    settings2.setEncodingMode(QtMediaServices::ConstantBitRateEncoding);
    QVERIFY(settings1 == settings2);
    settings2.setEncodingMode(QtMediaServices::TwoPassEncoding);
    QVERIFY(settings1 != settings2);

    settings1 = QVideoEncoderSettings();
    settings1.setQuality(QtMediaServices::NormalQuality);
    settings2 = QVideoEncoderSettings();
    settings2.setQuality(QtMediaServices::NormalQuality);
    QVERIFY(settings1 == settings2);
    settings2.setQuality(QtMediaServices::LowQuality);
    QVERIFY(settings1 != settings2);

    settings1 = QVideoEncoderSettings();
    settings1.setFrameRate(1);
    settings2 = QVideoEncoderSettings();
    settings2.setFrameRate(1);
    QVERIFY(settings1 == settings2);
    settings2.setFrameRate(2);
    QVERIFY(settings1 != settings2);
}


void tst_QMediaRecorder::nullMetaDataControl()
{
    const QString titleKey(QLatin1String("Title"));
    const QString title(QLatin1String("Host of Seraphim"));

    MockProvider recorderControl(0);
    MockService service(0, &recorderControl);
    service.hasControls = false;
    MockObject object(0, &service);

    QMediaRecorder recorder(&object);

    QSignalSpy spy(&recorder, SIGNAL(metaDataChanged()));

    QCOMPARE(recorder.isMetaDataAvailable(), false);
    QCOMPARE(recorder.isMetaDataWritable(), false);

    recorder.setMetaData(QtMediaServices::Title, title);
    recorder.setExtendedMetaData(titleKey, title);

    QCOMPARE(recorder.metaData(QtMediaServices::Title).toString(), QString());
    QCOMPARE(recorder.extendedMetaData(titleKey).toString(), QString());
    QCOMPARE(recorder.availableMetaData(), QList<QtMediaServices::MetaData>());
    QCOMPARE(recorder.availableExtendedMetaData(), QStringList());
    QCOMPARE(spy.count(), 0);
}

void tst_QMediaRecorder::isMetaDataAvailable()
{
    MockProvider recorderControl(0);
    MockService service(0, &recorderControl);
    service.mockMetaDataControl->setMetaDataAvailable(false);
    MockObject object(0, &service);

    QMediaRecorder recorder(&object);
    QCOMPARE(recorder.isMetaDataAvailable(), false);

    QSignalSpy spy(&recorder, SIGNAL(metaDataAvailableChanged(bool)));
    service.mockMetaDataControl->setMetaDataAvailable(true);

    QCOMPARE(recorder.isMetaDataAvailable(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);

    service.mockMetaDataControl->setMetaDataAvailable(false);

    QCOMPARE(recorder.isMetaDataAvailable(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void tst_QMediaRecorder::isWritable()
{
    MockProvider recorderControl(0);
    MockService service(0, &recorderControl);
    service.mockMetaDataControl->setWritable(false);

    MockObject object(0, &service);

    QMediaRecorder recorder(&object);

    QSignalSpy spy(&recorder, SIGNAL(metaDataWritableChanged(bool)));

    QCOMPARE(recorder.isMetaDataWritable(), false);

    service.mockMetaDataControl->setWritable(true);

    QCOMPARE(recorder.isMetaDataWritable(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);

    service.mockMetaDataControl->setWritable(false);

    QCOMPARE(recorder.isMetaDataWritable(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void tst_QMediaRecorder::metaDataChanged()
{
    MockProvider recorderControl(0);
    MockService service(0, &recorderControl);
    MockObject object(0, &service);

    QMediaRecorder recorder(&object);

    QSignalSpy spy(&recorder, SIGNAL(metaDataChanged()));

    service.mockMetaDataControl->metaDataChanged();
    QCOMPARE(spy.count(), 1);

    service.mockMetaDataControl->metaDataChanged();
    QCOMPARE(spy.count(), 2);
}

void tst_QMediaRecorder::metaData_data()
{
    QTest::addColumn<QString>("artist");
    QTest::addColumn<QString>("title");
    QTest::addColumn<QString>("genre");

    QTest::newRow("")
            << QString::fromLatin1("Dead Can Dance")
            << QString::fromLatin1("Host of Seraphim")
            << QString::fromLatin1("Awesome");
}

void tst_QMediaRecorder::metaData()
{
    QFETCH(QString, artist);
    QFETCH(QString, title);
    QFETCH(QString, genre);

    MockProvider recorderControl(0);
    MockService service(0, &recorderControl);
    service.mockMetaDataControl->populateMetaData();

    MockObject object(0, &service);

    QMediaRecorder recorder(&object);
    QVERIFY(object.availableMetaData().isEmpty());

    service.mockMetaDataControl->m_data.insert(QtMediaServices::AlbumArtist, artist);
    service.mockMetaDataControl->m_data.insert(QtMediaServices::Title, title);
    service.mockMetaDataControl->m_data.insert(QtMediaServices::Genre, genre);

    QCOMPARE(recorder.metaData(QtMediaServices::AlbumArtist).toString(), artist);
    QCOMPARE(recorder.metaData(QtMediaServices::Title).toString(), title);

    QList<QtMediaServices::MetaData> metaDataKeys = recorder.availableMetaData();
    QCOMPARE(metaDataKeys.size(), 3);
    QVERIFY(metaDataKeys.contains(QtMediaServices::AlbumArtist));
    QVERIFY(metaDataKeys.contains(QtMediaServices::Title));
    QVERIFY(metaDataKeys.contains(QtMediaServices::Genre));
}

void tst_QMediaRecorder::setMetaData_data()
{
    QTest::addColumn<QString>("title");

    QTest::newRow("")
            << QString::fromLatin1("In the Kingdom of the Blind the One eyed are Kings");
}

void tst_QMediaRecorder::setMetaData()
{
    QFETCH(QString, title);

    MockProvider recorderControl(0);
    MockService service(0, &recorderControl);
    service.mockMetaDataControl->populateMetaData();

    MockObject object(0, &service);

    QMediaRecorder recorder(&object);

    recorder.setMetaData(QtMediaServices::Title, title);
    QCOMPARE(recorder.metaData(QtMediaServices::Title).toString(), title);
    QCOMPARE(service.mockMetaDataControl->m_data.value(QtMediaServices::Title).toString(), title);
}

void tst_QMediaRecorder::extendedMetaData()
{
    QFETCH(QString, artist);
    QFETCH(QString, title);
    QFETCH(QString, genre);

    MockProvider recorderControl(0);
    MockService service(0, &recorderControl);
    MockObject object(0, &service);

    QMediaRecorder recorder(&object);
    QVERIFY(recorder.availableExtendedMetaData().isEmpty());

    service.mockMetaDataControl->m_extendedData.insert(QLatin1String("Artist"), artist);
    service.mockMetaDataControl->m_extendedData.insert(QLatin1String("Title"), title);
    service.mockMetaDataControl->m_extendedData.insert(QLatin1String("Genre"), genre);

    QCOMPARE(recorder.extendedMetaData(QLatin1String("Artist")).toString(), artist);
    QCOMPARE(recorder.extendedMetaData(QLatin1String("Title")).toString(), title);

    QStringList extendedKeys = recorder.availableExtendedMetaData();
    QCOMPARE(extendedKeys.size(), 3);
    QVERIFY(extendedKeys.contains(QLatin1String("Artist")));
    QVERIFY(extendedKeys.contains(QLatin1String("Title")));
    QVERIFY(extendedKeys.contains(QLatin1String("Genre")));
}

void tst_QMediaRecorder::setExtendedMetaData()
{
    MockProvider recorderControl(0);
    MockService service(0, &recorderControl);
    service.mockMetaDataControl->populateMetaData();

    MockObject object(0, &service);

    QMediaRecorder recorder(&object);

    QString title(QLatin1String("In the Kingdom of the Blind the One eyed are Kings"));

    recorder.setExtendedMetaData(QLatin1String("Title"), title);
    QCOMPARE(recorder.extendedMetaData(QLatin1String("Title")).toString(), title);
    QCOMPARE(service.mockMetaDataControl->m_extendedData.value(QLatin1String("Title")).toString(), title);
}


QTEST_MAIN(tst_QMediaRecorder)

#include "tst_qmediarecorder.moc"
