/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "audiocapturecontrol.h"
#include "audiocapturesession.h"

#include "qmediasource.h"

#include <QtCore/qdebug.h>


AudioCaptureControl::AudioCaptureControl(QObject *parent)
    :QAudioCaptureControl(parent)
{
    qWarning()<<"ccccccc";
}

AudioCaptureControl::AudioCaptureControl(AudioCaptureService *service, QObject *parent)
   :QAudioCaptureControl(parent), m_service(service)
{
    m_session = new AudioCaptureSession(this);
}

AudioCaptureControl::~AudioCaptureControl()
{
}

void AudioCaptureControl::start()
{
}

void AudioCaptureControl::stop()
{
}

QByteArray AudioCaptureControl::defaultDevice()
{
    return QByteArray();
}

QList<QByteArray> AudioCaptureControl::deviceList()
{
    QList<QByteArray> list;
    return list;
}

QStringList AudioCaptureControl::supportedCodecs()
{
    QStringList list;
    return list;
}

QList<int> AudioCaptureControl::supportedFrequencies()
{
    QList<int> list;
    return list;
}

QList<int> AudioCaptureControl::supportedChannels()
{
    QList<int> list;
    return list;
}

QList<int> AudioCaptureControl::supportedSampleSizes()
{
    QList<int> list;
    return list;
}

QList<QAudioFormat::Endian> AudioCaptureControl::supportedByteOrders()
{
    QList<QAudioFormat::Endian> list;
    return list;
}

QList<QAudioFormat::SampleType> AudioCaptureControl::supportedSampleTypes()
{
    QList<QAudioFormat::SampleType> list;
    return list;
}

QAudioFormat AudioCaptureControl::format()
{
    return QAudioFormat();
}

bool AudioCaptureControl::setFormat(const QAudioFormat &format)
{
    return true;
}

void AudioCaptureControl::setSink(QAbstractMediaObject* sink)
{
}


