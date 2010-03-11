/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgstreamervideoencode_maemo.h"
#include "qgstreamercapturesession_maemo.h"
#include "qgstreamermediacontainercontrol_maemo.h"

#include <QtCore/qdebug.h>

QGstreamerVideoEncode::QGstreamerVideoEncode(QGstreamerCaptureSession *session)
    :QVideoEncoderControl(session), m_session(session)
{
    QList<QByteArray> codecCandidates;
    codecCandidates << "video/mpeg4"; //"video/h264" << "video/xvid" << "video/mpeg4" << "video/mpeg1" << "video/mpeg2" << "video/theora";

    m_elementNames["video/mpeg4"] = "dspmp4venc";

    m_codecOptions["video/mpeg4"] = QStringList() << "quantizer";

    foreach( const QByteArray& codecName, codecCandidates ) {
        QByteArray elementName = m_elementNames[codecName];
        GstElementFactory *factory = gst_element_factory_find(elementName.constData());
        if (factory) {
            m_codecs.append(codecName);
            const gchar *descr = gst_element_factory_get_description(factory);
            m_codecDescriptions.insert(codecName, QString::fromUtf8(descr));

            m_streamTypes.insert(codecName,
                                 QGstreamerMediaContainerControl::supportedStreamTypes(factory, GST_PAD_SRC));

            gst_object_unref(GST_OBJECT(factory));
        }
    }

    if (!m_codecs.isEmpty())
        m_videoSettings.setCodec(m_codecs[0]);
}

QGstreamerVideoEncode::~QGstreamerVideoEncode()
{
}

QList<QSize> QGstreamerVideoEncode::supportedResolutions(const QVideoEncoderSettings &, bool *continuous) const
{
    if (continuous)
        *continuous = m_session->videoInput() != 0;

    return m_session->videoInput() ? m_session->videoInput()->supportedResolutions() : QList<QSize>();
}

QList< qreal > QGstreamerVideoEncode::supportedFrameRates(const QVideoEncoderSettings &, bool *continuous) const
{
    if (continuous)
        *continuous = false;

    return m_session->videoInput() ? m_session->videoInput()->supportedFrameRates() : QList<qreal>();
}

QStringList QGstreamerVideoEncode::supportedVideoCodecs() const
{
    return m_codecs;
}

QString QGstreamerVideoEncode::videoCodecDescription(const QString &codecName) const
{
    return m_codecDescriptions.value(codecName);
}

QStringList QGstreamerVideoEncode::supportedEncodingOptions(const QString &codec) const
{
    return m_codecOptions.value(codec);
}

QVariant QGstreamerVideoEncode::encodingOption(const QString &codec, const QString &name) const
{
    return m_options[codec].value(name);
}

void QGstreamerVideoEncode::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    m_options[codec][name] = value;
}

QVideoEncoderSettings QGstreamerVideoEncode::videoSettings() const
{
    return m_videoSettings;
}

void QGstreamerVideoEncode::setVideoSettings(const QVideoEncoderSettings &settings)
{
    m_videoSettings = settings;
}

GstElement *QGstreamerVideoEncode::createEncoder()
{
    QString codec = m_videoSettings.codec();

    GstElement *encoderElement = gst_element_factory_make( m_elementNames.value(codec).constData(), "video-encoder");

    return encoderElement;
}

QPair<int,int> QGstreamerVideoEncode::rateAsRational() const
{
    qreal frameRate = m_videoSettings.frameRate();

    if (frameRate > 0.001) {
        //convert to rational number
        QList<int> denumCandidates;
        denumCandidates << 1 << 2 << 3 << 5 << 10 << 1001 << 1000;

        qreal error = 1.0;
        int num = 1;
        int denum = 1;

        foreach (int curDenum, denumCandidates) {
            int curNum = qRound(frameRate*curDenum);
            qreal curError = qAbs(qreal(curNum)/curDenum - frameRate);

            if (curError < error) {
                error = curError;
                num = curNum;
                denum = curDenum;
            }

            if (curError < 1e-8)
                break;
        }

        return QPair<int,int>(num,denum);
    }

    return QPair<int,int>();
}


QSet<QString> QGstreamerVideoEncode::supportedStreamTypes(const QString &codecName) const
{
    return m_streamTypes.value(codecName);
}
