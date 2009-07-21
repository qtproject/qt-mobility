#include "qgstreameraudioencode.h"
#include "qgstreamercapturesession.h"

QGstreamerAudioEncode::QGstreamerAudioEncode(QObject *parent)
    :QAudioEncodeControl(parent)
{
    QList<QByteArray> codecCandidates;
    codecCandidates << "vorbisenc" << "lame" << "speexenc" << "gsmenc";

    m_codecOptions["vorbis"] = QStringList() << "quality" << "bitrate" << "min-bitrate" << "max-bitrate";
    m_codecOptions["lame"] = QStringList() << "quality" << "bitrate" << "mode" << "vbr";
    m_codecOptions["speexenc"] = QStringList() << "quality" << "bitrate" << "mode" << "vbr" << "vad" << "dtx";
    m_codecOptions["gsmenc"] = QStringList();    

    m_muxers["vorbisenc"] = "oggmux";
    m_muxers["speexenc"] = "oggmux";

    foreach( const QByteArray& codecName, codecCandidates ) {
        GstElementFactory *factory = gst_element_factory_find(codecName.constData());
        if (factory) {
            m_codecs.append(codecName);
            const gchar *descr = gst_element_factory_get_description(factory);            
            m_codecDescriptions.insert(codecName, QString::fromUtf8(descr));

            gst_object_unref(GST_OBJECT(factory));
        }
    }

    m_encoderBin = GST_BIN(gst_bin_new("audio-encoder-bin"));
    Q_ASSERT(m_encoderBin);
    gst_object_ref(GST_OBJECT(m_encoderBin)); //Take ownership
    gst_object_sink(GST_OBJECT(m_encoderBin));

    m_identity1 = gst_element_factory_make("identity", NULL);
    gst_object_ref(GST_OBJECT(m_identity1));
    gst_object_sink(GST_OBJECT(m_identity1));
    gst_bin_add(m_encoderBin, m_identity1);

    m_identity2 = gst_element_factory_make("identity", NULL);
    gst_object_ref(GST_OBJECT(m_identity2));
    gst_object_sink(GST_OBJECT(m_identity2));
    gst_bin_add(m_encoderBin, m_identity2);

    // add ghostpads
    GstPad *pad = gst_element_get_static_pad(m_identity1, "sink");
    gst_element_add_pad(GST_ELEMENT(m_encoderBin), gst_ghost_pad_new("sink", pad));
    gst_object_unref(GST_OBJECT(pad));

    pad = gst_element_get_static_pad(m_identity2, "src");
    gst_element_add_pad(GST_ELEMENT(m_encoderBin), gst_ghost_pad_new("src", pad));
    gst_object_unref(GST_OBJECT(pad));

    m_encoderElement = 0;
    m_muxerElement = 0;

    if (!m_codecs.isEmpty())
        setAudioCodec(m_codecs[0]);
}

QGstreamerAudioEncode::~QGstreamerAudioEncode()
{
    gst_object_unref(m_encoderBin);
}

QAudioFormat QGstreamerAudioEncode::format() const
{
    return QAudioFormat();
}

bool QGstreamerAudioEncode::isFormatSupported(const QAudioFormat &format) const
{
    Q_UNUSED(format);
    return false;
}

bool QGstreamerAudioEncode::setFormat(const QAudioFormat &format)
{
    Q_UNUSED(format);
    return false;
}

QStringList QGstreamerAudioEncode::supportedAudioCodecs() const
{
    return m_codecs;
}

QString QGstreamerAudioEncode::codecDescription(const QString &codecName)
{
    return m_codecDescriptions.value(codecName);
}

QString QGstreamerAudioEncode::audioCodec() const
{
    return m_codec;
}

bool QGstreamerAudioEncode::setAudioCodec(const QString &codecName)
{
    if (m_codec != codecName) {
        m_codec = codecName;
        m_options.clear();

        if (m_encoderElement) {
            gst_element_unlink(m_identity1, m_encoderElement);
            gst_element_unlink(m_encoderElement, m_muxerElement);

            gst_bin_remove(m_encoderBin, m_encoderElement);
            gst_object_unref(m_encoderElement);
            m_encoderElement = 0;

            gst_element_unlink(m_muxerElement, m_identity2);
            gst_bin_remove(m_encoderBin, m_muxerElement);
            gst_object_unref(m_muxerElement);
            m_muxerElement = 0;
        }

        m_encoderElement = gst_element_factory_make(codecName.toAscii(), "encoder");
        m_muxerElement = gst_element_factory_make( m_muxers.value(codecName, "identity").toAscii(), "muxer");
        gst_bin_add(m_encoderBin, m_encoderElement);
        gst_bin_add(m_encoderBin, m_muxerElement);

        gst_element_link_many(m_identity1, m_encoderElement, m_muxerElement, m_identity2, NULL);
    }

    return true;
}

int QGstreamerAudioEncode::bitrate() const
{
    return m_options.value(QLatin1String("bitrate"), QVariant(int(-1))).toInt();
}

void QGstreamerAudioEncode::setBitrate(int value)
{
    setEncodingOption(QLatin1String("bitrate"), QVariant(value));
}

qreal QGstreamerAudioEncode::quality() const
{
    return m_options.value(QLatin1String("quality"), QVariant(int(-1))).toDouble();
}

void QGstreamerAudioEncode::setQuality(qreal value)
{
    setEncodingOption(QLatin1String("quality"), QVariant(value));
}

QStringList QGstreamerAudioEncode::supportedEncodingOptions()
{
    return m_codecOptions.value(m_codec);
}

QVariant QGstreamerAudioEncode::encodingOption(const QString &name)
{
    return m_options.value(name);
}

void QGstreamerAudioEncode::setEncodingOption(const QString &name, const QVariant &value)
{
    m_options.insert(name,value);
}

void QGstreamerAudioEncode::applyOptions()
{
    if (m_encoderElement) {
    }
}

GstElement *QGstreamerAudioEncode::encoder()
{
    return GST_ELEMENT(m_encoderBin);
}
