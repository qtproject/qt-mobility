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

#ifndef QCAMERA_H
#define QCAMERA_H

#include <QList>
#include <QPair>
#include <QSize>
#include <QStringList>

#include "qabstractmediacontrol.h"
#include "qabstractmediaobject.h"
#include "qabstractmediaservice.h"

#include "qmediaserviceprovider.h"

class QCameraService;
class QCameraControl;

extern Q_MEDIA_EXPORT QAbstractMediaService *createCameraService(QMediaServiceProvider *provider = defaultServiceProvider("camera"));

class QCameraPrivate;

class Q_MEDIA_EXPORT QCamera : public QAbstractMediaObject
{
    Q_OBJECT

    Q_ENUMS(State)
public:
    enum State { LoadingState, ActiveState, PausedState, StoppedState };

    enum FlashMode {
        FlashOff = 0x1,
        FlashOn = 0x2,
        FlashAuto = 0x4,
        FlashRedEyeReduction  = 0x8,
        FlashFill = 0x10
    };
    Q_DECLARE_FLAGS(FlashModes, FlashMode)

    enum FocusMode {
        ManualFocus = 0x1,
        AutoFocus = 0x2,
        ContinuousFocus = 0x4
    };
    Q_DECLARE_FLAGS(FocusModes, FocusMode)

    enum FocusStatus {
        FocusDisabled, //manual mode
        FocusRequested,
        FocusReached,
        FocusLost,
        FocusUnableToReach
    };

    enum ExposureMode {
        ExposureManual = 0x1,
        ExposureAuto = 0x2,
        ExposureNight = 0x4,
        ExposureBacklight = 0x8,
        ExposureSpotlight = 0x10,
        ExposureSports = 0x20,
        ExposureSnow = 0x40,
        ExposureBeach  = 0x80,
        ExposureLargeAperture = 0x100,
        ExposureSmallAperture = 0x200,
        ExposurePortrait = 0x400,
        ExposureNightPortrait = 0x800
    };
    Q_DECLARE_FLAGS(ExposureModes, ExposureMode)

    enum ExposureStatus {
        CorrectExposure,
        UnderExposure,
        OverExposure
    };

    enum MeteringMode {
        MeteringAverage = 0x1,
        MeteringSpot = 0x2,
        MeteringMatrix = 0x4
    };
    Q_DECLARE_FLAGS(MeteringModes, MeteringMode)

    enum WhiteBalanceMode {
        WhiteBalanceManual = 0x1,
        WhiteBalanceAuto = 0x2,
        WhiteBalanceSunlight = 0x4,
        WhiteBalanceCloudy = 0x8,
        WhiteBalanceShade = 0x10,
        WhiteBalanceTungsten = 0x20,
        WhiteBalanceFluorescent = 0x40,
        WhiteBalanceIncandescent = 0x80,
        WhiteBalanceFlash = 0x100,
        WhiteBalanceSunset = 0x200
    };
    Q_DECLARE_FLAGS(WhiteBalanceModes, WhiteBalanceMode)

    Q_PROPERTY(QCamera::State state READ state NOTIFY stateChanged)

    QCamera(QAbstractMediaService *service = createCameraService(), QObject *parent = 0);
    //QCamera(const QByteArray &device, QObject *parent = 0);
    ~QCamera();

    QList<QByteArray> deviceList();
    void setDevice(const QByteArray& device);
    static QString deviceDescription(const QByteArray &device);

    bool isValid() const;
    QAbstractMediaService* service() const;

    void start();
    void stop();

    State state() const;

    FlashMode flashMode() const;
    void setFlashMode(FlashMode mode);
    FlashModes supportedFlashModes() const;
    bool isFlashReady() const;

    FocusMode focusMode() const;
    void setFocusMode(FocusMode mode);
    FocusModes supportedFocusModes() const;
    FocusStatus focusStatus() const;

    bool macroFocusingEnabled() const;
    bool isMacroFocusingSupported() const;
    void setMacroFocusingEnabled(bool);

    ExposureMode exposureMode() const;
    void setExposureMode(ExposureMode mode);
    ExposureModes supportedExposureModes() const;

    double exposureCompensation() const;
    void setExposureCompensation(double ev);

    MeteringMode meteringMode() const;
    void setMeteringMode(MeteringMode mode);
    MeteringModes supportedMeteringModes() const;

    WhiteBalanceMode whiteBalanceMode() const;
    void setWhiteBalanceMode(WhiteBalanceMode mode);
    WhiteBalanceModes supportedWhiteBalanceModes() const;
    int manualWhiteBalance() const;
    void setManualWhiteBalance(int colorTemperature);

    int isoSensitivity() const;
    QPair<int, int> supportedIsoSensitivityRange() const;
    void setManualIsoSensitivity(int iso);
    void setAutoIsoSensitivity();

    double aperture() const;
    QPair<double, double> supportedApertureRange() const;
    void setManualAperture(double aperture);
    void setAutoAperture();

    double shutterSpeed() const;
    QPair<double, double> supportedShutterSpeedRange() const;
    void setManualShutterSpeed(double seconds);
    void setAutoShutterSpeed();

    double maximumOpticalZoom() const;
    double maximumDigitalZoom() const;
    double zoomValue() const;
    void zoomTo(int value);

    bool isExposureLocked() const;
    bool isFocusLocked() const;

public Q_SLOTS:
    void lockExposure();
    void unlockExposure();

    void lockFocus();
    void unlockFocus();

Q_SIGNALS:
    void flashReady(bool);
    void focusStatusChanged(FocusStatus);
    void zoomValueChanged(double);
    void exposureLocked();
    void focusLocked();

    void stateChanged(QCamera::State);

private:
    Q_DISABLE_COPY(QCamera)
    Q_DECLARE_PRIVATE(QCamera)
};

#endif  // QCAMERA_H
