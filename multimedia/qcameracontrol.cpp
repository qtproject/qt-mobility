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

#include "qcameracontrol.h"
#include  "qabstractmediacontrol_p.h"

QCameraControl::~QCameraControl()
{
}

QCameraControl::QCameraControl(QObject *parent):
    QAbstractMediaControl(*new QAbstractMediaControlPrivate, parent)
{
}


void QCameraControl::lockExposure()
{
    emit exposureLocked();
}

void QCameraControl::unlockExposure()
{
}

void QCameraControl::lockFocus()
{
    emit focusLocked();
}

void QCameraControl::unlockFocus()
{
}

QCamera::FlashMode QCameraControl::flashMode() const
{
    return QCamera::FlashOff;
}

void QCameraControl::setFlashMode(QCamera::FlashMode mode)
{
    Q_UNUSED(mode);
}

QCamera::FlashModes QCameraControl::supportedFlashModes() const
{
    return QCamera::FlashOff;
}

bool QCameraControl::isFlashReady() const
{
    return true;
}

QCamera::FocusMode QCameraControl::focusMode() const
{
    return QCamera::AutoFocus;
}

void QCameraControl::setFocusMode(QCamera::FocusMode mode)
{
    Q_UNUSED(mode);
}

QCamera::FocusModes QCameraControl::supportedFocusModes() const
{
    return QCamera::AutoFocus;
}

QCamera::FocusStatus QCameraControl::focusStatus() const
{
    return QCamera::FocusDisabled;
}

bool QCameraControl::macroFocusingEnabled() const
{
    return false;
}

bool QCameraControl::isMacroFocusingSupported() const
{
    return false;
}

void QCameraControl::setMacroFocusingEnabled(bool)
{
}

QCamera::ExposureMode QCameraControl::exposureMode() const
{
    return QCamera::ExposureAuto;
}

void QCameraControl::setExposureMode(QCamera::ExposureMode mode)
{
    Q_UNUSED(mode);
}

QCamera::ExposureModes QCameraControl::supportedExposureModes() const
{
    return QCamera::ExposureAuto;
}

double QCameraControl::exposureCompensation() const
{
    return 0;
}

void QCameraControl::setExposureCompensation(double ev)
{
    Q_UNUSED(ev);
}

QCamera::MeteringMode QCameraControl::meteringMode() const
{
    return QCamera::MeteringMatrix;
}

void QCameraControl::setMeteringMode(QCamera::MeteringMode mode)
{
    Q_UNUSED(mode);
}

QCamera::MeteringModes QCameraControl::supportedMeteringModes() const
{
    return QCamera::MeteringMatrix;
}

QCamera::WhiteBalanceMode QCameraControl::whiteBalanceMode() const
{
    return QCamera::WhiteBalanceAuto;
}

void QCameraControl::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
{
    Q_UNUSED(mode);
}

QCamera::WhiteBalanceModes QCameraControl::supportedWhiteBalanceModes() const
{
    return QCamera::WhiteBalanceAuto;
}

int QCameraControl::manualWhiteBalance() const
{
    return -1;
}

void QCameraControl::setManualWhiteBalance(int colorTemperature)
{
    Q_UNUSED(colorTemperature);
}

int QCameraControl::isoSensitivity() const
{
    return -1;
}
QPair<int, int> QCameraControl::supportedIsoSensitivityRange() const
{
    return qMakePair<int,int>(-1,-1);
}

void QCameraControl::setManualIsoSensitivity(int iso)
{
    Q_UNUSED(iso);
}

void QCameraControl::setAutoIsoSensitivity()
{
}

double QCameraControl::aperture() const
{
    return -1.0;
}
QPair<double, double> QCameraControl::supportedApertureRange() const
{
    return qMakePair<double,double>(-1,-1);
}

void QCameraControl::setManualAperture(double aperture)
{
    Q_UNUSED(aperture);
}

void QCameraControl::setAutoAperture()
{
}

double QCameraControl::shutterSpeed() const
{
    return -1;
}

QPair<double, double> QCameraControl::supportedShutterSpeedRange() const
{
    return qMakePair<double,double>(-1,-1);
}

void QCameraControl::setManualShutterSpeed(double seconds)
{
    Q_UNUSED(seconds);
}

void QCameraControl::setAutoShutterSpeed()
{
}

double QCameraControl::maximumOpticalZoom() const
{
    return 1.0;
}

double QCameraControl::maximumDigitalZoom() const
{
    return 1.0;
}

double QCameraControl::zoomValue() const
{
    return 1.0;
}

void QCameraControl::zoomTo(int value)
{
    Q_UNUSED(value);
}

bool QCameraControl::isExposureLocked() const
{
    return true;
}

bool QCameraControl::isFocusLocked() const
{
    return true;
}
