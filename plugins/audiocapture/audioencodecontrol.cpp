#include "audioencodecontrol.h"
#include "audiocapturesession.h"

#include <QtMultimedia/qaudioformat.h>

#include <QtCore/qdebug.h>

AudioEncodeControl::AudioEncodeControl(QObject *parent)
    :QAudioEncodeControl(parent)
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

QAudioFormat AudioEncodeControl::format() const
{
    return m_session->format();
}

bool AudioEncodeControl::isFormatSupported(const QAudioFormat &format) const
{
    return m_session->isFormatSupported(format);
}

bool AudioEncodeControl::setFormat(const QAudioFormat &format)
{
    return m_session->setFormat(format);
}

QStringList AudioEncodeControl::supportedAudioCodecs() const
{
    return m_session->supportedAudioCodecs();
}

QString AudioEncodeControl::codecDescription(const QString &codecName)
{
    if(qstrcmp(codecName.toLocal8Bit().constData(),"audio/x-wav") == 0)
        return QString("wav file format");

    return QString();
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

int AudioEncodeControl::bitrate() const
{
    return (m_session->format().frequency()*m_session->format().channels()*(m_session->format().sampleSize()/8));
}

void AudioEncodeControl::setBitrate(int value)
{

}

qreal AudioEncodeControl::quality() const
{
    return 0;
}

void AudioEncodeControl::setQuality(qreal value)
{
}

QStringList AudioEncodeControl::supportedEncodingOptions()
{
    QStringList options;
    options << "bitrate";
    return options;
}

QVariant AudioEncodeControl::encodingOption(const QString &name)
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



