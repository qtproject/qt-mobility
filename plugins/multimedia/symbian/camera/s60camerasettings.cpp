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
{   
    m_cameraEngine = engine;
}

S60CameraSettings::~S60CameraSettings()
{
	
}

/*
 * Queries advanced settings information
 * Results are returned to member variable m_advancedSettings
 * @return boolean indicating if querying the info was a success
 */
bool S60CameraSettings::queryAdvancedSettingsInfo()
{
    qDebug() << "S60CameraSession::queryAdvancedSettingsInfo";

    bool returnValue = false;
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (m_cameraEngine) {
        qDebug() << "m_cameraEngine->AdvancedSettings";
        m_advancedSettings = m_cameraEngine->AdvancedSettings();   
        returnValue = true;
    }
#endif
    return returnValue;
}

void S60CameraSettings::setFocusMode(QCamera::FocusMode mode)
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        switch(mode) {
            case QCamera::ManualFocus: // Manual focus mode
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeManual);
                break;
            case QCamera::AutoFocus: // One-shot auto focus mode
                qDebug() << "set auto";
                m_advancedSettings->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle);
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeAuto);
                break;
            case QCamera::ContinuousFocus: // Continuous auto focus mode 
                qDebug() << "set auto continuous";
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeAuto);
                m_advancedSettings->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeContinuous);
                break;
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
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        CCamera::CCameraAdvancedSettings::TFocusMode mode = m_advancedSettings->FocusMode();
        switch(mode) {
            case CCamera::CCameraAdvancedSettings::EFocusModeManual:
                return QCamera::ManualFocus;
                break;
            case CCamera::CCameraAdvancedSettings::EFocusModeAuto:
                qDebug() << "CCamera::CCameraAdvancedSettings::EFocusModeAuto";
                CCamera::CCameraAdvancedSettings::TAutoFocusType type = m_advancedSettings->AutoFocusType();
                if (type == CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle) {
                    qDebug() << "CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle";
                    return QCamera::AutoFocus;
                }
                else if (type == CCamera::CCameraAdvancedSettings::EAutoFocusTypeContinuous) {
                    qDebug() << "CCamera::CCameraAdvancedSettings::EAutoFocusTypeContinuous";
                    return QCamera::ContinuousFocus;
                }
                break;
        }      
    }
#else
    // TODO:
#endif
}

QCamera::FocusModes S60CameraSettings::supportedFocusModes()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    TInt supportedModes = 0;
    TInt autoFocusTypes = 0;
    QCamera::FocusModes modes = QCamera::AutoFocus;
    if (queryAdvancedSettingsInfo()) {
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
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    // KUidECamEventCameraSettingAperture -event emit apertureChanged(qreal)
    // KUidECamEventCameraSettingExposureLock event emit exposureLocked
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

void S60CameraSettings::setFlashMode(QCamera::FlashMode mode)
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        switch(mode) {
            case QCamera::FlashOff:
                m_advancedSettings->SetFlashMode(CCamera::EFlashNone);
                break;
            case QCamera::FlashAuto:
                m_advancedSettings->SetFlashMode(CCamera::EFlashAuto);
                break;
            case QCamera::FlashOn:
                m_advancedSettings->SetFlashMode(CCamera::EFlashForced);
                break;
            case QCamera::FlashRedEyeReduction:
                m_advancedSettings->SetFlashMode(CCamera::EFlashRedEyeReduce);
                break;
            case QCamera::FlashFill:
                m_advancedSettings->SetFlashMode(CCamera::EFlashFillIn);
                break;
            default:
                break;
                
        }       
    }
#else
    // TODO:
#endif
}

QCamera::FlashModes S60CameraSettings::supportedFlashModes()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    TInt supportedModes = 0;
    QCamera::FlashModes modes = QCamera::FlashOff;
    if (queryAdvancedSettingsInfo()) {
        supportedModes = m_advancedSettings->SupportedFlashModes();            
        if (supportedModes == 0)
            return modes;
        if (supportedModes & CCamera::EFlashManual) {
             modes |= QCamera::FlashOff;          
        }
        if (supportedModes & CCamera::EFlashForced) {
             modes |= QCamera::FlashOn;          
        }
        if (supportedModes & CCamera::EFlashAuto) {
             modes |= QCamera::FlashAuto;          
        }
        if (supportedModes & CCamera::EFlashFillIn) {
             modes |= QCamera::FlashFill;          
        }
        if (supportedModes & CCamera::EFlashRedEyeReduce) {
             modes |= QCamera::FlashRedEyeReduction;          
        }
    }
    return modes;
#else
    // TODO:
#endif
}

QCamera::ExposureModes S60CameraSettings::supportedExposureModes()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    TInt supportedModes = 0;
    QCamera::ExposureModes modes = QCamera::ExposureAuto;
    
    if (queryAdvancedSettingsInfo()) {
        supportedModes = m_advancedSettings->SupportedExposureModes();            
        if (supportedModes == 0)
            return modes;
        if (supportedModes & CCamera::EExposureManual) {
             modes |= QCamera::ExposureManual;          
        }
        if (supportedModes & CCamera::EExposureAuto) {
             modes |= QCamera::ExposureAuto;          
        }
        if (supportedModes & CCamera::EExposureNight) {
             modes |= QCamera::ExposureNight;          
        }
        if (supportedModes & CCamera::EExposureBacklight) {
             modes |= QCamera::ExposureBacklight;          
        }
        if (supportedModes & CCamera::EExposureSport) {
             modes |= QCamera::ExposureSports;          
        }
        if (supportedModes & CCamera::EExposureSnow) {
             modes |= QCamera::ExposureSnow;          
        }
        if (supportedModes & CCamera::EExposureBeach) {
             modes |= QCamera::ExposureBeach;          
        }
        if (supportedModes & CCamera::EExposureSport) {
             modes |= QCamera::ExposureSports;          
        }
        
        // spotlight, large aperture, small aperture, portrait ?
    }
    return modes;
#else
    // TODO:
#endif
}

void S60CameraSettings::setExposureMode(QCamera::ExposureMode mode)
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        switch(mode) {
            case QCamera::ExposureManual:
                m_advancedSettings->SetExposureMode(CCamera::EExposureManual);
                break;
            case QCamera::ExposureAuto:
                m_advancedSettings->SetExposureMode(CCamera::EExposureAuto);
                break;
            case QCamera::ExposureNight:
                m_advancedSettings->SetExposureMode(CCamera::EExposureNight);
                break;
            case QCamera::ExposureBacklight:
                m_advancedSettings->SetExposureMode(CCamera::EExposureBacklight);
                break;
            case QCamera::ExposureSports:
                m_advancedSettings->SetExposureMode(CCamera::EExposureSport);
                break;
            case QCamera::ExposureSnow:
                m_advancedSettings->SetExposureMode(CCamera::EExposureSnow);
                break;
            case QCamera::ExposureBeach:
                m_advancedSettings->SetExposureMode(CCamera::EExposureBeach);
                break;
            case QCamera::ExposureLargeAperture:
            case QCamera::ExposureSmallAperture:
                //TODO: 
                //m_advancedSettings->SetExposureMode(CCamera::EExposureAperturePriority);
                break;
            case QCamera::ExposurePortrait:
            case QCamera::ExposureSpotlight:
            default:
                // not supported
                break;
        }
    }
#else
    // TODO:
#endif
}

void S60CameraSettings::setExposureCompensation(qreal ev)
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        m_advancedSettings->SetExposureCompensation(ev);
    }
#else
    // TODO:
#endif
}

qreal S60CameraSettings::exposureCompensation()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
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
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
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
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
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
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
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
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
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
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
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
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        m_advancedSettings->SetIsoRate(iso);
    }
#else
    // TODO:
#endif
}

qreal S60CameraSettings::aperture()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
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
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        m_advancedSettings->SetAperture(aperture);
    }
#else
    // TODO:
#endif
}

void S60CameraSettings::lockExposure(bool lock)
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
            m_advancedSettings->SetExposureLockOn(lock);
        }
#else
    // TODO:
#endif
}

bool S60CameraSettings::isExposureLocked()
{
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    if (queryAdvancedSettingsInfo()) {
        return m_advancedSettings->ExposureLockOn();
    } else {
        return false;
    }
#else
    // TODO:
#endif
}

