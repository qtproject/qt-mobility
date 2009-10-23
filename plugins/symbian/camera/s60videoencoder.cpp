#include "s60videoencoder.h"
#include "s60camerasession.h"

#include <QtCore/qdebug.h>

S60VideoEncoder::S60VideoEncoder(QObject *parent)
    :QVideoEncoderControl(parent)
{
}

S60VideoEncoder::S60VideoEncoder(QObject *session, QObject *parent)
   :QVideoEncoderControl(parent)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60CameraSession*>(session);

    m_frameRate = qMakePair<int,int>(-1,1);
    m_codecs.clear();
    QList<QVideoFrame::PixelFormat> formats = m_session->supportedPixelFormats();
    if(formats.contains(QVideoFrame::Format_YUYV)) {
        m_codecs << "yuyv";
        m_codecDescriptions.insert("yuyv", "yuyv image format");
    }
    if(formats.contains(QVideoFrame::Format_RGB24)) {
        m_codecs << "rgb24";
        m_codecDescriptions.insert("rbg24", "rgb24 image format");
    }
    if(formats.contains(QVideoFrame::Format_RGB565)) {
        m_codecs << "rgb565";
        m_codecDescriptions.insert("rbg565", "rgb565 image format");
    }
    if(formats.contains(QVideoFrame::Format_UYVY)) {
        m_codecs << "uyvy";
        m_codecDescriptions.insert("uyvy", "uyvy image format");
    }
}

S60VideoEncoder::~S60VideoEncoder()
{
}

QSize S60VideoEncoder::resolution() const
{
    return m_session->frameSize();
}

QSize S60VideoEncoder::minimumResolution() const
{
    QSize minimumSize;
    QList<QSize> sizes = m_session->supportedVideoResolutions();

    minimumSize = sizes.first();

    for(int i=0;i<sizes.size();i++) {
        if(sizes.at(i).width() < minimumSize.width())
            minimumSize = sizes.at(i);
    }

    return minimumSize;
}

QSize S60VideoEncoder::maximumResolution() const
{
    QSize maxSize;
    QList<QSize> sizes = m_session->supportedVideoResolutions();

    maxSize = sizes.first();

    for(int i=0;i<sizes.size();i++) {
        if(sizes.at(i).width() > maxSize.width())
            maxSize = sizes.at(i);
    }

    return maxSize;
}

void S60VideoEncoder::setResolution(const QSize &r)
{
    m_resolution = r;
    m_session->setFrameSize(r);
}

QMediaRecorder::FrameRate S60VideoEncoder::frameRate() const
{
    return m_frameRate;
}

QMediaRecorder::FrameRate S60VideoEncoder::minimumFrameRate() const
{
    return qMakePair<int,int>(1,1);
}

QMediaRecorder::FrameRate S60VideoEncoder::maximumFrameRate() const
{
    return qMakePair<int,int>(1024,1);
}

void S60VideoEncoder::setFrameRate(const QMediaRecorder::FrameRate& rate)
{
    Q_UNUSED(rate)
}

QList< QMediaRecorder::FrameRate > S60VideoEncoder::supportedFrameRates() const
{
    QList<QMediaRecorder::FrameRate> res;
    res << qMakePair<int,int>(25,1);
    return res;
}


QStringList S60VideoEncoder::supportedVideoCodecs() const
{
    return m_codecs;
}

QString S60VideoEncoder::videoCodecDescription(const QString &codecName) const
{
    return m_codecDescriptions.value(codecName);
}

QString S60VideoEncoder::audioCodec() const
{
    return m_codec;
}

bool S60VideoEncoder::setVideoCodec(const QString &codecName)
{
    Q_UNUSED(codecName);
    //TODO
    return true;
}

QString S60VideoEncoder::videoCodec() const
{
    return m_codec;
}

int S60VideoEncoder::bitrate() const
{
    return m_options.value(QLatin1String("bitrate"), QVariant(int(-1))).toInt();
}

void S60VideoEncoder::setBitrate(int value)
{
    setEncodingOption(QLatin1String("bitrate"), QVariant(value));
}

int S60VideoEncoder::quality() const
{
    return m_options.value(QLatin1String("quality"), QVariant(8.0)).toDouble();
}

void S60VideoEncoder::setQuality(int value)
{
    setEncodingOption(QLatin1String("quality"), QVariant(value));
}

QStringList S60VideoEncoder::supportedEncodingOptions()
{
    return m_codecOptions.value(m_codec);
}

QVariant S60VideoEncoder::encodingOption(const QString &name)
{
    return m_options.value(name);
}

void S60VideoEncoder::setEncodingOption(const QString &name, const QVariant &value)
{
    m_options.insert(name,value);
}
