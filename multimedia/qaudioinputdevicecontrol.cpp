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

#include "qaudioinputdevicecontrol.h"

/*!
    \class QAudioInputDeviceControl
    \preliminary
    \brief The QAudioInputDeviceControl class provides an audio input device selector media control.

    The QAudioInputDeviceControl class provides descriptions of the audio devices available on a system
    and allows one to be selected as the input of a media service.

    The interface name of QAudioInputDeviceControl is \c com.nokia.Qt.AudioInputDeviceControl as
    defined in QAudioInputDeviceControl_iid.

*/

/*!
    \macro QAudioInputDeviceControl_iid

    \c com.nokia.Qt.AudioInputDeviceControl

    Defines the interface name of QAudioInputDeviceControl.

    \relates QAudioInputDeviceControl
*/

/*!
    Constructs a new audio input device control with the given \a parent.
*/
QAudioInputDeviceControl::QAudioInputDeviceControl(QObject *parent)
    :QAbstractMediaControl(parent)
{
}

/*!
    Destroys an audio input device control.
*/
QAudioInputDeviceControl::~QAudioInputDeviceControl()
{
}

/*!
    \fn QAudioInputDeviceControl::deviceCount() const

    Returns the number of available devices;
*/

/*!
    \fn QAudioInputDeviceControl::name(int device) const

    Returns the name of a \a device.
*/

/*!
    \fn QAudioInputDeviceControl::description(int device) const

    Returns a description of a \a device.
*/

/*!
    \fn QAudioInputDeviceControl::icon(int device) const

    Returns an icon for a \a device.
*/

/*!
    \fn QAudioInputDeviceControl::defaultDevice() const

    Returns the index of the default input device.
*/

/*!
    \property QAudioInputDeviceControl::selectedDevice

    The index of the selected audio device.
*/


/*!
    \fn QAudioInputDeviceControl::devicesChanged()

    Signals that list of available devices was changed.
*/
