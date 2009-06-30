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

#include "qaudiodeviceendpoint.h"

#include <private/qobject_p.h>

/*!
    \class QAudioDeviceEndpointInterface
    \preliminary
    \internal
    \brief The QAudioDeviceEndpointInterface class provides an interface for audio device selector
    media end points.
*/

/*!
*/
QAudioDeviceEndpointInterface::~QAudioDeviceEndpointInterface()
{
}

class QAudioDeviceEndpointPrivate : public QObjectPrivate
{
public:
    QAudioDeviceEndpointPrivate()
        : directionFilter(QAudioDeviceEndpoint::InputOutputDevice)
        , roleFilter(QAudioDeviceEndpoint::AllRoles)
        , formFactorsFilter(QAudioDeviceEndpoint::AllFormFactors)
        , selectedDevice(0)
    {
    }

    QAudioDeviceEndpoint::Direction directionFilter;
    QAudioDeviceEndpoint::Roles roleFilter;
    QAudioDeviceEndpoint::FormFactors formFactorsFilter;
    int selectedDevice;
};

/*!
    \class QAudioDeviceEndpoint
    \preliminary
    \brief The QAudioDeviceEndpoint class provides an audio device selector media end point.
*/

/*!
    \enum QAudioDeviceEndpoint::Direction

    \value InputDevice
    \value OutputDevice
    \value InputOutputDevice
*/

/*!
    \enum QAudioDeviceEndpoint::Role

    \value ConsoleRole
    \value MultimediaRole
    \value CommunicationsRole
*/

/*!
    \enum QAudioDeviceEndpoint::FormFactor

    \value UnknownFormFactor
    \value RemoteNetworkDevice
    \value Speakers
    \value LineLevel
    \value Headphones
    \value Microphone
    \value Headset
    \value Handset
    \value UnknownDigitalPassthrough,
    \value SPDIF
    \value HDMI
*/

/*!
    Contructs a new audio device selector media end point.
*/
QAudioDeviceEndpoint::QAudioDeviceEndpoint(QObject *parent)
    : QObject(*new QAudioDeviceEndpointPrivate, parent)
{
}

/*!
    Destroys an audio device end point.
*/
QAudioDeviceEndpoint::~QAudioDeviceEndpoint()
{
}

/*!
    Returns the direction selectable devices are filtered on.
*/
QAudioDeviceEndpoint::Direction QAudioDeviceEndpoint::directionFilter() const
{
    return d_func()->directionFilter;
}

/*!
    Sets the \a direction selectable devices are filtered on.
*/
void QAudioDeviceEndpoint::setDirectionFilter(Direction direction)
{
    d_func()->directionFilter = direction;
}

/*!
    Returns the roles selectable devices are filtered on.
*/
QAudioDeviceEndpoint::Roles QAudioDeviceEndpoint::roleFilter() const
{
    return d_func()->roleFilter;
}

/*!
    Sets the \a roles selectables devices are filtered on.
*/
void QAudioDeviceEndpoint::setRoleFilter(Roles roles)
{
    d_func()->roleFilter = roles;
}

/*!
    Returns the form factors selectable devices are filtered on.
*/
QAudioDeviceEndpoint::FormFactors QAudioDeviceEndpoint::formFactorFilter() const
{
    return d_func()->formFactorsFilter;
}

/*!
    Sets the \a form factors selectable devices are filtered on.
*/
void QAudioDeviceEndpoint::setFormFactorFilter(FormFactors form)
{
    d_func()->formFactorsFilter = form;
}

/*!
    \fn QAudioDeviceEndpoint::deviceCount() const

    Returns the number of available devices;
*/

/*!
    \fn QAudioDeviceEndpoint::direction(int device) const

    Returns the direction of a \a device
*/

/*!
    \fn QAudioDeviceEndpoint::roles(int device) const

    Returns the roles of a \a device.
*/

/*!
    \fn QAudioDeviceEndpoint::formFactor(int device) const

    Returns the form factor of a \a device.
*/

/*!
    \fn QAudioDeviceEndpoint::defaultInputDevice(Role role) const

    Returns the index of the default input device for a \a role.
*/

/*!
    \fn QAudioDeviceEndpoint::defaultOutputDevice(Role role) const

    Returns the index of the default output device for a \a role.
*/

/*!
    Returns index of the selected audio device.
*/
int QAudioDeviceEndpoint::selectedDevice() const
{
    return d_func()->selectedDevice;
}

/*!
    Sets the selected \a device.
*/
void QAudioDeviceEndpoint::setSelectedDevice(int device)
{
    emit selectedDeviceChanged(d_func()->selectedDevice = device);
}

/*!
    \fn QAudioDeviceEndpoint::selectedDeviceChanged(int index)

    Signals that the \a index of the selected device has changed.
*/

/*!
    \fn QAudioDeviceEndpoint::devicesInserted(int index, int count)

    Signals that \a count new  audio devices have been inserted into the list of available devices
    at \a index.
*/

/*!
    \fn QAudioDeviceEndpoint::devicesRemoved(int index, int count)

    Signals that \a count audio devices have been removed from the list of available devices at
    \a index.
*/

/*!
    \fn QAudioDeviceEndpoint::devicesChanged(int index, int count)

    Signals that \a count audio devices have changed at \a index.
*/
