#include "audioencode.h"
#include "audiocapturesession.h"

#include <QtMultimedia/qaudioformat.h>

#include <QtCore/qdebug.h>

AudioEncode::AudioEncode(QObject *parent)
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

AudioEncode::~AudioEncode()
{
}

QAudioFormat AudioEncode::format() const
{
    return m_format;
}

bool AudioEncode::isFormatSupported(const QAudioFormat &format) const
{
    Q_UNUSED(format);
    return true;
}

bool AudioEncode::setFormat(const QAudioFormat &format)
{
    if(format.frequency() == -1)
        return false;

    m_format = format;
    return true;
}

QStringList AudioEncode::supportedAudioCodecs() const
{
    return m_codecs;
}

QString AudioEncode::codecDescription(const QString &codecName)
{
    return m_codecDescriptions.value(codecName);
}

QString AudioEncode::audioCodec() const
{
    return m_codec;
}

bool AudioEncode::setAudioCodec(const QString &codecName)
{
    Q_UNUSED(codecName)

    return true;
}

int AudioEncode::bitrate() const
{
    return m_options.value(QLatin1String("bitrate"), QVariant(int(-1))).toInt();
    return 0;
}

void AudioEncode::setBitrate(int value)
{
    setEncodingOption(QLatin1String("bitrate"), QVariant(value));
}

qreal AudioEncode::quality() const
{
    return m_options.value(QLatin1String("quality"), QVariant(8.0)).toDouble();
}

void AudioEncode::setQuality(qreal value)
{
    setEncodingOption(QLatin1String("quality"), QVariant(value));
}

QStringList AudioEncode::supportedEncodingOptions()
{
    return m_codecOptions.value(m_codec);
}

QVariant AudioEncode::encodingOption(const QString &name)
{
    return m_options.value(name);
}

void AudioEncode::setEncodingOption(const QString &name, const QVariant &value)
{
    m_options.insert(name,value);
}
