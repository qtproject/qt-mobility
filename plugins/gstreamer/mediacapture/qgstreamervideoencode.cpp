#include "qgstreamervideoencode.h"
#include "qgstreamercapturesession.h"

#include <QtCore/qdebug.h>

QGstreamerVideoEncode::QGstreamerVideoEncode(QObject *parent)
    :QVideoEncodeControl(parent)
{
    m_frameRate = qMakePair<int,int>(-1,1);

    QList<QByteArray> codecCandidates;
    codecCandidates << "x264enc" << "xvidenc" << "ffenc_mpeg4" << "ffenc_mpeg1video" << "ffenc_mpeg2video" << "theoraenc";

    m_codecOptions["x264enc"] = QStringList() << "quality" << "bitrate" << "quantizer";
    m_codecOptions["xvidenc"] = QStringList() << "quality" << "bitrate" << "quantizer" << "profile";
    m_codecOptions["ffenc_mpeg4"] = QStringList() << "quality" << "bitrate" << "quantizer";
    m_codecOptions["ffenc_mpeg1video"] = QStringList() << "quality" << "bitrate" << "quantizer";
    m_codecOptions["ffenc_mpeg2video"] = QStringList() << "quality" << "bitrate" << "quantizer";
    m_codecOptions["theoraenc"] = QStringList() << "quality" << "bitrate";

    foreach( const QByteArray& codecName, codecCandidates ) {
        GstElementFactory *factory = gst_element_factory_find(codecName.constData());
        if (factory) {
            m_codecs.append(codecName);
            const gchar *descr = gst_element_factory_get_description(factory);
            m_codecDescriptions.insert(codecName, QString::fromUtf8(descr));

            gst_object_unref(GST_OBJECT(factory));
        }
    }

    m_encoderBin = GST_BIN(gst_bin_new("video-encoder-bin"));
    Q_ASSERT(m_encoderBin);
    gst_object_ref(GST_OBJECT(m_encoderBin)); //Take ownership
    gst_object_sink(GST_OBJECT(m_encoderBin));

    m_capsfilter = gst_element_factory_make("capsfilter-video", NULL);
    gst_object_ref(GST_OBJECT(m_capsfilter));
    gst_object_sink(GST_OBJECT(m_capsfilter));
    gst_bin_add(m_encoderBin, m_capsfilter);

    m_identity = gst_element_factory_make("identity-video", NULL);
    gst_object_ref(GST_OBJECT(m_identity));
    gst_object_sink(GST_OBJECT(m_identity));
    gst_bin_add(m_encoderBin, m_identity);

    // add ghostpads
    GstPad *pad = gst_element_get_static_pad(m_capsfilter, "sink");
    gst_element_add_pad(GST_ELEMENT(m_encoderBin), gst_ghost_pad_new("sink", pad));
    gst_object_unref(GST_OBJECT(pad));

    pad = gst_element_get_static_pad(m_identity, "src");
    gst_element_add_pad(GST_ELEMENT(m_encoderBin), gst_ghost_pad_new("src", pad));
    gst_object_unref(GST_OBJECT(pad));

    m_encoderElement = 0;

    if (!m_codecs.isEmpty())
        setVideoCodec(m_codecs[0]);
}

QGstreamerVideoEncode::~QGstreamerVideoEncode()
{
    gst_object_unref(m_encoderBin);
}

QSize QGstreamerVideoEncode::resolution() const
{
    return m_resolution;
}

QSize QGstreamerVideoEncode::minimumResolution() const
{
    return QSize(16,16);
}

QSize QGstreamerVideoEncode::maximumResolution() const
{
    return QSize(4096,4096);
}

void QGstreamerVideoEncode::setResolution(const QSize &r)
{
    m_resolution = r;
}

QPair<int,int> QGstreamerVideoEncode::frameRate() const
{
    return m_frameRate;
}

QPair<int,int> QGstreamerVideoEncode::minumumFrameRate() const
{
    return qMakePair<int,int>(1,1);
}

QPair<int,int> QGstreamerVideoEncode::maximumFrameRate() const
{
    return qMakePair<int,int>(1024,1);
}

void QGstreamerVideoEncode::setFrameRate(QPair<int,int> rate)
{
    m_frameRate = rate;
}


QStringList QGstreamerVideoEncode::supportedVideoCodecs() const
{
    return m_codecs;
}

QString QGstreamerVideoEncode::videoCodecDescription(const QString &codecName) const
{
    return m_codecDescriptions.value(codecName);
}

QString QGstreamerVideoEncode::audioCodec() const
{
    return m_codec;
}

bool QGstreamerVideoEncode::setVideoCodec(const QString &codecName)
{
    if (m_codec != codecName) {
        m_codec = codecName;

        if (m_encoderElement) {
            gst_element_unlink(m_capsfilter, m_encoderElement);
            gst_element_unlink(m_encoderElement, m_identity);

            gst_bin_remove(m_encoderBin, m_encoderElement);
            gst_object_unref(m_encoderElement);
            m_encoderElement = 0;
        }

        m_encoderElement = gst_element_factory_make(codecName.toAscii(), "encoder");
        gst_bin_add(m_encoderBin, m_encoderElement);
        gst_element_link_many(m_capsfilter, m_encoderElement, m_identity, NULL);
    }

    return true;
}

QString QGstreamerVideoEncode::videoCodec() const
{
    return m_codec;
}

int QGstreamerVideoEncode::bitrate() const
{
    return m_options.value(QLatin1String("bitrate"), QVariant(int(-1))).toInt();
}

void QGstreamerVideoEncode::setBitrate(int value)
{
    setEncodingOption(QLatin1String("bitrate"), QVariant(value));
}

qreal QGstreamerVideoEncode::quality() const
{
    return m_options.value(QLatin1String("quality"), QVariant(8.0)).toDouble();
}

void QGstreamerVideoEncode::setQuality(qreal value)
{
    setEncodingOption(QLatin1String("quality"), QVariant(value));
}

QStringList QGstreamerVideoEncode::supportedEncodingOptions()
{
    return m_codecOptions.value(m_codec);
}

QVariant QGstreamerVideoEncode::encodingOption(const QString &name)
{
    return m_options.value(name);
}

void QGstreamerVideoEncode::setEncodingOption(const QString &name, const QVariant &value)
{
    m_options.insert(name,value);
}

void QGstreamerVideoEncode::applyOptions()
{
    if (m_encoderElement) {
        QMapIterator<QString,QVariant> it(m_options);
        while (it.hasNext()) {
            it.next();
            QString option = it.key();
            QVariant value = it.value();

            if (option == QLatin1String("quality")) {
                double qualityValue = value.toDouble();

                if (m_codec == QLatin1String("vorbisenc")) {
                    g_object_set(G_OBJECT(m_encoderElement), "quality", qualityValue/10.0, NULL);
                } else if (m_codec == QLatin1String("lame")) {
                    int presets[] = {1006, 1001, 1002, 1003}; //Medium, Standard, Extreme, Insane
                    int preset = presets[ qBound(0, qRound(qualityValue*0.3), 3) ];
                    g_object_set(G_OBJECT(m_encoderElement), "preset", preset, NULL);
                } else if (m_codec == QLatin1String("speexenc")) {
                    g_object_set(G_OBJECT(m_encoderElement), "quality", qualityValue, NULL);
                }

            } else {
                switch (value.type()) {
                case QVariant::Int:
                    g_object_set(G_OBJECT(m_encoderElement), option.toAscii(), value.toInt(), NULL);
                    break;
                case QVariant::Bool:
                    g_object_set(G_OBJECT(m_encoderElement), option.toAscii(), value.toBool(), NULL);
                    break;
                case QVariant::Double:
                    g_object_set(G_OBJECT(m_encoderElement), option.toAscii(), value.toDouble(), NULL);
                    break;
                case QVariant::String:
                    g_object_set(G_OBJECT(m_encoderElement), option.toAscii(), value.toString().toUtf8().constData(), NULL);
                    break;
                default:
                    qWarning() << "unsupported option type:" << option << value;
                    break;
                }
            }
        }
    }
}

GstElement *QGstreamerVideoEncode::encoder()
{
    return GST_ELEMENT(m_encoderBin);
}

