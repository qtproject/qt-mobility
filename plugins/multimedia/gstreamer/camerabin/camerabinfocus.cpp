/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "camerabinfocus.h"
#include "camerabinsession.h"

#include <gst/interfaces/photography.h>

#include <QDebug>

CameraBinFocus::CameraBinFocus(CameraBinSession *session)
    :QCameraFocusControl(session),
     m_session(session),
     m_focusMode(QCameraFocus::AutoFocus),
     m_focusZoneStatus(QCameraFocusZone::Selected)
{
}

CameraBinFocus::~CameraBinFocus()
{
}

QCameraFocus::FocusMode CameraBinFocus::focusMode() const
{
    return m_focusMode;
}

void CameraBinFocus::setFocusMode(QCameraFocus::FocusMode mode)
{
    if (isFocusModeSupported(mode)) {
        m_focusMode = mode;
    }
}

bool CameraBinFocus::isFocusModeSupported(QCameraFocus::FocusMode mode) const
{
    return mode & QCameraFocus::AutoFocus;
}

qreal CameraBinFocus::maximumOpticalZoom() const
{
    return 1.0;
}

qreal CameraBinFocus::maximumDigitalZoom() const
{
    return 10;
}

qreal CameraBinFocus::opticalZoom() const
{
    return 1.0;
}

qreal CameraBinFocus::digitalZoom() const
{
    gint zoomFactor = 0;
    g_object_get(GST_BIN(m_session->cameraBin()), "zoom", &zoomFactor, NULL);
    return zoomFactor/100.0;
}

void CameraBinFocus::zoomTo(qreal optical, qreal digital)
{
    Q_UNUSED(optical);
    digital = qBound(qreal(1.0), digital, qreal(10.0));
    g_object_set(GST_BIN(m_session->cameraBin()), "zoom", qRound(digital*100.0), NULL);
    emit digitalZoomChanged(digital);
}

QCameraFocus::FocusPointMode CameraBinFocus::focusPointMode() const
{
    return QCameraFocus::FocusPointAuto;
}

void CameraBinFocus::setFocusPointMode(QCameraFocus::FocusPointMode mode)
{
    Q_UNUSED(mode);
}

bool CameraBinFocus::isFocusPointModeSupported(QCameraFocus::FocusPointMode mode) const
{
    return mode == QCameraFocus::FocusPointAuto;
}

QPointF CameraBinFocus::customFocusPoint() const
{
    return QPointF(0.5, 0.5);
}

void CameraBinFocus::setCustomFocusPoint(const QPointF &point)
{
    Q_UNUSED(point);
}

void CameraBinFocus::_q_updateLockStatus(QCamera::LockType lockType,
                                         QCamera::LockStatus lockStatus,
                                         QCamera::LockChangeReason reason)
{
    if (lockType == QCamera::LockFocus) {
        QCameraFocusZone::FocusZoneStatus status =
                lockStatus == QCamera::Locked ?
                    QCameraFocusZone::Focused : QCameraFocusZone::Selected;

        if (m_focusZoneStatus != status) {
            m_focusZoneStatus = status;
            emit focusZonesChanged();
        }
    }
}

QCameraFocusZoneList CameraBinFocus::focusZones() const
{
    return QCameraFocusZoneList() << QCameraFocusZone(QRectF(0.35, 0.35, 0.3, 0.3), m_focusZoneStatus);
}
