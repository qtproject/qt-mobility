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

#include <multimedia/qaudiosource.h>
#include <multimedia/qaudioencodercontrol.h>
#include <multimedia/qmediarecordercontrol.h>
#include <multimedia/qaudiodevicecontrol.h>

class MockAudioEncoderControl : public QAudioEncoderControl
{
    Q_OBJECT
public:
    MockAudioEncoderControl(QObject *parent = 0):
            QAudioEncoderControl(parent)
    {
        m_codecs << "audio/pcm" << "audio/mpeg";
        m_descriptions << "Pulse Code Modulation" << "mp3 format";
        m_codec = "audio/pcm";
        m_freq = 8000;
        m_freqs << 8000 << 11025 << 22050 << 44100;
        m_freqRange = qMakePair(8000,44100);
        m_channels = 1;
        m_sampleSize = 8;
    }

    ~MockAudioEncoderControl() {}

    QStringList supportedAudioCodecs() const { return m_codecs; }
    QString audioCodec() const { return m_codec; }
    bool setAudioCodec(const QString &codecName) { m_codec = codecName; return true; }
    QString codecDescription(const QString &codecName) const { return m_descriptions.at(m_codecs.indexOf(codecName)); }
    int bitrate() const { return 0; }
    void setBitrate(int) {}
    int quality() const { return 0; }
    void setQuality(int) {}
    QStringList supportedEncodingOptions() const { return QStringList() << "bitrate"; }
    QVariant encodingOption(const QString &name) const { return m_optionValue; }
    void setEncodingOption(const QString &name, const QVariant &value) { m_optionValue = value; }
    int frequency() const { return m_freq; }
    void setFrequency(int frequency) { m_freq = frequency; }
    QList<int> supportedFrequencies() const { return m_freqs; }
    QPair<int,int> supportedFrequencyRange() const { return m_freqRange; }
    int channels() const { return m_channels; }
    void setChannels(int channels) { m_channels = channels; }
    QList<int> supportedChannelCounts() const { QList<int> list; list << 1 << 2; return list; }
    int sampleSize() const { return m_sampleSize; }
    void setSampleSize(int sampleSize) { m_sampleSize = sampleSize; }
    QList<int> supportedSampleSizes() const { QList<int> list; list << 8 << 16; return list; }

    QStringList m_codecs;
    QStringList m_descriptions;
    QString m_codec;
    int m_freq;
    QList<int> m_freqs;
    QPair<int,int> m_freqRange;
    int m_channels;
    int m_sampleSize;
    QVariant m_optionValue;
};

class MockMediaRecorderControl : public QMediaRecorderControl
{
    Q_OBJECT
public:
    MockMediaRecorderControl(QObject *parent = 0):
            QMediaRecorderControl(parent),
            m_state(QMediaRecorder::StoppedState),
            m_position(0) {}

    ~MockMediaRecorderControl() {}

    QUrl sink() const { return m_sink; }
    bool setSink(const QUrl &sink) { m_sink = sink; return true; }
    QMediaRecorder::State state() const { return m_state; }
    qint64 duration() const { return m_position; }

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

class MockAudioDeviceControl : public QAudioDeviceControl
{
    Q_OBJECT
public:
    MockAudioDeviceControl(QObject *parent):
        QAudioDeviceControl(parent)
    {
        m_names << "device1" << "device2" << "device3";
        m_descriptions << "dev1 comment" << "dev2 comment" << "dev3 comment";
        m_index = 0;
        emit devicesChanged();
    }
    ~MockAudioDeviceControl() {};

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


class MockAudioSourceService : public QMediaService
{
    Q_OBJECT

public:
    MockAudioSourceService(): QMediaService(0)
    {
        mockAudioEncoderControl = new MockAudioEncoderControl(this);
        mockMediaRecorderControl = new MockMediaRecorderControl(this);
        mockAudioDeviceControl = new MockAudioDeviceControl(this);
    }

    ~MockAudioSourceService()
    {
        delete mockAudioEncoderControl;
        delete mockMediaRecorderControl;
        delete mockAudioDeviceControl;
    }

    QMediaControl* control(const char *iid) const
    {
        if (qstrcmp(iid, QAudioEncoderControl_iid) == 0)
            return mockAudioEncoderControl;

        if (qstrcmp(iid, QMediaRecorderControl_iid) == 0)
            return mockMediaRecorderControl;

        if (qstrcmp(iid, QAudioDeviceControl_iid) == 0)
            return mockAudioDeviceControl;

        return 0;
    }

    MockAudioEncoderControl *mockAudioEncoderControl;
    MockMediaRecorderControl *mockMediaRecorderControl;
    MockAudioDeviceControl *mockAudioDeviceControl;
};

class MockProvider : public QMediaServiceProvider
{
public:
    MockProvider(MockAudioSourceService *service):mockService(service) {}
    QMediaService *requestService(const QByteArray&, const QList<QByteArray> &)
    {
        return mockService;
    }

    void releaseService(QMediaService *) {}

    MockAudioSourceService *mockService;
};


class tst_QAudioSource: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testAudioSource();
    void testOptions();
    void testDevices();

private:
    QAudioSource *audiosource;
    MockAudioSourceService  *mockAudioSourceService;
    MockProvider *mockProvider;
};

void tst_QAudioSource::initTestCase()
{
    mockAudioSourceService = new MockAudioSourceService;
    mockProvider = new MockProvider(mockAudioSourceService);
}

void tst_QAudioSource::cleanupTestCase()
{
    delete audiosource;
    delete mockProvider;
}

void tst_QAudioSource::testAudioSource()
{
    audiosource = new QAudioSource(0, mockProvider);

    QCOMPARE(audiosource->service(), mockAudioSourceService);

    QVERIFY(audiosource->isValid());
}

void tst_QAudioSource::testOptions()
{
    QStringList options = mockAudioSourceService->mockAudioEncoderControl->supportedEncodingOptions();
    QVERIFY(options.count() == 1);
    mockAudioSourceService->mockAudioEncoderControl->setEncodingOption(options.first(),8000);
    QVERIFY(mockAudioSourceService->mockAudioEncoderControl->encodingOption(options.first()).toInt() == 8000);
}

void tst_QAudioSource::testDevices()
{
    int devices = audiosource->deviceCount();
    QVERIFY(devices > 0);
    QVERIFY(audiosource->name(0).compare("device1") == 0);
    QVERIFY(audiosource->description(0).compare("dev1 comment") == 0);
    QVERIFY(audiosource->icon(0).isNull());
    QVERIFY(audiosource->defaultDevice() == 1);

    QSignalSpy checkSignal(audiosource, SIGNAL(selectedDeviceChanged(int)));
    audiosource->setSelectedDevice(2);
    QVERIFY(audiosource->selectedDevice() == 2);
    QVERIFY(checkSignal.count() == 1);
}

QTEST_MAIN(tst_QAudioSource)

#include "tst_qaudiosource.moc"
