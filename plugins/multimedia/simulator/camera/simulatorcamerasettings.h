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

#ifndef SIMULATORCAMERASETTINGS_H
#define SIMULATORCAMERASETTINGS_H

#include "qcamera.h"

QT_BEGIN_NAMESPACE

class SimulatorCameraSettings : public QObject
{
    Q_OBJECT

public:
    SimulatorCameraSettings(QObject *parent);
    ~SimulatorCameraSettings();

    // ISO Sensitivity
    int isoSensitivity() const;
    int defaultIsoSensitivity() const;
    void setManualIsoSensitivity(int iso);
    void setAutoIsoSensitivity();
    QList<int> supportedIsoSensitivities() const;

    // Aperture
    qreal aperture() const;
    qreal defaultAperture() const;
    void setManualAperture(qreal aperture);
    void setAutoAperture();
    QList<qreal> supportedApertures() const;

    // Shutter Speed
    qreal shutterSpeed() const;
    qreal defaultShutterSpeed() const;
    void setManualShutterSpeed(qreal speed);
    void setAutoShutterSpeed();
    QList<qreal> supportedShutterSpeeds() const;

    // ExposureCompensation
    qreal exposureCompensation() const;
    qreal defaultExposureCompensation() const;
    void setExposureCompensation(qreal ev);
    void setAutoExposureCompensation();
    QList<qreal> supportedExposureCompensationValues() const;

Q_SIGNALS: // Notifications
    // For QCameraExposureControl
    void flashReady(bool ready);
    void apertureChanged();
    void apertureRangeChanged();
    void shutterSpeedChanged();
    void isoSensitivityChanged();
    void exposureCompensationChanged();

    // Errors
    void error(int, const QString&);

private: // Data
    int mIsoSensitivity;
    QList<int> mSupportedIsoSensitivities;
    qreal mAperture;
    QList<qreal> mSupportedApertures;
    qreal mShutterSpeed;
    QList<qreal> mSupportedShutterSpeeds;
    qreal mExposureCompensation;
    QList<qreal> mSupportedExposureCompensations;
};

QT_END_NAMESPACE

#endif // SIMULATORCAMERASETTINGS_H
