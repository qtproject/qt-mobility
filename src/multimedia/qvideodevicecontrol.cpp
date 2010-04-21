/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qvideodevicecontrol.h"

QT_BEGIN_NAMESPACE

/*!
    \class QVideoDeviceControl
    \preliminary
    \brief The QVideoDeviceControl class provides an video device selector media control.
    \ingroup multimedia-serv

    The QVideoDeviceControl class provides descriptions of the video devices
    available on a system and allows one to be selected as the  endpoint of
    a media service.

    The interface name of QVideoDeviceControl is \c com.nokia.Qt.VideoDeviceControl as
    defined in QVideoDeviceControl_iid.
*/

/*!
    \macro QVideoDeviceControl_iid

    \c com.nokia.Qt.VideoDeviceControl

    Defines the interface name of the QVideoDeviceControl class.

    \relates QVideoDeviceControl
*/

/*!
    Constructs a video device control with the given \a parent.
*/
QVideoDeviceControl::QVideoDeviceControl(QObject *parent)
    :QMediaControl(parent)
{
}

/*!
    Destroys a video device control.
*/
QVideoDeviceControl::~QVideoDeviceControl()
{
}

/*!
    \fn QVideoDeviceControl::deviceCount() const

    Returns the number of available video devices;
*/

/*!
    \fn QVideoDeviceControl::deviceName(int index) const

    Returns the name of the video device at \a index.
*/

/*!
    \fn QVideoDeviceControl::deviceDescription(int index) const

    Returns a description of the video device at \a index.
*/

/*!
    \fn QVideoDeviceControl::deviceIcon(int index) const

    Returns an icon for the video device at \a index.
*/

/*!
    \fn QVideoDeviceControl::defaultDevice() const

    Returns the index of the default video device.
*/

/*!
    \fn QVideoDeviceControl::selectedDevice() const

    Returns the index of the selected video device.
*/

/*!
    \fn QVideoDeviceControl::setSelectedDevice(int index)

    Sets the selected video device \a index.
*/

/*!
    \fn QVideoDeviceControl::devicesChanged()

    Signals that the list of available video devices has changed.
*/

/*!
    \fn QVideoDeviceControl::selectedDeviceChanged(int index)

    Signals that the selected video device \a index has changed.
*/

/*!
    \fn QVideoDeviceControl::selectedDeviceChanged(const QString &name)

    Signals that the selected video device \a name has changed.
*/

#include "moc_qvideodevicecontrol.cpp"
QT_END_NAMESPACE

