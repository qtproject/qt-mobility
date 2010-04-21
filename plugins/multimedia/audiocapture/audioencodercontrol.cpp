/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
    m_session = qobject_cast<AudioCaptureSession*>(parent);

    QT_PREPEND_NAMESPACE(QAudioFormat) fmt;
    fmt.setSampleSize(8);
    fmt.setChannels(1);
    fmt.setFrequency(8000);
    fmt.setSampleType(QT_PREPEND_NAMESPACE(QAudioFormat)::SignedInt);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    m_session->setFormat(fmt);

    m_settings.setEncodingMode(QtMediaServices::ConstantQualityEncoding);
    m_settings.setCodec("audio/pcm");
    m_settings.setBitRate(8000);
    m_settings.setChannelCount(1);
    m_settings.setSampleRate(8000);
    m_settings.setQuality(QtMediaServices::LowQuality);
}

AudioEncoderControl::~AudioEncoderControl()
{
}

QStringList AudioEncoderControl::supportedAudioCodecs() const
{
    QStringList list;
    if (m_session->supportedContainers().size() > 0)
        list.append("audio/pcm");

    return list;
}

QString AudioEncoderControl::codecDescription(const QString &codecName) const
{
    if (codecName.contains(QLatin1String("audio/pcm")))
        return QString(tr("PCM audio data"));

    return QString();
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
    Q_UNUSED(name)

    return QVariant();
}

void AudioEncoderControl::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    Q_UNUSED(value)
    Q_UNUSED(codec)
    Q_UNUSED(name)
}

QList<int> AudioEncoderControl::supportedSampleRates(const QAudioEncoderSettings &, bool *continuous) const
{
    if (continuous)
        *continuous = false;

    return m_session->deviceInfo()->supportedFrequencies();
}

QAudioEncoderSettings AudioEncoderControl::audioSettings() const
{
    return m_settings;
}

void AudioEncoderControl::setAudioSettings(const QAudioEncoderSettings &settings)
{
    QAudioFormat fmt = m_session->format();

    if (settings.encodingMode() == QtMediaServices::ConstantQualityEncoding) {
        if (settings.quality() == QtMediaServices::LowQuality) {
            fmt.setSampleSize(8);
            fmt.setChannels(1);
            fmt.setFrequency(8000);
            fmt.setSampleType(QAudioFormat::UnSignedInt);

        } else if (settings.quality() == QtMediaServices::NormalQuality) {
            fmt.setSampleSize(16);
            fmt.setChannels(1);
            fmt.setFrequency(22050);
            fmt.setSampleType(QAudioFormat::SignedInt);

        } else {
            fmt.setSampleSize(16);
            fmt.setChannels(1);
            fmt.setFrequency(44100);
            fmt.setSampleType(QAudioFormat::SignedInt);
        }

    } else {
        fmt.setChannels(settings.channelCount());
        fmt.setFrequency(settings.sampleRate());
        if (settings.sampleRate() == 8000 && settings.bitRate() == 8000) {
            fmt.setSampleType(QAudioFormat::UnSignedInt);
            fmt.setSampleSize(8);
        } else {
            fmt.setSampleSize(16);
            fmt.setSampleType(QAudioFormat::SignedInt);
        }
    }
    fmt.setCodec("audio/pcm");

    m_session->setFormat(fmt);
    m_settings = settings;
}
