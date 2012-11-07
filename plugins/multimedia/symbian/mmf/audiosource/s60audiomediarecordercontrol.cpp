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

#include "s60audiomediarecordercontrol.h"
#include "s60audiocapturesession.h"
#include "s60mmtrace.h"

S60AudioMediaRecorderControl::S60AudioMediaRecorderControl(QObject *session, QObject *parent)
    : QMediaRecorderControl(parent), m_state(QMediaRecorder::StoppedState)
{
    TRACE("S60AudioMediaRecorderControl::S60AudioMediaRecorderControl" << qtThisPtr());
    m_session = qobject_cast<S60AudioCaptureSession*>(session);
    connect(m_session, SIGNAL(positionChanged(qint64)),
            this, SIGNAL(durationChanged(qint64)));
    connect(m_session, SIGNAL(stateChanged(S60AudioCaptureSession::TAudioCaptureState)),
            this, SLOT(updateState(S60AudioCaptureSession::TAudioCaptureState)));
    connect(m_session,SIGNAL(error(int, const QString &)),
            this, SIGNAL(error(int, const QString &)));
}

S60AudioMediaRecorderControl::~S60AudioMediaRecorderControl()
{
    TRACE("S60AudioMediaRecorderControl::~S60AudioMediaRecorderControl" << qtThisPtr());
}

QUrl S60AudioMediaRecorderControl::outputLocation() const
{
    return m_session->outputLocation();
}

bool S60AudioMediaRecorderControl::setOutputLocation(const QUrl &sink)
{
    TRACE("S60AudioMediaRecorderControl::setOutputLocation" << qtThisPtr()
          << "sink" << sink);
    return m_session->setOutputLocation(sink);
}

QMediaRecorder::State S60AudioMediaRecorderControl::convertState(S60AudioCaptureSession::TAudioCaptureState aState) const
{
    QMediaRecorder::State state = QMediaRecorder::StoppedState;;
    switch (aState) {
    case S60AudioCaptureSession::ERecording:
        state = QMediaRecorder::RecordingState;
        break;
    case S60AudioCaptureSession::EPaused:
        state = QMediaRecorder::PausedState;
        break;
    case S60AudioCaptureSession::ERecordComplete:
    case S60AudioCaptureSession::ENotInitialized:
    case S60AudioCaptureSession::EOpenCompelete:
    case S60AudioCaptureSession::EInitialized:
        state = QMediaRecorder::StoppedState;
        break;
    }
    return state;
}

void S60AudioMediaRecorderControl::updateState(S60AudioCaptureSession::TAudioCaptureState aState)
{
    QMediaRecorder::State newState = convertState(aState);
    if (m_state != newState) {
        m_state = newState;
        TRACE("S60AudioMediaRecorderControl::stateChanged" << qtThisPtr()
              << "state" << m_state);
        emit stateChanged(m_state);
    }
}

QMediaRecorder::State S60AudioMediaRecorderControl::state() const
{
    return m_state;
}

qint64 S60AudioMediaRecorderControl::duration() const
{
    return m_session->position();
}

void S60AudioMediaRecorderControl::record()
{
    TRACE("S60AudioMediaRecorderControl::record" << qtThisPtr());
    m_session->record();
}

void S60AudioMediaRecorderControl::pause()
{
    TRACE("S60AudioMediaRecorderControl::pause" << qtThisPtr());
    m_session->pause();
}

void S60AudioMediaRecorderControl::stop()
{
    TRACE("S60AudioMediaRecorderControl::stop" << qtThisPtr());
    m_session->stop();
}

bool S60AudioMediaRecorderControl::isMuted() const
{
    return m_session->muted();
}

void S60AudioMediaRecorderControl::applySettings()
{

}

void S60AudioMediaRecorderControl::setMuted(bool muted)
{
    if (muted != isMuted()) {
        TRACE("S60AudioMediaRecorderControl::setMuted" << qtThisPtr() << "muted" << muted);
        m_session->mute(muted);
    }
}
