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

#ifndef S60CAMERALOCKSCONTROL_H
#define S60CAMERALOCKSCONTROL_H

#include <QtCore/qobject.h>
#include "qcameralockscontrol.h"

QT_USE_NAMESPACE

QT_FORWARD_DECLARE_CLASS(S60CameraService)
QT_FORWARD_DECLARE_CLASS(S60ImageCaptureSession)
QT_FORWARD_DECLARE_CLASS(S60CameraFocusControl)

/*
 * Control for searching and locking 3A algorithms (AutoFocus, AutoExposure
 * and AutoWhitebalance).
 */
class S60CameraLocksControl : public QCameraLocksControl
{
    Q_OBJECT

public: // Contructors & Destrcutor

    S60CameraLocksControl(QObject *parent = 0);
    S60CameraLocksControl(S60CameraService *service,
                          S60ImageCaptureSession *session,
                          QObject *parent = 0);
    ~S60CameraLocksControl();

public: // QCameraLocksControl

    QCamera::LockTypes supportedLocks() const;

    QCamera::LockStatus lockStatus(QCamera::LockType lock) const;

    void searchAndLock(QCamera::LockTypes locks);
    void unlock(QCamera::LockTypes locks);

/*
Q_SIGNALS: // QCameraLocksControl

    void lockStatusChanged(QCamera::LockType type,
                           QCamera::LockStatus status,
                           QCamera::LockChangeReason reason);
*/

private slots: // Internal Slots

    void focusStatusChanged(QCamera::LockStatus status, QCamera::LockChangeReason reason);

private: // Internal

    // Focus
    void startFocusing();
    void cancelFocusing();

private: // Data

    S60ImageCaptureSession  *m_session;
    S60CameraService        *m_service;
    S60CameraFocusControl   *m_focusControl;
    QCamera::LockStatus     m_focusStatus;
    QCamera::LockStatus     m_exposureStatus;
    QCamera::LockStatus     m_whiteBalanceStatus;
};

#endif // S60CAMERALOCKSCONTROL_H
