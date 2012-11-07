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

#ifndef S60MEDIARECORDERCONTROL_H
#define S60MEDIARECORDERCONTROL_H

#include <QtCore/qurl.h>
#include <qmediarecorder.h>
#include <qmediarecordercontrol.h>

#include "s60videocapturesession.h"

QT_USE_NAMESPACE

QT_FORWARD_DECLARE_CLASS(S60VideoCaptureSession)
QT_FORWARD_DECLARE_CLASS(S60CameraService)
QT_FORWARD_DECLARE_CLASS(S60CameraControl)

/*
 * Control for video recording operations.
 */
class S60MediaRecorderControl : public QMediaRecorderControl
{
    Q_OBJECT

public: // Contructors & Destructor

    S60MediaRecorderControl(QObject *parent = 0);
    S60MediaRecorderControl(S60CameraService *service,
                            S60VideoCaptureSession *session,
                            QObject *parent = 0);
    ~S60MediaRecorderControl();

public: // QMediaRecorderControl

    QUrl outputLocation() const;
    bool setOutputLocation(const QUrl &sink);

    QMediaRecorder::State state() const;

    qint64 duration() const;

    bool isMuted() const;

    void applySettings();

/*
Q_SIGNALS: // QMediaRecorderControl
    void stateChanged(QMediaRecorder::State state);
    void durationChanged(qint64 position);
    void mutedChanged(bool muted);
    void error(int error, const QString &errorString);
*/

public slots: // QMediaRecorderControl

    void record();
    void pause();
    void stop();
    void setMuted(bool);

private:

    QMediaRecorder::State convertInternalStateToQtState(
        S60VideoCaptureSession::TVideoCaptureState aState) const;

private slots:

    void updateState(S60VideoCaptureSession::TVideoCaptureState state);

private: // Data

    S60VideoCaptureSession  *m_session;
    S60CameraService        *m_service;
    S60CameraControl        *m_cameraControl;
    QMediaRecorder::State   m_state;

};

#endif // S60MEDIARECORDERCONTROL_H
