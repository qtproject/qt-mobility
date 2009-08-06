/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCAMERACONTROL_H
#define QCAMERACONTROL_H

#include "qabstractmediacontrol.h"
#include "qabstractmediaobject.h"

#include "qcamera.h"

class QCameraControl : public QAbstractMediaControl
{
    Q_OBJECT

public:
    ~QCameraControl();

    virtual QCamera::FlashMode flashMode() const;
    virtual void setFlashMode(QCamera::FlashMode mode);
    virtual QCamera::FlashModes supportedFlashModes() const;
    virtual bool isFlashReady() const;

    virtual QCamera::FocusMode focusMode() const;
    virtual void setFocusMode(QCamera::FocusMode mode);
    virtual QCamera::FocusModes supportedFocusModes() const;
    virtual QCamera::FocusStatus focusStatus() const;

    virtual bool macroFocusingEnabled() const;
    virtual bool isMacroFocusingSupported() const;
    virtual void setMacroFocusingEnabled(bool);

    virtual QCamera::ExposureMode exposureMode() const;
    virtual void setExposureMode(QCamera::ExposureMode mode);
    virtual QCamera::ExposureModes supportedExposureModes() const;

    virtual double exposureCompensation() const;
    virtual void setExposureCompensation(double ev);

    virtual QCamera::MeteringMode meteringMode() const;
    virtual void setMeteringMode(QCamera::MeteringMode mode);
    virtual QCamera::MeteringModes supportedMeteringModes() const;

    virtual QCamera::WhiteBalanceMode whiteBalanceMode() const;
    virtual void setWhiteBalanceMode(QCamera::WhiteBalanceMode mode);
    virtual QCamera::WhiteBalanceModes supportedWhiteBalanceModes() const;
    virtual int manualWhiteBalance() const;
    virtual void setManualWhiteBalance(int colorTemperature);

    virtual int isoSensitivity() const;
    virtual QPair<int, int> supportedIsoSensitivityRange() const;
    virtual void setManualIsoSensitivity(int iso);
    virtual void setAutoIsoSensitivity();

    virtual double aperture() const;
    virtual QPair<double, double> supportedApertureRange() const;
    virtual void setManualAperture(double aperture);
    virtual void setAutoAperture();

    virtual double shutterSpeed() const;
    virtual QPair<double, double> supportedShutterSpeedRange() const;
    virtual void setManualShutterSpeed(double seconds);
    virtual void setAutoShutterSpeed();

    virtual double maximumOpticalZoom() const;
    virtual double maximumDigitalZoom() const;
    virtual double zoomValue() const;
    virtual void zoomTo(int value);

    virtual bool isValid() const = 0;

    virtual QSize frameSize() const = 0;
    virtual void setFrameSize(const QSize& s) = 0;

    virtual void setDevice(const QByteArray &device) = 0;
    virtual QList<QSize> supportedResolutions() = 0;

    virtual bool isExposureLocked() const;
    virtual bool isFocusLocked() const;

public Q_SLOTS:
    virtual void lockExposure();
    virtual void unlockExposure();

    virtual void lockFocus();
    virtual void unlockFocus();

Q_SIGNALS:
    void flashReady(bool);
    void focusStatusChanged(int);
    void zoomValueChanged(double);
    void exposureLocked();
    void focusLocked();

protected:
    QCameraControl(QObject* parent);
};

#define QCameraControl_iid "com.nokia.qt.CameraControl"
Q_MEDIA_DECLARE_CONTROL(QCameraControl, QCameraControl_iid)

#endif  // QCAMERACONTROL_H

