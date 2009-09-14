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

#include "qgstreamervideoencode.h"
#include "qgstreamercapturesession.h"

#include <QtCore/qdebug.h>

#include <math.h>

QGstreamerVideoEncode::QGstreamerVideoEncode(QObject *parent)
    :QVideoEncoderControl(parent)
{
    m_frameRate = qMakePair<int,int>(-1,1);

    QList<QByteArray> codecCandidates;
    codecCandidates << "video/h264" << "video/xvid" << "video/mpeg4" << "video/mpeg1" << "video/mpeg2" << "video/theora";

    m_elementNames["video/h264"] = "x264enc";
    m_elementNames["video/xvid"] = "xvidenc";
    m_elementNames["video/mpeg4"] = "ffenc_mpeg4";
    m_elementNames["video/mpeg1"] = "ffenc_mpeg1video";
    m_elementNames["video/mpeg2"] = "ffenc_mpeg2video";
    m_elementNames["video/theora"] = "theoraenc";

    m_codecOptions["video/h264"] = QStringList() << "quality" << "bitrate" << "quantizer";
    m_codecOptions["video/xvid"] = QStringList() << "quality" << "bitrate" << "quantizer" << "profile";
    m_codecOptions["video/mpeg4"] = QStringList() << "quality" << "bitrate" << "quantizer";
    m_codecOptions["video/mpeg1"] = QStringList() << "quality" << "bitrate" << "quantizer";
    m_codecOptions["video/mpeg2"] = QStringList() << "quality" << "bitrate" << "quantizer";
    m_codecOptions["video/theora"] = QStringList() << "quality" << "bitrate";

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
        setVideoCodec(m_codecs[0]);
}

QGstreamerVideoEncode::~QGstreamerVideoEncode()
{
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

QList<QSize> QGstreamerVideoEncode::supportedResolutions() const
{
    QList<QSize> res;
    res << QSize(160, 120);
    res << QSize(320, 240);
    res << QSize(640, 480);
    res << QSize(800, 600);
    res << QSize(960, 720);
    res << QSize(1600, 1200);

    return res;
}

void QGstreamerVideoEncode::setResolution(const QSize &r)
{
    m_resolution = r;
}

QMediaRecorder::FrameRate QGstreamerVideoEncode::frameRate() const
{
    return m_frameRate;
}

QMediaRecorder::FrameRate QGstreamerVideoEncode::minimumFrameRate() const
{
    return qMakePair<int,int>(1,1);
}

QMediaRecorder::FrameRate QGstreamerVideoEncode::maximumFrameRate() const
{
    return qMakePair<int,int>(30,1);
}

QList< QMediaRecorder::FrameRate > QGstreamerVideoEncode::supportedFrameRates() const
{
    QList<QMediaRecorder::FrameRate> res;
    res << qMakePair<int,int>(30,1);
    res << qMakePair<int,int>(25,1);
    res << qMakePair<int,int>(20,1);
    res << qMakePair<int,int>(15,1);
    res << qMakePair<int,int>(10,1);
    res << qMakePair<int,int>(5,1);

    return res;
}

void QGstreamerVideoEncode::setFrameRate(const QMediaRecorder::FrameRate &rate)
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
    m_codec = codecName;
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
    return m_options.value(QLatin1String("quality"), QVariant(50.0)).toDouble();
}

void QGstreamerVideoEncode::setQuality(qreal value)
{
    setEncodingOption(QLatin1String("quality"), QVariant(value));
}

QStringList QGstreamerVideoEncode::supportedEncodingOptions() const
{
    return m_codecOptions.value(m_codec);
}

QVariant QGstreamerVideoEncode::encodingOption(const QString &name) const
{
    return m_options.value(name);
}

void QGstreamerVideoEncode::setEncodingOption(const QString &name, const QVariant &value)
{
    m_options.insert(name,value);
}

GstElement *QGstreamerVideoEncode::createEncoder()
{
    GstBin *encoderBin = GST_BIN(gst_bin_new("video-encoder-bin"));
    Q_ASSERT(encoderBin);

    GstElement *capsFilter = gst_element_factory_make("capsfilter", "capsfilter-video");
    gst_bin_add(encoderBin, capsFilter);

    GstElement *colorspace = gst_element_factory_make("ffmpegcolorspace", NULL);
    gst_bin_add(encoderBin, colorspace);

    qDebug() << "create encoder for video codec" << m_codec;

    GstElement *encoderElement = gst_element_factory_make( m_elementNames.value(m_codec).constData(), "video-encoder");
    gst_bin_add(encoderBin, encoderElement);

    gst_element_link_many(capsFilter, colorspace, encoderElement, NULL);

    // add ghostpads
    GstPad *pad = gst_element_get_static_pad(capsFilter, "sink");
    gst_element_add_pad(GST_ELEMENT(encoderBin), gst_ghost_pad_new("sink", pad));
    gst_object_unref(GST_OBJECT(pad));

    pad = gst_element_get_static_pad(encoderElement, "src");
    gst_element_add_pad(GST_ELEMENT(encoderBin), gst_ghost_pad_new("src", pad));
    gst_object_unref(GST_OBJECT(pad));

    if (encoderElement) {
        QMapIterator<QString,QVariant> it(m_options);
        while (it.hasNext()) {
            it.next();
            QString option = it.key();
            QVariant value = it.value();

            if (option == QLatin1String("quality")) {
                double qualityValue = value.toDouble();

                if (m_codec == QLatin1String("video/h264")) {
                    //qant(0) = 50, quant(50) = 21, quant(100) = 1
                    int quant = qRound(50-49*(pow(qualityValue/100.0, 0.7567288)));
                    g_object_set(G_OBJECT(encoderElement), "quantizer", quant, NULL);
                } else if (m_codec == QLatin1String("video/xvid")) {
                    //quant from 2 to 32, default 4

                    int quant = qRound(31-29*(pow(qualityValue/100.0, 0.1)));
                    g_object_set(G_OBJECT(encoderElement), "quantizer", quant, NULL);
                } else if (m_codec == QLatin1String("video/mpeg4") ||
                           m_codec == QLatin1String("video/mpeg1") ||
                           m_codec == QLatin1String("video/mpeg2") ) {
                    //quant from 1 to 30, default ~3
                    double quant = 30.0-29*(pow(qualityValue/100.0, 0.15));
                    g_object_set(G_OBJECT(encoderElement), "quantizer", quant, NULL);
                } else if (m_codec == QLatin1String("video/theora")) {
                    //quality from 0 to 63, default 16
                    int quality = 63*(pow(qualityValue/100.0, 1.977));
                    g_object_set(G_OBJECT(encoderElement), "quality", quality, NULL);
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

    if (!m_resolution.isEmpty() || m_frameRate.first > 0) {
        GstCaps *caps = gst_caps_new_empty();
        QStringList structureTypes;
        structureTypes << "video/x-raw-yuv" << "video/x-raw-rgb";

        foreach(const QString &structureType, structureTypes) {
            GstStructure *structure = gst_structure_new(structureType.toAscii().constData(), NULL);

            if (!m_resolution.isEmpty()) {
                gst_structure_set(structure, "width", G_TYPE_INT, m_resolution.width(), NULL);
                gst_structure_set(structure, "height", G_TYPE_INT, m_resolution.height(), NULL);
            }

            if (m_frameRate.first > 0) {
                gst_structure_set(structure, "framerate", GST_TYPE_FRACTION, m_frameRate.first, m_frameRate.second, NULL);
            }

            gst_caps_append_structure(caps,structure);
        }

        //qDebug() << "set video caps filter:" << gst_caps_to_string(caps);

        g_object_set(G_OBJECT(capsFilter), "caps", caps, NULL);
    }

    return GST_ELEMENT(encoderBin);
}
