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

    \preliminary
    \brief The QCameraExposureControl class supplies control for exposure
    related camera parameters.

    \ingroup camera

    The interface name of QCameraExposureControl is \c com.nokia.Qt.QCameraExposureControl/1.0 as
    defined in QCameraExposureControl_iid.

The Camera API of Qt Mobility is still in \bold Technology Preview. It has not undergone
the same level of review and testing as the rest of the APIs.

The API exposed by the classes in this component are not stable, and will
undergo modification or removal prior to the final release of Qt Mobility.

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
  \fn QCamera::FlashModes QCameraExposureControl::flashMode() const

  Returns the current flash mode.
*/


/*!
  \fn void QCameraExposureControl::setFlashMode(QCamera::FlashModes mode)

  Set the current flash \a mode.

  Usually the single QCamera::FlashMode flag is used,
  but some non conflicting flags combination are also allowed,
  like QCamera::FlashManual | QCamera::FlashSlowSyncRearCurtain.
*/


/*!
  \fn QCamera::FlashModes QCameraExposureControl::supportedFlashModes() const

  Returns the flash modes available.
*/


/*!
  \fn qreal QCameraExposureControl::::flashCompensation() const

  Returns the flash exposure compensation in EV.

  \sa setFlashCompensation()
*/

/*!
  \fn QCameraExposureControl::setFlashCompensation(qreal ev)

  Sets the flash exposure compensation to \a ev

  \sa flashCompensation()
*/


/*!
  \fn qreal QCameraExposureControl::flashPower() const

  Returns the flash power in \l{QCamera::FlashManual}{manual flash mode}.

  \sa setFlashPower()
*/


/*!
  \fn QCameraExposureControl::setFlashPower(qreal power)

  Sets the flash \a power.

  Accepted power range is [0..1.0],
  with 0 value means no flash and 1.0 corresponds to full flash power.

  This value is only used in the \l{QCamera::FlashManual}{manual flash mode}.

  \sa flashPower()
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

  Sets the exposure compensation to \a ev
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
  \fn QList<int> QCameraExposureControl::supportedIsoSensitivities(bool *continuous) const

  Returns the list of ISO senitivities camera supports.

  If the camera supports arbitrary ISO sensitivities within the supported range,
  *\a continuous is set to true, otherwise *\a continuous is set to false.
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
  \fn QList<qreal> QCameraExposureControl::supportedApertures(bool *continuous) const

    Returns the list of aperture values camera supports.
    The apertures list can change depending on the focal length,
    in such a case the apertureRangeChanged() signal is emited.

    If the camera supports arbitrary aperture values within the supported range,
    *\a continuous is set to true, otherwise *\a continuous is set to false.
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
  \fn QList<qreal> QCameraExposureControl::supportedShutterSpeeds(bool *continuous) const

    Returns the list of shutter speed values in seconds camera supports.

    If the camera supports arbitrary shutter speed values within the supported range,
    *\a continuous is set to true, otherwise *\a continuous is set to false.
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

#include "moc_qcameraexposurecontrol.cpp"
QTM_END_NAMESPACE

