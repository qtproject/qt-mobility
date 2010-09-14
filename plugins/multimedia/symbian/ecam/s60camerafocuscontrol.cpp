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

#include <QtCore/qstring.h>

#include "s60camerafocuscontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"

S60CameraFocusControl::S60CameraFocusControl(QObject *parent) :
    QCameraFocusControl(parent)
{
    m_session = qobject_cast<S60ImageCaptureSession*> (parent);
}

S60CameraFocusControl::S60CameraFocusControl(S60ImageCaptureSession *session, QObject *parent) :
    QCameraFocusControl(parent),
    m_session(NULL),
    m_service(NULL),
    m_advancedSettings(NULL),
    m_focusLocked(false),
    m_opticalZoomValue(1.0),
    m_digitalZoomValue(1.0),
    m_focusMode(QCameraFocus::AutoFocus),
    m_maxZoom(1.0),
    m_maxDigitalZoom(1.0)
{
    m_session = session;

    connect(m_session, SIGNAL(advancedSettingCreated()), this, SLOT(resetAdvancedSetting()));
    m_advancedSettings = m_session->advancedSettings();

    connect(m_session, SIGNAL(opticalZoomChanged(qreal)), this, SIGNAL(opticalZoomChanged(qreal)));
    connect(m_session, SIGNAL(digitalZoomChanged(qreal)), this, SIGNAL(digitalZoomChanged(qreal)));

    m_session->setZoomFactor(m_opticalZoomValue, m_digitalZoomValue);
}

S60CameraFocusControl::~S60CameraFocusControl()
{
}

QCameraFocus::FocusMode S60CameraFocusControl::focusMode() const
{
    return m_focusMode;
}

void S60CameraFocusControl::setFocusMode(QCameraFocus::FocusMode mode)
{
    if (isFocusModeSupported(mode))
        m_focusMode = mode;
    else {
        m_session->setError(KErrNotSupported);
    }
}

bool S60CameraFocusControl::isFocusModeSupported(QCameraFocus::FocusMode mode) const
{
    if (m_advancedSettings) {
        return m_advancedSettings->supportedFocusModes() & mode;
    }

    return false;
}

qreal S60CameraFocusControl::maximumOpticalZoom() const
{
    return m_session->maximumZoom();
}

qreal S60CameraFocusControl::maximumDigitalZoom() const
{
    return m_session->maxDigitalZoom();
}

qreal S60CameraFocusControl::opticalZoom() const
{
    return m_session->zoomFactor();
}

qreal S60CameraFocusControl::digitalZoom() const
{
    return m_session->digitalZoomFactor();
}

void S60CameraFocusControl::zoomTo(qreal optical, qreal digital)
{
    m_session->setZoomFactor(optical, digital);
    m_opticalZoomValue = optical;
    m_digitalZoomValue = digital;
}

void S60CameraFocusControl::resetAdvancedSetting()
{
    m_advancedSettings = m_session->advancedSettings();
}

QCameraFocus::FocusPointMode S60CameraFocusControl::focusPointMode() const
{
    return QCameraFocus::FocusPointAuto;
}

void S60CameraFocusControl::setFocusPointMode(QCameraFocus::FocusPointMode mode)
{
    Q_UNUSED(mode);
}

bool S60CameraFocusControl::isFocusPointModeSupported(QCameraFocus::FocusPointMode mode) const
{
    return false;
}

QPointF S60CameraFocusControl::customFocusPoint() const
{
    // Return image center
    return QPointF(0.5, 0.5);
}

void S60CameraFocusControl::setCustomFocusPoint(const QPointF &point)
{
    Q_UNUSED(point);
}

QCameraFocusZoneList S60CameraFocusControl::focusZones() const
{
    return QCameraFocusZoneList();
}

// End of file

