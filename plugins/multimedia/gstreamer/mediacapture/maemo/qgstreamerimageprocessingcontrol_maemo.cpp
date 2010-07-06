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

#include "qgstreamerimageprocessingcontrol_maemo.h"
#include "qgstreamercapturesession_maemo.h"

QGstreamerImageProcessingControl::QGstreamerImageProcessingControl(GstElement &camerabin, QGstreamerCaptureSession *session)
    :QCameraImageProcessingControl(session),
     m_session(session),
     m_camerabin(camerabin)
{
    m_mappedWbValues[GST_PHOTOGRAPHY_WB_MODE_AUTO] = QCameraImageProcessing::WhiteBalanceAuto;
    m_mappedWbValues[GST_PHOTOGRAPHY_WB_MODE_DAYLIGHT] = QCameraImageProcessing::WhiteBalanceSunlight;
    m_mappedWbValues[GST_PHOTOGRAPHY_WB_MODE_CLOUDY] = QCameraImageProcessing::WhiteBalanceCloudy;
    m_mappedWbValues[GST_PHOTOGRAPHY_WB_MODE_SUNSET] = QCameraImageProcessing::WhiteBalanceSunset;
    m_mappedWbValues[GST_PHOTOGRAPHY_WB_MODE_TUNGSTEN] = QCameraImageProcessing::WhiteBalanceTungsten;
    m_mappedWbValues[GST_PHOTOGRAPHY_WB_MODE_FLUORESCENT] = QCameraImageProcessing::WhiteBalanceFluorescent;

    updateColorBalanceValues();
}

QGstreamerImageProcessingControl::~QGstreamerImageProcessingControl()
{
}

void QGstreamerImageProcessingControl::updateColorBalanceValues()
{
    if (!GST_IS_COLOR_BALANCE(&m_camerabin)) {
        // Camerabin doesn't implement gstcolorbalance interface
        return;
    }

    GstColorBalance *balance = GST_COLOR_BALANCE(&m_camerabin);
    const GList *controls = gst_color_balance_list_channels(balance);

    const GList *item;
    GstColorBalanceChannel *channel;
    gint cur_value;

    for (item = controls; item; item = g_list_next (item)) {
        channel = (GstColorBalanceChannel *)item->data;
        cur_value = gst_color_balance_get_value (balance, channel);

        if (!g_ascii_strcasecmp (channel->label, "brightness")) {
            m_values[QCameraImageProcessingControl::Brightness] = cur_value;
        } else if (!g_ascii_strcasecmp (channel->label, "contrast")) {
            m_values[QCameraImageProcessingControl::Contrast] = cur_value;
        } else if (!g_ascii_strcasecmp (channel->label, "saturation")) {
            m_values[QCameraImageProcessingControl::Saturation] = cur_value;
        }
    }
}

bool QGstreamerImageProcessingControl::setColorBalanceValue(const QString& channel, int value)
{

    if (!GST_IS_COLOR_BALANCE(&m_camerabin)) {
        // Camerabin doesn't implement gstcolorbalance interface
        return false;
    }

    GstColorBalance *balance = GST_COLOR_BALANCE(&m_camerabin);
    const GList *controls = gst_color_balance_list_channels(balance);

    const GList *item;
    GstColorBalanceChannel *colorBalanceChannel;

    for (item = controls; item; item = g_list_next (item)) {
        colorBalanceChannel = (GstColorBalanceChannel *)item->data;

        if (!g_ascii_strcasecmp (colorBalanceChannel->label, channel.toAscii())) {
            gst_color_balance_set_value (balance, colorBalanceChannel, value);
            return true;
        }
    }

    return false;
}

QCameraImageProcessing::WhiteBalanceMode QGstreamerImageProcessingControl::whiteBalanceMode() const
{
    GstWhiteBalanceMode wbMode;
    gst_photography_get_white_balance_mode(GST_PHOTOGRAPHY(&m_camerabin), &wbMode);
    return m_mappedWbValues[wbMode];
}

void QGstreamerImageProcessingControl::setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode mode)
{
    if (isWhiteBalanceModeSupported(mode))
        gst_photography_set_white_balance_mode(GST_PHOTOGRAPHY(&m_camerabin), m_mappedWbValues.key(mode));
}

bool QGstreamerImageProcessingControl::isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode mode) const
{
    return m_mappedWbValues.values().contains(mode);
}

int QGstreamerImageProcessingControl::manualWhiteBalance() const
{
    return 0;
}

void QGstreamerImageProcessingControl::setManualWhiteBalance(int colorTemperature)
{
    Q_UNUSED(colorTemperature);
}

bool QGstreamerImageProcessingControl::isProcessingParameterSupported(QCameraImageProcessingControl::ProcessingParameter parameter) const
{
    return parameter == QCameraImageProcessingControl::Contrast
            || parameter == QCameraImageProcessingControl::Brightness
            || parameter == QCameraImageProcessingControl::Saturation;
}

QVariant QGstreamerImageProcessingControl::processingParameter(
        QCameraImageProcessingControl::ProcessingParameter parameter) const
{
    if (m_values.contains(parameter))
        return m_values.value(parameter);
    else
        return QVariant();
}

void QGstreamerImageProcessingControl::setProcessingParameter(
        QCameraImageProcessingControl::ProcessingParameter parameter,
        QVariant value)
{
    switch (parameter) {
    case Contrast:
        setColorBalanceValue("contrast", value.toInt());
        break;
    case Brightness:
        setColorBalanceValue("brightness", value.toInt());
        break;
    case Saturation:
        setColorBalanceValue("saturation", value.toInt());
        break;
    default:
        break;
    }

    updateColorBalanceValues();
}

QList<QByteArray> QGstreamerImageProcessingControl::supportedPresets() const
{
    return m_presets;
}

QString QGstreamerImageProcessingControl::presetDescription(const QByteArray &preset) const
{
    return QString::fromLatin1(preset);
}

QByteArray QGstreamerImageProcessingControl::preset() const
{
    return m_preset;
}

void QGstreamerImageProcessingControl::setPreset(const QByteArray &preset)
{
    if (m_presets.contains(preset))
        m_preset = preset;
    else
        m_preset.clear();
}
