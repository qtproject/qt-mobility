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

#include "qcameracontrol.h"
#include  "qabstractmediacontrol_p.h"

/*!
    \class QCameraControl
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

/*!
    Constructs a camera control object with \a parent.
*/

QCameraControl::QCameraControl(QObject *parent):
    QAbstractMediaControl(*new QAbstractMediaControlPrivate, parent)
{
}

/*!
    Destruct the camera control object.
*/

QCameraControl::~QCameraControl()
{
}

/*!
    \fn QCameraControl::setDevice(const QByteArray& device)

    Set the camera device to \a device.
*/

/*!
    lock the exposure.
*/

void QCameraControl::lockExposure()
{
    emit exposureLocked();
}

/*!
    unlock the exposure.
*/

void QCameraControl::unlockExposure()
{
}

/*!
    lock the focus.
*/

void QCameraControl::lockFocus()
{
    emit focusLocked();
}

/*!
    unlock the focus.
*/

void QCameraControl::unlockFocus()
{
}

/*!
    Returns current flash mode.
*/

QCamera::FlashMode QCameraControl::flashMode() const
{
    return QCamera::FlashOff;
}

/*!
    Set the current flash \a mode.
*/

void QCameraControl::setFlashMode(QCamera::FlashMode mode)
{
    Q_UNUSED(mode);
}

/*!
    Returns the flash modes available.
*/

QCamera::FlashModes QCameraControl::supportedFlashModes() const
{
    return QCamera::FlashOff;
}

/*!
    Returns true if flash is charged.
*/

bool QCameraControl::isFlashReady() const
{
    return true;
}

/*!
    Returns the focus mode being used.
*/

QCamera::FocusMode QCameraControl::focusMode() const
{
    return QCamera::AutoFocus;
}

/*!
    Set the focus mode to \a mode.
*/

void QCameraControl::setFocusMode(QCamera::FocusMode mode)
{
    Q_UNUSED(mode);
}

/*!
    Returns focus modes available.
*/

QCamera::FocusModes QCameraControl::supportedFocusModes() const
{
    return QCamera::AutoFocus;
}

/*!
    Returns the focus status.
*/

QCamera::FocusStatus QCameraControl::focusStatus() const
{
    return QCamera::FocusDisabled;
}

/*!
    Returns true if macro focusing enabled.
*/

bool QCameraControl::macroFocusingEnabled() const
{
    return false;
}

/*!
    Returns true if macro focusing is available.
*/

bool QCameraControl::isMacroFocusingSupported() const
{
    return false;
}

/*!
    Set macro focusing to \a enable
*/

void QCameraControl::setMacroFocusingEnabled(bool enable)
{
    Q_UNUSED(enable);
}

/*!
    Returns the exposure mode.
*/

QCamera::ExposureMode QCameraControl::exposureMode() const
{
    return QCamera::ExposureAuto;
}

/*!
    Set the exposure mode to \a mode.
*/

void QCameraControl::setExposureMode(QCamera::ExposureMode mode)
{
    Q_UNUSED(mode);
}

/*!
    Returns the exposure modes available.
*/

QCamera::ExposureModes QCameraControl::supportedExposureModes() const
{
    return QCamera::ExposureAuto;
}

/*!
    Return the exposure compensation.
*/

double QCameraControl::exposureCompensation() const
{
    return 0;
}

/*!
   Set the exposure compensation to \a ev
*/

void QCameraControl::setExposureCompensation(double ev)
{
    Q_UNUSED(ev);
}

/*!
    Returns the current metering mode.
*/

QCamera::MeteringMode QCameraControl::meteringMode() const
{
    return QCamera::MeteringMatrix;
}

/*!
    Set the metering mode to \a mode.
*/

void QCameraControl::setMeteringMode(QCamera::MeteringMode mode)
{
    Q_UNUSED(mode);
}

/*!
    Return the metering modes available.
*/

QCamera::MeteringModes QCameraControl::supportedMeteringModes() const
{
    return QCamera::MeteringMatrix;
}

/*!
    Return the white balance mode being used.
*/

QCamera::WhiteBalanceMode QCameraControl::whiteBalanceMode() const
{
    return QCamera::WhiteBalanceAuto;
}

/*!
    Set the white balance mode to \a mode
*/

void QCameraControl::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
{
    Q_UNUSED(mode);
}

/*!
    Return the white balance modes available.
*/

QCamera::WhiteBalanceModes QCameraControl::supportedWhiteBalanceModes() const
{
    return QCamera::WhiteBalanceAuto;
}

/*!
    Return the white balance.
*/

int QCameraControl::manualWhiteBalance() const
{
    return -1;
}

/*!
    Set the white balance to \a colorTemperature
*/

void QCameraControl::setManualWhiteBalance(int colorTemperature)
{
    Q_UNUSED(colorTemperature);
}

/*!
    Return the sensitivity
*/

int QCameraControl::isoSensitivity() const
{
    return -1;
}

/*!
    Return the available sensitivity ranges.
*/

QPair<int, int> QCameraControl::supportedIsoSensitivityRange() const
{
    return qMakePair<int,int>(-1,-1);
}

/*!
    Set the sensitivity to \a iso
*/

void QCameraControl::setManualIsoSensitivity(int iso)
{
    Q_UNUSED(iso);
}

/*!
    Turn on auto sensitivity
*/

void QCameraControl::setAutoIsoSensitivity()
{
}

/*!
    Return the aperture
*/

double QCameraControl::aperture() const
{
    return -1.0;
}

/*!
    Return the available aperture ranges.
*/

QPair<double, double> QCameraControl::supportedApertureRange() const
{
    return qMakePair<double,double>(-1,-1);
}

/*!
    Set the aperture to \a aperture
*/

void QCameraControl::setManualAperture(double aperture)
{
    Q_UNUSED(aperture);
}

/*!
    Turn on auto aperture
*/

void QCameraControl::setAutoAperture()
{
}

/*!
    Return the shutter speed
*/

double QCameraControl::shutterSpeed() const
{
    return -1;
}

/*!
    Return the available shutter speed ranges.
*/

QPair<double, double> QCameraControl::supportedShutterSpeedRange() const
{
    return qMakePair<double,double>(-1,-1);
}

/*!
    Set the shutter speed to \a seconds
*/

void QCameraControl::setManualShutterSpeed(double seconds)
{
    Q_UNUSED(seconds);
}

/*!
    Turn on auto shutter speed
*/

void QCameraControl::setAutoShutterSpeed()
{
}

/*!
    Return the maximum optical zoom
*/

double QCameraControl::maximumOpticalZoom() const
{
    return 1.0;
}

/*!
    Return the maximum digital zoom
*/

double QCameraControl::maximumDigitalZoom() const
{
    return 1.0;
}

/*!
    Return the zoom value.
*/

double QCameraControl::zoomValue() const
{
    return 1.0;
}

/*!
    Set the zoom value to \a value
*/

void QCameraControl::zoomTo(int value)
{
    Q_UNUSED(value);
}

/*!
    Return true if exposure locked
*/

bool QCameraControl::isExposureLocked() const
{
    return true;
}

/*!
    Return true if focus locked.
*/

bool QCameraControl::isFocusLocked() const
{
    return true;
}

/*!
    \fn void QCameraControl::focusLocked()

    Signal emitted when focus is locked.
*/

/*!
    \fn void QCameraControl::focusStatusChanged(QCamera::FocusStatus status)

    Signal emitted when focus \a status changed.
*/

/*!
    \fn void QCameraControl::exposureLocked()

    Signal emitted when exposure locked.
*/

/*!
    \fn void QCameraControl::flashReady(bool ready)

    Signal emitted when flash state changes, flash is charged \a ready.
*/

/*!
    \fn void QCameraControl::zoomValueChanged(double zoom)

    Signal emitted when \a zoom value changed.
*/

/*!
    \fn void QCameraControl::stateChanged(QCamera::State state)

    Signal emitted when \a state changes state.
*/


