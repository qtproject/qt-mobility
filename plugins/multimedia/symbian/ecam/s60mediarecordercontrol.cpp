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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60cameraservice.h"
#include "s60mediarecordercontrol.h"
#include "s60cameracontrol.h"
#include "s60videocapturesession.h"

S60MediaRecorderControl::S60MediaRecorderControl(S60VideoCaptureSession *session, QObject *parent) :
    QMediaRecorderControl(parent),
    m_state(QMediaRecorder::StoppedState) // Default state
{
    m_session = session;

    if (qstrcmp(parent->metaObject()->className(), "S60CameraService") == 0) {
        m_service = qobject_cast<S60CameraService*>(parent);
    }

    if (m_service)
        m_cameraControl = qobject_cast<S60CameraControl *>(m_service->requestControl(QCameraControl_iid));

    connect(m_session, SIGNAL(stateChanged(S60VideoCaptureSession::TVideoCaptureState)),
        this, SLOT(updateState(S60VideoCaptureSession::TVideoCaptureState)));
    connect(m_session, SIGNAL(positionChanged(qint64)), this, SIGNAL(durationChanged(qint64)));
    connect(m_session, SIGNAL(error(int,const QString &)), this, SIGNAL(error(int,const QString &)));
}

S60MediaRecorderControl::~S60MediaRecorderControl()
{
}

QUrl S60MediaRecorderControl::outputLocation() const
{
    return m_session->outputLocation();
}

bool S60MediaRecorderControl::setOutputLocation(const QUrl& sink)
{
    // Output location can only be set in StoppedState
    if (m_state == QMediaRecorder::StoppedState)
        return m_session->setOutputLocation(sink);

    return false;
}

QMediaRecorder::State S60MediaRecorderControl::convertMMFStateToQtState(S60VideoCaptureSession::TVideoCaptureState aState) const
{
    QMediaRecorder::State state;

    switch (aState) {
        case S60VideoCaptureSession::ERecording:
            state = QMediaRecorder::RecordingState;
            break;
        case S60VideoCaptureSession::EPaused:
            state = QMediaRecorder::PausedState;
            break;

        default:
            // All others
            state = QMediaRecorder::StoppedState;
            break;
    }
    return state;
}

void S60MediaRecorderControl::updateState(S60VideoCaptureSession::TVideoCaptureState state)
{
    QMediaRecorder::State newState = convertMMFStateToQtState(state);

    if (m_state != newState) {
        m_state = newState;
        emit stateChanged(m_state);
    }
}

QMediaRecorder::State S60MediaRecorderControl::state() const
{
    return m_state;
}

qint64 S60MediaRecorderControl::duration() const
{
    return m_session->position();
}

/*
This method is called after encoder configuration is done.
Encoder can load necessary resources at this point,
to reduce delay before recording is started.
*/
void S60MediaRecorderControl::applySettings()
{
    m_session->applyAllSettings();
}

void S60MediaRecorderControl::record()
{
    if (m_state == QMediaRecorder::RecordingState) {
        return;
    }

    if (m_cameraControl && m_cameraControl->captureMode() != QCamera::CaptureVideo) {
        emit error(QCamera::CameraError, tr("Camera is not started for video capture."));
        return;
    }

    m_session->startRecording();
}

void S60MediaRecorderControl::pause()
{
    if (m_state != QMediaRecorder::RecordingState) {
        return;
    }

    m_session->pauseRecording();
}

void S60MediaRecorderControl::stop()
{
    if (m_state == QMediaRecorder::StoppedState) {
        // Ignore stop
        return;
    }

    m_session->stopRecording();
}

bool S60MediaRecorderControl::isMuted() const
{
    return m_session->isMuted();
}

void S60MediaRecorderControl::setMuted(bool muted)
{
        m_session->setMuted(muted);
}

// End of file
