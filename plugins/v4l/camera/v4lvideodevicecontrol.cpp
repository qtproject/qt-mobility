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

#include <QFile>
#include <QtGui/QIcon>
#include "v4lvideodevicecontrol.h"
#include "v4lcamerasession.h"

V4LVideoDeviceControl::V4LVideoDeviceControl(QObject *parent)
    : QVideoDeviceControl(parent)
{
    m_session = qobject_cast<V4LCameraSession*>(parent);

    QString name;
    QFile video0("/sys/class/video4linux/video0/name");
    if (video0.exists()) {
        devices.append("v4l:/dev/video0");
        char str[31];
        memset(str,0,31);
        video0.open(QIODevice::ReadOnly);
        video0.read(str,30);
        name = QString(str);
        descriptions.append(name.simplified());
        video0.close();
    }

    QFile video1("/sys/class/video4linux/video1/name");
    if (video0.exists()) {
        devices.append("v4l:/dev/video1");
        char str[31];
        memset(str,0,31);
        video1.open(QIODevice::ReadOnly);
        video1.read(str,30);
        name = QString(str);
        descriptions.append(name.simplified());
        video1.close();
    }
    selected = 0;
}

int V4LVideoDeviceControl::deviceCount() const
{
    return devices.count();
}

QString V4LVideoDeviceControl::name(int index) const
{
    if(index >= 0 && index <= devices.count())
        return devices.at(index);

    return QString();
}

QString V4LVideoDeviceControl::description(int index) const
{
    if(index >= 0 && index <= descriptions.count())
        return descriptions.at(index);

    return QString();
}

QIcon V4LVideoDeviceControl::icon(int index) const
{
    Q_UNUSED(index)

    return QIcon();
}

int V4LVideoDeviceControl::defaultDevice() const
{
    return 0;
}

int V4LVideoDeviceControl::selectedDevice() const
{
    return selected;
}

void V4LVideoDeviceControl::setSelectedDevice(int index)
{
    if(index >= 0 && index <= devices.count()) {
        if (m_session) {
            QString device = devices.at(index);
            if (device.startsWith("v4l:"))
                device.remove(0,4);
            m_session->setDevice(device);
        }
        selected = index;
    }
}

