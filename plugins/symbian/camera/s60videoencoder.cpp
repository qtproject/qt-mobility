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

    m_frameRate = 25.0;
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

//QSize S60VideoEncoder::resolution() const
//{
//    return m_session->frameSize();
//}

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
QList<QSize> S60VideoEncoder::supportedResolutions() const
{
    QList<QSize> res;
    res << QSize(160, 120);
    res << QSize(320, 240);

    return res;

}
//void S60VideoEncoder::setResolution(const QSize &r)
//{
//    m_resolution = r;
//    m_session->setFrameSize(r);
//}
//
//qreal S60VideoEncoder::frameRate() const
//{
//    return m_frameRate;
//}

qreal S60VideoEncoder::minimumFrameRate() const
{
    return 5.0;
}

qreal S60VideoEncoder::maximumFrameRate() const
{
    return 30.0;
}

//void S60VideoEncoder::setFrameRate(qreal &rate)
//{
//    Q_UNUSED(rate)
//}

QList< qreal > S60VideoEncoder::supportedFrameRates() const
{
    QList<qreal> res;
    res << 30.0 << 25.0 << 15.0 << 10.0 << 5.0;
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

//QString S60VideoEncoder::audioCodec() const
//{
//    return m_codec;
//}
//
//bool S60VideoEncoder::setVideoCodec(const QString &codecName)
//{
//    Q_UNUSED(codecName);
//    //TODO
//    return true;
//}
//
//QString S60VideoEncoder::videoCodec() const
//{
//    return m_codec;
//}
//
//int S60VideoEncoder::bitrate() const
//{
//    return m_options.value(QLatin1String("bitrate"), QVariant(int(-1))).toInt();
//}
//
//void S60VideoEncoder::setBitrate(int value)
//{
//    setEncodingOption(QLatin1String("bitrate"), QVariant(value));
//}
//
//int S60VideoEncoder::quality() const
//{
//    return m_options.value(QLatin1String("quality"), QVariant(8.0)).toDouble();
//}
//
//void S60VideoEncoder::setQuality(int value)
//{
//    setEncodingOption(QLatin1String("quality"), QVariant(value));
//}

QStringList S60VideoEncoder::supportedEncodingOptions(const QString &codec) const
{
    return m_codecOptions.value(codec);
}

QVariant S60VideoEncoder::encodingOption(const QString &codec, const QString &name) const
{
    return m_options[codec].value(name);
}

void S60VideoEncoder::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    m_options[codec][name] = value;
}

QVideoEncoderSettings S60VideoEncoder::videoSettings() const
{
    return m_videoSettings;
}

void S60VideoEncoder::setVideoSettings(const QVideoEncoderSettings &settings)
{
    m_videoSettings = settings;
}

