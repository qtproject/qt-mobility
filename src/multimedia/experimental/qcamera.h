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

#ifndef QCAMERA_H
#define QCAMERA_H

#include <QtCore/qstringlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qsize.h>

#include <qmediacontrol.h>
#include <qmediaobject.h>
#include <qmediaservice.h>

#include <qmediaserviceprovider.h>

QTM_BEGIN_NAMESPACE

class QCameraControl;


class QCameraPrivate;
class Q_MEDIA_EXPORT QCamera : public QMediaObject
{
    Q_OBJECT

    Q_ENUMS(State Error FocusStatus)

    Q_PROPERTY(qreal aperture READ aperture WRITE setManualAperture NOTIFY apertureChanged)
    Q_PROPERTY(qreal shutterSpeed READ shutterSpeed WRITE setManualShutterSpeed NOTIFY shutterSpeedChanged)
    Q_PROPERTY(int isoSensitivity READ isoSensitivity WRITE setManualIsoSensitivity NOTIFY isoSensitivityChanged)

public:
    enum State { ActiveState, StoppedState };

    enum Error
    {
        NoError,
        CameraError,
        NotReadyToCaptureError,
        InvalidRequestError,
        ServiceMissingError,
        NotSupportedFeatureError
    };

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
        HyperfocalFocus = 0x02,
        InfinityFocus = 0x04,
        AutoFocus = 0x8,
        ContinuousFocus = 0x10
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
        ExposurePortrait = 0x400
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
    Q_PROPERTY(bool readyForCapture READ isReadyForCapture NOTIFY readyForCaptureChanged)

    QCamera(QObject *parent = 0, QMediaServiceProvider *provider = QMediaServiceProvider::defaultServiceProvider());
    QCamera(const QByteArray& device, QObject *parent = 0);
    ~QCamera();

    static QList<QByteArray> availableDevices();
    static QString deviceDescription(const QByteArray &device);

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

    qreal exposureCompensation() const;
    void setExposureCompensation(qreal ev);

    MeteringMode meteringMode() const;
    void setMeteringMode(MeteringMode mode);
    MeteringModes supportedMeteringModes() const;

    WhiteBalanceMode whiteBalanceMode() const;
    void setWhiteBalanceMode(WhiteBalanceMode mode);
    WhiteBalanceModes supportedWhiteBalanceModes() const;
    int manualWhiteBalance() const;
    void setManualWhiteBalance(int colorTemperature);

    int isoSensitivity() const;
    int minimumIsoSensitivity() const;
    int maximumIsoSensitivity() const;
    QList<int> supportedIsoSensitivities() const;
    void setManualIsoSensitivity(int iso);
    void setAutoIsoSensitivity();

    qreal aperture() const;
    qreal minimumAperture() const;
    qreal maximumAperture() const;
    QList<qreal> supportedApertures() const;
    void setManualAperture(qreal aperture);
    void setAutoAperture();

    qreal shutterSpeed() const;
    qreal minimumShutterSpeed() const;
    qreal maximumShutterSpeed() const;
    QList<qreal> supportedShutterSpeeds() const;
    void setManualShutterSpeed(qreal seconds);
    void setAutoShutterSpeed();

    qreal maximumOpticalZoom() const;
    qreal maximumDigitalZoom() const;
    qreal zoomValue() const;
    void zoomTo(qreal value);

    bool isExposureLocked() const;
    bool isFocusLocked() const;

    bool isReadyForCapture() const;

    Error error() const;
    QString errorString() const;

public Q_SLOTS:
    void lockExposure();
    void unlockExposure();

    void lockFocus();
    void unlockFocus();

    void capture(const QString &fileName);

Q_SIGNALS:
    void flashReady(bool);
    void focusStatusChanged(QCamera::FocusStatus);
    void zoomValueChanged(qreal);

    void apertureChanged(qreal);
    void apertureRangeChanged();
    void shutterSpeedChanged(qreal);
    void isoSensitivityChanged(int);

    void exposureLocked();
    void focusLocked();

    void readyForCaptureChanged(bool);
    void imageCaptured(const QString &fileName, const QImage &preview);
    void imageSaved(const QString &fileName);

    void stateChanged(QCamera::State);
    void error(QCamera::Error);

private:
    Q_DISABLE_COPY(QCamera)
    Q_DECLARE_PRIVATE(QCamera)
    Q_PRIVATE_SLOT(d_func(), void _q_error(int, const QString &))
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QCamera::FlashModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QCamera::FocusModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QCamera::WhiteBalanceModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QCamera::MeteringModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QCamera::ExposureModes)

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QCamera)::State)
Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QCamera)::Error)
Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QCamera)::FocusStatus)


#endif  // QCAMERA_H
