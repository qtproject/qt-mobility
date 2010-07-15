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
    GstPhotoCaps caps = gst_photography_get_capabilities(GST_PHOTOGRAPHY(&m_camerabin));
    qDebug() << "Camera caps:" << caps;
}

QGstreamerCameraExposureControl::~QGstreamerCameraExposureControl()
{
}

QCameraExposure::FlashModes QGstreamerCameraExposureControl::flashMode() const
{
    GstFlashMode flashMode;
    gst_photography_get_flash_mode(GST_PHOTOGRAPHY(&m_camerabin), &flashMode);

    QCameraExposure::FlashModes modes;
    switch (flashMode) {
    case GST_PHOTOGRAPHY_FLASH_MODE_AUTO: modes |= QCameraExposure::FlashAuto; break;
    case GST_PHOTOGRAPHY_FLASH_MODE_OFF: modes |= QCameraExposure::FlashOff; break;
    case GST_PHOTOGRAPHY_FLASH_MODE_ON: modes |= QCameraExposure::FlashOn; break;
    case GST_PHOTOGRAPHY_FLASH_MODE_FILL_IN: modes |= QCameraExposure::FlashFill; break;
    case GST_PHOTOGRAPHY_FLASH_MODE_RED_EYE: modes |= QCameraExposure::FlashRedEyeReduction; break;
    default:
        modes |= QCameraExposure::FlashAuto;
        break;
    }
    return modes;
}

void QGstreamerCameraExposureControl::setFlashMode(QCameraExposure::FlashModes mode)
{
    GstFlashMode flashMode;
    gst_photography_get_flash_mode(GST_PHOTOGRAPHY(&m_camerabin), &flashMode);

    if (mode.testFlag(QCameraExposure::FlashAuto)) flashMode = GST_PHOTOGRAPHY_FLASH_MODE_AUTO;
    else if (mode.testFlag(QCameraExposure::FlashOff)) flashMode = GST_PHOTOGRAPHY_FLASH_MODE_OFF;
    else if (mode.testFlag(QCameraExposure::FlashOn)) flashMode = GST_PHOTOGRAPHY_FLASH_MODE_ON;
    else if (mode.testFlag(QCameraExposure::FlashFill)) flashMode = GST_PHOTOGRAPHY_FLASH_MODE_FILL_IN;
    else if (mode.testFlag(QCameraExposure::FlashRedEyeReduction)) flashMode = GST_PHOTOGRAPHY_FLASH_MODE_RED_EYE;

    gst_photography_set_flash_mode(GST_PHOTOGRAPHY(&m_camerabin), flashMode);
}

bool QGstreamerCameraExposureControl::isFlashModeSupported(QCameraExposure::FlashModes mode) const
{
    return  mode == QCameraExposure::FlashOff ||
            mode == QCameraExposure::FlashOn ||
            mode == QCameraExposure::FlashAuto ||
            mode == QCameraExposure::FlashRedEyeReduction ||
            mode == QCameraExposure::FlashFill;
}

bool QGstreamerCameraExposureControl::isFlashReady() const
{
    return true;
}

QCameraExposure::ExposureMode QGstreamerCameraExposureControl::exposureMode() const
{
    GstSceneMode sceneMode;
    gst_photography_get_scene_mode(GST_PHOTOGRAPHY(&m_camerabin), &sceneMode);

    switch (sceneMode) {
    case GST_PHOTOGRAPHY_SCENE_MODE_PORTRAIT: return QCameraExposure::ExposurePortrait;
    case GST_PHOTOGRAPHY_SCENE_MODE_SPORT: return QCameraExposure::ExposureSports;
    case GST_PHOTOGRAPHY_SCENE_MODE_NIGHT: return QCameraExposure::ExposureNight;
    case GST_PHOTOGRAPHY_SCENE_MODE_CLOSEUP:    //no direct mapping available so mapping to auto mode
    case GST_PHOTOGRAPHY_SCENE_MODE_LANDSCAPE:  //no direct mapping available so mapping to auto mode
    case GST_PHOTOGRAPHY_SCENE_MODE_AUTO:
        return QCameraExposure::ExposureAuto;
    case GST_PHOTOGRAPHY_SCENE_MODE_MANUAL:
    default:
         return QCameraExposure::ExposureManual;
    }
}

void QGstreamerCameraExposureControl::setExposureMode(QCameraExposure::ExposureMode mode)
{
    GstSceneMode sceneMode;
    gst_photography_get_scene_mode(GST_PHOTOGRAPHY(&m_camerabin), &sceneMode);

    switch (mode) {
    case QCameraExposure::ExposureManual: sceneMode = GST_PHOTOGRAPHY_SCENE_MODE_MANUAL; break;
    case QCameraExposure::ExposurePortrait: sceneMode = GST_PHOTOGRAPHY_SCENE_MODE_PORTRAIT; break;
    case QCameraExposure::ExposureSports: sceneMode = GST_PHOTOGRAPHY_SCENE_MODE_SPORT; break;
    case QCameraExposure::ExposureNight: sceneMode = GST_PHOTOGRAPHY_SCENE_MODE_NIGHT; break;
    case QCameraExposure::ExposureAuto: sceneMode = GST_PHOTOGRAPHY_SCENE_MODE_AUTO; break;
    default:
        break;
    }

    gst_photography_set_scene_mode(GST_PHOTOGRAPHY(&m_camerabin), sceneMode);
}

bool QGstreamerCameraExposureControl::isExposureModeSupported(QCameraExposure::ExposureMode mode) const
{
    //Similar mode names can be found in gst as GstSceneMode
    return  mode == QCameraExposure::ExposureAuto ||
            mode == QCameraExposure::ExposurePortrait ||
            mode == QCameraExposure::ExposureSports ||
            mode == QCameraExposure::ExposureNight;

    //No direct mapping available for GST_PHOTOGRAPHY_SCENE_MODE_CLOSEUP and
    //GST_PHOTOGRAPHY_SCENE_MODE_LANDSCAPE
}

QCameraExposure::MeteringMode QGstreamerCameraExposureControl::meteringMode() const
{
    return QCameraExposure::MeteringMatrix;
}

void QGstreamerCameraExposureControl::setMeteringMode(QCameraExposure::MeteringMode mode)
{
    Q_UNUSED(mode);
}

bool QGstreamerCameraExposureControl::isMeteringModeSupported(QCameraExposure::MeteringMode mode) const
{
    return mode == QCameraExposure::MeteringMatrix;
}

bool QGstreamerCameraExposureControl::isParameterSupported(ExposureParameter parameter) const
{
    switch (parameter) {
    case QCameraExposureControl::ExposureCompensation:
    case QCameraExposureControl::ISO:
    case QCameraExposureControl::Aperture:
    case QCameraExposureControl::ShutterSpeed:
        return true;
    default:
        return false;
    }
}

QVariant QGstreamerCameraExposureControl::exposureParameter(ExposureParameter parameter) const
{
    switch (parameter) {
    case QCameraExposureControl::ExposureCompensation:
        {
            gfloat ev;
            gst_photography_get_ev_compensation(GST_PHOTOGRAPHY(&m_camerabin), &ev);
            return QVariant(ev);
        }
    case QCameraExposureControl::ISO:
        {
            guint isoSpeed = 0;
            gst_photography_get_iso_speed(GST_PHOTOGRAPHY(&m_camerabin), &isoSpeed);
            return QVariant(isoSpeed);
        }
    case QCameraExposureControl::Aperture:
        return QVariant(2.8);
    case QCameraExposureControl::ShutterSpeed:
        {
            guint32 shutterSpeed = 0;
            gst_photography_get_exposure(GST_PHOTOGRAPHY(&m_camerabin), &shutterSpeed);

            return QVariant(shutterSpeed/1000000.0);
        }
    default:
        return QVariant();
    }
}

QCameraExposureControl::ParameterFlags QGstreamerCameraExposureControl::exposureParameterFlags(ExposureParameter parameter) const
{
    QCameraExposureControl::ParameterFlags flags = 0;

    switch (parameter) {
    case QCameraExposureControl::ExposureCompensation:
        flags |= ContinuousRange;
        break;
    case QCameraExposureControl::Aperture:
        flags |= ReadOnly;
        break;
    default:
        break;
    }

    return flags;
}

QVariantList QGstreamerCameraExposureControl::supportedParameterRange(ExposureParameter parameter) const
{
    QVariantList res;
    switch (parameter) {
    case QCameraExposureControl::ExposureCompensation:
        res << -2.0 << 2.0;
        break;
    case QCameraExposureControl::ISO:
        res << 100 << 200 << 400;
        break;
    case QCameraExposureControl::Aperture:
        res << 2.8;
        break;
    default:
        break;
    }

    return res;
}

bool QGstreamerCameraExposureControl::setExposureParameter(ExposureParameter parameter, const QVariant& value)
{
    switch (parameter) {
    case QCameraExposureControl::ExposureCompensation:
        gst_photography_set_ev_compensation(GST_PHOTOGRAPHY(&m_camerabin), value.toReal());
        break;
    case QCameraExposureControl::ISO:
        gst_photography_set_iso_speed(GST_PHOTOGRAPHY(&m_camerabin), value.toInt());
        break;
    case QCameraExposureControl::Aperture:
        gst_photography_set_aperture(GST_PHOTOGRAPHY(&m_camerabin), guint(value.toReal()*1000000));
        break;
    case QCameraExposureControl::ShutterSpeed:
        gst_photography_set_exposure(GST_PHOTOGRAPHY(&m_camerabin), guint(value.toReal()*1000000));
        break;
    default:
        return false;
    }

    return true;
}

QString QGstreamerCameraExposureControl::extendedParameterName(ExposureParameter)
{
    return QString();
}
