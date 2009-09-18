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

#include "s60cameracontrol.h"
#include "s60cameraservice.h"
#include "s60camerasession.h"

#include <QtCore/qdebug.h>


S60CameraControl::S60CameraControl(QObject *parent)
    :QCameraControl(parent)
{
    m_session = qobject_cast<S60CameraSession*>(parent);
}

S60CameraControl::S60CameraControl(S60CameraService *service, QObject *parent)
   :QCameraControl(parent), m_service(service)
{
    m_session = qobject_cast<S60CameraSession*>(parent);
}

S60CameraControl::~S60CameraControl()
{
}

void S60CameraControl::start()
{
    // start the default (primary camera)
    // this is also called with enable preview.
    m_session->startCamera();
}

void S60CameraControl::stop()
{
    m_session->stop();
}

QCamera::State S60CameraControl::state() const
{
    return (QCamera::State)m_session->state();
}

QCamera::FlashMode S60CameraControl::flashMode() const
{
    return QCamera::FlashOff;
}

void S60CameraControl::setFlashMode(QCamera::FlashMode mode)
{
    Q_UNUSED(mode)
}

QCamera::FlashModes S60CameraControl::supportedFlashModes() const
{
    return QCamera::FlashOff;
}

bool S60CameraControl::isFlashReady() const
{
    return false;
}

QCamera::FocusMode S60CameraControl::focusMode() const
{
    return QCamera::AutoFocus;
}

void S60CameraControl::setFocusMode(QCamera::FocusMode mode)
{
    Q_UNUSED(mode)
}

QCamera::FocusModes S60CameraControl::supportedFocusModes() const
{
    return QCamera::AutoFocus;
}

QCamera::FocusStatus S60CameraControl::focusStatus() const
{
    return QCamera::FocusDisabled;
}

bool S60CameraControl::macroFocusingEnabled() const
{
    return false;
}

bool S60CameraControl::isMacroFocusingSupported() const
{
    return false;
}

void S60CameraControl::setMacroFocusingEnabled(bool e)
{
    Q_UNUSED(e)
}

QCamera::ExposureMode S60CameraControl::exposureMode() const
{
    return QCamera::ExposureManual;
}

void S60CameraControl::setExposureMode(QCamera::ExposureMode mode)
{
    Q_UNUSED(mode)
}

QCamera::ExposureModes S60CameraControl::supportedExposureModes() const
{
    return QCamera::ExposureManual;
}

double S60CameraControl::exposureCompensation() const
{
    return 0;
}

void S60CameraControl::setExposureCompensation(double ev)
{
    Q_UNUSED(ev)
}

QCamera::MeteringMode S60CameraControl::meteringMode() const
{
    return QCamera::MeteringAverage;
}

void S60CameraControl::setMeteringMode(QCamera::MeteringMode mode)
{
    Q_UNUSED(mode)
}

QCamera::MeteringModes S60CameraControl::supportedMeteringModes() const
{
    return QCamera::MeteringAverage;
}

QCamera::WhiteBalanceMode S60CameraControl::whiteBalanceMode() const
{
    return QCamera::WhiteBalanceManual;
}

void S60CameraControl::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
{
    Q_UNUSED(mode)
}

QCamera::WhiteBalanceModes S60CameraControl::supportedWhiteBalanceModes() const
{
    return QCamera::WhiteBalanceManual;
}

int S60CameraControl::manualWhiteBalance() const
{
    return 0;
}

void S60CameraControl::setManualWhiteBalance(int colorTemperature)
{
    Q_UNUSED(colorTemperature)
}

int S60CameraControl::isoSensitivity() const
{
    return 0;
}

QPair<int, int> S60CameraControl::supportedIsoSensitivityRange() const
{
    return qMakePair<int,int>(-1,-1);
}

void S60CameraControl::setManualIsoSensitivity(int iso)
{
    Q_UNUSED(iso);
}

void S60CameraControl::setAutoIsoSensitivity()
{
}

double S60CameraControl::aperture() const
{
    return -1.0;
}

QPair<double, double> S60CameraControl::supportedApertureRange() const
{
    return qMakePair<double,double>(-1,-1);
}

void S60CameraControl::setManualAperture(double aperture)
{
    Q_UNUSED(aperture);
}

void S60CameraControl::setAutoAperture()
{
}

double S60CameraControl::shutterSpeed() const
{
    return -1;
}

QPair<double, double> S60CameraControl::supportedShutterSpeedRange() const
{
    return qMakePair<double,double>(-1,-1);
}

void S60CameraControl::setManualShutterSpeed(double seconds)
{
    Q_UNUSED(seconds);
}

void S60CameraControl::setAutoShutterSpeed()
{
}

double S60CameraControl::maximumOpticalZoom() const
{
    return 1.0;
}

double S60CameraControl::maximumDigitalZoom() const
{
    return 1.0;
}

double S60CameraControl::zoomValue() const
{
    return 1.0;
}

void S60CameraControl::zoomTo(int value)
{
    Q_UNUSED(value);
}
/*
void S60CameraControl::setDevice(const QString &device)
{
    m_session->setDevice(device);
}
*/
bool S60CameraControl::isExposureLocked() const
{
    return true;
}

bool S60CameraControl::isFocusLocked() const
{
    return true;
}

void S60CameraControl::lockExposure()
{
}

void S60CameraControl::unlockExposure()
{
}

void S60CameraControl::lockFocus()
{
}

void S60CameraControl::unlockFocus()
{
}

