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

#include "s60camerasettings.h"

#include <QtCore/qdebug.h>
#ifdef USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
#include <ecamadvancedsettings.h> // CCameraAdvancedSettings
#endif
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER
#include <ecamadvsettings.h>   // CCameraAdvancedSettings
#endif



S60CameraSettings::S60CameraSettings(QObject *parent, CCameraEngine *engine)
    : QObject(parent)
{   
    m_cameraEngine = engine;
    queryAdvancedSettingsInfo();
}

S60CameraSettings::~S60CameraSettings()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    delete m_advancedSettings;
    m_advancedSettings = NULL;
#endif
}

/*
 * Queries advanced settings information
 * Results are returned to member variable m_advancedSettings
 * @return boolean indicating if querying the info was a success
 */
bool S60CameraSettings::queryAdvancedSettingsInfo()
{
    bool returnValue = false;
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (m_cameraEngine) {
        m_advancedSettings = NULL;
        m_advancedSettings = m_cameraEngine->AdvancedSettings();
        if (m_advancedSettings)
            returnValue = true;
    }
#endif
    return returnValue;
}

void S60CameraSettings::setFocusMode(QCamera::FocusMode mode)
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (m_advancedSettings) {
        switch(mode) {
            case QCamera::ManualFocus: // Manual focus mode
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeManual);
                break;
            case QCamera::AutoFocus: // One-shot auto focus mode
                m_advancedSettings->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle);
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeAuto);
                break;
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER             
            case QCamera::ContinuousFocus: // Continuous auto focus mode 
                m_advancedSettings->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeContinuous);
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeAuto);
                break;
#endif
            case QCamera::InfinityFocus: // TODO:
            case QCamera::HyperfocalFocus: // TODO:
            default:
                break;

        }      
    }
#else
    // TODO:
#endif
}

QCamera::FocusMode S60CameraSettings::focusMode()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (m_advancedSettings) {
        CCamera::CCameraAdvancedSettings::TFocusMode mode = m_advancedSettings->FocusMode();
        switch(mode) {
            case CCamera::CCameraAdvancedSettings::EFocusModeManual:
                return QCamera::ManualFocus;
                break;
            case CCamera::CCameraAdvancedSettings::EFocusModeAuto:
                return QCamera::AutoFocus;
                break;
        }      
    }
#else
    // TODO:
#endif
}

QCamera::FocusModes S60CameraSettings::supportedFocusModes()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    TInt supportedModes = 0;
    TInt autoFocusTypes = 0;
    QCamera::FocusModes modes = QCamera::AutoFocus;
    if (m_advancedSettings) {
        supportedModes = m_advancedSettings->SupportedFocusModes();     
        autoFocusTypes = m_advancedSettings->SupportedAutoFocusTypes();
        if (supportedModes == 0)
            return modes;
        if (supportedModes & CCamera::CCameraAdvancedSettings::EFocusModeAuto) {
            if (autoFocusTypes & CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle)
                modes |= QCamera::AutoFocus;
            if (autoFocusTypes & CCamera::CCameraAdvancedSettings::EAutoFocusTypeContinuous) {
                modes |= QCamera::ContinuousFocus;
            }
        }
    }
    return modes;
#else
    // TODO:
#endif
}

// from MCameraObserver2
void S60CameraSettings::HandleEvent(const TECAMEvent& aEvent)
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (aEvent.iEventType == KUidECamEventCameraSettingExposureLock) {
        emit exposureLocked();
    }
    else if (aEvent.iEventType == KUidECamEventCameraSettingAperture) {

    }    
    else if (aEvent.iEventType == KUidECamEventCameraSettingExposureCompensation) {
    
    }

#else
    // TODO:
#endif
}

void S60CameraSettings::ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError)
{
    
}

void S60CameraSettings::ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError)
{
    
}
void S60CameraSettings::VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError)
{

}

bool S60CameraSettings::isFlashReady()
{
    TBool ready = false;
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        int i = m_advancedSettings->IsFlashReady(ready);
        if (i == KErrNotSupported) {
            // TODO: error flash not supported
            return false;
        }
    } 
#else
    // TODO:
#endif
    return ready;
}

void S60CameraSettings::setExposureCompensation(qreal ev)
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        m_advancedSettings->SetExposureCompensation(ev);
    }
#else
    // TODO:
#endif
}

qreal S60CameraSettings::exposureCompensation()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        m_advancedSettings->ExposureCompensation();
    } else {
        return 0;
    }
#else
    // TODO:
#endif
}

QCamera::MeteringMode S60CameraSettings::meteringMode()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        CCamera::CCameraAdvancedSettings::TMeteringMode mode = m_advancedSettings->MeteringMode();
        switch(mode) {
            case CCamera::CCameraAdvancedSettings::EMeteringModeCenterWeighted:
                return QCamera::MeteringAverage;
                break;
            case CCamera::CCameraAdvancedSettings::EMeteringModeEvaluative:
                return QCamera::MeteringMatrix;
                break;
            case CCamera::CCameraAdvancedSettings::EMeteringModeSpot:
                return QCamera::MeteringSpot;
                break;
            default:
                break;
        }      
    }
#else
    // TODO:
#endif
}

void S60CameraSettings::setMeteringMode(QCamera::MeteringMode mode)
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        switch(mode) {
            case QCamera::MeteringAverage:
                m_advancedSettings->SetMeteringMode(CCamera::CCameraAdvancedSettings::EMeteringModeCenterWeighted);
                break;
            case QCamera::MeteringMatrix:
                m_advancedSettings->SetMeteringMode(CCamera::CCameraAdvancedSettings::EMeteringModeEvaluative);
                break;
            case QCamera::MeteringSpot:
                m_advancedSettings->SetMeteringMode(CCamera::CCameraAdvancedSettings::EMeteringModeSpot);
                break;
            default:   
                break;
        }       
    }
#else
    // TODO:
#endif
}

QCamera::MeteringModes S60CameraSettings::supportedMeteringModes()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    TInt supportedModes = 0;
    QCamera::MeteringModes modes = QCamera::MeteringAverage;    
    if (queryAdvancedSettingsInfo()) {
        supportedModes = m_advancedSettings->SupportedMeteringModes();            
        if (supportedModes == 0)
            return modes;
        if (supportedModes & CCamera::CCameraAdvancedSettings::EMeteringModeCenterWeighted) {
             modes |= QCamera::MeteringAverage;          
        }
        if (supportedModes & CCamera::CCameraAdvancedSettings::EMeteringModeEvaluative) {
             modes |= QCamera::MeteringMatrix;          
        }
        if (supportedModes & CCamera::CCameraAdvancedSettings::EMeteringModeSpot) {
             modes |= QCamera::MeteringSpot;          
        }
    }
    return modes;
#else
    // TODO:
#endif
}

int S60CameraSettings::isoSensitivity()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        return m_advancedSettings->IsoRate();
    } else {
        return 0;
    }
#else
    // TODO:
#endif
}

QList<int> S60CameraSettings::supportedIsoSensitivities()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    QList<int> isoSentitivities;
    if (queryAdvancedSettingsInfo()) {
        RArray<TInt> supportedIsoRates;
        m_advancedSettings->GetSupportedIsoRatesL(supportedIsoRates);
        for (int i=0; i < supportedIsoRates.Count(); i++) {
            int q = supportedIsoRates[i];
            isoSentitivities.append(q);
        }
        supportedIsoRates.Close();
    }
    return isoSentitivities;  
#else
    // TODO:
#endif
}

void S60CameraSettings::setManualIsoSensitivity(int iso)
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        m_advancedSettings->SetIsoRate(iso);
    }
#else
    // TODO:
#endif
}

qreal S60CameraSettings::aperture()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        return m_advancedSettings->Aperture();
    } else { 
        return -1.0;
    }
#else
    // TODO:
#endif
}

QList<qreal> S60CameraSettings::supportedApertures(bool *continuous)
{
    // TODO:
}

void S60CameraSettings::setManualAperture(qreal aperture)
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        m_advancedSettings->SetAperture(aperture);
    }
#else
    // TODO:
#endif
}

void S60CameraSettings::lockExposure(bool lock)
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
            m_advancedSettings->SetExposureLockOn(lock);
        }
#else
    // TODO:
#endif
}

bool S60CameraSettings::isExposureLocked()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER || USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        return m_advancedSettings->ExposureLockOn();
    } else {
        return false;
    }
#else
    // TODO:
#endif
}

