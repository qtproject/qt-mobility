/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui/QIcon>
#include "v4lvideodevicecontrol.h"
#include "v4lcamerasession.h"

V4LVideoDeviceControl::V4LVideoDeviceControl(QObject *parent)
    : QVideoDeviceControl(parent)
{
    m_session = qobject_cast<V4LCameraSession*>(parent);
}

int V4LVideoDeviceControl::deviceCount() const
{
    return 0;
}

QString V4LVideoDeviceControl::name(int index) const
{
    if(index == 0)
        return QString("Camera capture device");

    return QString();
}

QString V4LVideoDeviceControl::description(int index) const
{
    Q_UNUSED(index)

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
    return 0;
}

void V4LVideoDeviceControl::setSelectedDevice(int index)
{
    Q_UNUSED(index)
}

