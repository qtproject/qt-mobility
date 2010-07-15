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

#ifndef QCAMERAEXPOSURE_H
#define QCAMERAEXPOSURE_H

#include <qmediaobject.h>

QT_BEGIN_NAMESPACE

class QCamera;
class QCameraExposurePrivate;

class Q_MULTIMEDIA_EXPORT QCameraExposure : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal aperture READ aperture WRITE setManualAperture NOTIFY apertureChanged)
    Q_PROPERTY(qreal shutterSpeed READ shutterSpeed WRITE setManualShutterSpeed NOTIFY shutterSpeedChanged)
    Q_PROPERTY(int isoSensitivity READ isoSensitivity WRITE setManualIsoSensitivity NOTIFY isoSensitivityChanged)
    Q_PROPERTY(qreal exposureCompensation READ exposureCompensation WRITE setExposureCompensation)
public:
    enum FlashMode {
        FlashOff = 0x1,
        FlashOn = 0x2,
        FlashAuto = 0x4,
        FlashRedEyeReduction  = 0x8,
        FlashFill = 0x10,
        FlashTorch = 0x20,
        FlashSlowSyncFrontCurtain = 0x40,
        FlashSlowSyncRearCurtain = 0x80,
        FlashManual = 0x100
    };
    Q_DECLARE_FLAGS(FlashModes, FlashMode)

    enum ExposureMode {
        ExposureManual,
        ExposureAuto,
        ExposureNight,
        ExposureBacklight,
        ExposureSpotlight,
        ExposureSports,
        ExposureSnow,
        ExposureBeach,
        ExposureLargeAperture,
        ExposureSmallAperture,
        ExposurePortrait
    };

    enum MeteringMode {
        MeteringMatrix,
        MeteringAverage,
        MeteringSpot
    };    

    bool isAvailable() const;

    FlashModes flashMode() const;
    bool isFlashModeSupported(FlashModes mode) const;
    bool isFlashReady() const;

    ExposureMode exposureMode() const;
    bool isExposureModeSupported(ExposureMode mode) const;

    qreal exposureCompensation() const;

    MeteringMode meteringMode() const;

    bool isMeteringModeSupported(MeteringMode mode) const;

    int isoSensitivity() const;
    QList<int> supportedIsoSensitivities(bool *continuous = 0) const;

    qreal aperture() const;
    QList<qreal> supportedApertures(bool *continuous = 0) const;

    qreal shutterSpeed() const;
    QList<qreal> supportedShutterSpeeds(bool *continuous = 0) const;

public Q_SLOTS:
    void setFlashMode(FlashModes mode);
    void setExposureMode(ExposureMode mode);

    void setExposureCompensation(qreal ev);

    void setMeteringMode(MeteringMode mode);

    void setManualIsoSensitivity(int iso);
    void setAutoIsoSensitivity();

    void setManualAperture(qreal aperture);
    void setAutoAperture();

    void setManualShutterSpeed(qreal seconds);
    void setAutoShutterSpeed();

Q_SIGNALS:
    void flashReady(bool);

    void apertureChanged(qreal);
    void apertureRangeChanged();
    void shutterSpeedChanged(qreal);
    void shutterSpeedRangeChanged();
    void isoSensitivityChanged(int);

private:
    friend class QCamera;
    explicit QCameraExposure(QCamera *parent = 0);
    virtual ~QCameraExposure();

    Q_DISABLE_COPY(QCameraExposure)
    Q_DECLARE_PRIVATE(QCameraExposure)
    Q_PRIVATE_SLOT(d_func(), void _q_exposureParameterChanged(int))
    Q_PRIVATE_SLOT(d_func(), void _q_exposureParameterRangeChanged(int))
    QCameraExposurePrivate *d_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QCameraExposure::FlashModes)

QT_END_NAMESPACE

#endif // QCAMERAEXPOSURE_H
