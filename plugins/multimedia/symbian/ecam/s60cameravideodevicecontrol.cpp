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
#include "s60cameracontrol.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>
#include <QtGui/qicon.h>

S60CameraVideoDeviceControl::S60CameraVideoDeviceControl(QObject *parent)
    :QVideoDeviceControl(parent), m_selectedDevice(0)
{
}

S60CameraVideoDeviceControl::S60CameraVideoDeviceControl(QObject *control, QObject *parent)
   :QVideoDeviceControl(parent), m_selectedDevice(0)
{
    m_control = qobject_cast<S60CameraControl*>(control);
}

S60CameraVideoDeviceControl::~S60CameraVideoDeviceControl()
{
}

int S60CameraVideoDeviceControl::deviceCount() const
{
    qDebug() << "S60CameraVideoDeviceControl::deviceCount" ;
    return S60CameraControl::deviceCount();
}

QString S60CameraVideoDeviceControl::deviceName(int index) const
{
    qDebug() << "S60CameraVideoDeviceControl::deviceName, index=" << index;
    return S60CameraControl::name(index);
}

QString S60CameraVideoDeviceControl::deviceDescription(int index) const
{
    qDebug() << "S60CameraVideoDeviceControl::deviceDescription, index=" << index;
    return S60CameraControl::description(index);
}

QIcon S60CameraVideoDeviceControl::deviceIcon(int index) const
{
    //qDebug() << "S60CameraVideoDeviceControl::deviceIcon(), index="<<index;
    return QIcon();
}

int S60CameraVideoDeviceControl::defaultDevice() const
{    
    return m_control->defaultDevice();   
}

int S60CameraVideoDeviceControl::selectedDevice() const
{
    qDebug() << "S60CameraVideoDeviceControl::selectedDevice() called";
    return m_selectedDevice;
}

void S60CameraVideoDeviceControl::setSelectedDevice(int index)
{
    qDebug() << "S60CameraVideoDeviceControl::setSelectedDevice(), index="<<index;
    // inform that we selected new device
    if (m_selectedDevice != index) {
        emit selectedDeviceChanged(index);
        emit selectedDeviceChanged(deviceName(index));
        m_selectedDevice = index;            
        m_control->setSelectedDevice(m_selectedDevice);
    }    
}
