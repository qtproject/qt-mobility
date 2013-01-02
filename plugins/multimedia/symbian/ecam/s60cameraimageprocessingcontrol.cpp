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

#include <QtCore/qstring.h>

#include "s60cameraimageprocessingcontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"
#include "s60imagecapturesettings.h"

S60CameraImageProcessingControl::S60CameraImageProcessingControl(QObject *parent) :
    QCameraImageProcessingControl(parent)
{
}

S60CameraImageProcessingControl::S60CameraImageProcessingControl(S60ImageCaptureSession *session, QObject *parent) :
    QCameraImageProcessingControl(parent),
    m_session(0)
{
    m_session = session;
}

S60CameraImageProcessingControl::~S60CameraImageProcessingControl()
{
}

QCameraImageProcessing::WhiteBalanceMode S60CameraImageProcessingControl::whiteBalanceMode() const
{
    return m_session->settings()->whiteBalanceMode();
}

void S60CameraImageProcessingControl::setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode mode)
{
    if (isWhiteBalanceModeSupported(mode))
        m_session->settings()->setWhiteBalanceMode(mode);
    else
        m_session->setError(KErrNotSupported, tr("Requested white balance mode is not supported."));
}

bool S60CameraImageProcessingControl::isWhiteBalanceModeSupported(
    QCameraImageProcessing::WhiteBalanceMode mode) const
{
    return m_session->settings()->isWhiteBalanceModeSupported(mode);
}

int S60CameraImageProcessingControl::manualWhiteBalance() const
{
    return 0;
}

void S60CameraImageProcessingControl::setManualWhiteBalance(int colorTemperature)
{
    m_session->setError(KErrNotSupported, tr("Setting manual white balance is not supported."));
    Q_UNUSED(colorTemperature)
}

bool S60CameraImageProcessingControl::isProcessingParameterSupported(ProcessingParameter parameter) const
{
    switch (parameter) {
    case QCameraImageProcessingControl::Contrast:
    case QCameraImageProcessingControl::Brightness:
        return m_session->settings()->areContrastAndBrightnessSupported();
    case QCameraImageProcessingControl::Saturation:
        return isSaturationSupported();
    case QCameraImageProcessingControl::Sharpening:
        return isSharpeningSupported();
    case QCameraImageProcessingControl::Denoising:
        return isDenoisingSupported();
    case QCameraImageProcessingControl::ColorTemperature:
        return false;
    default:
        return false;
    }
}

QVariant S60CameraImageProcessingControl::processingParameter(
    QCameraImageProcessingControl::ProcessingParameter parameter) const
{
    switch (parameter) {
    case QCameraImageProcessingControl::Contrast:
        return QVariant(contrast());
    case QCameraImageProcessingControl::Saturation:
        return QVariant(saturation());
    case QCameraImageProcessingControl::Brightness:
        return QVariant(brightness());
    case QCameraImageProcessingControl::Sharpening:
        return QVariant(sharpeningLevel());
    case QCameraImageProcessingControl::Denoising:
        return QVariant(denoisingLevel());
    case QCameraImageProcessingControl::ColorTemperature:
        return QVariant(manualWhiteBalance());
    default:
        return QVariant();
    }
}

void S60CameraImageProcessingControl::setProcessingParameter(
    QCameraImageProcessingControl::ProcessingParameter parameter, QVariant value)
{
    switch (parameter) {
    case QCameraImageProcessingControl::Contrast:
        setContrast(value.toInt());
        break;
    case QCameraImageProcessingControl::Saturation:
        setSaturation(value.toInt());
        break;
    case QCameraImageProcessingControl::Brightness:
        setBrightness(value.toInt());
        break;
    case QCameraImageProcessingControl::Sharpening:
        if (isSharpeningSupported())
            setSharpeningLevel(value.toInt());
        break;
    case QCameraImageProcessingControl::Denoising:
        if (isDenoisingSupported())
            setDenoisingLevel(value.toInt());
        break;
    case QCameraImageProcessingControl::ColorTemperature:
        setManualWhiteBalance(value.toInt());
        break;
    default:
        break;
    }
}

void S60CameraImageProcessingControl::setContrast(int value)
{
    m_session->settings()->setContrast(value);
}

int S60CameraImageProcessingControl::contrast() const
{
    return m_session->settings()->contrast();
}

void S60CameraImageProcessingControl::setBrightness(int value)
{
    m_session->settings()->setBrightness(value);
}

int S60CameraImageProcessingControl::brightness() const
{
    return m_session->settings()->brightness();
}

bool S60CameraImageProcessingControl::isSaturationSupported() const
{
    return m_session->settings()->isSaturationSupported();
}

void S60CameraImageProcessingControl::setSaturation(int value)
{
    m_session->settings()->setSaturation(value);
}

int S60CameraImageProcessingControl::saturation() const
{
    return m_session->settings()->saturation();
}

bool S60CameraImageProcessingControl::isDenoisingSupported() const
{
    return m_session->settings()->isDenoisingSupported();
}

void S60CameraImageProcessingControl::setDenoisingLevel(int value)
{
    m_session->settings()->setDenoising(value);
}

int S60CameraImageProcessingControl::denoisingLevel() const
{
    return m_session->settings()->denoising();
}

bool S60CameraImageProcessingControl::isSharpeningSupported() const
{
    return m_session->settings()->isSharpeningSupported();
}

void S60CameraImageProcessingControl::setSharpeningLevel(int value)
{
    m_session->settings()->setSharpening(value);
}

int S60CameraImageProcessingControl::sharpeningLevel() const
{
    return m_session->settings()->sharpening();
}

// End of file
