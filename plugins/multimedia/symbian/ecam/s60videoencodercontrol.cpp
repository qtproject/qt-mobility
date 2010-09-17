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

#include <QtCore/qdebug.h>  // For qWarning()

#include "s60videoencodercontrol.h"
#include "s60videocapturesession.h"

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
    return m_session->supportedVideoCaptureCodecs();
}

QString S60VideoEncoderControl::videoCodecDescription(const QString &codecName) const
{
    return m_session->videoCaptureCodecDescription(codecName);
}

/*
QString S60VideoEncoderControl::videoCodec() const
{
    QC_TRACE1("S60VideoEncoderControl::videoCodec: Codec =", m_session->videoCaptureCodec())
    return m_session->videoCaptureCodec();
}
*/
/*
void S60VideoEncoderControl::setVideoCodec(const QString &codecName)
{
    QC_TRACE1("S60VideoEncoderControl::setVideoCodec: New Codec =", codecName)
    m_session->setVideoCaptureCodec(codecName);
}
*/
/*
int S60VideoEncoderControl::bitRate() const
{
    QC_TRACE1("S60VideoEncoderControl::bitRate: BitRate =", m_session->bitrate())
    return m_session->bitrate();
}
*/
/*
void S60VideoEncoderControl::setBitRate(int bitRate)
{
    QC_TRACE1("S60VideoEncoderControl::setBitRate: New BitRate =", bitRate)
    m_session->setBitrate(bitRate);
}
*/
/*
QtMultimediaKit::EncodingQuality S60VideoEncoderControl::quality() const
{
    QC_TRACE1("S60VideoEncoderControl::quality: Quality =", m_session->videoCaptureQuality())
    return m_session->videoCaptureQuality();
}
*/
/*
void S60VideoEncoderControl::setQuality(QtMultimediaKit::EncodingQuality quality)
{
    QC_TRACE1("S60VideoEncoderControl::setQuality: New Quality =", quality)
    m_session->setVideoCaptureQuality(quality);
}
*/
QList<qreal> S60VideoEncoderControl::supportedFrameRates(const QVideoEncoderSettings &settings, bool *continuous) const
{
    if (continuous) {
        if (!settings.isNull())
            return m_session->supportedVideoFrameRates(settings, *continuous);
        return m_session->supportedVideoFrameRates(*continuous);
    } else {
        bool contTemp = false;
        if (!settings.isNull())
            return m_session->supportedVideoFrameRates(settings, contTemp);
        return m_session->supportedVideoFrameRates(contTemp);
    }
}

/*
qreal S60VideoEncoderControl::minimumFrameRate() const
{
    QList<qreal> rates = m_session->supportedVideoFrameRates();
    qreal minRate = 1000.0; // Safe start value for min FrameRate

    foreach (qreal rate, rates)
        minRate = qMin(minRate, rate);

    if (minRate == 1000.0)
        minRate = 0.0;

    QC_TRACE1("S60VideoEncoderControl::minimumFrameRate: Min FrameRate =", minRate)
    return minRate;
}

qreal S60VideoEncoderControl::maximumFrameRate() const
{
    QList<qreal> rates = m_session->supportedVideoFrameRates();
    qreal maxRate = 0.0; // Safe start value for max FrameRate

    foreach (qreal rate, rates)
        maxRate = qMax(maxRate, rate);

    QC_TRACE1("S60VideoEncoderControl::maximumFrameRate: Max FrameRate =", maxRate)
    return maxRate;
}
*/

/*
qreal S60VideoEncoderControl::frameRate() const
{
    QC_TRACE1("S60VideoEncoderControl::frameRate: FrameRate =", m_session->framerate())
    return m_session->framerate();
}
*/
/*
void S60VideoEncoderControl::setFrameRate(qreal frameRate)
{
    QC_TRACE1("S60VideoEncoderControl::setFrameRate: New FrameRate =", frameRate)
    m_session->setFrameRate(frameRate);
}
*/
QList<QSize> S60VideoEncoderControl::supportedResolutions(const QVideoEncoderSettings &settings, bool *continuous) const
{
    if (continuous) {
        if (!settings.isNull())
            return m_session->supportedVideoResolutions(settings, *continuous);
        return m_session->supportedVideoResolutions(*continuous);
    } else {
        bool contTemp = false;
        if (!settings.isNull())
            return m_session->supportedVideoResolutions(settings, contTemp);
        return m_session->supportedVideoResolutions(contTemp);
    }
}

/*
QSize S60VideoEncoderControl::minimumResolution() const
{
    QSize minSize;
    bool continuous = false;
    QList<QSize> sizes = m_session->supportedVideoResolutions(continuous);

    minSize = sizes.first();

    for (int i = 0; i < sizes.size(); i++) {
        if ((sizes.at(i).width() * sizes.at(i).height()) < (minSize.width() * minSize.height()))
            minSize = sizes.at(i);
    }

    QC_TRACE1("S60VideoEncoderControl::minimumResolution: Min Resolution =", minSize)
    return minSize;
}

QSize S60VideoEncoderControl::maximumResolution() const
{
    QC_TRACEIN("S60VideoEncoderControl::maximumResolution")

    QSize maxSize;
    bool continuous = false;
    QList<QSize> sizes = m_session->supportedVideoResolutions(continuous);

    maxSize = sizes.first();

    for (int i = 0; i < sizes.size(); i++) {
        if ((sizes.at(i).width() * sizes.at(i).height()) > (maxSize.width() * maxSize.height()))
            maxSize = sizes.at(i);
    }

    QC_TRACE1("S60VideoEncoderControl::maximumResolution: Max Resolution =", maxSize)
    return maxSize;
}
*/
/*
QSize S60VideoEncoderControl::videoResolution() const
{
    QC_TRACE1("S60VideoEncoderControl::videoResolution: Resolution =", m_session->videoResolution())
    return m_session->videoResolution();
}
*/
/*
void S60VideoEncoderControl::setResolution(const QSize resolution)
{
    QC_TRACE1("S60VideoEncoderControl::setResolution: New Resolution =", resolution)
    m_session->setVideoResolution(resolution);
}
*/
QStringList S60VideoEncoderControl::supportedEncodingOptions(const QString &codec) const
{
    // Possible settings: encodingMode, codec, resolution, framerate, bitRate, quality
    // Possible (codec specific) options: pixelAspectRatio, gain, maxClipSizeInBytes

    // Following options valid for all codecs
    Q_UNUSED(codec);

    QStringList options;
    options.append("pixelAspectRatio");
    options.append("gain");
    options.append("maxClipSizeInBytes");

    return options;
}

QVariant S60VideoEncoderControl::encodingOption(const QString &codec, const QString &name) const
{
    QVariant returnValue;

    if(qstrcmp(name.toLocal8Bit().constData(), "pixelAspectRatio") == 0) {
        returnValue.setValue(m_session->pixelAspectRatio());
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "gain") == 0) {
        returnValue.setValue((int)m_session->gain());
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "maxClipSizeInBytes") == 0) {
        returnValue.setValue(m_session->maxClipSizeInBytes());
    }

    return returnValue;
}

void S60VideoEncoderControl::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    // Set the codec first if not already set
    m_session->setVideoCaptureCodec(codec);

    if(qstrcmp(name.toLocal8Bit().constData(), "pixelAspectRatio") == 0) {
        m_session->setPixelAspectRatio(value.toSize());
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "gain") == 0) {
        m_session->setGain(value.toInt());
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "maxClipSizeInBytes") == 0) {
        m_session->setMaxClipSizeInBytes(value.toInt());
    }
    else {
        qWarning() << "Option:" << name << "is an unknown option!";
    }
}

QVideoEncoderSettings S60VideoEncoderControl::videoSettings() const
{
    QVideoEncoderSettings settings;
    m_session->videoEncoderSettings(settings);

    return settings;
}

void S60VideoEncoderControl::setVideoSettings(const QVideoEncoderSettings &settings)
{
    if (settings.codec().isEmpty()) {
        m_session->setVideoCaptureQuality(settings.quality(), S60VideoCaptureSession::EOnlyVideoQuality);
    } else if (settings.resolution() != QSize() && settings.frameRate() == 0 && settings.bitRate() == -1) { // Resolution
        m_session->setVideoCaptureCodec(settings.codec());
        m_session->setVideoEncodingMode(settings.encodingMode());
        m_session->setVideoResolution(settings.resolution());
        m_session->setVideoCaptureQuality(settings.quality(), S60VideoCaptureSession::EVideoQualityAndResolution);
    } else if (settings.resolution() != QSize() && settings.frameRate() == 0 && settings.bitRate() == -1) { // Framerate
        m_session->setVideoCaptureCodec(settings.codec());
        m_session->setVideoEncodingMode(settings.encodingMode());
        m_session->setFrameRate(settings.frameRate());
        m_session->setVideoCaptureQuality(settings.quality(), S60VideoCaptureSession::EVideoQualityAndFrameRate);
    } else if (settings.resolution() != QSize() && settings.frameRate() == 0 && settings.bitRate() == -1) { // BitRate
        m_session->setVideoCaptureCodec(settings.codec());
        m_session->setVideoEncodingMode(settings.encodingMode());
        m_session->setBitrate(settings.bitRate());
        m_session->setVideoCaptureQuality(settings.quality(), S60VideoCaptureSession::EVideoQualityAndBitRate);
    } else if (settings.resolution() != QSize() && settings.frameRate() == 0 && settings.bitRate() == -1) { // Resolution and FrameRate
        m_session->setVideoCaptureCodec(settings.codec());
        m_session->setVideoEncodingMode(settings.encodingMode());
        m_session->setVideoResolution(settings.resolution());
        m_session->setFrameRate(settings.frameRate());
        m_session->setVideoCaptureQuality(settings.quality(), S60VideoCaptureSession::EVideoQualityAndResolutionAndFrameRate);
    } else if (settings.resolution() != QSize() && settings.frameRate() == 0 && settings.bitRate() == -1) { // Resolution and BitRate
        m_session->setVideoCaptureCodec(settings.codec());
        m_session->setVideoEncodingMode(settings.encodingMode());
        m_session->setVideoResolution(settings.resolution());
        m_session->setBitrate(settings.bitRate());
        m_session->setVideoCaptureQuality(settings.quality(), S60VideoCaptureSession::EVideoQualityAndResolutionAndBitRate);
    } else if (settings.resolution() != QSize() && settings.frameRate() == 0 && settings.bitRate() == -1) { // FrameRate and BitRate
        m_session->setVideoCaptureCodec(settings.codec());
        m_session->setVideoEncodingMode(settings.encodingMode());
        m_session->setFrameRate(settings.frameRate());
        m_session->setBitrate(settings.bitRate());
        m_session->setVideoCaptureQuality(settings.quality(), S60VideoCaptureSession::EVideoQualityAndFrameRateAndBitRate);
    } else { // Resolution, BitRate and FrameRate
        m_session->setVideoCaptureCodec(settings.codec());
        m_session->setVideoEncodingMode(settings.encodingMode());
        m_session->setVideoResolution(settings.resolution());
        m_session->setFrameRate(settings.frameRate());
        m_session->setBitrate(settings.bitRate());
        m_session->setVideoCaptureQuality(settings.quality(), S60VideoCaptureSession::ENoVideoQuality);
    }
}

// End of file
