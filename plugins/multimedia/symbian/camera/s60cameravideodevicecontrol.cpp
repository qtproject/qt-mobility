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

#include "s60cameravideodevicecontrol.h"
#include "s60camerasession.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>
#include <QtGui/qicon.h>

const int defaultCameraDevice = 0;
const int defaultCameraDeviceCount = 0;

S60CameraVideoDeviceControl::S60CameraVideoDeviceControl(QObject *parent)
    :QVideoDeviceControl(parent)
{
}

S60CameraVideoDeviceControl::S60CameraVideoDeviceControl(QObject *session, QObject *parent)
   :QVideoDeviceControl(parent)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60CameraSession*>(session);
}


S60CameraVideoDeviceControl::~S60CameraVideoDeviceControl()
{
}

int S60CameraVideoDeviceControl::deviceCount() const
{
    if (m_session) {
        return m_session->deviceCount();
    }
    return defaultCameraDeviceCount;
}

QString S60CameraVideoDeviceControl::name(int index) const
{
    QString deviceName = QString();
    if (m_session) {
        deviceName = m_session->name(index);
    }
    return deviceName;

}
QString S60CameraVideoDeviceControl::description(int index) const
{
    QString deviceDesc = QString();
    if (m_session) {
        deviceDesc = m_session->description(index);
    }
    return deviceDesc;
}
QIcon S60CameraVideoDeviceControl::icon(int index) const
{
    QIcon deviceIcon = QIcon();
    if (m_session) {
        deviceIcon =  m_session->icon(index);
    }
    return deviceIcon;
}
int S60CameraVideoDeviceControl::defaultDevice() const
{
    if (m_session) {
        return m_session->defaultDevice();
    }
    return defaultCameraDevice;
}
int S60CameraVideoDeviceControl::selectedDevice() const
{
    if (m_session) {
       return m_session->selectedDevice();
    }
    return defaultCameraDevice;
}

void S60CameraVideoDeviceControl::setSelectedDevice(int index)
{
    if (m_session) {
        m_session->setSelectedDevice(index);
    }
}
