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

#ifndef S60CAMERAEXPOSURECONTROL_H
#define S60CAMERAEXPOSURECONTROL_H

#include <QtCore/qobject.h>
#include "qcameraexposurecontrol.h"
#include "s60camerasettings.h"

QTM_USE_NAMESPACE

class S60CameraService;
class S60ImageCaptureSession;

class S60CameraExposureControl : public QCameraExposureControl
{
    Q_OBJECT
public:
    S60CameraExposureControl(QObject *parent = 0);
    S60CameraExposureControl(QObject *session, QObject *parent = 0);
    ~S60CameraExposureControl();

    QCamera::FlashModes flashMode() const;
    void setFlashMode(QCamera::FlashModes mode);
    QCamera::FlashModes supportedFlashModes() const;
    bool isFlashReady() const;

    qreal flashCompensation() const;
    void setFlashCompensation(qreal ev);
    qreal flashPower() const;
    void setFlashPower(qreal power);

    QCamera::ExposureMode exposureMode() const;
    void setExposureMode(QCamera::ExposureMode mode);
    QCamera::ExposureModes supportedExposureModes() const;

    qreal exposureCompensation() const;
    void setExposureCompensation(qreal ev);

    QCamera::MeteringMode meteringMode() const;
    void setMeteringMode(QCamera::MeteringMode mode);
    QCamera::MeteringModes supportedMeteringModes() const;

    int isoSensitivity() const;
    QList<int> supportedIsoSensitivities(bool *continuous = 0) const;
    void setManualIsoSensitivity(int iso);
    void setAutoIsoSensitivity();

    qreal aperture() const;
    QList<qreal> supportedApertures(bool *continuous = 0) const;
    void setManualAperture(qreal aperture);
    void setAutoAperture();

    qreal shutterSpeed() const;
    QList<qreal> supportedShutterSpeeds(bool *continuous = 0) const;
    void setManualShutterSpeed(qreal seconds);
    void setAutoShutterSpeed();

    bool isExposureLocked() const;


public Q_SLOTS:
    void lockExposure();
    void unlockExposure();
    void resetAdvancedSetting();

private:
    S60ImageCaptureSession *m_session;
    S60CameraService *m_service;
    S60CameraSettings *m_advancedSettings;
    QCamera::Error m_error;
    QCamera::FlashModes m_flashMode;
    QCamera::ExposureMode m_exposureMode;
    QCamera::MeteringMode m_meteringMode;
    qreal m_ev;
};

#endif
