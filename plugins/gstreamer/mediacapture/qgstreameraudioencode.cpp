#include "qgstreameraudioencode.h"
#include "qgstreamercapturesession.h"

#include <QtCore/qdebug.h>

QGstreamerAudioEncode::QGstreamerAudioEncode(QObject *parent)
    :QAudioEncodeControl(parent)
{
    QList<QByteArray> codecCandidates;
    codecCandidates << "speexenc" << "lame" << "vorbisenc" << "gsmenc";

    m_codecOptions["vorbis"] = QStringList() << "quality" << "bitrate" << "min-bitrate" << "max-bitrate";
    m_codecOptions["lame"] = QStringList() << "quality" << "bitrate" << "mode" << "vbr";
    m_codecOptions["speexenc"] = QStringList() << "quality" << "bitrate" << "mode" << "vbr" << "vad" << "dtx";
    m_codecOptions["gsmenc"] = QStringList();

    foreach( const QByteArray& codecName, codecCandidates ) {
        GstElementFactory *factory = gst_element_factory_find(codecName.constData());
        if (factory) {
            m_codecs.append(codecName);
            const gchar *descr = gst_element_factory_get_description(factory);
            m_codecDescriptions.insert(codecName, QString::fromUtf8(descr));

            gst_object_unref(GST_OBJECT(factory));
        }
    }

    if (!m_codecs.isEmpty())
        setAudioCodec(m_codecs[0]);
}

QGstreamerAudioEncode::~QGstreamerAudioEncode()
{
}

QAudioFormat QGstreamerAudioEncode::format() const
{
    return m_audioFormat;
}

bool QGstreamerAudioEncode::isFormatSupported(const QAudioFormat &format) const
{
    Q_UNUSED(format);
    return true;
}

bool QGstreamerAudioEncode::setFormat(const QAudioFormat &format)
{
    m_audioFormat = format;
    return true;
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
    m_codec = codecName;
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
    return m_options.value(QLatin1String("quality"), QVariant(8.0)).toDouble();
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

GstElement *QGstreamerAudioEncode::createEncoder()
{
    GstBin * encoderBin = GST_BIN(gst_bin_new("audio-encoder-bin"));
    Q_ASSERT(encoderBin);

    GstElement *capsFilter = gst_element_factory_make("capsfilter", NULL);
    GstElement *encoderElement = gst_element_factory_make(m_codec.toAscii(), NULL);

    Q_ASSERT(encoderElement);

    gst_bin_add(encoderBin, capsFilter);
    gst_bin_add(encoderBin, encoderElement);
    gst_element_link(capsFilter, encoderElement);

    // add ghostpads
    GstPad *pad = gst_element_get_static_pad(capsFilter, "sink");
    gst_element_add_pad(GST_ELEMENT(encoderBin), gst_ghost_pad_new("sink", pad));
    gst_object_unref(GST_OBJECT(pad));

    pad = gst_element_get_static_pad(encoderElement, "src");
    gst_element_add_pad(GST_ELEMENT(encoderBin), gst_ghost_pad_new("src", pad));
    gst_object_unref(GST_OBJECT(pad));

    if (!m_audioFormat.isNull()) {
        GstCaps *caps = gst_caps_new_empty();
        GstStructure *structure = gst_structure_new("audio/x-raw-int", NULL);

        if ( m_audioFormat.frequency() > 0 )
            gst_structure_set(structure, "rate", G_TYPE_INT, m_audioFormat.frequency(), NULL );

        if ( m_audioFormat.channels() > 0 )
            gst_structure_set(structure, "channels", G_TYPE_INT, m_audioFormat.channels(), NULL );

        if ( m_audioFormat.sampleSize() > 0 )
            gst_structure_set(structure, "width", G_TYPE_INT, m_audioFormat.sampleSize(), NULL );


        gst_caps_append_structure(caps,structure);

        qDebug() << "set caps filter:" << gst_caps_to_string(caps);

        g_object_set(G_OBJECT(capsFilter), "caps", caps, NULL);
    }

    if (encoderElement) {
        QMapIterator<QString,QVariant> it(m_options);
        while (it.hasNext()) {
            it.next();
            QString option = it.key();
            QVariant value = it.value();

            if (option == QLatin1String("quality")) {
                double qualityValue = value.toDouble();

                if (m_codec == QLatin1String("vorbisenc")) {
                    g_object_set(G_OBJECT(encoderElement), "quality", qualityValue/10.0, NULL);
                } else if (m_codec == QLatin1String("lame")) {
                    int presets[] = {1006, 1001, 1002, 1003}; //Medium, Standard, Extreme, Insane
                    int preset = presets[ qBound(0, qRound(qualityValue*0.3), 3) ];
                    g_object_set(G_OBJECT(encoderElement), "preset", preset, NULL);
                } else if (m_codec == QLatin1String("speexenc")) {
                    g_object_set(G_OBJECT(encoderElement), "quality", qualityValue, NULL);
                }

            } else {
                switch (value.type()) {
                case QVariant::Int:
                    g_object_set(G_OBJECT(encoderElement), option.toAscii(), value.toInt(), NULL);
                    break;
                case QVariant::Bool:
                    g_object_set(G_OBJECT(encoderElement), option.toAscii(), value.toBool(), NULL);
                    break;
                case QVariant::Double:
                    g_object_set(G_OBJECT(encoderElement), option.toAscii(), value.toDouble(), NULL);
                    break;
                case QVariant::String:
                    g_object_set(G_OBJECT(encoderElement), option.toAscii(), value.toString().toUtf8().constData(), NULL);
                    break;
                default:
                    qWarning() << "unsupported option type:" << option << value;
                    break;
                }
            }
        }
    }

    return GST_ELEMENT(encoderBin);
}
