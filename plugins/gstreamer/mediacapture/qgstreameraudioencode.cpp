/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgstreameraudioencode.h"
#include "qgstreamercapturesession.h"

#include <QtCore/qdebug.h>

#include <math.h>

QGstreamerAudioEncode::QGstreamerAudioEncode(QObject *parent)
    :QAudioEncoderControl(parent),
    m_sampleRate(-1),
    m_channels(-1),
    m_sampleSize(-1)
{
    QList<QByteArray> codecCandidates;
    codecCandidates << "audio/mpeg" << "audio/vorbis" << "audio/speex" << "audio/GSM";

    m_elementNames["audio/mpeg"] = "lame";
    m_elementNames["audio/vorbis"] = "vorbisenc";
    m_elementNames["audio/speex"] = "speexenc";
    m_elementNames["audio/GSM"] = "gsmenc";


    m_codecOptions["audio/vorbis"] = QStringList() << "quality" << "bitrate" << "min-bitrate" << "max-bitrate";
    m_codecOptions["audio/mpeg"] = QStringList() << "quality" << "bitrate" << "mode" << "vbr";
    m_codecOptions["audio/speex"] = QStringList() << "quality" << "bitrate" << "mode" << "vbr" << "vad" << "dtx";
    m_codecOptions["audio/GSM"] = QStringList();

    foreach( const QByteArray& codecName, codecCandidates ) {
        QByteArray elementName = m_elementNames[codecName];
        GstElementFactory *factory = gst_element_factory_find(elementName.constData());

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

QStringList QGstreamerAudioEncode::supportedAudioCodecs() const
{
    return m_codecs;
}

QString QGstreamerAudioEncode::codecDescription(const QString &codecName) const
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

    //speex is optimised for a limited set of frequencies
    if (codecName == QLatin1String("audio/speex") && m_sampleRate == -1) {
        m_sampleRate = 32000;
    }

    return true;
}

int QGstreamerAudioEncode::bitrate() const
{
    return m_options.value(QLatin1String("bitrate"), QVariant(int(-1))).toInt();
}

void QGstreamerAudioEncode::setBitrate(int value)
{
    setEncodingOption(m_codec, QLatin1String("bitrate"), QVariant(value));
}

QtMedia::EncodingQuality QGstreamerAudioEncode::quality() const
{
    return QtMedia::EncodingQuality(m_options.value(QLatin1String("quality"),
                                                           QVariant(QtMedia::NormalQuality)).toInt());
}

void QGstreamerAudioEncode::setQuality(QtMedia::EncodingQuality value)
{
    setEncodingOption(m_codec, QLatin1String("quality"), QVariant(value));
}

QStringList QGstreamerAudioEncode::supportedEncodingOptions(const QString &codec) const
{
    return m_codecOptions.value(codec);
}

QVariant QGstreamerAudioEncode::encodingOption(
        const QString &codec, const QString &name) const
{
    return codec == m_codec ? m_options.value(name) : QVariant();
}

void QGstreamerAudioEncode::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    if (codec == m_codec)
        m_options.insert(name,value);
}

QList<int> QGstreamerAudioEncode::supportedSampleRates() const
{
    //TODO check element caps to find actual values

    return QList<int>();
}

QList<int> QGstreamerAudioEncode::supportedChannelCounts() const
{
    //TODO check element caps to find actual values

    return QList<int>() << 1 << 2;
}

QList<int> QGstreamerAudioEncode::supportedSampleSizes() const
{
    //TODO check element caps to find actual values

    return QList<int>() << 16;
}


GstElement *QGstreamerAudioEncode::createEncoder()
{
    GstBin * encoderBin = GST_BIN(gst_bin_new("audio-encoder-bin"));
    Q_ASSERT(encoderBin);

    GstElement *capsFilter = gst_element_factory_make("capsfilter", NULL);
    GstElement *encoderElement = gst_element_factory_make(m_elementNames.value(m_codec).constData(), NULL);

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

    if (m_sampleRate > 0 || m_channels > 0 || m_sampleSize > 0) {
        GstCaps *caps = gst_caps_new_empty();
        GstStructure *structure = gst_structure_new("audio/x-raw-int", NULL);

        if ( m_sampleRate > 0 )
            gst_structure_set(structure, "rate", G_TYPE_INT, m_sampleRate, NULL );

        if ( m_channels > 0 )
            gst_structure_set(structure, "channels", G_TYPE_INT, m_channels, NULL );

        if ( m_sampleSize > 0 )
            gst_structure_set(structure, "width", G_TYPE_INT, m_sampleSize, NULL );


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

            //skip the default values
            if (option == QLatin1String("bitrate")) {
                int bitrate = value.toInt();
                if (bitrate <= 0)
                    continue;
            }

            if (option == QLatin1String("quality")) {
                int qualityValue = qBound(int(QtMedia::VeryLowQuality), value.toInt(), int(QtMedia::VeryHighQuality));

                if (m_codec == QLatin1String("audio/vorbis")) {
                    double qualityTable[] = {
                        0.1, //VeryLow
                        0.3, //Low
                        0.5, //Normal
                        0.7, //High
                        1.0 //VeryHigh
                    };
                    g_object_set(G_OBJECT(encoderElement), "quality", qualityTable[qualityValue], NULL);
                } else if (m_codec == QLatin1String("audio/mpeg")) {
                    int presets[] = {
                        1006, //VeryLow - Medium
                        1006, //Low - Medium
                        1001, //Normal - Standard
                        1002, //High - Extreme
                        1003 //VeryHigh - Insane
                    };

                    g_object_set(G_OBJECT(encoderElement), "preset", presets[qualityValue], NULL);
                } else if (m_codec == QLatin1String("audio/speex")) {
                    //0-10 range with default 8
                    double qualityTable[] = {
                        2, //VeryLow
                        5, //Low
                        8, //Normal
                        9, //High
                        10 //VeryHigh
                    };
                    g_object_set(G_OBJECT(encoderElement), "quality", qualityTable[qualityValue], NULL);
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
