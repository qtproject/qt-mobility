/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60CAMERASETTINGS_H
#define S60CAMERASETTINGS_H

#include <QtCore/qobject.h>
#include "qcamera.h"

#include "s60cameraengine.h"
#include "s60cameraengineobserver.h"

QT_USE_NAMESPACE

class S60CameraSettings : public QObject, public MAdvancedSettingsObserver
{
    Q_OBJECT
 
public: // Contructor & Destructor
    
    S60CameraSettings(QObject *parent = 0, CCameraEngine *engine = 0);
    ~S60CameraSettings();
    
public: // Methods
    
    // Focus
    QCameraFocus::FocusMode focusMode();
    void setFocusMode(QCameraFocus::FocusMode mode);
    QCameraFocus::FocusModes supportedFocusModes();
    void cancelFocusing();
    
    // Flash
    bool isFlashReady();
    
    // Exposure
    void setExposureMode(QCameraExposure::ExposureMode mode);
    void lockExposure(bool lock);
    bool isExposureLocked(); 
    
    // Metering Mode
    QCameraExposure::MeteringMode meteringMode();
    void setMeteringMode(QCameraExposure::MeteringMode mode);
    bool isMeteringModeSupported(QCameraExposure::MeteringMode mode);
    
    // ISO Sensitivity
    int isoSensitivity();
    void setManualIsoSensitivity(int iso);
    void setAutoIsoSensitivity();
    QList<int> supportedIsoSensitivities();
    
    // Aperture
    qreal aperture();
    void setManualAperture(qreal aperture);
    QList<qreal> supportedApertures();
    
    // Shutter Speed
    TInt shutterSpeed();
    void setManualShutterSpeed(TInt speed);
    QList<qreal> supportedShutterSpeeds();
    
    // ExposureCompensation
    qreal exposureCompensation();
    void setExposureCompensation(qreal ev);
    QList<qreal> supportedExposureCompensationValues();
    
    // Sharpening Level
    int sharpeningLevel() const;
    void setSharpeningLevel(int value);
    bool isSharpeningSupported() const;
    
    // Saturation
    int saturation() const;
    void setSaturation(int value);
    
Q_SIGNALS: // Notifications

    // For QCameraExposureControl
    void flashReady(bool ready);
    void apertureChanged();
    void apertureRangeChanged();
    void shutterSpeedChanged();
    void isoSensitivityChanged();
    
    // For QCameraLocksControl
    void exposureStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason);
    void focusStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason);
    
    void error(QCamera::Error);
    
protected: // MAdvancedSettingsObserver
    
    void HandleAdvancedEvent(const TECAMEvent& aEvent);

private:
    
    bool queryAdvancedSettingsInfo();

private: // Data
    
#ifndef S60_CAM_AUTOFOCUS_SUPPORT // Post S60 3.1 Platforms
    CCamera::CCameraAdvancedSettings    *m_advancedSettings;
    CCamera::CCameraImageProcessing     *m_imageProcessingSettings;
#endif // S60_CAM_AUTOFOCUS_SUPPORT
    CCameraEngine                       *m_cameraEngine;
};

#endif // S60CAMERASETTINGS_H
