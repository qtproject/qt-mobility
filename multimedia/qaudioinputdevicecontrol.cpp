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
