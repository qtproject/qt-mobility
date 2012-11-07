/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60videoencodercontrol.h"
#include "s60videocapturesession.h"
#include "s60videocapturesettings.h"

S60VideoEncoderControl::S60VideoEncoderControl(QObject *parent) :
    QVideoEncoderControl(parent)
{
}

S60VideoEncoderControl::S60VideoEncoderControl(S60VideoCaptureSession *session, QObject *parent) :
    QVideoEncoderControl(parent)
{
    m_session = session;
}

S60VideoEncoderControl::~S60VideoEncoderControl()
{
}

QStringList S60VideoEncoderControl::supportedVideoCodecs() const
{
    return m_session->settings()->supportedVideoCodecs();
}

QString S60VideoEncoderControl::videoCodecDescription(const QString &codecName) const
{
    return m_session->settings()->videoCodecDescription(codecName);
}

QList<qreal> S60VideoEncoderControl::supportedFrameRates(const QVideoEncoderSettings &settings, bool *continuous) const
{
    if (!settings.isNull())
        return m_session->settings()->supportedVideoFrameRates(settings, continuous);
    return m_session->settings()->supportedVideoFrameRates(continuous);
}

QList<QSize> S60VideoEncoderControl::supportedResolutions(const QVideoEncoderSettings &settings, bool *continuous) const
{
    if (!settings.isNull())
        return m_session->settings()->supportedVideoResolutions(settings, continuous);
    return m_session->settings()->supportedVideoResolutions(continuous);
}

QStringList S60VideoEncoderControl::supportedEncodingOptions(const QString &codec) const
{
    // Possible settings: EncodingMode, Codec, Resolution, FrameRate, BitRate, Quality
    // Possible (codec specific) options: PixelAspectRatio, MaxClipSizeInBytes

    // Following options are valid for all codecs
    Q_UNUSED(codec);

    QStringList options;
    options << QLatin1String("pixelAspectRatio");
    options << QLatin1String("maxClipSizeInBytes");

    return options;
}

QVariant S60VideoEncoderControl::encodingOption(const QString &codec, const QString &name) const
{
    Q_UNUSED(codec);

    // Possible settings: EncodingMode, Codec, Resolution, FrameRate, BitRate, Quality
    // Possible (codec specific) options: PixelAspectRatio, MaxClipSizeInBytes

    if (qstrcmp(name.toLocal8Bit().constData(), "pixelAspectRatio") == 0)
        return QVariant(m_session->settings()->pixelAspectRatio());
    else if (qstrcmp(name.toLocal8Bit().constData(), "maxClipSizeInBytes") == 0)
        return QVariant(m_session->settings()->maxClipSizeInBytes());

    return QVariant();
}

void S60VideoEncoderControl::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    // Set the codec first if not already set
    m_session->settings()->setVideoCodec(codec);

    if (qstrcmp(name.toLocal8Bit().constData(), "pixelAspectRatio") == 0) {
        m_session->settings()->setPixelAspectRatio(value.toSize());
        return;
    } else if (qstrcmp(name.toLocal8Bit().constData(), "maxClipSizeInBytes") == 0) {
        m_session->settings()->setMaxClipSizeInBytes(value.toInt());
        return;
    }

    m_session->setError(KErrNotSupported, tr("Requested encoding option is not supported"));
}

QVideoEncoderSettings S60VideoEncoderControl::videoSettings() const
{
    return m_session->settings()->videoEncoderSettings();
}

void S60VideoEncoderControl::setVideoSettings(const QVideoEncoderSettings &settings)
{
    if (!settings.isNull()) {
        // Notify that settings have been implicitly set and there's no need to
        // initialize them in case camera is changed
        m_session->settings()->notifySettingsSet();

        if (!settings.codec().isEmpty())
            m_session->settings()->setVideoCodec(settings.codec());

        // Set quality before resolution, framerate and bitrate (as quality defines unset settings)
        m_session->settings()->setVideoQuality(settings.quality());
        m_session->settings()->setVideoResolution(settings.resolution());
        m_session->settings()->setVideoFrameRate(settings.frameRate());
        m_session->settings()->setVideoBitrate(settings.bitRate());
        m_session->settings()->setVideoEncodingMode(settings.encodingMode());
    } else {
        m_session->setError(KErrNotSupported, tr("Unable to set undefined settings."));
    }
}

// End of file
