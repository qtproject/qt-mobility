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
#include <gst/interfaces/photography.h>

#include <QDebug>

QGstreamerCameraExposureControl::QGstreamerCameraExposureControl(GstElement &camerabin, QGstreamerCaptureSession *session)
    :QCameraExposureControl(session),
     m_camerabin(camerabin),
     m_session(session)
{
}

QGstreamerCameraExposureControl::~QGstreamerCameraExposureControl()
{
}

QCamera::FlashModes QGstreamerCameraExposureControl::flashMode() const
{
    GstPhotoSettings config;
    gst_photography_get_config(GST_PHOTOGRAPHY(&m_camerabin), &config);
    GstFlashMode flashMode = config.flash_mode;

    QCamera::FlashModes modes;
    switch (flashMode) {
    case GST_PHOTOGRAPHY_FLASH_MODE_AUTO: modes |= QCamera::FlashAuto; break;
    case GST_PHOTOGRAPHY_FLASH_MODE_OFF: modes |= QCamera::FlashOff; break;
    case GST_PHOTOGRAPHY_FLASH_MODE_ON: modes |= QCamera::FlashOn; break;
    case GST_PHOTOGRAPHY_FLASH_MODE_FILL_IN: modes |= QCamera::FlashFill; break;
    case GST_PHOTOGRAPHY_FLASH_MODE_RED_EYE: modes |= QCamera::FlashRedEyeReduction; break;
    default:
        modes |= QCamera::FlashManual;
        break;
    }
    return modes;
}

void QGstreamerCameraExposureControl::setFlashMode(QCamera::FlashModes mode)
{
    GstPhotoSettings config;
    gst_photography_get_config(GST_PHOTOGRAPHY(&m_camerabin), &config);

    GstFlashMode flashMode;
    if (mode.testFlag(QCamera::FlashAuto)) flashMode = GST_PHOTOGRAPHY_FLASH_MODE_AUTO;
    else if (mode.testFlag(QCamera::FlashOff)) flashMode = GST_PHOTOGRAPHY_FLASH_MODE_OFF;
    else if (mode.testFlag(QCamera::FlashOn)) flashMode = GST_PHOTOGRAPHY_FLASH_MODE_ON;
    else if (mode.testFlag(QCamera::FlashFill)) flashMode = GST_PHOTOGRAPHY_FLASH_MODE_FILL_IN;
    else if (mode.testFlag(QCamera::FlashRedEyeReduction)) flashMode = GST_PHOTOGRAPHY_FLASH_MODE_RED_EYE;

    config.flash_mode = flashMode;
    gst_photography_set_config(GST_PHOTOGRAPHY(&m_camerabin), &config);
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

