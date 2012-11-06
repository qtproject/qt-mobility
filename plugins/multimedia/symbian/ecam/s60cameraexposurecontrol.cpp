/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstring.h>

#include "s60cameraexposurecontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"
#include "s60imagecapturesettings.h"

S60CameraExposureControl::S60CameraExposureControl(QObject *parent) :
    QCameraExposureControl(parent)
{
}

S60CameraExposureControl::S60CameraExposureControl(S60ImageCaptureSession *session, QObject *parent) :
    QCameraExposureControl(parent),
    m_session(0),
    m_service(0)
{
    m_session = session;

    connect(m_session->settings(), SIGNAL(apertureChanged()), this, SLOT(apertureChanged()));
    connect(m_session->settings(), SIGNAL(apertureRangeChanged()), this, SLOT(apertureRangeChanged()));
    connect(m_session->settings(), SIGNAL(shutterSpeedChanged()), this, SLOT(shutterSpeedChanged()));
    connect(m_session->settings(), SIGNAL(isoSensitivityChanged()), this, SLOT(isoSensitivityChanged()));
    connect(m_session->settings(), SIGNAL(evChanged()), this, SLOT(evChanged()));
}

S60CameraExposureControl::~S60CameraExposureControl()
{
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

void S60CameraExposureControl::evChanged()
{
    emit exposureParameterChanged(QCameraExposureControl::ExposureCompensation);
}

QCameraExposure::ExposureMode S60CameraExposureControl::exposureMode() const
{
    return m_session->settings()->exposureMode();
}

void S60CameraExposureControl::setExposureMode(QCameraExposure::ExposureMode mode)
{
    if (isExposureModeSupported(mode))
        m_session->settings()->setExposureMode(mode);
    else
        m_session->setError(KErrNotSupported, tr("Requested exposure mode is not supported."));
}

bool S60CameraExposureControl::isExposureModeSupported(QCameraExposure::ExposureMode mode) const
{
    if (m_session->settings()->isExposureModeSupported(mode))
        return true;
    return false;
}

QCameraExposure::MeteringMode S60CameraExposureControl::meteringMode() const
{
    return m_session->settings()->meteringMode();
}

void S60CameraExposureControl::setMeteringMode(QCameraExposure::MeteringMode mode)
{
    if (isMeteringModeSupported(mode))
        m_session->settings()->setMeteringMode(mode);
    else
        m_session->setError(KErrNotSupported, tr("Requested metering mode is not supported."));
}

bool S60CameraExposureControl::isMeteringModeSupported(QCameraExposure::MeteringMode mode) const
{
    return m_session->settings()->isMeteringModeSupported(mode);
}

bool S60CameraExposureControl::isParameterSupported(ExposureParameter parameter) const
{
    switch (parameter) {
    case QCameraExposureControl::ISO:
        if (m_session->settings()->supportedIsoSensitivities().count() > 0)
            return true;
        else
            return false;
    case QCameraExposureControl::Aperture:
        if (m_session->settings()->supportedApertures().count() > 0)
            return true;
        else
            return false;
    case QCameraExposureControl::ShutterSpeed:
        if (m_session->settings()->supportedShutterSpeeds().count() > 0)
            return true;
        else
            return false;
    case QCameraExposureControl::ExposureCompensation:
        if (m_session->settings()->supportedExposureCompensations().count() > 0)
            return true;
        else
            return false;
    case QCameraExposureControl::FlashPower:
    case QCameraExposureControl::FlashCompensation:
        return false;
    default:
        return false;
    }
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
     * ISO, ExposureCompensation:
     *  - Automatic/Manual
     *  - Read/Write
     *  - Discrete range
     *
     * Aperture, ShutterSpeed, FlashPower, FlashCompensation:
     *  - Not supported
     */
    switch (parameter) {
    case QCameraExposureControl::ISO:
    case QCameraExposureControl::ExposureCompensation:
        flags |= QCameraExposureControl::AutomaticValue;
        break;
    case QCameraExposureControl::Aperture:
    case QCameraExposureControl::ShutterSpeed:
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
    switch (parameter) {
    case QCameraExposureControl::ISO:
        foreach (int iso, m_session->settings()->supportedIsoSensitivities())
            valueList << QVariant(iso);
        break;
    case QCameraExposureControl::Aperture:
        foreach (qreal aperture, m_session->settings()->supportedApertures())
            valueList << QVariant(aperture);
        break;
    case QCameraExposureControl::ShutterSpeed:
        foreach (qreal shutterSpeed, m_session->settings()->supportedShutterSpeeds())
            valueList << QVariant(shutterSpeed);
        break;
    case QCameraExposureControl::ExposureCompensation:
        foreach (qreal ev, m_session->settings()->supportedExposureCompensations())
            valueList << QVariant(ev);
        break;
    case QCameraExposureControl::FlashPower:
    case QCameraExposureControl::FlashCompensation:
        // Not supported in Symbian
        break;
    default:
        // Not supported in Symbian
        break;
    }
    return valueList;
}

bool S60CameraExposureControl::setExposureParameter(ExposureParameter parameter, const QVariant& value)
{
    if (!value.isNull()) {
        switch (parameter) {
        case QCameraExposureControl::ISO:
            return setManualIsoSensitivity(value.toInt());
        case QCameraExposureControl::Aperture:
            return setManualAperture(value.toFloat());
        case QCameraExposureControl::ShutterSpeed:
            return setManualShutterSpeed(value.toFloat());
        case QCameraExposureControl::ExposureCompensation:
            return setManualExposureCompensation(value.toFloat());
        case QCameraExposureControl::FlashPower:
        case QCameraExposureControl::FlashCompensation:
            return false;
        default:
            // Not supported in Symbian
            return false;
        }

    } else {
        // Default value
        switch (parameter) {
        case QCameraExposureControl::ISO:
            setAutoIsoSensitivity();
            return false;
        case QCameraExposureControl::Aperture:
            setAutoAperture();
            return false;
        case QCameraExposureControl::ShutterSpeed:
            setAutoShutterSpeed();
            return false;
        case QCameraExposureControl::ExposureCompensation:
            setAutoExposureCompensation();
            return false;
        case QCameraExposureControl::FlashPower:
        case QCameraExposureControl::FlashCompensation:
            return false;
        default:
            // Not supported in Symbian
            return false;
        }
    }
}

QString S60CameraExposureControl::extendedParameterName(ExposureParameter parameter)
{
    switch (parameter) {
    case QCameraExposureControl::ISO:
        return QLatin1String("ISO Sensitivity");
    case QCameraExposureControl::Aperture:
        return QLatin1String("Aperture");
    case QCameraExposureControl::ShutterSpeed:
        return QLatin1String("Shutter Speed");
    case QCameraExposureControl::ExposureCompensation:
        return QLatin1String("Exposure Compensation");
    case QCameraExposureControl::FlashPower:
        return QLatin1String("Flash Power");
    case QCameraExposureControl::FlashCompensation:
        return QLatin1String("Flash Compensation");
    default:
        return QString();
    }
}

int S60CameraExposureControl::isoSensitivity() const
{
    return m_session->settings()->isoSensitivity();
}

bool S60CameraExposureControl::isIsoSensitivitySupported(const int iso) const
{
    if (m_session->settings()->supportedIsoSensitivities().contains(iso))
        return true;
    else
        return false;
}

bool S60CameraExposureControl::setManualIsoSensitivity(int iso)
{
    if (isIsoSensitivitySupported(iso)) {
        m_session->settings()->setIsoSensitivity(iso);
        return true;
    } else {
        return false;
    }
}

void S60CameraExposureControl::setAutoIsoSensitivity()
{
    m_session->settings()->setAutoIsoSensitivity();
}

qreal S60CameraExposureControl::aperture() const
{
    return m_session->settings()->aperture();
}

bool S60CameraExposureControl::isApertureSupported(const qreal aperture) const
{
    if(m_session->settings()->supportedApertures().indexOf(aperture) != -1)
        return true;
    else
        return false;
}

bool S60CameraExposureControl::setManualAperture(qreal aperture)
{
    if (isApertureSupported(aperture)) {
        m_session->settings()->setAperture(aperture);
        return true;
    } else {
        QList<qreal> supportedApertureValues = m_session->settings()->supportedApertures();
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
        m_session->settings()->setAperture(aperture);
        return false;
    }
}

void S60CameraExposureControl::setAutoAperture()
{
    // Not supported in Symbian
}

qreal S60CameraExposureControl::shutterSpeed() const
{
    return m_session->settings()->shutterSpeed();
}

bool S60CameraExposureControl::isShutterSpeedSupported(const qreal seconds) const
{
    if(m_session->settings()->supportedShutterSpeeds().indexOf(seconds) != -1)
        return true;
    else
        return false;
}

bool S60CameraExposureControl::setManualShutterSpeed(qreal seconds)
{
    if (isShutterSpeedSupported(seconds)) {
        m_session->settings()->setShutterSpeed(seconds);
        return true;
    } else {
        QList<qreal> supportedShutterSpeeds = m_session->settings()->supportedShutterSpeeds();

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
        m_session->settings()->setShutterSpeed(seconds);
        return false;
    }
}

void S60CameraExposureControl::setAutoShutterSpeed()
{
    // Not supported in Symbian
}

qreal S60CameraExposureControl::exposureCompensation() const
{
    return m_session->settings()->exposureCompensation();
}

bool S60CameraExposureControl::isExposureCompensationSupported(const qreal ev) const
{
    if(m_session->settings()->supportedExposureCompensations().indexOf(ev) != -1)
        return true;
    else
        return false;
}

bool S60CameraExposureControl::setManualExposureCompensation(qreal ev)
{
    if (isExposureCompensationSupported(ev)) {
        m_session->settings()->setExposureCompensation(ev);
        return true;
    } else {
        QList<qreal> supportedEVs = m_session->settings()->supportedExposureCompensations();

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
        m_session->settings()->setExposureCompensation(ev);
        return false;
    }
}

void S60CameraExposureControl::setAutoExposureCompensation()
{
    // Not supported in Symbian
}

// End of file
