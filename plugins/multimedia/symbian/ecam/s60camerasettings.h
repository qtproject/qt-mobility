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

#ifndef S60CAMERASETTINGS_H
#define S60CAMERASETTINGS_H

#include <QtCore/qobject.h>
#include <s60cameraengine.h>
#include <s60cameraengineobserver.h>

#include "qcamera.h"

QTM_USE_NAMESPACE

class S60CameraSettings : public QObject, public MAdvancedSettingsObserver
{

    Q_OBJECT
 
public:
    S60CameraSettings(QObject *parent = 0, CCameraEngine *engine = 0);
    ~S60CameraSettings();
    
    bool isFlashReady();
    void setExposureMode(QCamera::ExposureMode mode);
    void setExposureCompensation(qreal ev);
    qreal exposureCompensation();
    QCamera::MeteringMode meteringMode();
    void setMeteringMode(QCamera::MeteringMode mode);
    QCamera::MeteringModes supportedMeteringModes();
    int isoSensitivity();
    QList<int> supportedIsoSensitivities();
    void setManualIsoSensitivity(int iso);
    qreal aperture();
    QList<qreal> supportedApertures(bool *continuous);
    void setManualAperture(qreal aperture);
    void lockExposure(bool lock);
    bool isExposureLocked(); 
    
    void setFocusMode(QCamera::FocusMode mode);
    QCamera::FocusMode focusMode();
    QCamera::FocusModes supportedFocusModes();
    void cancelFocusing();
    
    TInt shutterSpeed();
    void setShutterSpeed(TInt speed);
    QList<qreal> supportedShutterSpeeds(bool *continuous);
    
    void setSharpeningLevel(int value);
    bool isSharpeningSupported() const;
    int sharpeningLevel() const;
    
    void setSaturation(int value);
    int saturation() const;
    
Q_SIGNALS:
    void exposureLocked();
    void flashReady(bool ready);
    void apertureChanged(qreal aperture);
    void apertureRangeChanged();
    void shutterSpeedChanged(qreal speed);
    void isoSensitivityChanged(int iso);
    void focusStatusChanged(QCamera::FocusStatus);
    void error(QCamera::Error);    
    
protected: // from MAdvancedSettingsObserver
    void HandleAdvancedEvent(const TECAMEvent& aEvent);

private:    
    bool queryAdvancedSettingsInfo();

private:
#if defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER)
    CCamera::CCameraAdvancedSettings *m_advancedSettings;
    CCamera::CCameraImageProcessing * m_imageProcessingSettings;
#endif
    CCameraEngine *m_cameraEngine;

};

#endif

