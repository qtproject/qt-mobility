/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "s60audiomediarecordercontrol.h"
#include "s60audiocapturesession.h"

#include <QtCore/qdebug.h>

S60AudioMediaRecorderControl::S60AudioMediaRecorderControl(QObject *session, QObject *parent)
    :QMediaRecorderControl(parent), m_state(QMediaRecorder::StoppedState)
{
    m_session = qobject_cast<S60AudioCaptureSession*>(session);
    connect(m_session, SIGNAL(positionChanged(qint64)), this, SIGNAL(durationChanged(qint64)));
    connect(m_session, SIGNAL(stateChanged(S60AudioCaptureSession::TAudioCaptureState)), this, SLOT(updateState(S60AudioCaptureSession::TAudioCaptureState)));
    connect(m_session,SIGNAL(error(int,const QString &)),this,SIGNAL(error(int,const QString &)));
}

S60AudioMediaRecorderControl::~S60AudioMediaRecorderControl()
{
}

QUrl S60AudioMediaRecorderControl::outputLocation() const
{
    return m_session->outputLocation();
}

bool S60AudioMediaRecorderControl::setOutputLocation(const QUrl& sink)
{
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
    m_session->record();
}

void S60AudioMediaRecorderControl::pause()
{
    m_session->pause();
}

void S60AudioMediaRecorderControl::stop()
{
    m_session->stop();
}

bool S60AudioMediaRecorderControl::isMuted() const
{
    return false;
}

void S60AudioMediaRecorderControl::setMuted(bool)
{
}
