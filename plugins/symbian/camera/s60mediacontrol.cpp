/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
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
    m_session = qobject_cast<S60CameraSession*>(parent);
}

S60MediaControl::~S60MediaControl()
{
}

QUrl S60MediaControl::sink() const
{
    return m_session->sink();
}

bool S60MediaControl::setSink(const QUrl& sink)
{
    return m_session->setSink(sink);
}

QMediaRecorder::State S60MediaControl::state() const
{
    return (QMediaRecorder::State)m_session->state();
}

qint64 S60MediaControl::duration() const
{
    return m_session->position();
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
