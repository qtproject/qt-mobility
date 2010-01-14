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

qreal S60VideoEncoder::minimumFrameRate() const
{
    return 5.0;
}

qreal S60VideoEncoder::maximumFrameRate() const
{
    return 30.0;
}

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

