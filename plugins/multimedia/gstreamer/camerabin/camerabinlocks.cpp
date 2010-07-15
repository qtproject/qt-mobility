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

#include "camerabinlocks.h"
#include "camerabinsession.h"

#include <gst/interfaces/photography.h>

#include <QDebug>

CameraBinLocks::CameraBinLocks(GstElement &camerabin, CameraBinSession *session)
    :QCameraLocksControl(session),
     m_session(session),
     m_camerabin(camerabin),
     m_focusStatus(QCamera::Unlocked)
{
    connect(m_session, SIGNAL(focusStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason)),
            this, SLOT(updateFocusStatus(QCamera::LockStatus, QCamera::LockChangeReason)));
}

CameraBinLocks::~CameraBinLocks()
{
}

QCamera::LockTypes CameraBinLocks::supportedLocks() const
{
    return QCamera::LockFocus;
}

QCamera::LockStatus CameraBinLocks::lockStatus(QCamera::LockType lock) const
{
    return lock == QCamera::LockFocus ? m_focusStatus : QCamera::Unlocked;
}

void CameraBinLocks::searchAndLock(QCamera::LockTypes locks)
{
    if (locks & QCamera::LockFocus) {
        m_focusStatus = QCamera::Searching;
        gst_photography_set_autofocus(GST_PHOTOGRAPHY(&m_camerabin), TRUE);
        emit lockStatusChanged(QCamera::LockFocus, m_focusStatus, QCamera::UserRequest);
    }
}

void CameraBinLocks::unlock(QCamera::LockTypes locks)
{
    if (locks & QCamera::LockFocus) {
        m_focusStatus = QCamera::Unlocked;
        gst_photography_set_autofocus(GST_PHOTOGRAPHY(&m_camerabin), FALSE);
        emit lockStatusChanged(QCamera::LockFocus, m_focusStatus, QCamera::UserRequest);
    }
}

void CameraBinLocks::updateFocusStatus(QCamera::LockStatus status, QCamera::LockChangeReason reason)
{
    if (m_focusStatus != status)
        emit lockStatusChanged(QCamera::LockFocus, m_focusStatus = status, reason);
}
