/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qaudiodeviceinfo_pulse.h"
#include "qpulseaudioengine.h"
#include "qpulsehelpers.h"

QT_BEGIN_NAMESPACE

QPulseAudioDeviceInfo::QPulseAudioDeviceInfo(QByteArray device, QAudio::Mode mode)
    : m_device(device)
    , m_mode(mode)
{
}

bool QPulseAudioDeviceInfo::isFormatSupported(const QAudioFormat &format) const
{
    pa_sample_spec spec = QPulseAudioInternal::audioFormatToSampleSpec(format);
    if (!pa_sample_spec_valid(&spec))
        return false;

    return true;
}

QAudioFormat QPulseAudioDeviceInfo::preferredFormat() const
{
    QAudioFormat nearest;
    if (m_mode == QAudio::AudioOutput) {
        nearest.setFrequency(44100);
        nearest.setChannels(2);
        nearest.setByteOrder(QAudioFormat::LittleEndian);
        nearest.setSampleType(QAudioFormat::SignedInt);
        nearest.setSampleSize(16);
        nearest.setCodec(QLatin1String("audio/pcm"));
    } else {
        nearest.setFrequency(8000);
        nearest.setChannels(1);
        nearest.setSampleType(QAudioFormat::UnSignedInt);
        nearest.setSampleSize(8);
        nearest.setCodec(QLatin1String("audio/pcm"));
        nearest.setByteOrder(QAudioFormat::LittleEndian);
    }
    return nearest;
}

QString QPulseAudioDeviceInfo::deviceName() const
{
    return m_device;
}

QStringList QPulseAudioDeviceInfo::supportedCodecs()
{
    return QStringList() << "audio/pcm";
}

QList<int> QPulseAudioDeviceInfo::supportedSampleRates()
{
    return QList<int>() << 8000 << 11025 << 22050 << 44100 << 48000;
}

QList<int> QPulseAudioDeviceInfo::supportedChannelCounts()
{
    return QList<int>() << 1 << 2 << 4 << 6 << 8;
}

QList<int> QPulseAudioDeviceInfo::supportedSampleSizes()
{
    return QList<int>() << 8 << 16 << 24 << 32;
}

QList<QAudioFormat::Endian> QPulseAudioDeviceInfo::supportedByteOrders()
{
    return QList<QAudioFormat::Endian>() << QAudioFormat::BigEndian << QAudioFormat::LittleEndian;
}

QList<QAudioFormat::SampleType> QPulseAudioDeviceInfo::supportedSampleTypes()
{
    return QList<QAudioFormat::SampleType>() << QAudioFormat::SignedInt << QAudioFormat::UnSignedInt;// << QAudioFormat::Float;
}

QT_END_NAMESPACE
