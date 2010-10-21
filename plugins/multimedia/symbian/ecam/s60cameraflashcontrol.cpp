/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstring.h>

#include "s60cameraflashcontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"

S60CameraFlashControl::S60CameraFlashControl(QObject *parent) :
    QCameraFlashControl(parent)
{
}

S60CameraFlashControl::S60CameraFlashControl(S60ImageCaptureSession *session, QObject *parent) :
    QCameraFlashControl(parent),
    m_session(NULL),
    m_service(NULL),
    m_advancedSettings(NULL),
    m_flashMode(QCameraExposure::FlashOff)
{
    if (session)
        m_session = session;
    else
        Q_ASSERT(true);
    // From now on it is safe to assume session exists

    connect(m_session, SIGNAL(advancedSettingChanged()), this, SLOT(resetAdvancedSetting()));
    m_advancedSettings = m_session->advancedSettings();

    if (m_advancedSettings)
        connect(m_advancedSettings, SIGNAL(flashReady(bool)), this, SIGNAL(flashReady(bool)));
}

S60CameraFlashControl::~S60CameraFlashControl()
{
    m_advancedSettings = NULL;
}

void S60CameraFlashControl::resetAdvancedSetting()
{
    m_advancedSettings = m_session->advancedSettings();
    if (m_advancedSettings)
        connect(m_advancedSettings, SIGNAL(flashReady(bool)), this, SIGNAL(flashReady(bool)));
}

QCameraExposure::FlashModes S60CameraFlashControl::flashMode() const
{
    return m_session->flashMode();
}

void S60CameraFlashControl::setFlashMode(QCameraExposure::FlashModes mode)
{
    if (isFlashModeSupported(mode)) {
        m_flashMode = mode;
        m_session->setFlashMode(m_flashMode);
    }
    else
        m_session->setError(KErrNotSupported, QString("Requested flash mode is not supported."));
}

bool S60CameraFlashControl::isFlashModeSupported(QCameraExposure::FlashModes mode) const
{
    return m_session->supportedFlashModes() & mode;
}

bool S60CameraFlashControl::isFlashReady() const
{
    if (m_advancedSettings) {
        return m_advancedSettings->isFlashReady();
    }

    return false;
}

// End of file
