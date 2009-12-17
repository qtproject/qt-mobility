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

#include "s60cameraexposurecontrol.h"
#include "s60cameraservice.h"
#include "s60camerasession.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>


S60CameraExposureControl::S60CameraExposureControl(QObject *parent)
    :QCameraExposureControl(parent)
{
}

S60CameraExposureControl::S60CameraExposureControl(QObject *session, QObject *parent)
   :QCameraExposureControl(parent)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60CameraSession*>(session);
}

S60CameraExposureControl::~S60CameraExposureControl()
{
}


QCamera::FlashMode S60CameraExposureControl::flashMode() const
{
    //TODO return current flash modes
    return QCamera::FlashOff;
}

void S60CameraExposureControl::setFlashMode(QCamera::FlashMode mode)
{
    //TODO set flash modes
    Q_UNUSED(mode)
}

QCamera::FlashModes S60CameraExposureControl::supportedFlashModes() const
{
    //TODO return correct flash modes
    return QCamera::FlashOff;
}

bool S60CameraExposureControl::isFlashReady() const
{
    //TODO return is flash ready
    return false;
}

QCamera::ExposureMode S60CameraExposureControl::exposureMode() const
{
    return QCamera::ExposureManual;
}

void S60CameraExposureControl::setExposureMode(QCamera::ExposureMode mode)
{
    Q_UNUSED(mode)
}

QCamera::ExposureModes S60CameraExposureControl::supportedExposureModes() const
{
    return QCamera::ExposureManual;
}

qreal S60CameraExposureControl::exposureCompensation() const
{
    return 0;
}

void S60CameraExposureControl::setExposureCompensation(qreal ev)
{
    Q_UNUSED(ev)
}

QCamera::MeteringMode S60CameraExposureControl::meteringMode() const
{
    return QCamera::MeteringAverage;
}

void S60CameraExposureControl::setMeteringMode(QCamera::MeteringMode mode)
{
    Q_UNUSED(mode)
}

QCamera::MeteringModes S60CameraExposureControl::supportedMeteringModes() const
{
    return QCamera::MeteringAverage;
}

QCamera::WhiteBalanceMode S60CameraExposureControl::whiteBalanceMode() const
{
    return QCamera::WhiteBalanceManual;
}

void S60CameraExposureControl::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
{
    Q_UNUSED(mode)
}

QCamera::WhiteBalanceModes S60CameraExposureControl::supportedWhiteBalanceModes() const
{
    return QCamera::WhiteBalanceManual;
}

int S60CameraExposureControl::manualWhiteBalance() const
{
    return 0;
}

void S60CameraExposureControl::setManualWhiteBalance(int colorTemperature)
{
    Q_UNUSED(colorTemperature)
}

int S60CameraExposureControl::isoSensitivity() const
{
    return 0;
}
int S60CameraExposureControl::minimumIsoSensitivity() const
{
    return 0;
}
int S60CameraExposureControl::maximumIsoSensitivity() const
{
    return 3200;
}
QList<int> S60CameraExposureControl::supportedIsoSensitivities(bool *continuous) const
{
    QList<int> res;
    res << 100;
    return res;
}

void S60CameraExposureControl::setManualIsoSensitivity(int iso)
{
    Q_UNUSED(iso);
}

void S60CameraExposureControl::setAutoIsoSensitivity()
{
}

qreal S60CameraExposureControl::aperture() const
{
    return -1.0;
}

QList<qreal> S60CameraExposureControl::supportedApertures(bool *continuous) const
{
    QList<qreal> res;
    res << 2.8 << 16.0;
    return res;
}

void S60CameraExposureControl::setManualAperture(qreal aperture)
{
    Q_UNUSED(aperture);
}

void S60CameraExposureControl::setAutoAperture()
{
}
qreal S60CameraExposureControl::minimumAperture() const
{
    return 2.8;
}

qreal S60CameraExposureControl::maximumAperture() const
{
    return 16.0;
}

qreal S60CameraExposureControl::shutterSpeed() const
{
    return -1;
}
qreal S60CameraExposureControl::minimumShutterSpeed() const
{
    return 0.001;
}

qreal S60CameraExposureControl::maximumShutterSpeed() const
{
    return 30.0;
}
/*
  Returns the list of shutter speed values if camera supports only fixed set of shutter speed values,
  otherwise returns an empty list.
 */
QList<qreal> S60CameraExposureControl::supportedShutterSpeeds(bool *continuous) const
{
    QList<qreal> res;
    return res;
}

void S60CameraExposureControl::setManualShutterSpeed(qreal seconds)
{
    Q_UNUSED(seconds);
}

void S60CameraExposureControl::setAutoShutterSpeed()
{
}

bool S60CameraExposureControl::isExposureLocked() const
{
    return true;
}

void S60CameraExposureControl::lockExposure()
{
}

void S60CameraExposureControl::unlockExposure()
{
}
