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
#include <ecamadvancedsettings.h> // CCameraAdvancedSettings (includes TValueInfo)
#endif
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER
#include <ecamadvsettings.h>   // CCameraAdvancedSettings
#include <ecam/ecamconstants.h>  // TValueInfo
#endif

S60CameraSettings::S60CameraSettings(QObject *parent, CCameraEngine *engine)
    : QObject(parent)
{
    m_cameraEngine = engine;
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    TRAP_IGNORE(m_advancedSettings = CCamera::CCameraAdvancedSettings::NewL(*m_cameraEngine->Camera()));
    TRAP_IGNORE(m_imageProcessingSettings = CCamera::CCameraImageProcessing::NewL(*m_cameraEngine->Camera()));
#endif
}

S60CameraSettings::~S60CameraSettings()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))    
    delete m_advancedSettings;    
    m_advancedSettings = NULL;
    delete m_imageProcessingSettings;    
    m_imageProcessingSettings = NULL;    
#endif
}

void S60CameraSettings::setFocusMode(QCamera::FocusMode mode)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        switch(mode) {
            case QCamera::ManualFocus: // Manual focus mode
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeManual);
                break;
            case QCamera::AutoFocus: // One-shot auto focus mode
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeAuto);
                m_advancedSettings->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle);
                break;
        }
    }
#endif
}

void S60CameraSettings::cancelFocusing()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        m_advancedSettings->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff); 
    }
#endif
}

QCamera::FocusMode S60CameraSettings::focusMode()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        CCamera::CCameraAdvancedSettings::TFocusMode mode = m_advancedSettings->FocusMode();
        switch(mode) {
            case CCamera::CCameraAdvancedSettings::EFocusModeManual:
                return QCamera::ManualFocus;
            case CCamera::CCameraAdvancedSettings::EFocusModeAuto:
                return QCamera::AutoFocus;
        }
    } 
#endif
}

QCamera::FocusModes S60CameraSettings::supportedFocusModes()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    TInt supportedModes = 0;
    TInt autoFocusTypes = 0;
    QCamera::FocusModes modes = QCamera::AutoFocus;
    if (m_advancedSettings) {
        //qDebug() << "Settings: SupportedFocusModes";
        supportedModes = m_advancedSettings->SupportedFocusModes();
        //qDebug() << "Settings: SupportedAutoFocusTypes";
        autoFocusTypes = m_advancedSettings->SupportedAutoFocusTypes();
        if (supportedModes == 0)
            return modes;
        if (supportedModes & CCamera::CCameraAdvancedSettings::EFocusModeAuto) {
            if (autoFocusTypes & CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle)
                modes |= QCamera::AutoFocus;
            if (autoFocusTypes & CCamera::CCameraAdvancedSettings::EAutoFocusTypeContinuous)
                modes |= QCamera::ContinuousFocus;
        }
    }
    return modes;
#endif
}

// from MCameraObserver2
void S60CameraSettings::HandleAdvancedEvent(const TECAMEvent& aEvent)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (aEvent.iEventType == KUidECamEventCameraSettingExposureLock) {
        emit exposureLocked();
    } else if (aEvent.iEventType == KUidECamEventCameraSettingAperture) {
        emit apertureChanged(aperture());
    } else if (aEvent.iEventType == KUidECamEventCameraSettingApertureRange) {
        emit apertureRangeChanged();
    } else if (aEvent.iEventType == KUidECamEventCameraSettingIsoRate) {
        emit isoSensitivityChanged(isoSensitivity());
    } else if (aEvent.iEventType == KUidECamEventCameraSettingShutterSpeed) {
        emit shutterSpeedChanged(shutterSpeed());
    } else if (aEvent.iEventType == KUidECamEventCameraSettingExposureLock) {
        emit exposureLocked();
    } else if (aEvent.iEventType == KUidECamEventFlashReady) {
        emit flashReady(true);
    } else if (aEvent.iEventType == KUidECamEventFlashNotReady) {
        emit flashReady(false);
    } else if (aEvent.iEventType.iUid == KUidECamEventCameraSettingsOptimalFocusUidValue) {
        if (aEvent.iErrorCode == KErrNone) {
            emit focusStatusChanged(QCamera::FocusReached);
        }
    } else if (aEvent.iEventType == KUidECamEventCameraSettingFocusMode) {
        emit focusStatusChanged(QCamera::FocusRequested);
    }    
#endif
}

bool S60CameraSettings::isFlashReady()
{
    TBool ready = false;
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        int i = m_advancedSettings->IsFlashReady(ready);
        if (i == KErrNotSupported) {
            // TODO: error flash not supported
            return false;
        }
    }
#endif
    return ready;
}

void S60CameraSettings::setExposureCompensation(qreal ev)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        m_advancedSettings->SetExposureCompensation(ev);
    }
#endif
}

qreal S60CameraSettings::exposureCompensation()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        return m_advancedSettings->ExposureCompensation();
    } else {
        return 0;
    }
#else
    return 0;
#endif
}

QCamera::MeteringMode S60CameraSettings::meteringMode()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        CCamera::CCameraAdvancedSettings::TMeteringMode mode = m_advancedSettings->MeteringMode();
        switch(mode) {
            case CCamera::CCameraAdvancedSettings::EMeteringModeCenterWeighted:
                return QCamera::MeteringAverage;
            case CCamera::CCameraAdvancedSettings::EMeteringModeEvaluative:
                return QCamera::MeteringMatrix;
            case CCamera::CCameraAdvancedSettings::EMeteringModeSpot:
                return QCamera::MeteringSpot;
            default:
                break;
        }
    }
#endif
}

void S60CameraSettings::setMeteringMode(QCamera::MeteringMode mode)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
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
#endif
}

QCamera::MeteringModes S60CameraSettings::supportedMeteringModes()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    TInt supportedModes = 0;
    QCamera::MeteringModes modes;// = QCamera::MeteringAverage;
    if (m_advancedSettings) {
        supportedModes = m_advancedSettings->SupportedMeteringModes();
        if (supportedModes == 0) {
            return modes;
        } if (supportedModes & CCamera::CCameraAdvancedSettings::EMeteringModeCenterWeighted) {
            modes |= QCamera::MeteringAverage;
        } if (supportedModes & CCamera::CCameraAdvancedSettings::EMeteringModeEvaluative) {
            modes |= QCamera::MeteringMatrix;
        } if (supportedModes & CCamera::CCameraAdvancedSettings::EMeteringModeSpot) {
            modes |= QCamera::MeteringSpot;
        }
    }
    return modes;
#endif
}

int S60CameraSettings::isoSensitivity()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        return m_advancedSettings->IsoRate();
    } else {
        return 0;
    }
#endif
}

QList<int> S60CameraSettings::supportedIsoSensitivities()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    QList<int> isoSentitivities;
    if (m_advancedSettings) {
        RArray<TInt> supportedIsoRates;
        TRAPD(err, m_advancedSettings->GetSupportedIsoRatesL(supportedIsoRates));
        if (err != KErrNone) {
            emit error(QCamera::NotSupportedFeatureError);
        } else {
            for (int i=0; i < supportedIsoRates.Count(); i++) {
                int q = supportedIsoRates[i];
                isoSentitivities.append(q);
            }
        }
        supportedIsoRates.Close();
    }
    return isoSentitivities;
#endif
}

void S60CameraSettings::setManualIsoSensitivity(int iso)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        m_advancedSettings->SetIsoRate(iso);
    }
#endif
}

qreal S60CameraSettings::aperture()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        return m_advancedSettings->Aperture();
    } else {
        return -1.0;
    }
#endif
}

QList<qreal> S60CameraSettings::supportedApertures(bool *continuous)
{
    Q_UNUSED(continuous);
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    QList<qreal> apertures;
    if (m_advancedSettings) {
        RArray<TInt> supportedApertures;
        TValueInfo info = ENotActive;
        TRAPD(err, m_advancedSettings->GetAperturesL(supportedApertures, info));
        if (err != KErrNone) {
            emit error(QCamera::NotSupportedFeatureError);
        } else {
            for (int i=0; i < supportedApertures.Count(); i++) {
                qreal q = supportedApertures[i];
                apertures.append(q);
            }
        }
        supportedApertures.Close();
    }
    return apertures;
#endif
}

void S60CameraSettings::setManualAperture(qreal aperture)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        m_advancedSettings->SetAperture(aperture);
    }
#endif
}

void S60CameraSettings::lockExposure(bool lock)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
            m_advancedSettings->SetExposureLockOn(lock);
        }
#endif
}

bool S60CameraSettings::isExposureLocked()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        return m_advancedSettings->ExposureLockOn();
    } else {
        return false;
    }
#endif
}

TInt S60CameraSettings::shutterSpeed()
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        return m_advancedSettings->ShutterSpeed();
    } else {
        return -1.0;
    }
#endif
}

QList<qreal> S60CameraSettings::supportedShutterSpeeds(bool *continuous)
{
    Q_UNUSED(continuous);
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    QList<qreal> speeds;
    if (m_advancedSettings) {
        RArray<TInt> supportedSpeeds;
        TValueInfo info = ENotActive;
        TRAPD(err, m_advancedSettings->GetShutterSpeedsL(supportedSpeeds, info));
        if (err != KErrNone) {
            emit error(QCamera::NotSupportedFeatureError);
        } else {
            for (int i=0; i < supportedSpeeds.Count(); i++) {
                qreal q = supportedSpeeds[i];
                speeds.append(q);
            }
        }
        supportedSpeeds.Close();
    }
    return speeds;
#endif
}

void S60CameraSettings::setShutterSpeed(TInt speed)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_advancedSettings) {
        m_advancedSettings->SetShutterSpeed(speed);
    }
#endif
}

void S60CameraSettings::setSharpeningLevel(int value)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_imageProcessingSettings && isSharpeningSupported()) {
        m_imageProcessingSettings->SetTransformationValue(KUidECamEventImageProcessingAdjustSharpness, value);
    }
#endif
}

bool S60CameraSettings::isSharpeningSupported() const
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_imageProcessingSettings) {
        RArray<TUid> suppTransforms;
        m_imageProcessingSettings->GetSupportedTransformationsL(suppTransforms);
        if (suppTransforms.Find(KUidECamEventImageProcessingAdjustSharpness)) {
            return true;
        } else {
            return false;
        }
    }
#else
    return false;
#endif    
}

int S60CameraSettings::sharpeningLevel() const
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_imageProcessingSettings && isSharpeningSupported()) {
        return m_imageProcessingSettings->TransformationValue(KUidECamEventImageProcessingAdjustSharpness);
    } else {
        return 0;
    }
#else
    return 0;
#endif
}

void S60CameraSettings::setSaturation(int value)
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_imageProcessingSettings) {
        RArray<TUid> suppTransforms;
        m_imageProcessingSettings->GetSupportedTransformationsL(suppTransforms);
        if (suppTransforms.Find(KUidECamEventtImageProcessingAdjustSaturation)) {
            m_imageProcessingSettings->SetTransformationValue(KUidECamEventtImageProcessingAdjustSaturation, value == -1 ? 0 : value*2-100);
        }
    }
#endif
}

int S60CameraSettings::saturation() const
{
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    if (m_imageProcessingSettings) {
        RArray<TUid> suppTransforms;
        m_imageProcessingSettings->GetSupportedTransformationsL(suppTransforms);
        if (suppTransforms.Find(KUidECamEventtImageProcessingAdjustSaturation)) {
            return m_imageProcessingSettings->TransformationValue(KUidECamEventtImageProcessingAdjustSaturation);
        }
    }
    return 0;
#else
    return 0;
#endif
}


