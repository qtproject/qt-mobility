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
   , m_focusLocked(false)
   , m_zoomValue(1.0)
   , m_maxZoom(1.0)
   , m_maxDigitalZoom(1.0)
   , m_macroFocusingEnabled(false)
   , m_focusMode(QCamera::AutoFocus)
   , m_focusStatus(QCamera::FocusInitial)
   , m_error(QCamera::NoError)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60CameraSession*>(session);
    connect(m_session, SIGNAL(focusLocked()), this, SIGNAL(focusLocked()));
    connect(m_session, SIGNAL(zoomValueChanged(qreal)), this, SIGNAL(zoomValueChanged(qreal)));
}

S60CameraFocusControl::~S60CameraFocusControl()
{
}

QCamera::FocusMode S60CameraFocusControl::focusMode() const
{
    return m_session->focusMode();
}

void S60CameraFocusControl::setFocusMode(QCamera::FocusMode mode)
{
    m_session->setFocusMode(mode);
	m_focusMode = mode;
}

QCamera::FocusModes S60CameraFocusControl::supportedFocusModes() const
{
    return m_session->supportedFocusModes();
}

QCamera::FocusStatus S60CameraFocusControl::focusStatus() const
{
    return m_focusStatus;
}

bool S60CameraFocusControl::macroFocusingEnabled() const
{
    return m_macroFocusingEnabled;
}

bool S60CameraFocusControl::isMacroFocusingSupported() const
{
    // not supported
   // m_error = QCamera::NotSupportedFeatureError;
    return false;
}

void S60CameraFocusControl::setMacroFocusingEnabled(bool /*e*/)
{
    // not supported
    m_error = QCamera::NotSupportedFeatureError;
}
qreal S60CameraFocusControl::maximumOpticalZoom() const
{
    if (m_session->maximumZoom() == 0) // optical zoom not supported!
        return 1.0;
    else
        return m_session->maximumZoom();
}

qreal S60CameraFocusControl::maximumDigitalZoom() const
{
	return m_session->maxDigitalZoom();
}

qreal S60CameraFocusControl::zoomValue() const
{	
	return m_session->zoomFactor();
}

void S60CameraFocusControl::zoomTo(qreal value)
{	
    qDebug() << "S60CameraFocusControl::zoomTo value: " << value;
	m_session->setZoomFactor(value);
	m_zoomValue = value;
}

void S60CameraFocusControl::startFocusing()
{
    m_session->startFocus();
}

void S60CameraFocusControl::cancelFocusing()
{
    m_session->cancelFocus();
}

