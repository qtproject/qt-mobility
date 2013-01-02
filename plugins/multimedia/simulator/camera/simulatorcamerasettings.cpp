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

#include "simulatorcamerasettings.h"

SimulatorCameraSettings::SimulatorCameraSettings(QObject *parent)
    : QObject(parent)
    , mIsoSensitivity(400)
    , mAperture(4)
    , mShutterSpeed(0.008)
    , mExposureCompensation(0)
{
    mSupportedIsoSensitivities << 50 << 100 << 200 << 400 << 800;
    mSupportedApertures << 1.8 << 2.8 << 4 << 5.6 << 8;
    mSupportedShutterSpeeds << 0.002 << 0.004 << 0.008 << 1./60 << 1./30;
    mSupportedExposureCompensations << -2 << 2;
}

SimulatorCameraSettings::~SimulatorCameraSettings()
{
}

int SimulatorCameraSettings::isoSensitivity() const
{
    return mIsoSensitivity;
}

QList<int> SimulatorCameraSettings::supportedIsoSensitivities() const
{
    return mSupportedIsoSensitivities;
}

void SimulatorCameraSettings::setManualIsoSensitivity(int iso)
{
    if (iso != mIsoSensitivity && supportedIsoSensitivities().contains(iso)) {
        mIsoSensitivity = iso;
        emit isoSensitivityChanged();
    }
}

void SimulatorCameraSettings::setAutoIsoSensitivity()
{
    setManualIsoSensitivity(defaultIsoSensitivity());
}

qreal SimulatorCameraSettings::aperture() const
{
    return mAperture;
}

QList<qreal> SimulatorCameraSettings::supportedApertures() const
{
    return mSupportedApertures;
}

void SimulatorCameraSettings::setManualAperture(qreal aperture)
{
    if (aperture != mAperture && supportedApertures().contains(aperture)) {
        mAperture = aperture;
        emit apertureChanged();
    }
}

void SimulatorCameraSettings::setAutoAperture()
{
    setManualAperture(defaultAperture());
}

qreal SimulatorCameraSettings::shutterSpeed() const
{
    return mShutterSpeed;
}

QList<qreal> SimulatorCameraSettings::supportedShutterSpeeds() const
{
    return mSupportedShutterSpeeds;
}

void SimulatorCameraSettings::setManualShutterSpeed(qreal speed)
{
    if (speed != mShutterSpeed && supportedShutterSpeeds().contains(speed)) {
        mShutterSpeed = speed;
        emit shutterSpeedChanged();
    }
}

void SimulatorCameraSettings::setAutoShutterSpeed()
{
    setManualShutterSpeed(defaultShutterSpeed());
}

void SimulatorCameraSettings::setExposureCompensation(qreal ev)
{
    if (ev != mExposureCompensation && ev >= mSupportedExposureCompensations.first()
            && ev <= mSupportedExposureCompensations.last()) {
        mExposureCompensation = ev;
        emit exposureCompensationChanged();
    }
}

qreal SimulatorCameraSettings::exposureCompensation() const
{
    return mExposureCompensation;
}

QList<qreal> SimulatorCameraSettings::supportedExposureCompensationValues() const
{
    return mSupportedExposureCompensations;
}

void SimulatorCameraSettings::setAutoExposureCompensation()
{
    setExposureCompensation(defaultExposureCompensation());
}

int SimulatorCameraSettings::defaultIsoSensitivity() const
{
    return 400;
}

qreal SimulatorCameraSettings::defaultAperture() const
{
    return 4;
}

qreal SimulatorCameraSettings::defaultShutterSpeed() const
{
    return 0.008;
}

qreal SimulatorCameraSettings::defaultExposureCompensation() const
{
    return 0;
}

// End of file
