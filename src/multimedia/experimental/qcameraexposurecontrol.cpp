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

#include <experimental/qcameraexposurecontrol.h>
#include  "qmediacontrol_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QCameraExposureControl
    \ingroup multimedia

    \preliminary
    \brief The QCameraExposureControl class supplies control for exposure
    related camera parameters.

    The interface name of QCameraExposureControl is \c com.nokia.Qt.QCameraExposureControl/1.0 as
    defined in QCameraExposureControl_iid.

    \sa QMediaService::control(), QCamera
*/

/*!
    \macro QCameraExposureControl_iid

    \c com.nokia.Qt.QCameraExposureControl/1.0

    Defines the interface name of the QCameraExposureControl class.

    \relates QCameraExposureControl
*/

/*!
    Constructs a camera exposure control object with \a parent.
*/

QCameraExposureControl::QCameraExposureControl(QObject *parent):
    QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    Destruct the camera control object.
*/

QCameraExposureControl::~QCameraExposureControl()
{
}

/*!
  \fn void QCameraExposureControl::lockExposure()

  Lock the exposure.
*/


/*!
  \fn void QCameraExposureControl::unlockExposure()

  Unlock the exposure.
*/


/*!
  \fn QCamera::FlashMode QCameraExposureControl::flashMode() const

  Returns the current flash mode.
*/


/*!
  \fn void QCameraExposureControl::setFlashMode(QCamera::FlashMode mode)

  Set the current flash \a mode.
*/


/*!
  \fn QCamera::FlashModes QCameraExposureControl::supportedFlashModes() const

  Returns the flash modes available.
*/


/*!
  \fn bool QCameraExposureControl::isFlashReady() const

  Returns true if flash is charged.
*/



/*!
  \fn QCamera::ExposureMode QCameraExposureControl::exposureMode() const

  Returns the exposure mode.
*/


/*!
  \fn void QCameraExposureControl::setExposureMode(QCamera::ExposureMode mode)

  Set the exposure mode to \a mode.
*/


/*!
  \fn QCamera::ExposureModes QCameraExposureControl::supportedExposureModes() const

  Returns the available exposure modes.
*/


/*!
  \fn qreal QCameraExposureControl::exposureCompensation() const

  Returns the exposure compensation in EV.
*/


/*!
  \fn void QCameraExposureControl::setExposureCompensation(qreal ev)

  Set the exposure compensation to \a ev
*/



/*!
  \fn QCamera::MeteringMode QCameraExposureControl::meteringMode() const
  Returns the current metering mode.
*/


/*!
  \fn void QCameraExposureControl::setMeteringMode(QCamera::MeteringMode mode)

Set the metering mode to \a mode.
*/


/*!
  \fn QCamera::MeteringModes QCameraExposureControl::supportedMeteringModes() const
    Return the metering modes available.
*/

/*!
  \fn int QCameraExposureControl::isoSensitivity() const

  Returns the ISO sensitivity, or -1 if unknown.
*/

/*!
  \fn int QCameraExposureControl::minimumIsoSensitivity() const

  Returns the minimum iso sensitivity.
*/

/*!
  \fn int QCameraExposureControl::maximumIsoSensitivity() const

  Returns the maximum iso sensitivity.
*/

/*!
  \fn QList<int> QCameraExposureControl::supportedIsoSensitivities() const

  Returns the available sensitivities.
*/


/*!
  \fn void QCameraExposureControl::setManualIsoSensitivity(int iso)

  Set the sensitivity to \a iso
*/


/*!
  \fn void QCameraExposureControl::setAutoIsoSensitivity()

  Turn on auto sensitivity
*/


/*!
  \fn qreal QCameraExposureControl::aperture() const

  Returns the aperture
*/

/*!
  \fn qreal QCameraExposureControl::minimumAperture() const

  Returns the minimum aperture available.
*/

/*!
  \fn qreal QCameraExposureControl::maximumAperture() const

  Returns the maximum aperture available.
*/

/*!
  \fn QList<qreal> QCameraExposureControl::supportedApertures() const

  Returns the supported apertures.
*/


/*!
  \fn void QCameraExposureControl::setManualAperture(qreal aperture)

  Set the aperture to \a aperture
*/


/*!
  \fn void QCameraExposureControl::setAutoAperture()

  Turn on auto aperture
*/



/*!
  \fn qreal QCameraExposureControl::shutterSpeed() const

  Returns the shutter speed
*/

/*!
    \fn qreal QCameraExposureControl::minimumShutterSpeed() const

    Returns the maximum shutter speed.
*/

/*!
  \fn qreal QCameraExposureControl::maximumShutterSpeed() const

  Returns the maximum shutter speed.
*/

/*!
  \fn QList<qreal> QCameraExposureControl::supportedShutterSpeeds() const

  Returns the available shutter speeds.
*/


/*!
  \fn void QCameraExposureControl::setManualShutterSpeed(qreal seconds)

  Set the shutter speed to \a seconds
*/


/*!
  \fn void QCameraExposureControl::setAutoShutterSpeed()

  Turn on auto shutter speed
*/



/*!
  \fn bool QCameraExposureControl::isExposureLocked() const

  Returns true if exposure locked
*/

/*!
    \fn void QCameraExposureControl::exposureLocked()

    Signal emitted when exposure locked.
*/

/*!
    \fn void QCameraExposureControl::flashReady(bool ready)

    Signal emitted when flash state changes, flash is charged \a ready.
*/

/*!
    \fn void QCameraExposureControl::apertureChanged(qreal value)

    Signal emitted when the aperture value has changed to \a value.
*/

/*!
    \fn void QCameraExposureControl::apertureRangeChanged()

    Signal emitted when the aperture range has changed.
*/

/*!
    \fn void QCameraExposureControl::shutterSpeedChanged(qreal value)

    Signal emitted when the shutter speed changes to \a value.
*/

/*!
    \fn void QCameraExposureControl::isoSensitivityChanged(int value)

    Signal emitted when sensitity value changes to \a value.
*/

QTM_END_NAMESPACE

