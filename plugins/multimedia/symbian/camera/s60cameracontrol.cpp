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

#include "s60cameracontrol.h"
#include "s60cameraservice.h"
#include "s60camerasession.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>


S60CameraControl::S60CameraControl(QObject *parent)
    : QCameraControl(parent), m_captureMode(QCamera::CaptureStillImage)
{
}

S60CameraControl::S60CameraControl(QObject *session, QObject *parent)
   : QCameraControl(parent), m_captureMode(QCamera::CaptureStillImage)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60CameraSession*>(session);
    // connect signals to session
    connect (m_session,SIGNAL(stateChanged(QCamera::State)),this,SIGNAL(stateChanged(QCamera::State)));
    connect (m_session,SIGNAL(error(int,const QString &)),this,SIGNAL(error(int,const QString &)));
}

S60CameraControl::~S60CameraControl()
{
}

void S60CameraControl::start()
{
    if (m_session) {
        m_session->startCamera();
    }
    else 
        qDebug() << "S60CameraControl::start() - NO SESSION!";
        
}
void S60CameraControl::stop()
{
    if (m_session)
        m_session->stopCamera();
}

QCamera::State S60CameraControl::state() const
{
    if (m_session) {
       return (QCamera::State)m_session->state();
    }
    // we have no session, thus no camera is active.
    return QCamera::StoppedState;
}
QCamera::CaptureMode S60CameraControl::captureMode() const
{
    return m_captureMode;
}
void S60CameraControl::setCaptureMode(QCamera::CaptureMode mode)
{
    //TODO: set capture mode for session
    if (m_captureMode != mode) {
        m_captureMode = mode;
        emit captureModeChanged(mode);
    }
    
}
QCamera::CaptureModes S60CameraControl::supportedCaptureModes() const
{
    return QCamera::CaptureStillImage | QCamera::CaptureVideo;
}


void S60CameraControl::setVideoOutput(QObject *output)
{
    if (m_session)
        m_session->setVideoRenderer(output);
}

