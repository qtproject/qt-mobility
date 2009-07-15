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

    QAudioDeviceEndpoint::DeviceDirection directionFilter;
    QAudioDeviceEndpoint::Roles roleFilter;
    QAudioDeviceEndpoint::FormFactors formFactorsFilter;
    int selectedDevice;
};

/*!
    \class QAudioDeviceEndpoint
    \preliminary
    \brief The QAudioDeviceEndpoint class provides an audio device selector media end point.

    The QAudioDeviceEndpoint class provides descriptions of the audio devices available on a system
    and allows one to be selected as the input or output of a media service.

    The interface name of QAudioDeviceEndpoint is \c com.nokia.Qt.QAudioDeviceEndpoint/1.0 as
    defined in QAudioDeviceEndpoint_iid.

    \sa QAbstractMediaService::setAudioInput(), QAbstractMediaService::setAudioOutput()
*/

/*!
    \macro QAudioDeviceEndpoint_iid

    \c com.nokia.Qt.QAudioDeviceEndpoint/1.0

    Defines the interface name of QAudioDeviceEndpoint.

    \relates QAudioDeviceEndpoint
*/

/*!
    \enum QAudioDeviceEndpoint::DeviceDirection

    Identifies if an audio device produces or consumes audio data.

    \value InputDevice The device produces audio data.
    \value OutputDevice The device consumes audio data.
    \value InputOutputDevice  The device both produces and consumes audio data.
*/

/*!
    \enum QAudioDeviceEndpoint::Role

    Identifies the type of use an audio is appropriate for.

    \value ConsoleRole A device appropriate for system notifications, or game sounds.
    \value MultimediaRole A device appropriate for music or video audio.
    \value CommunicationsRole A device appropriate for voice or video calls.
    \value AllRoles Mask for all roles.
*/

/*!
    \enum QAudioDeviceEndpoint::FormFactor

    Identifies the form factor of a device.

    \value UnknownFormFactor The form factor of the device is unknown.
    \value RemoteNetworkDevice The device outputs to a remote network device.
    \value Speakers The device outputs to speakers.
    \value LineLevel The device renders to a line level socket.
    \value Headphones The device outputs to head phones.
    \value Microphone The devices receives input from a microphone.
    \value Headset The device is a headset.
    \value Handset The device is a handset.
    \value UnknownDigitalPassthrough The device outputs to an unknown digital pass-through.
    \value SPDIF The device outputs to an SPDIF socket.
    \value HDMI The device outputs to an HDMI socket.
    \value AllFormFactors Mask for all form factors.
*/

/*!
    Contructs a new audio device selector media end point with the given \a parent.
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
    \reimp
*/
QMediaEndpointInterface::Direction QAudioDeviceEndpoint::direction() const
{
    return InputOutput;
}

/*!
    \property QAudioDeviceEndpoint::directionFilter

    The direction selectable devices are filtered on.
*/

/*!
    Returns the direction selectable devices are filtered on.
*/
QAudioDeviceEndpoint::DeviceDirection QAudioDeviceEndpoint::directionFilter() const
{
    return d_func()->directionFilter;
}

/*!
    Sets the \a direction selectable devices are filtered on.
*/
void QAudioDeviceEndpoint::setDirectionFilter(DeviceDirection direction)
{
    d_func()->directionFilter = direction;
}

/*!
    \property QAudioDeviceEndpoint::roleFilter

    The roles selectable devices are filtered on.
*/

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
    \property QAudioDeviceEndpoint::formFactorFilter

    The form factors selectable devices are filtered on.
*/

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
    \fn QAudioDeviceEndpoint::name(int device) const

    Returns the name of a \a device.
*/

/*!
    \fn QAudioDeviceEndpoint::description(int device) const

    Returns a description of a \a device.
*/

/*!
    \fn QAudioDeviceEndpoint::icon(int device) const

    Returns an icon for a \a device.
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
    \property QAudioDeviceEndpoint::selectedDevice

    The index of the selected audio device.
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
    emit selectedDeviceChanged(name(device));
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
