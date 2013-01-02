/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstring.h>

#include "s60camerafocuscontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"
#include "s60cameraconstants.h"
#include "s60imagecapturesettings.h"

using namespace S60CameraConstants;

S60CameraFocusControl::S60CameraFocusControl(QObject *parent) :
    QCameraFocusControl(parent)
{
}

S60CameraFocusControl::S60CameraFocusControl(S60ImageCaptureSession *session, QObject *parent) :
    QCameraFocusControl(parent),
    m_session(0),
    m_service(0),
    m_opticalZoomValue(KDefaultOpticalZoom),
    m_digitalZoomValue(KDefaultDigitalZoom)
{
    m_session = session;
}

S60CameraFocusControl::~S60CameraFocusControl()
{
}

QCameraFocus::FocusMode S60CameraFocusControl::focusMode() const
{
    return m_session->settings()->focusMode();
}

void S60CameraFocusControl::setFocusMode(QCameraFocus::FocusMode mode)
{
    if (isFocusModeSupported(mode))
        // FocusMode and FocusRange will be set. Focusing is triggered by setting
        // the corresponding FocusType active by calling searchAndLock in LocksControl
        m_session->settings()->setFocusMode(mode);
    else
        m_session->setError(KErrNotSupported, tr("Requested focus mode is not supported."));
}

bool S60CameraFocusControl::isFocusModeSupported(QCameraFocus::FocusMode mode) const
{
#ifndef S60_CAM_AUTOFOCUS_SUPPORT
    return m_session->settings()->isFocusModeSupported(mode);
#else // S60_CAM_AUTOFOCUS_SUPPORT
    if (mode == QCameraFocus::AutoFocus)
        return m_session->settings()->isFocusSupported();
    else
        return false;
#endif // S60_CAM_AUTOFOCUS_SUPPORT
}

qreal S60CameraFocusControl::maximumOpticalZoom() const
{
    return m_session->settings()->maximumZoom();
}

qreal S60CameraFocusControl::maximumDigitalZoom() const
{
    return m_session->settings()->maxDigitalZoom();
}

qreal S60CameraFocusControl::opticalZoom() const
{
    return m_session->settings()->opticalZoomFactor();
}

qreal S60CameraFocusControl::digitalZoom() const
{
    return m_session->settings()->digitalZoomFactor();
}

void S60CameraFocusControl::zoomTo(qreal optical, qreal digital)
{
    TRAPD(err, m_session->settings()->doSetZoomFactorL(optical, digital));
    if (err)
        m_session->setError(KErrNotSupported, tr("Requested zoom factor is not supported."));

    // Query new values
    if (m_opticalZoomValue != m_session->settings()->opticalZoomFactor()) {
        m_opticalZoomValue = m_session->settings()->opticalZoomFactor();
        emit opticalZoomChanged(m_opticalZoomValue);
    }
    if (m_digitalZoomValue != m_session->settings()->digitalZoomFactor()) {
        m_digitalZoomValue = m_session->settings()->digitalZoomFactor();
        emit digitalZoomChanged(m_digitalZoomValue);
    }
}

QCameraFocus::FocusPointMode S60CameraFocusControl::focusPointMode() const
{
    // Not supported in Symbian
    return QCameraFocus::FocusPointAuto;
}

void S60CameraFocusControl::setFocusPointMode(QCameraFocus::FocusPointMode mode)
{
    if (mode != QCameraFocus::FocusPointAuto)
        m_session->setError(KErrNotSupported, tr("Requested focus point mode is not supported."));
}

bool S60CameraFocusControl::isFocusPointModeSupported(QCameraFocus::FocusPointMode mode) const
{
    // Not supported in Symbian
    if (mode == QCameraFocus::FocusPointAuto)
        return true;
    else
        return false;
}

QPointF S60CameraFocusControl::customFocusPoint() const
{
    // Not supported in Symbian, return image center
    return QPointF(0.5, 0.5);
}

void S60CameraFocusControl::setCustomFocusPoint(const QPointF &point)
{
    // Not supported in Symbian
    Q_UNUSED(point);
    m_session->setError(KErrNotSupported, tr("Setting custom focus point is not supported."));
}

QCameraFocusZoneList S60CameraFocusControl::focusZones() const
{
    // Not supported in Symbian
    return QCameraFocusZoneList();
}

// End of file

