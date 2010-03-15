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

#ifndef QGSTREAMERCAMERAEXPOSURECONTROL_MAEMO_H
#define QGSTREAMERCAMERAEXPOSURECONTROL_MAEMO_H

#include <experimental/qcamera.h>
#include <experimental/qcameraexposurecontrol.h>

#include <gst/gst.h>
#include <glib.h>

class QGstreamerCaptureSession;

QTM_USE_NAMESPACE

class Q_MEDIA_EXPORT QGstreamerCameraExposureControl : public QCameraExposureControl
{
    Q_OBJECT

public:
    QGstreamerCameraExposureControl(GstElement &camerabin, QGstreamerCaptureSession *session);
    virtual ~QGstreamerCameraExposureControl();

    QCamera::FlashModes flashMode() const;
    void setFlashMode(QCamera::FlashModes mode);
    QCamera::FlashModes supportedFlashModes() const;

    qreal flashCompensation() const;
    void setFlashCompensation(qreal ev);

    qreal flashPower() const;
    void setFlashPower(qreal power);

    bool isFlashReady() const;

    QCamera::ExposureMode exposureMode() const;
    void setExposureMode(QCamera::ExposureMode mode);
    QCamera::ExposureModes supportedExposureModes() const;

    qreal exposureCompensation() const;
    void setExposureCompensation(qreal ev);

    QCamera::MeteringMode meteringMode() const;
    void setMeteringMode(QCamera::MeteringMode mode);
    QCamera::MeteringModes supportedMeteringModes() const;

    int isoSensitivity() const;
    QList<int> supportedIsoSensitivities(bool *continuous) const;
    void setManualIsoSensitivity(int iso);
    void setAutoIsoSensitivity();

    qreal aperture() const;
    QList<qreal> supportedApertures(bool *continuous) const;
    void setManualAperture(qreal aperture);
    void setAutoAperture();

    qreal shutterSpeed() const;
    QList<qreal> supportedShutterSpeeds(bool *continuous) const;
    void setManualShutterSpeed(qreal seconds);
    void setAutoShutterSpeed();

    bool isExposureLocked() const;

public Q_SLOTS:
    void lockExposure();
    void unlockExposure();

private:
    GstElement &m_camerabin;
    QGstreamerCaptureSession *m_session;    
/*Q_SIGNALS:
    void exposureLocked();
    void flashReady(bool);

    void apertureChanged(qreal);
    void apertureRangeChanged();
    void shutterSpeedChanged(qreal);
    void isoSensitivityChanged(int);
*/

};

#endif // QGSTREAMERCAMERAEXPOSURECONTROL_MAEMO_H
