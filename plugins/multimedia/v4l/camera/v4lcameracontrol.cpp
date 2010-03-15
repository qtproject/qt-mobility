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

#include "v4lcameracontrol.h"
#include "v4lcameraservice.h"
#include "v4lcamerasession.h"


V4LCameraControl::V4LCameraControl(QObject *parent)
    :QCameraControl(parent),m_captureMode(QCamera::CaptureStillImage)
{
    m_session = qobject_cast<V4LCameraSession*>(parent);
    connect(m_session, SIGNAL(cameraStateChanged(QCamera::State)),this, SIGNAL(stateChanged(QCamera::State)));
}

V4LCameraControl::~V4LCameraControl()
{
}

void V4LCameraControl::start()
{
    m_session->previewMode(true);
    m_session->captureToFile(false);
    m_session->record();
}

void V4LCameraControl::stop()
{
    m_session->previewMode(false);
    m_session->captureToFile(false);
    m_session->stop();
}

QCamera::State V4LCameraControl::state() const
{
    if(m_session->state() == QMediaRecorder::RecordingState)
        return QCamera::ActiveState;

    return QCamera::StoppedState;
}
