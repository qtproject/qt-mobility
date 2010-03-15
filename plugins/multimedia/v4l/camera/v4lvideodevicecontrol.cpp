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

#include <QtCore/qfile.h>
#include <QtCore/qdir.h>
#include <QtGui/qicon.h>

#include "v4lvideodevicecontrol.h"
#include "v4lcamerasession.h"

V4LVideoDeviceControl::V4LVideoDeviceControl(QObject *parent)
    : QVideoDeviceControl(parent)
{
    m_session = qobject_cast<V4LCameraSession*>(parent);

    QDir cameras = QDir::root();
    cameras.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    if (!cameras.cd("sys/class/video4linux"))
        return;
    else {
        QFileInfoList list = cameras.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            if (fileInfo.fileName().contains(QLatin1String("video"))) {
                QString videoName = QString("%1/%2/name").arg("/sys/class/video4linux").
                    arg(fileInfo.fileName());
                QFile videoFile(videoName);
                if (videoFile.exists()) {
                    if (videoFile.open(QIODevice::ReadOnly)) {
                        char str[31];
                        memset(str,0,31);
                        if (videoFile.read(str,30) > 0) {
                            devices.append(QString("v4l:/dev/%1").arg(fileInfo.fileName()).
                                    toLocal8Bit().constData());
                            descriptions.append(QString(str).simplified());
                        }
                        videoFile.close();
                    }
                }
            }
        }
    }
    selected = 0;
}

int V4LVideoDeviceControl::deviceCount() const
{
    return devices.count();
}

QString V4LVideoDeviceControl::deviceName(int index) const
{
    if(index >= 0 && index <= devices.count())
        return devices.at(index);

    return QString();
}

QString V4LVideoDeviceControl::deviceDescription(int index) const
{
    if(index >= 0 && index <= descriptions.count())
        return descriptions.at(index);

    return QString();
}

QIcon V4LVideoDeviceControl::deviceIcon(int index) const
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

