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

#ifndef QGSTREAMERIMAGEPROCESSINGCONTROL_H
#define QGSTREAMERIMAGEPROCESSINGCONTROL_H

#include <qcamera.h>
#include <qcameraimageprocessingcontrol.h>

#include <gst/gst.h>
#include <glib.h>

#include <gst/interfaces/photography.h>
#include <gst/interfaces/colorbalance.h>

class QGstreamerCaptureSession;

QT_USE_NAMESPACE

class QGstreamerImageProcessingControl : public QCameraImageProcessingControl
{
    Q_OBJECT

public:
    QGstreamerImageProcessingControl(GstElement &camerabin, QGstreamerCaptureSession *session);
    virtual ~QGstreamerImageProcessingControl();

    QCameraImageProcessing::WhiteBalanceMode whiteBalanceMode() const;
    void setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode mode);
    bool isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode mode) const;

    int manualWhiteBalance() const;
    void setManualWhiteBalance(int colorTemperature);

    bool isProcessingParameterSupported(ProcessingParameter) const;
    QVariant processingParameter(ProcessingParameter parameter) const;
    void setProcessingParameter(ProcessingParameter parameter, QVariant value);

    QList<QByteArray> supportedPresets() const;
    QString presetDescription(const QByteArray &preset) const;
    QByteArray preset() const;
    void setPreset(const QByteArray &preset);

private:
    bool setColorBalanceValue(const QString& channel, int value);
    void updateColorBalanceValues();

private:
    QGstreamerCaptureSession *m_session;
    GstElement &m_camerabin;
    QMap<QCameraImageProcessingControl::ProcessingParameter, int> m_values;
    QMap<GstWhiteBalanceMode, QCameraImageProcessing::WhiteBalanceMode> m_mappedWbValues;
    QList<QByteArray> m_presets;
    QByteArray m_preset;
};

#endif // QGSTREAMERIMAGEPROCESSINGCONTROL_H
