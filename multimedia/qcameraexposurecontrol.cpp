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

#include <multimedia/qcameraexposurecontrol.h>
#include  "qabstractmediacontrol_p.h"

/*!
    \class QCameraExposureControl
    \ingroup multimedia

    \preliminary
    \brief Media control for exposure related camera parameters.

    \sa QCamera
*/

/*!
    Constructs a camera exposure control object with \a parent.
*/

QCameraExposureControl::QCameraExposureControl(QObject *parent):
    QAbstractMediaControl(*new QAbstractMediaControlPrivate, parent)
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
  \fn QPair<int, int> QCameraExposureControl::supportedIsoSensitivityRange() const

  Returns the available sensitivity ranges.
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
  \fn QPair<qreal, qreal> QCameraExposureControl::supportedApertureRange() const

  Returns the supported aperture ranges.
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
  \fn QPair<qreal, qreal> QCameraExposureControl::supportedShutterSpeedRange() const

  Returns the available shutter speed ranges.
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
