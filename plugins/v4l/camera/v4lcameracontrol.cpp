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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "v4lcameracontrol.h"
#include "v4lcameraservice.h"
#include "v4lcamerasession.h"

#include <QtCore/qdebug.h>


V4LCameraControl::V4LCameraControl(QObject *parent)
    :QCameraControl(parent)
{
    m_session = qobject_cast<V4LCameraSession*>(parent);
}

V4LCameraControl::V4LCameraControl(V4LCameraService *service, QObject *parent)
   :QCameraControl(parent), m_service(service)
{
    m_session = qobject_cast<V4LCameraSession*>(parent);
}

V4LCameraControl::~V4LCameraControl()
{
}

void V4LCameraControl::start()
{
    m_session->record();
}

void V4LCameraControl::stop()
{
    m_session->stop();
}

QCamera::State V4LCameraControl::state() const
{
    return (QCamera::State)m_session->state();
}

QCamera::FlashMode V4LCameraControl::flashMode() const
{
    return QCamera::FlashOff;
}

void V4LCameraControl::setFlashMode(QCamera::FlashMode mode)
{
    Q_UNUSED(mode)
}

QCamera::FlashModes V4LCameraControl::supportedFlashModes() const
{
    return QCamera::FlashOff;
}

bool V4LCameraControl::isFlashReady() const
{
    return false;
}

QCamera::FocusMode V4LCameraControl::focusMode() const
{
    return QCamera::AutoFocus;
}

void V4LCameraControl::setFocusMode(QCamera::FocusMode mode)
{
    Q_UNUSED(mode)
}

QCamera::FocusModes V4LCameraControl::supportedFocusModes() const
{
    return QCamera::AutoFocus;
}

QCamera::FocusStatus V4LCameraControl::focusStatus() const
{
    return QCamera::FocusDisabled;
}

bool V4LCameraControl::macroFocusingEnabled() const
{
    return false;
}

bool V4LCameraControl::isMacroFocusingSupported() const
{
    return false;
}

void V4LCameraControl::setMacroFocusingEnabled(bool e)
{
    Q_UNUSED(e)
}

QCamera::ExposureMode V4LCameraControl::exposureMode() const
{
    return QCamera::ExposureManual;
}

void V4LCameraControl::setExposureMode(QCamera::ExposureMode mode)
{
    Q_UNUSED(mode)
}

QCamera::ExposureModes V4LCameraControl::supportedExposureModes() const
{
    return QCamera::ExposureManual;
}

double V4LCameraControl::exposureCompensation() const
{
    return 0;
}

void V4LCameraControl::setExposureCompensation(double ev)
{
    Q_UNUSED(ev)
}

QCamera::MeteringMode V4LCameraControl::meteringMode() const
{
    return QCamera::MeteringAverage;
}

void V4LCameraControl::setMeteringMode(QCamera::MeteringMode mode)
{
    Q_UNUSED(mode)
}

QCamera::MeteringModes V4LCameraControl::supportedMeteringModes() const
{
    return QCamera::MeteringAverage;
}

QCamera::WhiteBalanceMode V4LCameraControl::whiteBalanceMode() const
{
    return QCamera::WhiteBalanceManual;
}

void V4LCameraControl::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
{
    Q_UNUSED(mode)
}

QCamera::WhiteBalanceModes V4LCameraControl::supportedWhiteBalanceModes() const
{
    return QCamera::WhiteBalanceManual;
}

int V4LCameraControl::manualWhiteBalance() const
{
    return 0;
}

void V4LCameraControl::setManualWhiteBalance(int colorTemperature)
{
    Q_UNUSED(colorTemperature)
}

int V4LCameraControl::isoSensitivity() const
{
    return 0;
}

QPair<int, int> V4LCameraControl::supportedIsoSensitivityRange() const
{
    return qMakePair<int,int>(-1,-1);
}

void V4LCameraControl::setManualIsoSensitivity(int iso)
{
    Q_UNUSED(iso);
}

void V4LCameraControl::setAutoIsoSensitivity()
{
}

double V4LCameraControl::aperture() const
{
    return -1.0;
}

QPair<double, double> V4LCameraControl::supportedApertureRange() const
{
    return qMakePair<double,double>(-1,-1);
}

void V4LCameraControl::setManualAperture(double aperture)
{
    Q_UNUSED(aperture);
}

void V4LCameraControl::setAutoAperture()
{
}

double V4LCameraControl::shutterSpeed() const
{
    return -1;
}

QPair<double, double> V4LCameraControl::supportedShutterSpeedRange() const
{
    return qMakePair<double,double>(-1,-1);
}

void V4LCameraControl::setManualShutterSpeed(double seconds)
{
    Q_UNUSED(seconds);
}

void V4LCameraControl::setAutoShutterSpeed()
{
}

double V4LCameraControl::maximumOpticalZoom() const
{
    return 1.0;
}

double V4LCameraControl::maximumDigitalZoom() const
{
    return 1.0;
}

double V4LCameraControl::zoomValue() const
{
    return 1.0;
}

void V4LCameraControl::zoomTo(int value)
{
    Q_UNUSED(value);
}

void V4LCameraControl::setDevice(const QByteArray &device)
{
    m_session->setDevice(device);
}

bool V4LCameraControl::isExposureLocked() const
{
    return true;
}

bool V4LCameraControl::isFocusLocked() const
{
    return true;
}

void V4LCameraControl::lockExposure()
{
}

void V4LCameraControl::unlockExposure()
{
}

void V4LCameraControl::lockFocus()
{
}

void V4LCameraControl::unlockFocus()
{
}

