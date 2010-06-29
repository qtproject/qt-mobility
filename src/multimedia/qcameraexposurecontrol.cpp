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

#include <qcameraexposurecontrol.h>
#include  "qmediacontrol_p.h"

QT_BEGIN_NAMESPACE

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
  \fn QCamera::FlashModes QCameraExposureControl::flashMode() const

  Returns the current flash mode.
*/


/*!
  \fn void QCameraExposureControl::setFlashMode(QCameraExposure::FlashModes mode)

  Set the current flash \a mode.

  Usually the single QCameraExposure::FlashMode flag is used,
  but some non conflicting flags combination are also allowed,
  like QCameraExposure::FlashManual | QCameraExposure::FlashSlowSyncRearCurtain.
*/


/*!
  \fn QCameraExposureControl::isFlashModeSupported(QCameraExposure::FlashModes mode) const

  Return true if reqested flash \a mode is supported.
  Some QCameraExposure::FlashMode values can be combined,
  for example QCameraExposure::FlashManual | QCameraExposure::FlashSlowSyncRearCurtain
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
  \fn void QCameraExposureControl::setExposureMode(QCameraExposure::ExposureMode mode)

  Set the exposure mode to \a mode.
*/


/*!
  \fn bool QCameraExposureControl::isExposureModeSupported(QCameraExposure::ExposureMode mode) const

  Returns true if the exposure \a mode is supported.
*/


/*!
  \fn QCameraExposure::MeteringMode QCameraExposureControl::meteringMode() const
  Returns the current metering mode.
*/

/*!
  \fn void QCameraExposureControl::setMeteringMode(QCameraExposure::MeteringMode mode)

Set the metering mode to \a mode.
*/


/*!
  \fn bool QCameraExposureControl::isMeteringModeSupported(QCameraExposure::MeteringMode mode) const
    Returns true if the metering \a mode is supported.
*/

/*!
  \enum QCameraExposureControl::ExposureParameter
  \value ISO
         Camera ISO sensitivity, specified as integer value.
  \value Aperture
         Lens aperture is specified as an qreal F number.
         The supported apertures list can change depending on the focal length,
         in such a case the exposureParameterRangeChanged() signal is emited.
  \value ShutterSpeed
         Shutter speed in seconds, specified as qreal.
  \value ExposureCompensation
         Exposure compensation, specified as qreal EV value.
  \value FlashPower
         Manual flash power, specified as qreal value.
         Accepted power range is [0..1.0],
         with 0 value means no flash and 1.0 corresponds to full flash power.

         This value is only used in the \l{QCameraExposure::FlashManual}{manual flash mode}.
  \value FlashCompensation
         Flash compensation, specified as qreal EV value.
  \value ExtendedExposureParameter
         The base value for platform specific extended parameters.
         For such parameters the sequential values starting from ExtendedExposureParameter shuld be used.
*/

/*!
  \fn QCameraExposureControl::isParameterSupported(ExposureParameter parameter) const

  Returns true is exposure \a parameter is supported by backend.
*/

/*!
  \fn QCameraExposureControl::exposureParameter(ExposureParameter parameter) const

  Returns the exposure \a parameter value, or invalid QVariant() if the value is unknown or not supported.
*/


/*!
  \fn QCameraExposureControl::supportedParameterRange(ExposureParameter parameter, bool *continuous) const

  Returns the list of supported \a parameter values;

  If the camera supports arbitrary values within the supported range,
  *\a continuous is set to true, otherwise *\a continuous is set to false.
  In this case at least the minimum and maximim values should be returned in the list.
  Backend still can return more than two values if some of them are preffered intended to be displayed to user.
*/

/*!
  \fn bool QCameraExposureControl::setExposureParameter(ExposureParameter parameter, const QVariant& value)

  Set the exposure \a parameter to \a value.
  If a null or invalid QVariant is passed, backend should choose the value automaticaly,
  and if possible report the actual value to user with QCameraExposureControl::exposureParameter().

  Returns true if parameter is supported and value is correct.
*/

/*!
  \fn QCameraExposureControl::extendedParameterName(ExposureParameter parameter)

  Returns the extended exposure \a parameter name.
*/

/*!
    \fn void QCameraExposureControl::flashReady(bool ready)

    Signal emitted when flash state changes, flash is charged \a ready.
*/

/*!
    \fn void QCameraExposureControl::exposureParameterChanged(int parameter)

    Signal emitted when the exposure \a parameter has changed to \a value.
*/

/*!

    \fn void QCameraExposureControl::exposureParameterRangeChanged(int parameter)

    Signal emitted when the exposure \a parameter range has changed.
*/


#include "moc_qcameraexposurecontrol.cpp"
QT_END_NAMESPACE

