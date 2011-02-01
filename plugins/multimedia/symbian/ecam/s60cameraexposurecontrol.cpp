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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstring.h>

#include "s60cameraexposurecontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"

S60CameraExposureControl::S60CameraExposureControl(QObject *parent) :
    QCameraExposureControl(parent)
{
}

S60CameraExposureControl::S60CameraExposureControl(S60ImageCaptureSession *session, QObject *parent) :
    QCameraExposureControl(parent),
    m_session(NULL),
    m_service(NULL),
    m_advancedSettings(NULL),
    m_exposureMode(QCameraExposure::ExposureAuto),
    m_meteringMode(QCameraExposure::MeteringMatrix)
{
    if (session)
        m_session = session;
    else
        Q_ASSERT(true);
    // From now on it is safe to assume session exists

    connect(m_session, SIGNAL(advancedSettingChanged()), this, SLOT(resetAdvancedSetting()));
    m_advancedSettings = m_session->advancedSettings();

    if (m_advancedSettings) {
        connect(m_advancedSettings, SIGNAL(apertureChanged()), this, SLOT(apertureChanged()));
        connect(m_advancedSettings, SIGNAL(apertureRangeChanged()), this, SLOT(apertureRangeChanged()));
        connect(m_advancedSettings, SIGNAL(shutterSpeedChanged()), this, SLOT(shutterSpeedChanged()));
        connect(m_advancedSettings, SIGNAL(isoSensitivityChanged()), this, SLOT(isoSensitivityChanged()));
    }
}

S60CameraExposureControl::~S60CameraExposureControl()
{
    m_advancedSettings = NULL;
}

void S60CameraExposureControl::resetAdvancedSetting()
{
    m_advancedSettings = m_session->advancedSettings();
    if (m_advancedSettings) {
        connect(m_advancedSettings, SIGNAL(apertureChanged()), this, SLOT(apertureChanged()));
        connect(m_advancedSettings, SIGNAL(apertureRangeChanged()), this, SLOT(apertureRangeChanged()));
        connect(m_advancedSettings, SIGNAL(shutterSpeedChanged()), this, SLOT(shutterSpeedChanged()));
        connect(m_advancedSettings, SIGNAL(isoSensitivityChanged()), this, SLOT(isoSensitivityChanged()));
    }
}

void S60CameraExposureControl::apertureChanged()
{
    emit exposureParameterChanged(QCameraExposureControl::Aperture);
}

void S60CameraExposureControl::apertureRangeChanged()
{
    emit exposureParameterRangeChanged(QCameraExposureControl::Aperture);
}

void S60CameraExposureControl::shutterSpeedChanged()
{
    emit exposureParameterChanged(QCameraExposureControl::ShutterSpeed);
}

void S60CameraExposureControl::isoSensitivityChanged()
{
    emit exposureParameterChanged(QCameraExposureControl::ISO);
}

QCameraExposure::ExposureMode S60CameraExposureControl::exposureMode() const
{
    return m_session->exposureMode();
}

void S60CameraExposureControl::setExposureMode(QCameraExposure::ExposureMode mode)
{
    if (isExposureModeSupported(mode)) {
        m_exposureMode = mode;
        m_session->setExposureMode(m_exposureMode);
        return;
    }

    m_session->setError(KErrNotSupported, QString("Requested exposure mode is not supported."));
}

bool S60CameraExposureControl::isExposureModeSupported(QCameraExposure::ExposureMode mode) const
{
    if (m_session->isExposureModeSupported(mode))
        return true;

    return false;
}

QCameraExposure::MeteringMode S60CameraExposureControl::meteringMode() const
{
    if (m_advancedSettings)
        return m_advancedSettings->meteringMode();

    return QCameraExposure::MeteringMode();
}

void S60CameraExposureControl::setMeteringMode(QCameraExposure::MeteringMode mode)
{
    if (m_advancedSettings) {
        if (isMeteringModeSupported(mode)) {
            m_meteringMode = mode;
            m_advancedSettings->setMeteringMode(mode);
            return;
        }
    }

    m_session->setError(KErrNotSupported, QString("Requested metering mode is not supported."));
}

bool S60CameraExposureControl::isMeteringModeSupported(QCameraExposure::MeteringMode mode) const
{
    if (m_advancedSettings)
        return m_advancedSettings->isMeteringModeSupported(mode);

    return false;
}

bool S60CameraExposureControl::isParameterSupported(ExposureParameter parameter) const
{
    // Settings supported only if advanced settings available
    if (m_advancedSettings) {
        switch (parameter) {
            case QCameraExposureControl::ISO:
            case QCameraExposureControl::Aperture:
            case QCameraExposureControl::ShutterSpeed:
            case QCameraExposureControl::ExposureCompensation:
                return true;
            case QCameraExposureControl::FlashPower:
            case QCameraExposureControl::FlashCompensation:
                return false;

            default:
                return false;
        }
    }

    return false;
}

QVariant S60CameraExposureControl::exposureParameter(ExposureParameter parameter) const
{
    switch (parameter) {
        case QCameraExposureControl::ISO:
            return QVariant(isoSensitivity());
        case QCameraExposureControl::Aperture:
            return QVariant(aperture());
        case QCameraExposureControl::ShutterSpeed:
            return QVariant(shutterSpeed());
        case QCameraExposureControl::ExposureCompensation:
            return QVariant(exposureCompensation());
        case QCameraExposureControl::FlashPower:
        case QCameraExposureControl::FlashCompensation:
            // Not supported in Symbian
            return QVariant();

        default:
            // Not supported in Symbian
            return QVariant();
    }
}

QCameraExposureControl::ParameterFlags S60CameraExposureControl::exposureParameterFlags(ExposureParameter parameter) const
{
    QCameraExposureControl::ParameterFlags flags;

    /*
     * ISO, Aperture, ShutterSpeed:
     *  - Automatic/Manual
     *  - Read/Write
     *  - Discrete range
     *
     * ExposureCompensation:
     *  - Automatic/Manual
     *  - Read/Write
     *  - Continuous range
     *
     * FlashPower, FlashCompensation:
     *  - Not supported
     */
    switch (parameter) {
        case QCameraExposureControl::ISO:
        case QCameraExposureControl::Aperture:
        case QCameraExposureControl::ShutterSpeed:
            flags |= QCameraExposureControl::AutomaticValue;
            break;
        case QCameraExposureControl::ExposureCompensation:
            flags |= QCameraExposureControl::AutomaticValue;
            flags |= QCameraExposureControl::ContinuousRange;
            break;
        case QCameraExposureControl::FlashPower:
        case QCameraExposureControl::FlashCompensation:
            // Do nothing - no flags
            break;

        default:
            // Do nothing - no flags
            break;
    }

    return flags;
}

QVariantList S60CameraExposureControl::supportedParameterRange(ExposureParameter parameter) const
{
    QVariantList valueList;

    if (m_advancedSettings) {
        switch (parameter) {
            case QCameraExposureControl::ISO: {
                QList<int> exposureValues = m_advancedSettings->supportedIsoSensitivities();
                for (int i = 0; i < exposureValues.count(); ++i)
                    valueList.append(QVariant(exposureValues[i]));
                break;
            }
            case QCameraExposureControl::Aperture: {
                QList<qreal> apertureValues = m_advancedSettings->supportedApertures();
                for (int i = 0; i < apertureValues.count(); ++i)
                    valueList.append(QVariant(apertureValues[i]));
                break;
            }
            case QCameraExposureControl::ShutterSpeed: {
                QList<qreal> shutterSpeedValues = m_advancedSettings->supportedShutterSpeeds();
                for (int i = 0; i < shutterSpeedValues.count(); ++i)
                    valueList.append(QVariant(shutterSpeedValues[i]));
                break;
            }
            case QCameraExposureControl::ExposureCompensation: {
                QList<qreal> evValues = m_advancedSettings->supportedExposureCompensationValues();
                for (int i = 0; i < evValues.count(); ++i)
                    valueList.append(QVariant(evValues[i]));
                break;
            }
            case QCameraExposureControl::FlashPower:
            case QCameraExposureControl::FlashCompensation:
                // Not supported in Symbian
                break;

            default:
                // Not supported in Symbian
                return QVariantList();
        }
    }

    return valueList;
}

bool S60CameraExposureControl::setExposureParameter(ExposureParameter parameter, const QVariant& value)
{
    bool useDefaultValue = false;

    if (value.isNull())
        useDefaultValue = true;

    switch (parameter) {
        case QCameraExposureControl::ISO:
            if (useDefaultValue) {
                setAutoIsoSensitivity();
                return false;
            }
            else
                return setManualIsoSensitivity(value.toInt());

        case QCameraExposureControl::Aperture:
            if (useDefaultValue) {
                setAutoAperture();
                return false;
            }
            else
                return setManualAperture(value.toFloat());

        case QCameraExposureControl::ShutterSpeed:
            if (useDefaultValue) {
                setAutoShutterSpeed();
                return false;
            }
            else
                return setManualIsoSensitivity(value.toFloat());

        case QCameraExposureControl::ExposureCompensation:
            if (useDefaultValue) {
                setAutoExposureCompensation();
                return false;
            }
            else
                return setManualExposureCompensation(value.toFloat());

        case QCameraExposureControl::FlashPower:
            return false;
        case QCameraExposureControl::FlashCompensation:
            return false;

        default:
            // Not supported in Symbian
            return false;
    }
}

QString S60CameraExposureControl::extendedParameterName(ExposureParameter parameter)
{
    switch (parameter) {
        case QCameraExposureControl::ISO:
            return QString("ISO Sensitivity");
        case QCameraExposureControl::Aperture:
            return QString("Aperture");
        case QCameraExposureControl::ShutterSpeed:
            return QString("Shutter Speed");
        case QCameraExposureControl::ExposureCompensation:
            return QString("Exposure Compensation");
        case QCameraExposureControl::FlashPower:
            return QString("Flash Power");
        case QCameraExposureControl::FlashCompensation:
            return QString("Flash Compensation");

        default:
            return QString();
    }
}

int S60CameraExposureControl::isoSensitivity() const
{
    if (m_advancedSettings)
        return m_advancedSettings->isoSensitivity();
    return 0;
}

bool S60CameraExposureControl::isIsoSensitivitySupported(const int iso) const
{
    if (m_advancedSettings) {
        QList<int> supportedValues = m_advancedSettings->supportedIsoSensitivities();
        if(supportedValues.indexOf(iso) != -1)
            return true;
    }

    return false;
}

bool S60CameraExposureControl::setManualIsoSensitivity(int iso)
{
    if (m_advancedSettings) {
        if (isIsoSensitivitySupported(iso)) {
            m_advancedSettings->setManualIsoSensitivity(iso);
            return true;
        } else {
            QList<int> supportedIsoValues = m_advancedSettings->supportedIsoSensitivities();
            int minIso = supportedIsoValues.first();
            int maxIso = supportedIsoValues.last();

            if (iso < minIso) { // Smaller than minimum
                iso = minIso;
            } else if (iso > maxIso) { // Bigger than maximum
                iso = maxIso;
            } else { // Find closest
                int indexOfClosest = 0;
                int smallestDiff = 10000000; // Sensible max diff
                for(int i = 0; i < supportedIsoValues.count(); ++i) {
                    if((abs(iso - supportedIsoValues[i])) < smallestDiff) {
                        smallestDiff = abs(iso - supportedIsoValues[i]);
                        indexOfClosest = i;
                    }
                }
                iso = supportedIsoValues[indexOfClosest];
            }
            m_advancedSettings->setManualIsoSensitivity(iso);
        }
    }

    return false;
}

void S60CameraExposureControl::setAutoIsoSensitivity()
{
    if (m_advancedSettings)
        m_advancedSettings->setAutoIsoSensitivity();
}

qreal S60CameraExposureControl::aperture() const
{
    if (m_advancedSettings)
        return m_advancedSettings->aperture();
    return 0.0;
}

bool S60CameraExposureControl::isApertureSupported(const qreal aperture) const
{
    if (m_advancedSettings) {
        QList<qreal> supportedValues = m_advancedSettings->supportedApertures();
        if(supportedValues.indexOf(aperture) != -1)
            return true;
    }

    return false;
}

bool S60CameraExposureControl::setManualAperture(qreal aperture)
{
    if (m_advancedSettings) {
        if (isApertureSupported(aperture)) {
            m_advancedSettings->setManualAperture(aperture);
            return true;
        } else {
            QList<qreal> supportedApertureValues = m_advancedSettings->supportedApertures();
            int minAperture = supportedApertureValues.first();
            int maxAperture = supportedApertureValues.last();

            if (aperture < minAperture) { // Smaller than minimum
                aperture = minAperture;
            } else if (aperture > maxAperture) { // Bigger than maximum
                aperture = maxAperture;
            } else { // Find closest
                int indexOfClosest = 0;
                int smallestDiff = 100000000; // Sensible max diff
                for(int i = 0; i < supportedApertureValues.count(); ++i) {
                    if((abs((aperture*100) - (supportedApertureValues[i]*100))) < smallestDiff) {
                        smallestDiff = abs((aperture*100) - (supportedApertureValues[i]*100));
                        indexOfClosest = i;
                    }
                }
                aperture = supportedApertureValues[indexOfClosest];
            }
            m_advancedSettings->setManualAperture(aperture);
        }
    }

    return false;
}

void S60CameraExposureControl::setAutoAperture()
{
    // Not supported in Symbian
}

qreal S60CameraExposureControl::shutterSpeed() const
{
    if (m_advancedSettings)
        return m_advancedSettings->shutterSpeed();
    return 0.0;
}

bool S60CameraExposureControl::isShutterSpeedSupported(const qreal seconds) const
{
    if (m_advancedSettings) {
        QList<qreal> supportedValues = m_advancedSettings->supportedShutterSpeeds();
        if(supportedValues.indexOf(seconds) != -1)
            return true;
    }

    return false;
}

bool S60CameraExposureControl::setManualShutterSpeed(qreal seconds)
{
    if (m_advancedSettings) {
        if (isShutterSpeedSupported(seconds)) {
            m_advancedSettings->setManualShutterSpeed(seconds);
            return true;
        } else {
            QList<qreal> supportedShutterSpeeds = m_advancedSettings->supportedShutterSpeeds();

            if (supportedShutterSpeeds.count() == 0)
                return false;

            int minShutterSpeed = supportedShutterSpeeds.first();
            int maxShutterSpeed = supportedShutterSpeeds.last();

            if (seconds < minShutterSpeed) { // Smaller than minimum
                seconds = minShutterSpeed;
            } else if (seconds > maxShutterSpeed) { // Bigger than maximum
                seconds = maxShutterSpeed;
            } else { // Find closest
                int indexOfClosest = 0;
                int smallestDiff = 100000000; // Sensible max diff
                for(int i = 0; i < supportedShutterSpeeds.count(); ++i) {
                    if((abs((seconds*100) - (supportedShutterSpeeds[i]*100))) < smallestDiff) {
                        smallestDiff = abs((seconds*100) - (supportedShutterSpeeds[i]*100));
                        indexOfClosest = i;
                    }
                }
                seconds = supportedShutterSpeeds[indexOfClosest];
            }
            m_advancedSettings->setManualShutterSpeed(seconds);
        }
    }

    return false;
}

void S60CameraExposureControl::setAutoShutterSpeed()
{
    // Not supported in Symbian
}

qreal S60CameraExposureControl::exposureCompensation() const
{
    if (m_advancedSettings)
        return m_advancedSettings->exposureCompensation();
    return 0.0;
}

bool S60CameraExposureControl::isExposureCompensationSupported(const qreal ev) const
{
    if (m_advancedSettings) {
        QList<qreal> supportedValues = m_advancedSettings->supportedExposureCompensationValues();
        if(supportedValues.indexOf(ev) != -1)
            return true;
    }

    return false;
}

bool S60CameraExposureControl::setManualExposureCompensation(qreal ev)
{
    if (m_advancedSettings) {
        if (isExposureCompensationSupported(ev)) {
            m_advancedSettings->setExposureCompensation(ev);
            return true;
        } else {
            QList<qreal> supportedEVs = m_advancedSettings->supportedExposureCompensationValues();

            if (supportedEVs.count() == 0)
                return false;

            int minEV = supportedEVs.first();
            int maxEV = supportedEVs.last();

            if (ev < minEV) { // Smaller than minimum
                ev = minEV;
            } else if (ev > maxEV) { // Bigger than maximum
                ev = maxEV;
            } else { // Find closest
                int indexOfClosest = 0;
                int smallestDiff = 100000000; // Sensible max diff
                for(int i = 0; i < supportedEVs.count(); ++i) {
                    if((abs((ev*100) - (supportedEVs[i]*100))) < smallestDiff) {
                        smallestDiff = abs((ev*100) - (supportedEVs[i]*100));
                        indexOfClosest = i;
                    }
                }
                ev = supportedEVs[indexOfClosest];
            }
            m_advancedSettings->setExposureCompensation(ev);
        }
    }

    return false;
}

void S60CameraExposureControl::setAutoExposureCompensation()
{
    // Not supported in Symbian
}

// End of file
