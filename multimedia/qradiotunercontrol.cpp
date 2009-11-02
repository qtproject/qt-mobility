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

#include <multimedia/qradiotunercontrol.h>
#include <multimedia/qmediacontrol_p.h>


/*!
    \class QRadioTunerControl
    \preliminary
    \brief The QRadioTunerControl class provides access to the radio tuning functionality of a
    QMediaService.

    If a QMediaService can tune an anolog radio device it will implement QRadioTunerControl.  This
    control provides a means to tune a radio device to a specific \l {setFrequency()}{frequency} as
    well as search \l {searchForward()}{forwards} and \l {searchBackward()}{backwards} for a
    signal.

    The functionality provided by this control is exposed to application code through the
    QRadioTuner class.

    The interface name of QRadioTunerControl is \c com.nokia.Qt.QRadioTunerControl/1.0 as
    defined in QRadioTunerControl_iid.

    \sa QMediaService::control(), QRadioTuner
*/

/*!
    \macro QRadioTunerControl_iid

    \c com.nokia.Qt.QRadioTunerControl/1.0

    Defines the interface name of the QRadioTunerControl class.

    \relates QRadioTunerControl
*/

/*!
    Construct a radio tuner control class parented to \a parent.
*/

QRadioTunerControl::QRadioTunerControl(QObject *parent):
    QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    Destruct the radio tuner control class.
*/

QRadioTunerControl::~QRadioTunerControl()
{
}

/*!
    \fn QRadioTuner::Band QRadioTunerControl::band() const

    Returns the current band being used.
*/

/*!
    \fn void QRadioTunerControl::bandChanged(QRadioTuner::Band band)

    Signal emitted when \a band changed.
*/

/*!
    \fn void QRadioTunerControl::setBand(QRadioTuner::Band band)

    Set current band to \a band.
*/

/*!
    \fn bool QRadioTunerControl::isBandSupported(QRadioTuner::Band band) const

    Return true if \a band is available.
*/

/*!
    \fn int QRadioTunerControl::frequency() const

    Returns the current frequency being used.
*/

/*!
    \fn int QRadioTunerControl::frequencyStep(QRadioTuner::Band band) const

    Returns the incremental step size in Hertz when changing frequency for \a band.
*/

/*!
    \fn QPair<int,int> QRadioTunerControl::frequencyRange(QRadioTuner::Band band) const

    Returns the frequency range for the \a band.
*/

/*!
    \fn void QRadioTunerControl::setFrequency(int frequency)

    Set the frequency to \a frequency.
*/

/*!
    \fn bool QRadioTunerControl::isStereo() const

    Return true if in stereo mode.
*/

/*!
    \fn QRadioTuner::StereoMode QRadioTunerControl::stereoMode() const

    Returns the stereo mode.

    \sa QRadioTuner::StereoMode
*/

/*!
    \fn void QRadioTunerControl::setStereoMode(QRadioTuner::StereoMode mode)

    Sets stereo mode to \a mode.

    \sa QRadioTuner::StereoMode
*/

/*!
    \fn int QRadioTunerControl::signalStrength() const

    Return the current signal strength as a percentage.
*/

/*!
    \fn int QRadioTunerControl::volume() const

    Returns the volume as a percentage.
*/

/*!
    \fn void QRadioTunerControl::setVolume(int volume)

    Set the volume to \a volume (percentage)
*/

/*!
    \fn bool QRadioTunerControl::isMuted() const

    Returns true if muted.
*/

/*!
    \fn void QRadioTunerControl::setMuted(bool muted)

    Set mute state to \a muted.
*/

/*!
    \fn bool QRadioTunerControl::isSearching() const

    Returns true if currently scanning.
*/

/*!
    \fn  void QRadioTunerControl::searchForward()

    Start scanning, ascending freqeuncy.
*/

/*!
    \fn void QRadioTunerControl::searchBackward()

    Start scanning, descending frequency.
*/

/*!
    \fn void QRadioTunerControl::cancelSearch()

    Stop any scanning being done.
*/

/*!
    \fn void QRadioTunerControl::start()

    Activate the radio device.
*/

/*!
    \fn QRadioTunerControl::stop()

    Deactivate the radio device.
*/

/*!
    \fn QRadioTuner::Error QRadioTunerControl::error() const

    Returns the error state that has occurred.
*/

/*!
    \fn QString QRadioTunerControl::errorString() const

    Returns the current error string.
*/

/*!
    \fn void QRadioTunerControl::frequencyChanged(int frequency)

    Signal emitted when \a frequency changed.
*/

/*!
    \fn void QRadioTunerControl::stereoStatusChanged(bool stereo)

    Signal emitted when \a stereo mode changed.
*/

/*!
    \fn void QRadioTunerControl::searchingStatusChanged(bool searching)

    Signal emitted when \a searching status changed.
*/

/*!
    \fn void QRadioTunerControl::signalStrengthChanged(int signalStrength)

    Signal emitted when \a signalStrength changed (percentage).
*/

/*!
    \fn void QRadioTunerControl::volumeChanged(int volume)

    Signal emitted when \a volume changed (percentage).
*/

/*!
    \fn void QRadioTunerControl::mutingChanged(bool muted)

    Signal emitted when \a muted state changed.
*/

/*!
    \fn void QRadioTunerControl::error(QRadioTuner::Error err)

    Signal emitted when an error of \a err has occurred.
*/
