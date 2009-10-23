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

#include "audiocapturesession.h"
#include "audiodevicecontrol.h"

#include <QtGui/QIcon>
#include <QtCore/QDebug>

#include <QtMultimedia/qaudiodeviceinfo.h>


AudioDeviceControl::AudioDeviceControl(QObject *parent)
    :QAudioDeviceControl(parent)
{
    m_session = qobject_cast<AudioCaptureSession*>(parent);

    update();
}

AudioDeviceControl::~AudioDeviceControl()
{
}

int AudioDeviceControl::deviceCount() const
{
    return m_names.count();
}

QString AudioDeviceControl::name(int index) const
{
    if(index < m_names.count())
        return m_names[index];

    return QString();
}

QString AudioDeviceControl::description(int index) const
{
    if(index < m_names.count())
        return m_descriptions[index];

    return QString();
}

QIcon AudioDeviceControl::icon(int index) const
{
    Q_UNUSED(index);
    return QIcon();
}

int AudioDeviceControl::defaultDevice() const
{
    QAudioDeviceInfo idx = QAudioDeviceInfo::defaultInputDevice();
    QList<QAudioDeviceInfo> devices;
    devices = QAudioDeviceInfo::deviceList(QAudio::AudioInput);
    QString devName = QAudioDeviceInfo(idx).deviceName();
    for(int i=0;i<m_names.count();i++) {
        if(qstrcmp(devName.toLocal8Bit().constData(),
                    m_names.at(i).toLocal8Bit().constData()) == 0)
            return i;
    }
    return 0;
}

int AudioDeviceControl::selectedDevice() const
{
    for(int i=0;i<m_names.count();i++) {
        if(qstrcmp(m_device.toLocal8Bit().constData(),
                    m_names[i].toLocal8Bit().constData()) == 0)
            return i;
    }
    return 0;
}

void AudioDeviceControl::setSelectedDevice(int index)
{
    if(index < m_names.count())
        m_session->setCaptureDevice(m_names[index]);
}

void AudioDeviceControl::update()
{
    m_names.clear();
    m_descriptions.clear();

    QList<QAudioDeviceInfo> devices;
    devices = QAudioDeviceInfo::deviceList(QAudio::AudioInput);
    for(int i = 0; i < devices.size(); ++i) {
        m_names.append(devices.at(i).deviceName());
        m_descriptions.append(devices.at(i).deviceName());
    }
    m_device = QAudioDeviceInfo(QAudioDeviceInfo::defaultInputDevice()).deviceName();
}
