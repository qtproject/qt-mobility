#include "audioencodecontrol.h"
#include "audiocapturesession.h"

#include <QtMultimedia/qaudioformat.h>

#include <QtCore/qdebug.h>

AudioEncodeControl::AudioEncodeControl(QObject *parent)
    :QAudioEncodeControl(parent)
{
    m_codecs.append("pcm");
    m_codecDescriptions.insert("pcm", "WAV encoding");

    if (!m_codecs.isEmpty())
        setAudioCodec(m_codecs[0]);

    m_format.setFrequency(8000);
    m_format.setChannels(1);
    m_format.setSampleSize(8);
    m_format.setCodec("audio/pcm");
}

AudioEncodeControl::~AudioEncodeControl()
{
}

QAudioFormat AudioEncodeControl::format() const
{
    return m_format;
}

bool AudioEncodeControl::isFormatSupported(const QAudioFormat &format) const
{
    Q_UNUSED(format);
    return true;
}

bool AudioEncodeControl::setFormat(const QAudioFormat &format)
{
    if(format.frequency() == -1)
        return false;

    m_format = format;
    return true;
}

QStringList AudioEncodeControl::supportedAudioCodecs() const
{
    return m_codecs;
}

QString AudioEncodeControl::codecDescription(const QString &codecName)
{
    return m_codecDescriptions.value(codecName);
}

QString AudioEncodeControl::audioCodec() const
{
    return m_codec;
}

bool AudioEncodeControl::setAudioCodec(const QString &codecName)
{
    Q_UNUSED(codecName)

    return true;
}

int AudioEncodeControl::bitrate() const
{
    return m_options.value(QLatin1String("bitrate"), QVariant(int(-1))).toInt();
    return 0;
}

void AudioEncodeControl::setBitrate(int value)
{
    setEncodingOption(QLatin1String("bitrate"), QVariant(value));
}

qreal AudioEncodeControl::quality() const
{
    return m_options.value(QLatin1String("quality"), QVariant(8.0)).toDouble();
}

void AudioEncodeControl::setQuality(qreal value)
{
    setEncodingOption(QLatin1String("quality"), QVariant(value));
}

QStringList AudioEncodeControl::supportedEncodingOptions()
{
    return m_codecOptions.value(m_codec);
}

QVariant AudioEncodeControl::encodingOption(const QString &name)
{
    return m_options.value(name);
}

void AudioEncodeControl::setEncodingOption(const QString &name, const QVariant &value)
{
    m_options.insert(name,value);
}
