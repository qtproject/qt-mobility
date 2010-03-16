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

#include "v4lvideoencode.h"
#include "v4lcamerasession.h"

V4LVideoEncode::V4LVideoEncode(QObject *parent)
    :QVideoEncoderControl(parent)
{
    m_session = qobject_cast<V4LCameraSession*>(parent);

    m_codecs.append("video/mj2");
    m_codecDescriptions.insert("video/mj2",QString("Motion JPEG"));
    m_videoSettings.setCodec(m_codecs[0]);
}

V4LVideoEncode::~V4LVideoEncode()
{
}

QList<QSize> V4LVideoEncode::supportedResolutions(const QVideoEncoderSettings &, bool *continuous) const
{
    if (continuous)
        *continuous = false;

    if(m_session)
        return m_session->supportedResolutions();

    QList<QSize> res;
    return res;
}


QList< qreal > V4LVideoEncode::supportedFrameRates(const QVideoEncoderSettings &, bool *continuous) const
{
    if (continuous)
        *continuous = false;

    QList<qreal> res;
    res << 24.0 << 12.0;
    return res;
}

QStringList V4LVideoEncode::supportedVideoCodecs() const
{
    return m_codecs;
}

QString V4LVideoEncode::videoCodecDescription(const QString &codecName) const
{
    Q_UNUSED(codecName)

    return QString("Motion JPEG");
}

QStringList V4LVideoEncode::supportedEncodingOptions(const QString &codec) const
{
    Q_UNUSED(codec)

    return QStringList();
}

QVariant V4LVideoEncode::encodingOption(const QString &codec, const QString &name) const
{
    Q_UNUSED(codec)
    Q_UNUSED(name)

    return QVariant();
}

void V4LVideoEncode::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    Q_UNUSED(codec)
    Q_UNUSED(name)
    Q_UNUSED(value)
}

QVideoEncoderSettings V4LVideoEncode::videoSettings() const
{
    return m_videoSettings;
}

void V4LVideoEncode::setVideoSettings(const QVideoEncoderSettings &settings)
{
    m_videoSettings = settings;
}

