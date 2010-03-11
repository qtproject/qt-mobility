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

#include "qgstreamercameraexposurecontrol_maemo.h"
#include "qgstreamercapturesession_maemo.h"

#include <QDebug>
QGstreamerCameraExposureControl::QGstreamerCameraExposureControl(QGstreamerCaptureSession *session)
    :QCameraExposureControl(session),
     m_session(session),
     m_flashMode(QCamera::FlashOff)
{
}

QGstreamerCameraExposureControl::~QGstreamerCameraExposureControl()
{
}

QCamera::FlashMode QGstreamerCameraExposureControl::flashMode() const
{
    return m_flashMode;
}

void QGstreamerCameraExposureControl::setFlashMode(QCamera::FlashMode mode)
{
    qDebug() << "QGstreamerCameraExposureControl::setFlashMode(QCamera::FlashMode mode)";
    m_flashMode = mode;
    m_session->setFlashMode();
}

QCamera::FlashModes QGstreamerCameraExposureControl::supportedFlashModes() const
{
    QCamera::FlashModes modes;
    modes |= QCamera::FlashOff;
    modes |= QCamera::FlashOn;
    modes |= QCamera::FlashAuto;
    modes |= QCamera::FlashRedEyeReduction;
    modes |= QCamera::FlashFill;

    return modes;
}

qreal QGstreamerCameraExposureControl::flashCompensation() const
{
}

void QGstreamerCameraExposureControl::setFlashCompensation(qreal ev)
{
}

qreal QGstreamerCameraExposureControl::flashPower() const
{
}

void QGstreamerCameraExposureControl::setFlashPower(qreal power)
{
}

bool QGstreamerCameraExposureControl::isFlashReady() const
{
}

QCamera::ExposureMode QGstreamerCameraExposureControl::exposureMode() const
{
}

void QGstreamerCameraExposureControl::setExposureMode(QCamera::ExposureMode mode)
{
}

QCamera::ExposureModes QGstreamerCameraExposureControl::supportedExposureModes() const
{
}

qreal QGstreamerCameraExposureControl::exposureCompensation() const
{
}

void QGstreamerCameraExposureControl::setExposureCompensation(qreal ev)
{
}

QCamera::MeteringMode QGstreamerCameraExposureControl::meteringMode() const
{
}

void QGstreamerCameraExposureControl::setMeteringMode(QCamera::MeteringMode mode)
{
}

QCamera::MeteringModes QGstreamerCameraExposureControl::supportedMeteringModes() const
{
}

int QGstreamerCameraExposureControl::isoSensitivity() const
{
}

QList<int> QGstreamerCameraExposureControl::supportedIsoSensitivities(bool *continuous) const
{
}

void QGstreamerCameraExposureControl::setManualIsoSensitivity(int iso)
{
}

void QGstreamerCameraExposureControl::setAutoIsoSensitivity()
{
}

qreal QGstreamerCameraExposureControl::aperture() const
{
}

QList<qreal> QGstreamerCameraExposureControl::supportedApertures(bool *continuous) const
{
}

void QGstreamerCameraExposureControl::setManualAperture(qreal aperture)
{
}

void QGstreamerCameraExposureControl::setAutoAperture()
{
}

qreal QGstreamerCameraExposureControl::shutterSpeed() const
{
}

QList<qreal> QGstreamerCameraExposureControl::supportedShutterSpeeds(bool *continuous) const
{
}

void QGstreamerCameraExposureControl::setManualShutterSpeed(qreal seconds)
{
}

void QGstreamerCameraExposureControl::setAutoShutterSpeed()
{
}

bool QGstreamerCameraExposureControl::isExposureLocked() const
{
}

void QGstreamerCameraExposureControl::lockExposure()
{
}

void QGstreamerCameraExposureControl::unlockExposure()
{

}

