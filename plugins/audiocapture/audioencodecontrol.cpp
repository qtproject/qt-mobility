#include "audioencodecontrol.h"
#include "audiocapturesession.h"

#include <QtMultimedia/qaudioformat.h>

#include <QtCore/qdebug.h>

AudioEncodeControl::AudioEncodeControl(QObject *parent)
    :QAudioEncoderControl(parent)
{
    QAudioFormat fmt;
    fmt.setSampleSize(8);
    fmt.setChannels(1);
    fmt.setFrequency(8000);
    fmt.setSampleType(QAudioFormat::SignedInt);
    fmt.setCodec("audio/pcm");

    m_session = qobject_cast<AudioCaptureSession*>(parent);
}

AudioEncodeControl::~AudioEncodeControl()
{
}

QStringList AudioEncodeControl::supportedAudioCodecs() const
{
    return m_session->supportedAudioCodecs();
}

QString AudioEncodeControl::audioCodec() const
{
    return m_session->format().codec();
}

bool AudioEncodeControl::setAudioCodec(const QString &codecName)
{
    QAudioFormat fmt = m_session->format();
    fmt.setCodec(codecName);
    return m_session->setFormat(fmt);
}

QString AudioEncodeControl::codecDescription(const QString &codecName) const
{
    if(qstrcmp(codecName.toLocal8Bit().constData(),"audio/x-wav") == 0)
        return QString("wav file format");

    return QString();
}

int AudioEncodeControl::bitrate() const
{
    return (m_session->format().frequency()*m_session->format().channels()*(m_session->format().sampleSize()/8));
}

void AudioEncodeControl::setBitrate(int value)
{
}

int AudioEncodeControl::quality() const
{
    return 0;
}

void AudioEncodeControl::setQuality(int value)
{
}

QStringList AudioEncodeControl::supportedEncodingOptions() const
{
    QStringList list;
    return list;
}

QVariant AudioEncodeControl::encodingOption(const QString &name) const
{
    if(qstrcmp(name.toLocal8Bit().constData(),"bitrate") == 0) {
        return QVariant(8000);
    }

    return QVariant();
}

void AudioEncodeControl::setEncodingOption(const QString &name, const QVariant &value)
{
    QAudioFormat fmt = m_session->format();

    if(qstrcmp(name.toLocal8Bit().constData(),"bitrate") == 0) {
        //TODO

    } else if(qstrcmp(name.toLocal8Bit().constData(),"quality") == 0) {
        //TODO

    } else
        qWarning()<<"option: "<<name<<" is an unknown option!";
}

int AudioEncodeControl::frequency() const
{
    return m_session->format().frequency();
}

void AudioEncodeControl::setFrequency(int frequency)
{
    QAudioFormat fmt = m_session->format();
    fmt.setFrequency(frequency);
    m_session->setFormat(fmt);
}

QList<int> AudioEncodeControl::supportedFrequencies() const
{
    return m_session->deviceInfo()->supportedFrequencies();
}

QPair<int,int> AudioEncodeControl::supportedFrequencyRange() const
{
    return qMakePair<int,int>(8000, 48000);
}

int AudioEncodeControl::channels() const
{
    return m_session->format().channels();
}

void AudioEncodeControl::setChannels(int channels)
{
    QAudioFormat fmt = m_session->format();
    fmt.setChannels(channels);
    m_session->setFormat(fmt);
}

QList<int> AudioEncodeControl::supportedChannelCounts() const
{
    return QList<int>() << 1 << 2;
}

int AudioEncodeControl::sampleSize() const
{
    return m_session->format().sampleSize();
}

void AudioEncodeControl::setSampleSize(int sampleSize)
{
    QAudioFormat fmt = m_session->format();
    fmt.setSampleSize(sampleSize);
    m_session->setFormat(fmt);
}

QList<int> AudioEncodeControl::supportedSampleSizes() const
{
    QList<int> sizes = m_session->deviceInfo()->supportedSampleSizes();
    return sizes;
}

