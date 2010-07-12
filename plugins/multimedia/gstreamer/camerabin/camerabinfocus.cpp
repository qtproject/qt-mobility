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

#include "camerabinfocus.h"
#include "camerabinsession.h"

#include <gst/interfaces/photography.h>

#include <QDebug>

CameraBinFocus::CameraBinFocus(GstElement &camerabin, CameraBinSession *session)
    :QCameraFocusControl(session),
     m_session(session),
     m_camerabin(camerabin),
     m_focusMode(QCameraFocus::AutoFocus)
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
    g_object_get(GST_BIN(&m_camerabin), "zoom", &zoomFactor, NULL);
    return zoomFactor/100.0;
}

void CameraBinFocus::zoomTo(qreal optical, qreal digital)
{
    Q_UNUSED(optical);
    digital = qBound(qreal(1.0), digital, qreal(10.0));
    g_object_set(GST_BIN(&m_camerabin), "zoom", qRound(digital*100.0), NULL);
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

QCameraFocusZoneList CameraBinFocus::focusZones() const
{
    return QCameraFocusZoneList() << QCameraFocusZone(QRectF(0.3, 0.3, 0.4, 0.4));
}
