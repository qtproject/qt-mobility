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

#include "s60audioencodercontrol.h"
#include "s60audiocapturesession.h"

#include <QAudioFormat>

#include <QtCore/qdebug.h>

S60AudioEncoderControl::S60AudioEncoderControl(QObject *session, QObject *parent)
    :QAudioEncoderControl(parent), m_quality(QtMediaServices::NormalQuality)
{   
    m_session = qobject_cast<S60AudioCaptureSession*>(session);
    QAudioFormat fmt = m_session->format();
    // medium, 22050Hz mono S16
    fmt.setSampleType(QAudioFormat::SignedInt);
    fmt.setSampleSize(16);
    fmt.setFrequency(22050);
    fmt.setChannels(1);
    m_session->setFormat(fmt);
}

S60AudioEncoderControl::~S60AudioEncoderControl()
{
}

QStringList S60AudioEncoderControl::supportedAudioCodecs() const
{
    return m_session->supportedAudioCodecs();
}

QString S60AudioEncoderControl::codecDescription(const QString &codecName) const
{
	return m_session->codecDescription(codecName);    
}

QtMediaServices::EncodingQuality S60AudioEncoderControl::quality() const
{
    return m_quality;
}

void S60AudioEncoderControl::setQuality(QtMediaServices::EncodingQuality value, QAudioFormat &fmt)
{
    switch (value) {
    case QtMediaServices::VeryLowQuality:
    case QtMediaServices::LowQuality:
        // low, 8000Hz mono U8
        fmt.setSampleType(QAudioFormat::UnSignedInt);
        fmt.setSampleSize(8);
        fmt.setFrequency(8000);
        fmt.setChannels(1);
        break;
    case QtMediaServices::NormalQuality:
        // medium, 22050Hz mono S16
        fmt.setSampleType(QAudioFormat::SignedInt);
        fmt.setSampleSize(16);
        fmt.setFrequency(22050);
        fmt.setChannels(1);
        break;
    case QtMediaServices::HighQuality:
    case QtMediaServices::VeryHighQuality:    
        // high, 44100Hz mono S16
        fmt.setSampleType(QAudioFormat::SignedInt);
        fmt.setSampleSize(16);
        fmt.setFrequency(44100);
        fmt.setChannels(2);
        break;
    default:
        break;
    }
}

QStringList S60AudioEncoderControl::supportedEncodingOptions(const QString &codec) const
{
    Q_UNUSED(codec)
    QStringList list;
    if (codec == "PCM") 
        list << "quality" << "channels" << "samplerate";        
     return list;
}

QVariant S60AudioEncoderControl::encodingOption(const QString &codec, const QString &name) const
{
    if (codec == "PCM") {
        QAudioFormat fmt = m_session->format();
        
        if(qstrcmp(name.toLocal8Bit().constData(), "quality") == 0) {
            return QVariant(quality());
        }        
        else if(qstrcmp(name.toLocal8Bit().constData(), "channels") == 0) {
            return QVariant(fmt.channels());
        }                
        else if(qstrcmp(name.toLocal8Bit().constData(), "samplerate") == 0) {
            return QVariant(fmt.frequency());
        }       
    }
    return QVariant();
}

void S60AudioEncoderControl::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    if (codec == "PCM") {        
        QAudioFormat fmt = m_session->format();

        if(qstrcmp(name.toLocal8Bit().constData(), "quality") == 0) {
            setQuality((QtMediaServices::EncodingQuality)value.toInt(), fmt);
        } else if(qstrcmp(name.toLocal8Bit().constData(), "channels") == 0) {
            fmt.setChannels(value.toInt());
        } else if(qstrcmp(name.toLocal8Bit().constData(), "samplerate") == 0) {
            fmt.setFrequency(value.toInt());
        }
        m_session->setFormat(fmt);
    }
}

QList<int> S60AudioEncoderControl::supportedSampleRates(const QAudioEncoderSettings &settings, bool *continuous) const
{
    if (continuous)
        *continuous = false;
    
    return m_session->supportedAudioSampleRates(settings);       
}

QAudioEncoderSettings S60AudioEncoderControl::audioSettings() const
{
    return m_settings;
}

void S60AudioEncoderControl::setAudioSettings(const QAudioEncoderSettings &settings)
{
    QAudioFormat fmt = m_session->format();
    if (settings.encodingMode() == QtMediaServices::ConstantQualityEncoding) {
        fmt.setCodec(settings.codec());
        setQuality(settings.quality(), fmt);
        if (settings.sampleRate() > 0) {
            fmt.setFrequency(settings.sampleRate());
        }
        if (settings.channelCount() > 0)
            fmt.setChannels(settings.channelCount());
    }else {
        if (settings.sampleRate() == 8000) {
            fmt.setSampleType(QAudioFormat::UnSignedInt);
            fmt.setSampleSize(8);
        } else {
            fmt.setSampleType(QAudioFormat::SignedInt);
            fmt.setSampleSize(16);
        }
        fmt.setCodec(settings.codec());
        fmt.setFrequency(settings.sampleRate());
        fmt.setChannels(settings.channelCount());
    }
    m_session->setFormat(fmt);
    m_settings = settings;
}
