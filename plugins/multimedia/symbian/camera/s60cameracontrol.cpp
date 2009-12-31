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
    : QCameraControl(parent)
{
}

S60CameraControl::S60CameraControl(QObject *session, QObject *parent)
   : QCameraControl(parent)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60CameraSession*>(session);
}

S60CameraControl::~S60CameraControl()
{
}

void S60CameraControl::start()
{
    // start the default (primary camera)
    // this is also called with enable preview.
    if (m_session->startCamera()) {
        //TODO fix error code
        emit error(-1, QLatin1String("Unable to start camera or no cameras in devices"));
    }
        
}
void S60CameraControl::stop()
{
    m_session->stopCamera();
}

QCamera::State S60CameraControl::state() const
{
    return (QCamera::State)m_session->state();
}

void S60CameraControl::setVideoOutput(QObject *output)
{
    m_session->setVideoRenderer(output);
}

