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

#ifndef S60CAMERAIMAGECAPTURECONTROL_H
#define S60CAMERAIMAGECAPTURECONTROL_H

#include "qcameraimagecapturecontrol.h"

QT_USE_NAMESPACE

QT_FORWARD_DECLARE_CLASS(S60CameraService)
QT_FORWARD_DECLARE_CLASS(S60ImageCaptureSession)
QT_FORWARD_DECLARE_CLASS(S60CameraControl)

/*
 * Control for image capture operations.
 */
class S60CameraImageCaptureControl : public QCameraImageCaptureControl
{
    Q_OBJECT

public: // Contructors & Destrcutor

    S60CameraImageCaptureControl(QObject *parent = 0);
    S60CameraImageCaptureControl(S60CameraService *service,
                                 S60ImageCaptureSession *session,
                                 QObject *parent = 0);
    ~S60CameraImageCaptureControl();

public: // QCameraImageCaptureControl

    bool isReadyForCapture() const;

    // Drive Mode
    QCameraImageCapture::DriveMode driveMode() const;
    void setDriveMode(QCameraImageCapture::DriveMode mode);

    // Capture
    int capture(const QString &fileName);
    void cancelCapture();

/*
Q_SIGNALS: // QCameraImageCaptureControl
    void readyForCaptureChanged(bool);

    void imageExposed(int id);
    void imageCaptured(int id, const QImage &preview);
    void imageAvailable(int id, const QVideoFrame &image);
    void imageSaved(int id, const QString &fileName);

    void error(int id, int error, const QString &errorString);
*/

private: // Data

    S60ImageCaptureSession          *m_session;
    S60CameraService                *m_service;
    S60CameraControl                *m_cameraControl;
    QCameraImageCapture::DriveMode  m_driveMode;
};

#endif // S60CAMERAIMAGECAPTURECONTROL_H
