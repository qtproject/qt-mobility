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
    :QAudioEncoderControl(parent)
{
    QList<QByteArray> codecCandidates;
    codecCandidates << "mp3" << "vorbis" << "speex" << "gsm";

    m_elementNames["mp3"] = "lame";
    m_elementNames["vorbis"] = "vorbisenc";
    m_elementNames["speex"] = "speexenc";
    m_elementNames["gsm"] = "gsmenc";


    m_codecOptions["vorbis"] = QStringList() << "quality" << "bitrate" << "min-bitrate" << "max-bitrate";
    m_codecOptions["mp3"] = QStringList() << "quality" << "bitrate" << "mode" << "vbr";
    m_codecOptions["speex"] = QStringList() << "quality" << "bitrate" << "mode" << "vbr" << "vad" << "dtx";
    m_codecOptions["gsm"] = QStringList();

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
    return m_options.value(QLatin1String("quality"), QVariant(50.0)).toDouble();
}

void QGstreamerAudioEncode::setQuality(qreal value)
{
    setEncodingOption(QLatin1String("quality"), QVariant(value));
}

QStringList QGstreamerAudioEncode::supportedEncodingOptions() const
{
    return m_codecOptions.value(m_codec);
}

QVariant QGstreamerAudioEncode::encodingOption(const QString &name) const
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

                if (m_codec == QLatin1String("vorbis")) {
                    g_object_set(G_OBJECT(encoderElement), "quality", qualityValue/10.0, NULL);
                } else if (m_codec == QLatin1String("mp3")) {
                    int preset = 1006; // Medium
                    if (qualityValue > 40)
                        preset = 1001; // Standard
                    if (qualityValue > 60)
                        preset = 1002; // Extreme
                    if (qualityValue > 80 )
                        preset = 1003; // Insane
                    g_object_set(G_OBJECT(encoderElement), "preset", preset, NULL);
                } else if (m_codec == QLatin1String("speex")) {
                    //0-10 range with default 8
                    double q = 10.0*pow(qualityValue/100.0, 0.32);
                    g_object_set(G_OBJECT(encoderElement), "quality", q, NULL);
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
