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

#include "audioencodercontrol.h"
#include "audiocapturesession.h"

#include <QtMultimedia/qaudioformat.h>

#include <QtCore/qdebug.h>

AudioEncoderControl::AudioEncoderControl(QObject *parent)
    :QAudioEncoderControl(parent)
{
    QT_PREPEND_NAMESPACE(QAudioFormat) fmt;
    fmt.setSampleSize(8);
    fmt.setChannels(1);
    fmt.setFrequency(8000);
    fmt.setSampleType(QT_PREPEND_NAMESPACE(QAudioFormat)::SignedInt);
    fmt.setCodec("audio/pcm");

    m_session = qobject_cast<AudioCaptureSession*>(parent);
}

AudioEncoderControl::~AudioEncoderControl()
{
}

QStringList AudioEncoderControl::supportedAudioCodecs() const
{
    return m_session->supportedAudioCodecs();
}

QString AudioEncoderControl::audioCodec() const
{
    return m_session->format().codec();
}

bool AudioEncoderControl::setAudioCodec(const QString &codecName)
{
    QT_PREPEND_NAMESPACE(QAudioFormat) fmt = m_session->format();
    fmt.setCodec(codecName);
    return m_session->setFormat(fmt);
}

QString AudioEncoderControl::codecDescription(const QString &codecName) const
{
    if(qstrcmp(codecName.toLocal8Bit().constData(),"audio/x-wav") == 0)
        return QString("wav file format");
    if(qstrcmp(codecName.toLocal8Bit().constData(),"audio/pcm") == 0)
                return QString("raw audio");

    return QString();
}

int AudioEncoderControl::bitrate() const
{
    return (m_session->format().frequency()*m_session->format().channels()*(m_session->format().sampleSize()/8));
}

void AudioEncoderControl::setBitrate(int value)
{
    QT_PREPEND_NAMESPACE(QAudioFormat) fmt = m_session->format();

    if(value <= 8) {
        // low, 8000Hz mono U8
        fmt.setSampleType(QT_PREPEND_NAMESPACE(QAudioFormat)::UnSignedInt);
        fmt.setSampleSize(8);
        fmt.setFrequency(8000);
        fmt.setChannels(1);

    } else if(value <= 44) {
        // medium, 22050Hz mono S16
        fmt.setSampleType(QT_PREPEND_NAMESPACE(QAudioFormat)::SignedInt);
        fmt.setSampleSize(16);
        fmt.setFrequency(22050);
        fmt.setChannels(1);

    } else {
        // high, 44100Hz mono S16
        fmt.setSampleType(QT_PREPEND_NAMESPACE(QAudioFormat)::SignedInt);
        fmt.setSampleSize(16);
        fmt.setFrequency(44100);
        fmt.setChannels(1);

    }
    m_session->setFormat(fmt);
}

QtMedia::EncodingQuality AudioEncoderControl::quality() const
{
    return QtMedia::NormalQuality;
}

void AudioEncoderControl::setQuality(QtMedia::EncodingQuality value)
{
    Q_UNUSED(value)
}

QStringList AudioEncoderControl::supportedEncodingOptions(const QString &codec) const
{
    Q_UNUSED(codec)

    QStringList list;
    return list;
}

QVariant AudioEncoderControl::encodingOption(const QString &codec, const QString &name) const
{
    Q_UNUSED(codec)
    if(qstrcmp(name.toLocal8Bit().constData(),"bitrate") == 0) {
        return QVariant(8000);
    }

    return QVariant();
}

void AudioEncoderControl::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    Q_UNUSED(value)
    Q_UNUSED(codec)

    QT_PREPEND_NAMESPACE(QAudioFormat) fmt = m_session->format();

    if(qstrcmp(name.toLocal8Bit().constData(),"bitrate") == 0) {
        //TODO

    } else if(qstrcmp(name.toLocal8Bit().constData(),"quality") == 0) {
        //TODO

    } else
        qWarning()<<"option: "<<name<<" is an unknown option!";
}

int AudioEncoderControl::sampleRate() const
{
    return m_session->format().frequency();
}

void AudioEncoderControl::setSampleRate(int sampleRate)
{
    QT_PREPEND_NAMESPACE(QAudioFormat) fmt = m_session->format();
    fmt.setFrequency(sampleRate);
    m_session->setFormat(fmt);
}

QList<int> AudioEncoderControl::supportedSampleRates(const QAudioEncoderSettings &, bool *continuous) const
{
    if (continuous)
        *continuous = false;

    return m_session->deviceInfo()->supportedFrequencies();
}

int AudioEncoderControl::channels() const
{
    return m_session->format().channels();
}

void AudioEncoderControl::setChannels(int channels)
{
    QT_PREPEND_NAMESPACE(QAudioFormat) fmt = m_session->format();
    fmt.setChannels(channels);
    m_session->setFormat(fmt);
}

QList<int> AudioEncoderControl::supportedChannelCounts() const
{
    return QList<int>() << 1 << 2;
}

int AudioEncoderControl::sampleSize() const
{
    return m_session->format().sampleSize();
}

void AudioEncoderControl::setSampleSize(int sampleSize)
{
    QT_PREPEND_NAMESPACE(QAudioFormat) fmt = m_session->format();
    fmt.setSampleSize(sampleSize);
    m_session->setFormat(fmt);
}

QList<int> AudioEncoderControl::supportedSampleSizes() const
{
    QList<int> sizes = m_session->deviceInfo()->supportedSampleSizes();
    return sizes;
}

QAudioEncoderSettings AudioEncoderControl::audioSettings() const
{
    QAudioEncoderSettings settings;
    settings.setCodec(audioCodec());
    settings.setBitrate(bitrate());
    settings.setQuality(quality());
    settings.setSampleRate(sampleRate());
    settings.setChannels(channels());
    return settings;
}

void AudioEncoderControl::setAudioSettings(const QAudioEncoderSettings &settings)
{
    setAudioCodec(settings.codec());
    setBitrate(settings.bitrate());
    setQuality(settings.quality());
    setSampleRate(settings.sampleRate());
    setChannels(settings.channels());
}
