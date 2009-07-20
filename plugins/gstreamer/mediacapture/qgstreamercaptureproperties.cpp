#include "qgstreamercaptureproperties.h"
#include "qgstreamercapturesession.h"

QGstreamerCaptureProperties::QGstreamerCaptureProperties(QGstreamerCaptureSession *session, QObject *parent)
    :QAudioCapturePropertiesControl(parent), m_session(session)
{
    QList<QByteArray> codecCandidates;
    codecCandidates << "vorbis" << "lame" << "speex" << "ffenc_g726" << "faac" << "ffenc_wmav1" << "ffenc_wmav2";

    foreach( const QByteArray& codecName, codecCandidates ) {
        GstElementFactory *factory = gst_element_factory_find(codecName.constData());
        if (factory) {
            m_codecs.append(codecName);
            gst_object_unref(GST_OBJECT(factory));
        }
    }

    m_codec = m_codecs[0];
}

QGstreamerCaptureProperties::~QGstreamerCaptureProperties()
{
}

QAudioFormat QGstreamerCaptureProperties::format() const
{
    return QAudioFormat();
}

bool QGstreamerCaptureProperties::isFormatSupported(const QAudioFormat &format) const
{
    Q_UNUSED(format);
    return false;
}

bool QGstreamerCaptureProperties::setFormat(const QAudioFormat &format)
{
    Q_UNUSED(format);
    return false;
}

QStringList QGstreamerCaptureProperties::supportedAudioCodecs() const
{
    return m_codecs;
}

QString QGstreamerCaptureProperties::codecDescription(const QString &codecName)
{
    return m_codecDescriptions.value(codecName);
}

bool QGstreamerCaptureProperties::setAudioCodec(const QString &codecName)
{
    m_codec = codecName;
    return true;
}

int QGstreamerCaptureProperties::bitrate() const
{
    return m_options.value(QLatin1String("bitrate"), QVariant(int(-1))).toInt();
}

void QGstreamerCaptureProperties::setBitrate(int value)
{
    setEncodingOption(QLatin1String("bitrate"), QVariant(value));
}

qreal QGstreamerCaptureProperties::quality() const
{
    return m_options.value(QLatin1String("quality"), QVariant(int(-1))).toDouble();
}

void QGstreamerCaptureProperties::setQuality(qreal value)
{
    setEncodingOption(QLatin1String("quality"), QVariant(value));
}

QStringList QGstreamerCaptureProperties::supportedEncodingOptions()
{
    return m_codecOptions.value(m_codec);
}

QVariant QGstreamerCaptureProperties::encodingOption(const QString &name)
{
    return m_options.value(name);
}

void QGstreamerCaptureProperties::setEncodingOption(const QString &name, const QVariant &value)
{
    m_options.insert(name,value);
}
