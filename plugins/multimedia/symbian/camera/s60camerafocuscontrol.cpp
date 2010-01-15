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

#include "s60camerafocuscontrol.h"
#include "s60cameraservice.h"
#include "s60camerasession.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>


S60CameraFocusControl::S60CameraFocusControl(QObject *parent)
    :QCameraFocusControl(parent)
{
    m_session = qobject_cast<S60CameraSession*>(parent);
}

S60CameraFocusControl::S60CameraFocusControl(QObject *session, QObject *parent)
   :QCameraFocusControl(parent)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60CameraSession*>(session);
    connect(m_session, SIGNAL(focusLocked()), this, SIGNAL(focusLocked()));
}


S60CameraFocusControl::~S60CameraFocusControl()
{
}

QCamera::FocusMode S60CameraFocusControl::focusMode() const
{
    return QCamera::AutoFocus;
}

void S60CameraFocusControl::setFocusMode(QCamera::FocusMode mode)
{
    Q_UNUSED(mode)
}

QCamera::FocusModes S60CameraFocusControl::supportedFocusModes() const
{
    return QCamera::AutoFocus;
}

QCamera::FocusStatus S60CameraFocusControl::focusStatus() const
{
    //TODO: return correct focus status
    return QCamera::FocusInitial;
}

bool S60CameraFocusControl::macroFocusingEnabled() const
{
    return false;
}

bool S60CameraFocusControl::isMacroFocusingSupported() const
{
    return false;
}

void S60CameraFocusControl::setMacroFocusingEnabled(bool /*e*/)
{

}
qreal S60CameraFocusControl::maximumOpticalZoom() const
{
    return 1.0;
}

qreal S60CameraFocusControl::maximumDigitalZoom() const
{
    return 1.0;
}

qreal S60CameraFocusControl::zoomValue() const
{
    return 1.0;
}

void S60CameraFocusControl::zoomTo(qreal value)
{
    Q_UNUSED(value);
}

void S60CameraFocusControl::startFocusing()
{
}

void S60CameraFocusControl::cancelFocusing()
{
}
