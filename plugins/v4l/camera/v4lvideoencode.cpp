#include "v4lvideoencode.h"
#include "v4lcamerasession.h"

#include <QtCore/qdebug.h>

V4LVideoEncode::V4LVideoEncode(QObject *parent)
    :QVideoEncodeControl(parent)
{
    m_session = qobject_cast<V4LCameraSession*>(parent);

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

V4LVideoEncode::~V4LVideoEncode()
{
}

QSize V4LVideoEncode::resolution() const
{
    return m_session->frameSize();
}

QSize V4LVideoEncode::minimumResolution() const
{
    QSize minimumSize;
    QList<QSize> sizes = m_session->supportedResolutions();

    minimumSize = sizes.first();

    for(int i=0;i<sizes.size();i++) {
        if(sizes.at(i).width() < minimumSize.width())
            minimumSize = sizes.at(i);
    }

    return minimumSize;
}

QSize V4LVideoEncode::maximumResolution() const
{
    QSize maxSize;
    QList<QSize> sizes = m_session->supportedResolutions();

    maxSize = sizes.first();

    for(int i=0;i<sizes.size();i++) {
        if(sizes.at(i).width() > maxSize.width())
            maxSize = sizes.at(i);
    }

    return maxSize;
}

void V4LVideoEncode::setResolution(const QSize &r)
{
    m_resolution = r;
    m_session->setFrameSize(r);
}

QMediaRecorder::FrameRate V4LVideoEncode::frameRate() const
{
    return m_frameRate;
}

QMediaRecorder::FrameRate V4LVideoEncode::minimumFrameRate() const
{
    return qMakePair<int,int>(1,1);
}

QMediaRecorder::FrameRate V4LVideoEncode::maximumFrameRate() const
{
    return qMakePair<int,int>(1024,1);
}

void V4LVideoEncode::setFrameRate(const QMediaRecorder::FrameRate& rate)
{
    Q_UNUSED(rate)
}

QList< QMediaRecorder::FrameRate > V4LVideoEncode::supportedFrameRates() const
{
    QList<QMediaRecorder::FrameRate> res;
    res << qMakePair<int,int>(25,1);
    return res;
}


QStringList V4LVideoEncode::supportedVideoCodecs() const
{
    return m_codecs;
}

QString V4LVideoEncode::videoCodecDescription(const QString &codecName) const
{
    return m_codecDescriptions.value(codecName);
}

QString V4LVideoEncode::audioCodec() const
{
    return m_codec;
}

bool V4LVideoEncode::setVideoCodec(const QString &codecName)
{
    //TODO
    return true;
}

QString V4LVideoEncode::videoCodec() const
{
    return m_codec;
}

int V4LVideoEncode::bitrate() const
{
    return m_options.value(QLatin1String("bitrate"), QVariant(int(-1))).toInt();
}

void V4LVideoEncode::setBitrate(int value)
{
    setEncodingOption(QLatin1String("bitrate"), QVariant(value));
}

qreal V4LVideoEncode::quality() const
{
    return m_options.value(QLatin1String("quality"), QVariant(8.0)).toDouble();
}

void V4LVideoEncode::setQuality(qreal value)
{
    setEncodingOption(QLatin1String("quality"), QVariant(value));
}

QStringList V4LVideoEncode::supportedEncodingOptions()
{
    return m_codecOptions.value(m_codec);
}

QVariant V4LVideoEncode::encodingOption(const QString &name)
{
    return m_options.value(name);
}

void V4LVideoEncode::setEncodingOption(const QString &name, const QVariant &value)
{
    m_options.insert(name,value);
}
