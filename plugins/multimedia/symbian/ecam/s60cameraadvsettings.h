/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60CAMERAADVSETTINGS_H
#define S60CAMERAADVSETTINGS_H

#include <qcameraimageprocessingcontrol.h>

#include "s60cameraengine.h"
#include "s60cameraengineobserver.h"

#include <e32base.h>

QT_USE_NAMESPACE

/*
 * Class handling all the more advanced camera settings. In practice this means
 * the settings set via the ECam CCameraAdvancedSettings or
 * CCameraImageProcessing API classes.
 */
class S60CameraAdvSettings : public QObject,
                             public MAdvancedSettingsObserver
{
    Q_OBJECT

public: // Static Contructor & Destructor

    static S60CameraAdvSettings* New(int &error, QObject *parent = 0, CCameraEngine *engine = 0);
    ~S60CameraAdvSettings();

public: // Methods

    // Focus
    QCameraFocus::FocusMode focusMode();
    void setFocusMode(QCameraFocus::FocusMode mode);
    QCameraFocus::FocusModes supportedFocusModes();
    void startFocusing();
    void cancelFocusing();

    // Zoom
    qreal opticalZoomFactorL() const;
    void setOpticalZoomFactorL(const qreal zoomFactor);
    QList<qreal> supportedDigitalZoomFactors() const;
    qreal digitalZoomFactorL() const;
    void setDigitalZoomFactorL(const qreal zoomFactor);

    // Flash
    bool isFlashReady();

    // Metering Mode
    QCameraExposure::MeteringMode meteringMode();
    void setMeteringMode(QCameraExposure::MeteringMode mode);
    bool isMeteringModeSupported(QCameraExposure::MeteringMode mode);

    // ISO Sensitivity
    int isoSensitivity();
    void setManualIsoSensitivity(int iso);
    void setAutoIsoSensitivity();
    QList<int> supportedIsoSensitivitiesL();

    // Aperture
    qreal aperture();
    void setManualAperture(qreal aperture);
    QList<qreal> supportedApertures();

    // Shutter Speed
    qreal shutterSpeed();
    void setManualShutterSpeed(qreal speed);
    QList<qreal> supportedShutterSpeeds();

    // ExposureCompensation
    qreal exposureCompensation();
    void setExposureCompensation(qreal ev);
    QList<qreal> supportedExposureCompensationValues();

    // Saturation Control
    bool isSaturationSupported();
    int saturation();
    void setSaturation(int value);

    // Sharpening Control
    bool isSharpeningSupported();
    int sharpeningLevel();
    void setSharpeningLevel(int value);

    // Denoising Control
    bool isDenoisingSupported();
    int denoising();
    void setDenoising(int value);

signals: // Notifications

    // For QCameraExposureControl
    void flashReady(bool ready);
    void apertureChanged();
    void apertureRangeChanged();
    void shutterSpeedChanged();
    void isoSensitivityChanged();
    void evChanged();

    // For QCameraLocksControl
    void focusStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason);

    // Errors
    void error(int, const QString&);

protected: // Protected constructors

    S60CameraAdvSettings(QObject *parent, CCameraEngine *engine);
    void ConstructL();

protected: // MAdvancedSettingsObserver

    void HandleAdvancedEvent(const TECAMEvent& aEvent);

private: // Internal

    bool queryAdvancedSettingsInfo();
    QList<QCameraImageProcessingControl::ProcessingParameter> supportedProcessingParametersL();

private: // Enums

    enum EcamErrors {
        KErrECamCameraDisabled =        -12100, // The camera has been disabled, hence calls do not succeed
        KErrECamSettingDisabled =       -12101, // This parameter or operation is supported, but presently is disabled.
        KErrECamParameterNotInRange =   -12102, // This value is out of range.
        KErrECamSettingNotSupported =   -12103, // This parameter or operation is not supported.
        KErrECamNotOptimalFocus =       -12104  // The optimum focus is lost
    };

private: // Data

#ifndef S60_31_PLATFORM // Post S60 3.1 Platforms
    CCamera::CCameraAdvancedSettings    *m_advancedSettings;
    CCamera::CCameraImageProcessing     *m_imageProcessingSettings;
#endif // S60_31_PLATFORM
    CCameraEngine                       *m_cameraEngine;
    QList<int>                          m_supportedSymbianDigitalZoomFactors;
    bool                                m_continuousFocusing;
};

#endif // S60CAMERAADVSETTINGS_H
