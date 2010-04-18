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
    :QAudioEncoderControl(parent)
{
    qDebug()<<"S60AudioEncoderControl::S60AudioEncoderControl";
    QAudioFormat fmt;
    fmt.setSampleSize(8);
    fmt.setChannels(1);
    fmt.setFrequency(8000);
    fmt.setSampleType(QAudioFormat::SignedInt);
    //fmt.setCodec("audio/pcm");

    m_session = qobject_cast<S60AudioCaptureSession*>(session);
}

S60AudioEncoderControl::~S60AudioEncoderControl()
{
}

QStringList S60AudioEncoderControl::supportedAudioCodecs() const
{
    return m_session->supportedAudioCodecs();
}

QString S60AudioEncoderControl::audioCodec() const
{
    return m_session->format().codec();
}

bool S60AudioEncoderControl::setAudioCodec(const QString &codecName)
{
    QAudioFormat fmt = m_session->format();
    fmt.setCodec(codecName);
    return m_session->setFormat(fmt);
}

QString S60AudioEncoderControl::codecDescription(const QString &codecName) const
{
	return m_session->codecDescription(codecName);    
}

int S60AudioEncoderControl::bitRate() const
{
    return (m_session->format().frequency() * m_session->format().channels() * (m_session->format().sampleSize() / 8));
}

void S60AudioEncoderControl::setBitRate(int value)
{
    Q_UNUSED(value)
}

QtMediaServices::EncodingQuality S60AudioEncoderControl::quality() const
{
    return QtMediaServices::NormalQuality;
}

void S60AudioEncoderControl::setQuality(QtMediaServices::EncodingQuality value)
{
    QAudioFormat fmt = m_session->format();

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
            fmt.setChannels(1);
            break;
        default:
            break;
    }
    m_session->setFormat(fmt);
}

QStringList S60AudioEncoderControl::supportedEncodingOptions(const QString &codec) const
{
    Q_UNUSED(codec)

    QStringList list;
    return list;
}

QVariant S60AudioEncoderControl::encodingOption(const QString &codec, const QString &name) const
{
    Q_UNUSED(codec)
	QAudioFormat fmt = m_session->format();
    
    if(qstrcmp(name.toLocal8Bit().constData(), "bitrate") == 0) {
        return QVariant(fmt.frequency());
    }

    return QVariant();
}

void S60AudioEncoderControl::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    Q_UNUSED(value)
    Q_UNUSED(codec)

    //QAudioFormat fmt = m_session->format();

    if(qstrcmp(name.toLocal8Bit().constData(), "bitrate") == 0) {
        if (value.toString() == "vbr")
            setBitRate(-1);
        else
            setBitRate(value.toInt());

    } else if(qstrcmp(name.toLocal8Bit().constData(), "quality") == 0) {
        setQuality((QtMediaServices::EncodingQuality)value.toInt());

    } else
        qWarning() << "option: " << name << " is an unknown option!";
}

int S60AudioEncoderControl::sampleRate() const
{
    return m_session->format().frequency();
}

void S60AudioEncoderControl::setSampleRate(int sampleRate)
{
    if (sampleRate > 0) {
        QAudioFormat fmt = m_session->format();
        fmt.setFrequency(sampleRate);
        m_session->setFormat(fmt);
    }
}

QList<int> S60AudioEncoderControl::supportedSampleRates(const QAudioEncoderSettings &settings, bool *continuous) const
{
    if (continuous)
        *continuous = false;
    
    return m_session->supportedAudioSampleRates(settings);       
}

int S60AudioEncoderControl::channelCount() const
{
    return m_session->format().channels();
}

void S60AudioEncoderControl::setChannelCount(int channels)
{
    if (channels > 0) {
        QAudioFormat fmt = m_session->format();
        fmt.setChannels(channels);
        m_session->setFormat(fmt);
    }
}

QList<int> S60AudioEncoderControl::supportedChannelCounts() const
{
    return QList<int>() << 1 << 2;
}

int S60AudioEncoderControl::sampleSize() const
{
    return m_session->format().sampleSize();
}

void S60AudioEncoderControl::setSampleSize(int sampleSize)
{
    QAudioFormat fmt = m_session->format();
    fmt.setSampleSize(sampleSize);
    m_session->setFormat(fmt);
}

QList<int> S60AudioEncoderControl::supportedSampleSizes() const
{
    //QList<int> sizes = m_session->deviceInfo()->supportedSampleSizes();
    return QList<int>(); //sizes;
}

QAudioEncoderSettings S60AudioEncoderControl::audioSettings() const
{
    QAudioEncoderSettings settings;
    settings.setCodec(audioCodec());
    settings.setBitRate(bitRate());
    settings.setQuality(quality());
    settings.setSampleRate(sampleRate());
    settings.setChannelCount(channelCount());
    return settings;
}

void S60AudioEncoderControl::setAudioSettings(const QAudioEncoderSettings &settings)
{
    setAudioCodec(settings.codec());
    setBitRate(settings.bitRate());
    setSampleRate(settings.sampleRate());
    setChannelCount(settings.channelCount());
    // Setting quality will override other settings except codec
    setQuality(settings.quality());
}
