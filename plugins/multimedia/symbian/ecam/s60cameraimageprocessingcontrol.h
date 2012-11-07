/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef S60CAMERAIMAGEPROCESSINGCONTROL_H
#define S60CAMERAIMAGEPROCESSINGCONTROL_H

#include <qcameraimageprocessing.h>
#include <qcameraimageprocessingcontrol.h>

QT_USE_NAMESPACE

QT_FORWARD_DECLARE_CLASS(S60CameraService)
QT_FORWARD_DECLARE_CLASS(S60ImageCaptureSession)

/*
 * Control for image processing related camera operations (inc. white balance).
 */
class S60CameraImageProcessingControl : public QCameraImageProcessingControl
{
    Q_OBJECT

public: // Constructors & Destructor

    S60CameraImageProcessingControl(QObject *parent = 0);
    S60CameraImageProcessingControl(S60ImageCaptureSession *session, QObject *parent = 0);
    ~S60CameraImageProcessingControl();

public: // QCameraImageProcessingControl

    // White Balance
    QCameraImageProcessing::WhiteBalanceMode whiteBalanceMode() const;
    void setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode mode);
    bool isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode mode) const;

    // Processing Parameter
    bool isProcessingParameterSupported(ProcessingParameter parameter) const;
    QVariant processingParameter(QCameraImageProcessingControl::ProcessingParameter parameter) const;
    void setProcessingParameter(QCameraImageProcessingControl::ProcessingParameter parameter, QVariant value);

private: // Internal operations - Implementing ProcessingParameter

    // Manual White Balance (Color Temperature)
    int manualWhiteBalance() const;
    void setManualWhiteBalance(int colorTemperature);

    // Contrast
    int contrast() const;
    void setContrast(int value);

    // Brightness
    int brightness() const;
    void setBrightness(int value);

    // Saturation
    bool isSaturationSupported() const;
    int saturation() const;
    void setSaturation(int value);

    // Sharpening
    bool isSharpeningSupported() const;
    int sharpeningLevel() const;
    void setSharpeningLevel(int value);

    // Denoising
    bool isDenoisingSupported() const;
    int denoisingLevel() const;
    void setDenoisingLevel(int value);

private: // Data

    S60ImageCaptureSession  *m_session;
};

#endif // S60CAMERAIMAGEPROCESSINGCONTROL_H
