/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "s60audiocapturesession.h"
#include "s60audioendpointselector.h"

#include <QtMultimedia/qaudiodeviceinfo.h>

S60AudioEndpointSelector::S60AudioEndpointSelector(QObject *session, QObject *parent)
    :QAudioEndpointSelector(parent)
{    
    m_session = qobject_cast<S60AudioCaptureSession*>(session); 
    update();    
    m_audioInput = defaultEndpoint();
}

S60AudioEndpointSelector::~S60AudioEndpointSelector()
{
}

QList<QString> S60AudioEndpointSelector::availableEndpoints() const
{
    return m_names;
}

QString S60AudioEndpointSelector::endpointDescription(const QString& name) const
{
    QString desc;
    for(int i = 0; i < m_names.count(); i++) {
        if (m_names.at(i).compare(name) == 0) {
            desc = m_descriptions.at(i);
            break;
        }
    }
    return desc;
}

QString S60AudioEndpointSelector::defaultEndpoint() const
{
    return QAudioDeviceInfo(QAudioDeviceInfo::defaultInputDevice()).deviceName();
}

QString S60AudioEndpointSelector::activeEndpoint() const
{
    return m_audioInput;
}

void S60AudioEndpointSelector::setActiveEndpoint(const QString& name)
{
    if (m_audioInput.compare(name) != 0) {
        m_audioInput = name;    
        m_session->setCaptureDevice(name);
        emit activeEndpointChanged(name);
    }
}

void S60AudioEndpointSelector::update()
{
    m_names.clear();
    m_descriptions.clear();    
    QList<QAudioDeviceInfo> devices;
    devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(int i = 0; i < devices.size(); ++i) {
        m_names.append(devices.at(i).deviceName());
        m_descriptions.append(devices.at(i).deviceName());
    }
    if (m_names.isEmpty())
        m_names.append("MMF");
}
