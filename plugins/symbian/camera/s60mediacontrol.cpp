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
#include "s60camerasession.h"

#include <QtCore/qdebug.h>

S60MediaControl::S60MediaControl(QObject *parent)
    :QMediaRecorderControl(parent)
{
}

S60MediaControl::S60MediaControl(QObject *session, QObject *parent)
   :QMediaRecorderControl(parent)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60CameraSession*>(session);
}

S60MediaControl::~S60MediaControl()
{
}

QUrl S60MediaControl::outputLocation() const
{
    return m_session->outputLocation();
}

bool S60MediaControl::setOutputLocation(const QUrl& sink)
{
    return m_session->setOutputLocation(sink);
}

QMediaRecorder::State S60MediaControl::state() const
{
    return (QMediaRecorder::State)m_session->state();
}

qint64 S60MediaControl::duration() const
{
    return m_session->position();
}
/*
This method is called after encoder configuration is done.
Encoder can load necessary resources at this point,
to reduce delay before recording is started.
*/
void S60MediaControl::applySettings()
{
    //TODO: create impl.
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
