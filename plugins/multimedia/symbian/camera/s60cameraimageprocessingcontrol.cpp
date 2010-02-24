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

#include "s60cameraimageprocessingcontrol.h"
#include "s60cameraservice.h"
#include "s60camerasession.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>


S60CameraImageProcessingControl::S60CameraImageProcessingControl(QObject *parent)
    :QImageProcessingControl(parent)
{
    m_session = qobject_cast<S60CameraSession*>(parent);
    m_advancedSettings = m_session->advancedSettings();
}

S60CameraImageProcessingControl::S60CameraImageProcessingControl(QObject *session, QObject *parent)
   :QImageProcessingControl(parent)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60CameraSession*>(session);
}

S60CameraImageProcessingControl::~S60CameraImageProcessingControl()
{
}

QCamera::WhiteBalanceMode S60CameraImageProcessingControl::whiteBalanceMode() const
{
    return m_session->whiteBalanceMode();
}

void S60CameraImageProcessingControl::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
{
    QCamera::WhiteBalanceModes supportedModes = supportedWhiteBalanceModes();
    if (supportedModes & mode) {
        m_session->setWhiteBalanceMode(mode);
    }
}

QCamera::WhiteBalanceModes S60CameraImageProcessingControl::supportedWhiteBalanceModes() const
{
    return m_session->supportedWhiteBalanceModes();
}

int S60CameraImageProcessingControl::manualWhiteBalance() const
{
    return 0;
}

void S60CameraImageProcessingControl::setManualWhiteBalance(int colorTemperature)
{
    Q_UNUSED(colorTemperature)
}

void S60CameraImageProcessingControl::setContrast(int value)
{
    m_session->setContrast(value);
}

int S60CameraImageProcessingControl::contrast() const
{
    return m_session->contrast();
}

void S60CameraImageProcessingControl::setSaturation(int value)
{
    m_advancedSettings->setSaturation(value);
}

int S60CameraImageProcessingControl::saturation() const
{
    return m_advancedSettings->saturation();
}

void S60CameraImageProcessingControl::setDenoisingLevel(int value)
{
    Q_UNUSED(value); // not supported for S60
}

bool S60CameraImageProcessingControl::isDenoisingSupported() const
{
    return false; // not supported for S60
}

int S60CameraImageProcessingControl::denoisingLevel() const
{
    return 0; // not supported for S60
}

void S60CameraImageProcessingControl::setSharpeningLevel(int value)
{
    m_advancedSettings->setSharpeningLevel(value);
}

bool S60CameraImageProcessingControl::isSharpeningSupported() const
{
    return m_advancedSettings->isSharpeningSupported();
}

int S60CameraImageProcessingControl::sharpeningLevel() const
{
    return m_advancedSettings->sharpeningLevel();
}

