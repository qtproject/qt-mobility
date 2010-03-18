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

#include "s60mediacontrol.h"
#include "s60cameracontrol.h"
#include "s60videocapturesession.h"

#include <QtCore/qdebug.h>

S60MediaControl::S60MediaControl(QObject *session, QObject *parent)
   :QMediaRecorderControl(parent), m_state(QMediaRecorder::StoppedState)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60VideoCaptureSession*>(session);

    connect(m_session,SIGNAL(stateChanged(S60VideoCaptureSession::TVideoCaptureState)),this,SLOT(updateState(S60VideoCaptureSession::TVideoCaptureState)));
    connect(m_session, SIGNAL(positionChanged(qint64)), this, SIGNAL(durationChanged(qint64)));
    connect(m_session,SIGNAL(error(int,const QString &)),this,SIGNAL(error(int,const QString &)));
}

S60MediaControl::~S60MediaControl()
{
}

QUrl S60MediaControl::outputLocation() const
{
    if (m_session)
        return m_session->outputLocation();
    return QUrl();
}

bool S60MediaControl::setOutputLocation(const QUrl& sink)
{
    if (m_session)
        return m_session->setOutputLocation(sink);
    return false;
}

QMediaRecorder::State S60MediaControl::convertState(S60VideoCaptureSession::TVideoCaptureState aState) const
{
    QMediaRecorder::State state = QMediaRecorder::StoppedState;;       
    switch (aState) {
    case S60VideoCaptureSession::ERecording:
        state = QMediaRecorder::RecordingState;            
        break;            
    case S60VideoCaptureSession::EPaused:            
        state = QMediaRecorder::PausedState;            
        break;            
    case S60VideoCaptureSession::ERecordComplete:             
    case S60VideoCaptureSession::ENotInitialized:
    case S60VideoCaptureSession::EOpenCompelete:
    case S60VideoCaptureSession::EInitialized:            
        state = QMediaRecorder::StoppedState;            
        break;            
    }            
    return state;         
}

void S60MediaControl::updateState(S60VideoCaptureSession::TVideoCaptureState aState)
{   
    QMediaRecorder::State newState = convertState(aState);
    if (m_state != newState) {
        m_state = newState;
        emit stateChanged(m_state);
    }
}

QMediaRecorder::State S60MediaControl::state() const
{   
    return m_state;
}

qint64 S60MediaControl::duration() const
{
    if (m_session)
        return m_session->position();
    return -1;
}
/*
This method is called after encoder configuration is done.
Encoder can load necessary resources at this point,
to reduce delay before recording is started.
*/
void S60MediaControl::applySettings()
{
}

void S60MediaControl::record()
{     
    m_session->startRecording();    
}

void S60MediaControl::pause()
{     
    m_session->pauseRecording();    
}

void S60MediaControl::stop()
{
    m_session->stopRecording();     
}
