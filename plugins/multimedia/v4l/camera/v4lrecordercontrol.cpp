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

#include "v4lrecordercontrol.h"

V4LRecorderControl::V4LRecorderControl(V4LCameraSession *session)
    :QMediaRecorderControl(session), m_session(session), m_state(QMediaRecorder::StoppedState)
{
    connect(m_session, SIGNAL(recordStateChanged(QMediaRecorder::State)), SLOT(updateState(QMediaRecorder::State)));
    connect(m_session, SIGNAL(error(int,QString)), SIGNAL(error(int,QString)));
    connect(m_session, SIGNAL(durationChanged(qint64)), SIGNAL(durationChanged(qint64)));
}

V4LRecorderControl::~V4LRecorderControl()
{
}

QUrl V4LRecorderControl::outputLocation() const
{
    return m_session->outputLocation();
}

bool V4LRecorderControl::setOutputLocation(const QUrl &sink)
{
    m_session->setOutputLocation(sink);
    return true;
}


QMediaRecorder::State V4LRecorderControl::state() const
{
    return m_session->state();
}

void V4LRecorderControl::updateState(QMediaRecorder::State newState)
{
    if (m_state != newState) {
        m_state = newState;
        emit stateChanged(m_state);
    }
}

qint64 V4LRecorderControl::duration() const
{
    return m_session->position();
}

void V4LRecorderControl::record()
{
    m_session->captureToFile(true);
    m_session->record();
}

void V4LRecorderControl::pause()
{
    m_session->pause();
}

void V4LRecorderControl::stop()
{
    m_session->stop();
    m_session->captureToFile(false);
}
