/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef V4LCAMERACONTROL_H
#define V4LCAMERACONTROL_H

#include <QtCore/qobject.h>
#include "qcameracontrol.h"

class V4LCameraService;
class V4LCameraSession;

class V4LCameraControl : public QCameraControl
{
    Q_OBJECT
public:
    V4LCameraControl(QObject *parent = 0);
    V4LCameraControl(V4LCameraService *service, QObject *parent = 0);
    ~V4LCameraControl();

    void start();
    void stop();
    QCamera::State state() const;

    QCamera::FlashMode flashMode() const;
    void setFlashMode(QCamera::FlashMode mode);
    QCamera::FlashModes supportedFlashModes() const;
    bool isFlashReady() const;

    QCamera::FocusMode focusMode() const;
    void setFocusMode(QCamera::FocusMode mode);
    QCamera::FocusModes supportedFocusModes() const;
    QCamera::FocusStatus focusStatus() const;

    bool macroFocusingEnabled() const;
    bool isMacroFocusingSupported() const;
    void setMacroFocusingEnabled(bool);

    QCamera::ExposureMode exposureMode() const;
    void setExposureMode(QCamera::ExposureMode mode);
    QCamera::ExposureModes supportedExposureModes() const;

    double exposureCompensation() const;
    void setExposureCompensation(double ev);

    QCamera::MeteringMode meteringMode() const;
    void setMeteringMode(QCamera::MeteringMode mode);
    QCamera::MeteringModes supportedMeteringModes() const;

    QCamera::WhiteBalanceMode whiteBalanceMode() const;
    void setWhiteBalanceMode(QCamera::WhiteBalanceMode mode);
    QCamera::WhiteBalanceModes supportedWhiteBalanceModes() const;
    int manualWhiteBalance() const;
    void setManualWhiteBalance(int colorTemperature);

    int isoSensitivity() const;
    QPair<int, int> supportedIsoSensitivityRange() const;
    void setManualIsoSensitivity(int iso);
    void setAutoIsoSensitivity();

    double aperture() const;
    QPair<double, double> supportedApertureRange() const;
    void setManualAperture(double aperture);
    void setAutoAperture();

    double shutterSpeed() const;
    QPair<double, double> supportedShutterSpeedRange() const;
    void setManualShutterSpeed(double seconds);
    void setAutoShutterSpeed();

    double maximumOpticalZoom() const;
    double maximumDigitalZoom() const;
    double zoomValue() const;
    void zoomTo(int value);

    void setDevice(const QByteArray &device);

    bool isExposureLocked() const;
    bool isFocusLocked() const;

public Q_SLOTS:
    void lockExposure();
    void unlockExposure();

    void lockFocus();
    void unlockFocus();

private:
    V4LCameraSession *m_session;
    V4LCameraService *m_service;
};

#endif
