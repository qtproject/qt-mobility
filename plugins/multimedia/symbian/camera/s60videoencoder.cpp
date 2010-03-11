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
#include "s60videocapturesession.h"

#include <QtCore/qdebug.h>

S60VideoEncoder::S60VideoEncoder(QObject *parent)
    :QVideoEncoderControl(parent)
{
}

S60VideoEncoder::S60VideoEncoder(QObject *session, QObject *parent)
   :QVideoEncoderControl(parent)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60VideoCaptureSession*>(session);    
}

S60VideoEncoder::~S60VideoEncoder()
{
}

QStringList S60VideoEncoder::supportedVideoCodecs() const
{
    return m_session->supportedVideoCaptureCodecs();
}

QString S60VideoEncoder::videoCodecDescription(const QString &codecName) const
{
    return m_session->videoCaptureCodecDescription(codecName);   
}

QString S60VideoEncoder::videoCodec() const
{
    return m_session->videoCaptureCodec();
}

void S60VideoEncoder::setVideoCodec(const QString &codecName)
{
    m_session->setVideoCaptureCodec(codecName);
}

int S60VideoEncoder::bitRate() const
{
    return m_session->bitrate();
}
void S60VideoEncoder::setBitRate(int bitRate)
{
    m_session->setBitrate(bitRate);
}

QtMedia::EncodingQuality S60VideoEncoder::quality() const
{
    return m_session->videoCaptureQuality();
}

void S60VideoEncoder::setQuality(QtMedia::EncodingQuality quality)
{
    m_session->setVideoCaptureQuality(quality);
}

QList< qreal > S60VideoEncoder::supportedFrameRates(const QVideoEncoderSettings &settings, bool *continuous) const
{    
    if (!settings.isNull())
        return m_session->supportedVideoFrameRates(settings);
    return m_session->supportedVideoFrameRates();
}

qreal S60VideoEncoder::minimumFrameRate() const
{
    QList<qreal> rates = m_session->supportedVideoFrameRates();
    qreal minRate = 30.0;
    foreach (qreal rate, rates)
        minRate = qMin(minRate, rate);
    return minRate;
}

qreal S60VideoEncoder::maximumFrameRate() const
{
    QList<qreal> rates = m_session->supportedVideoFrameRates();
    qreal maxRate = 0.0;
    foreach (qreal rate, rates)
        maxRate = qMax(maxRate, rate);
    return maxRate;
}

qreal S60VideoEncoder::frameRate() const
{
    return m_session->framerate();
}
void S60VideoEncoder::setFrameRate(qreal frameRate)
{
    m_session->setFrameRate(frameRate);
}

QList<QSize> S60VideoEncoder::supportedResolutions(const QVideoEncoderSettings &settings, bool *continuous) const
{    
    if (!settings.isNull())
        return m_session->supportedVideoResolutions(settings);
    return m_session->supportedVideoResolutions();
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

QSize S60VideoEncoder::videoResolution() const
{
    return m_session->videoResolution();
}

void S60VideoEncoder::setResolution(const QSize resolution)
{
    m_session->setVideoResolution(resolution);
}

QStringList S60VideoEncoder::supportedEncodingOptions(const QString &codec) const
{    
    Q_UNUSED(codec)    
    //TODO define encoding for specific codecs
    return QStringList()<<"bitrate"<<"quality"<<"framerate"<<"resolution";
}

QVariant S60VideoEncoder::encodingOption(const QString &codec, const QString &name) const
{
    Q_UNUSED(codec)
 
    if(qstrcmp(name.toLocal8Bit().constData(), "bitrate") == 0) {
        return QVariant(m_session->bitrate());
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "quality") == 0) {
        return QVariant(m_session->videoCaptureQuality()); 
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "framerate") == 0) {
        return QVariant(m_session->framerate());
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "resolution") == 0) {
        return QVariant(m_session->videoResolution());
    }
    else
        return QVariant();    
}

void S60VideoEncoder::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{    
    Q_UNUSED(codec)
    
    if(qstrcmp(name.toLocal8Bit().constData(), "bitrate") == 0) {
        setBitRate(value.toInt());
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "quality") == 0) {
        setQuality((QtMedia::EncodingQuality)value.toInt());
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "framerate") == 0) {
        setFrameRate(value.toInt());
    }
    else if(qstrcmp(name.toLocal8Bit().constData(), "resolution") == 0) {
        setResolution(value.toSize());
    }
    else
        qWarning() << "option: " << name << " is an unknown option!";
}

QVideoEncoderSettings S60VideoEncoder::videoSettings() const
{    
    QVideoEncoderSettings settings;    
    m_session->getCurrentVideoEncoderSettings(settings);    
    return settings;
}

void S60VideoEncoder::setVideoSettings(const QVideoEncoderSettings &settings)
{
    QVideoEncoderSettings tempSettings = settings;    
    m_session->saveVideoEncoderSettings(tempSettings);
}



