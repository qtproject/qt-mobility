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

#include "S60audioencoder.h"
#include "s60videocapturesession.h"

#include <QtCore/qdebug.h>

S60AudioEncoder::S60AudioEncoder(QObject *session, QObject *parent)
    :QAudioEncoderControl(parent)
{    
    m_session = qobject_cast<S60VideoCaptureSession*>(session); 
}

S60AudioEncoder::~S60AudioEncoder()
{
}

QStringList S60AudioEncoder::supportedAudioCodecs() const
{
    qDebug() << "S60AudioEncoder::supportedAudioCodecs";  
    return m_session->supportedAudioCaptureCodecs();
}

QString S60AudioEncoder::audioCodec() const
{
    return m_session->audioCaptureCodec();
}

void S60AudioEncoder::setAudioCodec(const QString &codecName)
{    
    m_session->setAudioCaptureCodec(codecName);
}

QString S60AudioEncoder::codecDescription(const QString &codecName) const
{   
	if(qstrcmp(codecName.toLocal8Bit().constData(), "audio/aac") == 0)
        return QString("Advanced Audio Coding");
	else if(qstrcmp(codecName.toLocal8Bit().constData(), "audio/amr") == 0)
        return QString("Adaptive Multi-Rate audio codec");

    return QString();    
}

int S60AudioEncoder::bitRate() const
{
    //return m_session->audioBitrate();
    return 0;
}

void S60AudioEncoder::setBitRate(int value)
{
    Q_UNUSED(value)
   // m_session->setAudioBitrate();
}

QtMedia::EncodingQuality S60AudioEncoder::quality() const
{
    return QtMedia::NormalQuality;
}

void S60AudioEncoder::setQuality(QtMedia::EncodingQuality value)
{
    Q_UNUSED(value)
/*    QAudioFormat fmt = m_session->format();

    switch (value) {
        case QtMedia::VeryLowQuality:
        case QtMedia::LowQuality:
            // low, 8000Hz mono U8
            fmt.setSampleType(QAudioFormat::UnSignedInt);
            fmt.setSampleSize(8);
            fmt.setFrequency(8000);
            fmt.setChannels(1);
            break;
        case QtMedia::NormalQuality:
            // medium, 22050Hz mono S16
            fmt.setSampleType(QAudioFormat::SignedInt);
            fmt.setSampleSize(16);
            fmt.setFrequency(22050);
            fmt.setChannels(1);
            break;
        case QtMedia::HighQuality:
        case QtMedia::VeryHighQuality:    
            // high, 44100Hz mono S16
            fmt.setSampleType(QAudioFormat::SignedInt);
            fmt.setSampleSize(16);
            fmt.setFrequency(44100);
            fmt.setChannels(1);
            break;
        default:
            break;
    }
    m_session->setFormat(fmt);*/
}

QStringList S60AudioEncoder::supportedEncodingOptions(const QString &codec) const
{
    Q_UNUSED(codec)
    QStringList list;
    return list;
}

QVariant S60AudioEncoder::encodingOption(const QString &codec, const QString &name) const
{
    Q_UNUSED(codec)	
    return QVariant();
}

void S60AudioEncoder::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    Q_UNUSED(value)
    Q_UNUSED(codec)    
}

int S60AudioEncoder::sampleRate() const
{
   // return m_session->audioSampleRate();
   return 0;
}

void S60AudioEncoder::setSampleRate(int sampleRate)
{
    Q_UNUSED(sampleRate)
  /*   if (sampleRate > 0) {        
        m_session->setAudioSampleRate(sampleRate);
    }*/
    
}

QList<int> S60AudioEncoder::supportedSampleRates(const QAudioEncoderSettings &settings, bool *continuous) const
{
    if (continuous)
        *continuous = false;

    //TODO: return supported sample rates
    //return m_session->supportedSampleRates();
    return QList<int>();
}

int S60AudioEncoder::channelCount() const
{
    //return m_session->channelCount();
    return 0;
}

void S60AudioEncoder::setChannelCount(int channels)
{
  /*  if (channels > 0) {
        m_session->setChannelCount(channels);
    }*/
    Q_UNUSED(channels)
}

QList<int> S60AudioEncoder::supportedChannelCounts() const
{
   // return m_session->supportedChannels();
    return QList<int>();
}

int S60AudioEncoder::sampleSize() const
{
   // return m_session->audioSampleSize();
    return 0;
}

void S60AudioEncoder::setSampleSize(int sampleSize)
{  
    Q_UNUSED(sampleSize)
   // m_session->setAudioSampleSize();
}

QList<int> S60AudioEncoder::supportedSampleSizes() const
{
    //QList<int> sizes = m_session->deviceInfo()->supportedSampleSizes();
    return QList<int>(); //sizes;
}

QAudioEncoderSettings S60AudioEncoder::audioSettings() const
{
    qDebug() << "S60AudioEncoder::audioSettings";  
    QAudioEncoderSettings settings;    
    m_session->getCurrentAudioEncoderSettings(settings);    
    return settings;
}

void S60AudioEncoder::setAudioSettings(const QAudioEncoderSettings &settings)
{
    QAudioEncoderSettings tempSettings = settings;    
    m_session->saveAudioEncoderSettings(tempSettings);
}
