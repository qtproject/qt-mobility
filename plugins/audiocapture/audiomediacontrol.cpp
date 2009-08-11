/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "audiocapturesession.h"
#include "audiomediacontrol.h"

#include <QtCore/qdebug.h>

AudioMediaControl::AudioMediaControl(QObject *parent)
    :QMediaRecorderControl(parent)
{
    m_session = qobject_cast<AudioCaptureSession*>(parent);
    connect(m_session,SIGNAL(positionChanged(qint64)),this,SIGNAL(positionChanged(qint64)));
    connect(m_session,SIGNAL(stateChanged(QMediaRecorder::State)),this,SIGNAL(stateChanged(QMediaRecorder::State)));
}

AudioMediaControl::~AudioMediaControl()
{
}

QMediaSink AudioMediaControl::sink() const
{
    return m_session->sink();
}

bool AudioMediaControl::setSink(const QMediaSink& sink)
{
    return m_session->setSink(sink);
}

int AudioMediaControl::state() const
{
    return m_session->state();
}

qint64 AudioMediaControl::position() const
{
    return m_session->position();
}

void AudioMediaControl::record()
{
    m_session->record();
}

void AudioMediaControl::pause()
{
    m_session->stop();
}

void AudioMediaControl::stop()
{
    m_session->stop();
}
