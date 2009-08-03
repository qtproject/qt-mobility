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

#include "audiodeviceendpoint.h"

#include <QtGui/QIcon>
#include <QtCore/QDebug>

#include <QtMultimedia/qaudiodeviceinfo.h>


AudioDeviceEndpoint::AudioDeviceEndpoint(QObject *parent)
    :QAudioDeviceEndpoint(parent)
{
    update();
}

AudioDeviceEndpoint::~AudioDeviceEndpoint()
{
}

void AudioDeviceEndpoint::setDirectionFilter(QAudioDeviceEndpoint::DeviceDirection direction)
{
    if (direction != directionFilter()) {
        QAudioDeviceEndpoint::setDirectionFilter(direction);
        update();
    }
}

void AudioDeviceEndpoint::setRoleFilter(QAudioDeviceEndpoint::Roles roles)
{
    if (roles != roleFilter()) {
        QAudioDeviceEndpoint::setRoleFilter(roles);
        update();
    }
}

void AudioDeviceEndpoint::setFormFactorFilter(QAudioDeviceEndpoint::FormFactors forms)
{
    if (forms != formFactorFilter()) {
        QAudioDeviceEndpoint::setFormFactorFilter(forms);
        update();
    }
}

int AudioDeviceEndpoint::deviceCount() const
{
    return m_names.count();
}

int AudioDeviceEndpoint::direction(int index) const
{
    return m_directions[index];
}

QAudioDeviceEndpoint::Roles AudioDeviceEndpoint::roles(int index) const
{
    return m_roles[index];
}

QAudioDeviceEndpoint::FormFactor AudioDeviceEndpoint::formFactor(int index) const
{
    return m_formFactors[index];
}

QString AudioDeviceEndpoint::name(int index) const
{
    return m_names[index];
}

QString AudioDeviceEndpoint::description(int index) const
{
    return m_descriptions[index];
}

QIcon AudioDeviceEndpoint::icon(int index) const
{
    Q_UNUSED(index);
    return QIcon();
}

int AudioDeviceEndpoint::defaultInputDevice(QAudioDeviceEndpoint::Role role) const
{
    Q_UNUSED(role);
    return 0;
}

int AudioDeviceEndpoint::defaultOutputDevice(QAudioDeviceEndpoint::Role role) const
{
    Q_UNUSED(role);
    return 0;
}

void AudioDeviceEndpoint::update()
{
    m_names.clear();
    m_descriptions.clear();
    m_directions.clear();
    m_roles.clear();
    m_formFactors.clear();

    QList<QAudioDeviceId> devices;

    if(directionFilter() == InputDevice) {
        devices = QAudioDeviceInfo::deviceList(QAudio::AudioInput);
        for(int i = 0; i < devices.size(); ++i) {
            m_names.append(QAudioDeviceInfo(devices.at(i)).deviceName());
            m_descriptions.append(QAudioDeviceInfo(devices.at(i)).deviceName());
            m_directions.append(InputDevice);
            m_roles.append(AllRoles);
            m_formFactors.append(UnknownFormFactor);
        }
    } else {
        devices = QAudioDeviceInfo::deviceList(QAudio::AudioOutput);
        for(int i = 0; i < devices.size(); ++i) {
            m_names.append(QAudioDeviceInfo(devices.at(i)).deviceName());
            m_descriptions.append(QAudioDeviceInfo(devices.at(i)).deviceName());
            m_directions.append(OutputDevice);
            m_roles.append(AllRoles);
            m_formFactors.append(UnknownFormFactor);
        }
    }
}
