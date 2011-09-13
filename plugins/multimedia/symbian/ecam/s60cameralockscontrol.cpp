/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstring.h>
#include <qcamerafocus.h>   // FocusMode

#include "s60cameralockscontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"
#include "s60camerafocuscontrol.h"

S60CameraLocksControl::S60CameraLocksControl(QObject *parent) :
    QCameraLocksControl(parent)
{
}

S60CameraLocksControl::S60CameraLocksControl(S60CameraService *service,
                                             S60ImageCaptureSession *session,
                                             QObject *parent) :
    QCameraLocksControl(parent),
    m_session(0),
    m_service(0),
    m_focusControl(0),
    m_focusStatus(QCamera::Unlocked),
    m_exposureStatus(QCamera::Unlocked),
    m_whiteBalanceStatus(QCamera::Unlocked)
{
    m_session = session;
    m_service = service;
    m_focusControl = qobject_cast<S60CameraFocusControl *>(m_service->requestControl(QCameraFocusControl_iid));

    connect(m_session, SIGNAL(focusStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason)),
        this, SLOT(focusStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason)));
}

S60CameraLocksControl::~S60CameraLocksControl()
{
}

QCamera::LockTypes S60CameraLocksControl::supportedLocks() const
{
    QCamera::LockTypes supportedLocks = 0;

#ifdef S60_CAM_AUTOFOCUS_SUPPORT // S60 3.1
    if (m_session)
        if (m_session->isFocusSupported())
            supportedLocks |= QCamera::LockFocus;
#else // S60 3.2 and later
    if (m_session->isFocusModeSupported(QCameraFocus::AutoFocus) ||
        m_session->isFocusModeSupported(QCameraFocus::ContinuousFocus) ||
        m_session->isFocusModeSupported(QCameraFocus::MacroFocus))
        supportedLocks |= QCamera::LockFocus;
#endif // S60_CAM_AUTOFOCUS_SUPPORT

    // Exposure/WhiteBalance Locking not implemented in Symbian
    return supportedLocks;
}

QCamera::LockStatus S60CameraLocksControl::lockStatus(QCamera::LockType lock) const
{
    switch (lock) {
    case QCamera::LockExposure:
        return m_exposureStatus;
    case QCamera::LockWhiteBalance:
        return m_whiteBalanceStatus;
    case QCamera::LockFocus:
        return m_focusStatus;
    default:
        // Unsupported lock
        return QCamera::Unlocked;
    }
}

void S60CameraLocksControl::searchAndLock(QCamera::LockTypes locks)
{
    // Ignore Exposure and WhiteBalance locks
    if (locks & QCamera::LockFocus)
        startFocusing();
}

void S60CameraLocksControl::unlock(QCamera::LockTypes locks)
{
    if (locks & QCamera::LockFocus)
        cancelFocusing();
}

void S60CameraLocksControl::focusStatusChanged(QCamera::LockStatus status,
                                               QCamera::LockChangeReason reason)
{
    if(status != m_focusStatus) {
        m_focusStatus = status;
        emit lockStatusChanged(QCamera::LockFocus, status, reason);
    }
}

void S60CameraLocksControl::startFocusing()
{
#ifndef S60_CAM_AUTOFOCUS_SUPPORT // S60 3.2 or later
    // Focusing is triggered on Symbian by setting the FocusType corresponding
    // to the FocusMode set to FocusControl
    m_session->startFocusing();
#else // S60 3.1
    if (m_focusControl && m_focusControl->focusMode() == QCameraFocus::AutoFocus) {
        m_session->startFocus();
        m_focusStatus = QCamera::Searching;
        emit lockStatusChanged(QCamera::LockFocus, QCamera::Searching, QCamera::UserRequest);
    } else {
        emit lockStatusChanged(QCamera::LockFocus, QCamera::Unlocked, QCamera::LockFailed);
    }
#endif // S60_CAM_AUTOFOCUS_SUPPORT
}

void S60CameraLocksControl::cancelFocusing()
{
    if (m_focusStatus == QCamera::Unlocked)
        return;

#ifndef S60_CAM_AUTOFOCUS_SUPPORT // S60 3.2 or later
    m_session->cancelFocusing();
#else // S60 3.1
    m_session->cancelFocus();
    m_focusStatus = QCamera::Unlocked;
    emit lockStatusChanged(QCamera::LockFocus, QCamera::Unlocked, QCamera::UserRequest);
#endif // S60_CAM_AUTOFOCUS_SUPPORT
}

// End of file
