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

// S60 3.2 Platform
#ifdef USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
#define POST_31_PLATFORM
#include <ecamadvancedsettings.h> // CCameraAdvancedSettings (inc. TValueInfo)
#endif // S60 3.2

// S60 5.0 or later
#ifdef USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER
#define POST_31_PLATFORM
#include <ecamadvsettings.h>    // CCameraAdvancedSettings
#include <ecam/ecamconstants.h> // TValueInfo
#endif // S60 5.0 or later

S60CameraSettings::S60CameraSettings(QObject *parent, CCameraEngine *engine) :
    QObject(parent)
{
    m_cameraEngine = engine;
    
#ifdef POST_31_PLATFORM
    TInt err = KErrNone;
    TRAP(err, m_advancedSettings = CCamera::CCameraAdvancedSettings::NewL(*m_cameraEngine->Camera()));
    TRAP(err, m_imageProcessingSettings = CCamera::CCameraImageProcessing::NewL(*m_cameraEngine->Camera()));
#endif // POST_31_PLATFORM
}

S60CameraSettings::~S60CameraSettings()
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        delete m_advancedSettings;
        m_advancedSettings = NULL;
    }
    
    if (m_imageProcessingSettings) {
        delete m_imageProcessingSettings;
        m_imageProcessingSettings = NULL;
    }
#endif // POST_31_PLATFORM
}

void S60CameraSettings::setFocusMode(QCameraFocus::FocusMode mode)
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        switch (mode) {
            case QCameraFocus::ManualFocus: // Manual focus mode
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeManual);
                break;
            case QCameraFocus::AutoFocus: // One-shot autofocus mode
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeAuto);
                m_advancedSettings->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle);
                break;
                                
            default:
                break;
        }
        return;
    }
#else // S60 3.1
    Q_UNUSED(mode);
#endif // POST_31_PLATFORM

    emit error(QCamera::CameraError);
}

void S60CameraSettings::cancelFocusing()
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        m_advancedSettings->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff);
    }
#endif // POST_31_PLATFORM
}

QCameraFocus::FocusMode S60CameraSettings::focusMode()
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        CCamera::CCameraAdvancedSettings::TFocusMode mode = m_advancedSettings->FocusMode();
        switch (mode) {
            case CCamera::CCameraAdvancedSettings::EFocusModeManual:
                return QCameraFocus::ManualFocus;
            case CCamera::CCameraAdvancedSettings::EFocusModeAuto:
                return QCameraFocus::AutoFocus;
                
            default:
                return (QCameraFocus::FocusMode)0; // No mode
        }
    }
#endif // POST_31_PLATFORM
    
    return (QCameraFocus::FocusMode)0; // No mode
}

QCameraFocus::FocusModes S60CameraSettings::supportedFocusModes()
{
    QCameraFocus::FocusModes modes = 0;
    
#ifdef POST_31_PLATFORM
    TInt supportedModes = 0;
    TInt autoFocusTypes = 0;
//    QCameraFocus::FocusModes modes = QCameraFocus::AutoFocus;
    
    if (m_advancedSettings) {
        supportedModes = m_advancedSettings->SupportedFocusModes();
        autoFocusTypes = m_advancedSettings->SupportedAutoFocusTypes();
        
        if (supportedModes == 0)
            return modes;
        
        if (supportedModes & CCamera::CCameraAdvancedSettings::EFocusModeAuto) {
            if (autoFocusTypes & CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle)
                modes |= QCameraFocus::AutoFocus;
            if (autoFocusTypes & CCamera::CCameraAdvancedSettings::EAutoFocusTypeContinuous)
                modes |= QCameraFocus::ContinuousFocus;
        }
    }
#endif // POST_31_PLATFORM
    
    return modes;
}

qreal S60CameraSettings::opticalZoomFactorL() const
{
    qreal factor = 1.0;

#ifdef POST_31_PLATFORM
    int symbianFactor = 0;
    if (m_advancedSettings)
        symbianFactor = m_advancedSettings->OpticalZoom();
        else
            User::Leave(KErrNotSupported);

    if (symbianFactor != 0)
        factor = symbianFactor / KSymbianFineResolutionFactor;
#endif // POST_31_PLATFORM

    return factor;
}

void S60CameraSettings::setOpticalZoomFactorL(const qreal zoomFactor)
{
#ifdef POST_31_PLATFORM
    int symbianFactor = zoomFactor * KSymbianFineResolutionFactor;

    // Make sure value is supported, and modify if needed

    if (m_advancedSettings)
        m_advancedSettings->SetDigitalZoom(symbianFactor);
    else
        User::Leave(KErrNotSupported);
#else // S60 3.1 Platform
    Q_UNUSED(zoomFactor);
    emit error(QCamera::NotSupportedFeatureError, QString("Settings optical zoom factor is not supported."));
#endif // POST_31_PLATFORM
}

QList<qreal> *S60CameraSettings::supportedDigitalZoomFactors()
{
    return &m_supportedDigitalZoomFactors;
}

qreal S60CameraSettings::digitalZoomFactorL() const
{
    qreal factor = 1.0;

#ifdef POST_31_PLATFORM
    int symbianFactor = 0;
    if (m_advancedSettings)
        symbianFactor = m_advancedSettings->DigitalZoom();
        else
            User::Leave(KErrNotSupported);

    if (symbianFactor != 0)
        factor = symbianFactor / KSymbianFineResolutionFactor;
#endif // POST_31_PLATFORM

    return factor;
}

void S60CameraSettings::setDigitalZoomFactorL(const qreal zoomFactor)
{
#ifdef POST_31_PLATFORM
    int symbianFactor = zoomFactor * KSymbianFineResolutionFactor;

    // Make sure value is supported, and modify if needed

    if (m_advancedSettings)
        m_advancedSettings->SetDigitalZoom(symbianFactor);
    else
        User::Leave(KErrNotSupported);
#else // S60 3.1 Platform
    Q_UNUSED(zoomFactor);
    emit error(QCamera::NotSupportedFeatureError, QString("Settings digital zoom factor is not supported."));
#endif // POST_31_PLATFORM
}

// MCameraObserver2
void S60CameraSettings::HandleAdvancedEvent(const TECAMEvent& aEvent)
{
#ifdef POST_31_PLATFORM
    if (aEvent.iErrorCode != KErrNone) {
        switch (aEvent.iErrorCode) {
            case KErrECamCameraDisabled:
                emit error(QCamera::CameraError, QString("Unexpected camera error."));
                return;
            case KErrECamSettingDisabled:
                emit error(QCamera::CameraError, QString("Unexpected camera error."));
                return;
            case KErrECamParameterNotInRange:
                emit error(QCamera::NotSupportedFeatureError, QString("Requested value is not in supported range."));
                return;
            case KErrECamSettingNotSupported:
                emit error(QCamera::NotSupportedFeatureError, QString("Requested setting is not supported."));
                return;
            case KErrECamNotOptimalFocus:
                emit focusStatusChanged(QCamera::Unlocked, QCamera::LockFailed);
                return;
        }

        if (aEvent.iEventType == KUidECamEventCameraSettingFocusRange ||
            aEvent.iEventType == KUidECamEventCameraSettingAutoFocusType2) {
            emit focusStatusChanged(QCamera::Unlocked, QCamera::LockFailed);
            return;
        } else if (aEvent.iEventType == KUidECamEventCameraSettingIsoRate) {
            if (aEvent.iErrorCode == KErrNotSupported)
                emit error(QCamera::NotSupportedFeatureError, QString("Requested ISO value is not supported."));
            else
                emit error(QCamera::CameraError, QString("Setting ISO value failed."));
            return;
        } else if (aEvent.iEventType == KUidECamEventCameraSettingAperture) {
            if (aEvent.iErrorCode == KErrNotSupported)
                emit error(QCamera::NotSupportedFeatureError, QString("Requested aperture value is not supported."));
            else
                emit error(QCamera::CameraError, QString("Setting aperture value failed."));
            return;
        } else if (aEvent.iEventType == KUidECamEventCameraSettingOpticalZoom ||
                   aEvent.iEventType == KUidECamEventCameraSettingDigitalZoom) {
            if (aEvent.iErrorCode == KErrNotSupported)
                return; // Discard
            else {
                emit error(QCamera::CameraError, QString("Setting zoom factor failed."));
                return;
            }
        } else {
            emit error(QCamera::CameraError, QString("Unexpected camera error."));
            return;
        }
    }

    if (aEvent.iEventType == KUidECamEventCameraSettingExposureLock) {
        if (m_advancedSettings->ExposureLockOn())
            emit exposureStatusChanged(QCamera::Locked, QCamera::LockAcquired);
        else
            emit exposureStatusChanged(QCamera::Unlocked, QCamera::LockLost);
    }
    else if (aEvent.iEventType == KUidECamEventCameraSettingAperture)
        emit apertureChanged();
        
    else if (aEvent.iEventType == KUidECamEventCameraSettingApertureRange)
        emit apertureRangeChanged();
        
    else if (aEvent.iEventType == KUidECamEventCameraSettingIsoRate)
        emit isoSensitivityChanged();
        
    else if (aEvent.iEventType == KUidECamEventCameraSettingShutterSpeed)
        emit shutterSpeedChanged();
        
    else if (aEvent.iEventType == KUidECamEventFlashReady)
        emit flashReady(true);
        
    else if (aEvent.iEventType == KUidECamEventFlashNotReady)
        emit flashReady(false);
        
    else if (aEvent.iEventType.iUid == KUidECamEventCameraSettingsOptimalFocusUidValue) {
        if (aEvent.iErrorCode == KErrNone)
            emit focusStatusChanged(QCamera::Locked, QCamera::LockAcquired);
        else {
            emit focusStatusChanged(QCamera::Unlocked, QCamera::LockFailed);
        }
        
    } else if (aEvent.iEventType == KUidECamEventCameraSettingFocusMode)
        emit focusStatusChanged(QCamera::Searching, QCamera::UserRequest);
#endif // POST_31_PLATFORM
}

bool S60CameraSettings::isFlashReady()
{
    TBool isReady = false;
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        int flashErr = m_advancedSettings->IsFlashReady(isReady);
        if (flashErr == KErrNotSupported) {
            emit error(QCamera::CameraError);
            return false;
        }
    }
#endif
    return isReady;
}

QCameraExposure::MeteringMode S60CameraSettings::meteringMode()
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        CCamera::CCameraAdvancedSettings::TMeteringMode mode = m_advancedSettings->MeteringMode();
        switch (mode) {
            case CCamera::CCameraAdvancedSettings::EMeteringModeCenterWeighted:
                return QCameraExposure::MeteringAverage;
            case CCamera::CCameraAdvancedSettings::EMeteringModeEvaluative:
                return QCameraExposure::MeteringMatrix;
            case CCamera::CCameraAdvancedSettings::EMeteringModeSpot:
                return QCameraExposure::MeteringSpot;
                
            default:
                return (QCameraExposure::MeteringMode)0;
        }
    }
#endif // POST_31_PLATFORM
    
    return (QCameraExposure::MeteringMode)0;
}

void S60CameraSettings::setMeteringMode(QCameraExposure::MeteringMode mode)
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        switch(mode) {
            case QCameraExposure::MeteringAverage:
                m_advancedSettings->SetMeteringMode(CCamera::CCameraAdvancedSettings::EMeteringModeCenterWeighted);
                break;
            case QCameraExposure::MeteringMatrix:
                m_advancedSettings->SetMeteringMode(CCamera::CCameraAdvancedSettings::EMeteringModeEvaluative);
                break;
            case QCameraExposure::MeteringSpot:
                m_advancedSettings->SetMeteringMode(CCamera::CCameraAdvancedSettings::EMeteringModeSpot);
                break;
            default:
                break;
        }
        return;
    }
#else // S60 3.1
    Q_UNUSED(mode);
#endif // POST_31_PLATFORM
    
    emit error(QCamera::CameraError);
}

bool S60CameraSettings::isMeteringModeSupported(QCameraExposure::MeteringMode mode)
{
#ifdef POST_31_PLATFORM
    TInt supportedModes = 0;
    
    if (m_advancedSettings) {
        supportedModes = m_advancedSettings->SupportedMeteringModes();
        if (supportedModes == 0)
            return false;

        switch (mode) {
            case QCameraExposure::MeteringMatrix:
                if (supportedModes & CCamera::CCameraAdvancedSettings::EMeteringModeEvaluative)
                    return true;
                else
                    return false;
            case QCameraExposure::MeteringAverage:
                if (supportedModes & CCamera::CCameraAdvancedSettings::EMeteringModeCenterWeighted)
                    return true;
                else
                    return false;
            case QCameraExposure::MeteringSpot:
                if (supportedModes & CCamera::CCameraAdvancedSettings::EMeteringModeSpot)
                    return true;
                else
                    return false;
                
            default:
                return false;
        }
    }
#else // S60 3.1
    Q_UNUSED(mode);
#endif // POST_31_PLATFORM
    
    return false;
}

int S60CameraSettings::isoSensitivity()
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings)
        return m_advancedSettings->IsoRate();
#endif // POST_31_PLATFORM
    
    return 0;
}

QList<int> S60CameraSettings::supportedIsoSensitivities()
{
    QList<int> isoSentitivities;
#ifdef POST_31_PLATFORM
    
    if (m_advancedSettings) {
        RArray<TInt> supportedIsoRates;
        
        TRAPD(err, m_advancedSettings->GetSupportedIsoRatesL(supportedIsoRates));
        if (err != KErrNone)
            emit error(QCamera::NotSupportedFeatureError);
        else {
            for (int i = 0; i < supportedIsoRates.Count(); i++) {
                int q = supportedIsoRates[i];
                isoSentitivities.append(q);
            }
        }
        supportedIsoRates.Close();
        
        return isoSentitivities;
    }
    
#endif // POST_31_PLATFORM
    
    return isoSentitivities;
}

void S60CameraSettings::setManualIsoSensitivity(int iso)
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        m_advancedSettings->SetIsoRate(iso);
        return;
    }
#else // S60 3.1
    Q_UNUSED(iso);
#endif // POST_31_PLATFORM
    
    emit error(QCamera::CameraError);
}

void S60CameraSettings::setAutoIsoSensitivity()
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        TRAPD(err, m_advancedSettings->SetISORateL(CCamera::CCameraAdvancedSettings::EISOAutoUnPrioritised, 0));
        if (err) {
            emit error(QCamera::CameraError);
        }
        return;
    }
#endif // POST_31_PLATFORM
    
    emit error(QCamera::CameraError);
}

qreal S60CameraSettings::aperture()
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings)
        return m_advancedSettings->Aperture();
#endif // POST_31_PLATFORM
    
    return -1.0;
}

QList<qreal> S60CameraSettings::supportedApertures()
{
    QList<qreal> apertures;
    
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        RArray<TInt> supportedApertures;
        TValueInfo info = ENotActive;
        
        TRAPD(err, m_advancedSettings->GetAperturesL(supportedApertures, info));
        if (err != KErrNone)
            emit error(QCamera::NotSupportedFeatureError);
        else {
            for (int i = 0; i < supportedApertures.Count(); i++) {
                qreal q = supportedApertures[i];
                apertures.append(q);
            }
        }
        supportedApertures.Close();
        
        return apertures;
    }
#endif // POST_31_PLATFORM
    
    return apertures;
}

void S60CameraSettings::setManualAperture(qreal aperture)
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        int symbianAperture = (aperture * 100); // KECamFineResolutionFactor = 100
        m_advancedSettings->SetAperture(symbianAperture);
        return;
    }
#else // S60 3.1
    Q_UNUSED(aperture);
#endif // POST_31_PLATFORM
    
    emit error(QCamera::CameraError);
}

void S60CameraSettings::lockExposure(bool lock)
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        m_advancedSettings->SetExposureLockOn(lock);
        return;
    }
#else // S60 3.1
    Q_UNUSED(lock);
#endif // POST_31_PLATFORM
    
    emit error(QCamera::CameraError);
}

bool S60CameraSettings::isExposureLocked()
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings)
        return m_advancedSettings->ExposureLockOn();
#endif // POST_31_PLATFORM
    
    return false;
}

TInt S60CameraSettings::shutterSpeed()
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings)
        return m_advancedSettings->ShutterSpeed();
#endif // POST_31_PLATFORM
    
    return -1;
}

QList<qreal> S60CameraSettings::supportedShutterSpeeds()
{
    QList<qreal> speeds;
    
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        RArray<TInt> supportedSpeeds;
        TValueInfo info = ENotActive;
        
        TRAPD(err, m_advancedSettings->GetShutterSpeedsL(supportedSpeeds, info));
        if (err != KErrNone)
            emit error(QCamera::NotSupportedFeatureError);
        else {
            for (int i = 0; i < supportedSpeeds.Count(); i++) {
                qreal q = supportedSpeeds[i];
                speeds.append(q);
            }
        }
        
        supportedSpeeds.Close();
        return speeds;
    }
#endif // POST_31_PLATFORM
    
    return speeds;
}

void S60CameraSettings::setManualShutterSpeed(TInt speed)
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        m_advancedSettings->SetShutterSpeed(speed);
        return;
    }
#else // S60 3.1
    Q_UNUSED(speed);
#endif // POST_31_PLATFORM
    
    emit error(QCamera::CameraError);
}

void S60CameraSettings::setExposureCompensation(qreal ev)
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        m_advancedSettings->SetExposureCompensation(ev);
        return;
    }
#else // S60 3.1
    Q_UNUSED(ev);
#endif

    emit error(QCamera::CameraError);
}

qreal S60CameraSettings::exposureCompensation()
{
#ifdef POST_31_PLATFORM
    if (m_advancedSettings)
        return m_advancedSettings->ExposureCompensation();
#endif // POST_31_PLATFORM

    return 0;
}

QList<qreal> S60CameraSettings::supportedExposureCompensationValues()
{
    QList<qreal> valueList;

#ifdef POST_31_PLATFORM
    if (m_advancedSettings) {
        RArray<TInt> evSteps;
        TValueInfo info;
        TRAPD(err, m_advancedSettings->GetExposureCompensationStepsL(evSteps, info));
        if (err) {
            return valueList;
        }

        if (info == ENotActive || evSteps.Count() == 0) {
            // EV not supported, return empty list
            return valueList;
        }

        for (int i = 0; i < evSteps.Count(); ++i) {
            qreal appendValue = evSteps[i];
            appendValue /= KECamFineResolutionFactor;
            valueList.append(appendValue);
        }
        return valueList;
    }
#endif // POST_31_PLATFORM

    return valueList;
}

void S60CameraSettings::setSharpeningLevel(int value)
{
#ifdef POST_31_PLATFORM
    if (m_imageProcessingSettings && isSharpeningSupported()) {
        m_imageProcessingSettings->SetTransformationValue(KUidECamEventImageProcessingAdjustSharpness, value);
        return;
    }
#else // S60 3.1
    Q_UNUSED(value);
#endif // POST_31_PLATFORM

    emit error(QCamera::CameraError);
}

bool S60CameraSettings::isSharpeningSupported() const
{
#ifdef POST_31_PLATFORM
    if (m_imageProcessingSettings) {
        RArray<TUid> suppTransforms;
        m_imageProcessingSettings->GetSupportedTransformationsL(suppTransforms);

        if (suppTransforms.Find(KUidECamEventImageProcessingAdjustSharpness))
            return true;
    }
#endif // POST_31_PLATFORM

    return false;
}

int S60CameraSettings::sharpeningLevel() const
{
#ifdef POST_31_PLATFORM
    if (m_imageProcessingSettings && isSharpeningSupported())
        return m_imageProcessingSettings->TransformationValue(KUidECamEventImageProcessingAdjustSharpness);
#endif // POST_31_PLATFORM

    return 0;
}

void S60CameraSettings::setSaturation(int value)
{
#ifdef POST_31_PLATFORM
    if (m_imageProcessingSettings) {
        RArray<TUid> suppTransforms;
        m_imageProcessingSettings->GetSupportedTransformationsL(suppTransforms);

        if (suppTransforms.Find(KUidECamEventtImageProcessingAdjustSaturation))
            m_imageProcessingSettings->SetTransformationValue(KUidECamEventtImageProcessingAdjustSaturation, value == -1 ? 0 : value*2-100);
        return;
    }
#else // S60 3.1
    Q_UNUSED(value);
#endif // POST_31_PLATFORM

    emit error(QCamera::CameraError);
}

int S60CameraSettings::saturation() const
{
#ifdef POST_31_PLATFORM
    if (m_imageProcessingSettings) {
        RArray<TUid> suppTransforms;
        m_imageProcessingSettings->GetSupportedTransformationsL(suppTransforms);

        if (suppTransforms.Find(KUidECamEventtImageProcessingAdjustSaturation))
            return m_imageProcessingSettings->TransformationValue(KUidECamEventtImageProcessingAdjustSaturation);
    }
#endif // POST_31_PLATFORM

    return 0;
}

// End of file
