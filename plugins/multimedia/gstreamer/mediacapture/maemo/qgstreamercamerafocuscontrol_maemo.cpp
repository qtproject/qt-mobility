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

#include "qgstreamercamerafocuscontrol_maemo.h"
#include "qgstreamercapturesession_maemo.h"

#include <gst/interfaces/photography.h>

#include <QDebug>

QGstreamerCameraFocusControl::QGstreamerCameraFocusControl(GstElement &camerabin, QGstreamerCaptureSession *session)
    :QCameraFocusControl(session),
     m_session(session),
     m_camerabin(camerabin),
     m_focusStatus(QCamera::FocusInitial),
     m_focusMode(QCamera::ManualFocus)
{
    connect(m_session, SIGNAL(focusStatusChanged(QCamera::FocusStatus)),
            this, SIGNAL(focusStatusChanged(QCamera::FocusStatus)));
}

QGstreamerCameraFocusControl::~QGstreamerCameraFocusControl()
{
}

QCamera::FocusMode QGstreamerCameraFocusControl::focusMode() const
{
    return m_focusMode;
}

void QGstreamerCameraFocusControl::setFocusMode(QCamera::FocusMode mode)
{
    if (supportedFocusModes() & mode) {
        m_focusMode = mode;
    }
}

QCamera::FocusModes QGstreamerCameraFocusControl::supportedFocusModes() const
{
    QCamera::FocusModes focusModes;
    focusModes |= QCamera::AutoFocus;
    focusModes |= QCamera::ManualFocus;
    return focusModes;
}

QCamera::FocusStatus QGstreamerCameraFocusControl::focusStatus() const
{
    // TODO: Real focus states can be retrieved via bus (?)
    return m_focusStatus;
}

bool QGstreamerCameraFocusControl::macroFocusingEnabled() const
{
    return false;
}

bool QGstreamerCameraFocusControl::isMacroFocusingSupported() const
{
    return false;
}

void QGstreamerCameraFocusControl::setMacroFocusingEnabled(bool)
{
    // Macro focusing isn't supported. (Auto focus mode is only supported).
}

qreal QGstreamerCameraFocusControl::maximumOpticalZoom() const
{
    return 1.0;
}

qreal QGstreamerCameraFocusControl::maximumDigitalZoom() const
{
    return 1000;
}

qreal QGstreamerCameraFocusControl::opticalZoom() const
{
    return 1.0;
}

qreal QGstreamerCameraFocusControl::digitalZoom() const
{
    gint zoomFactor = 0;
    g_object_get(GST_BIN(&m_camerabin), "zoom", &zoomFactor, NULL);
    return zoomFactor;
}

void QGstreamerCameraFocusControl::zoomTo(qreal optical, qreal digital)
{
    Q_UNUSED(optical);
    g_object_set(GST_BIN(&m_camerabin), "zoom", digital, NULL);
}

QCamera::FocusPointMode QGstreamerCameraFocusControl::focusPointMode() const
{
    return QCamera::FocusPointAuto;
}

void QGstreamerCameraFocusControl::setFocusPointMode(QCamera::FocusPointMode mode)
{
    Q_UNUSED(mode);
}

QCamera::FocusPointModes QGstreamerCameraFocusControl::supportedFocusPointModes() const
{
    return QCamera::FocusPointAuto;
}

QPointF QGstreamerCameraFocusControl::customFocusPoint() const
{
    return QPointF(0.5, 0.5);
}

void QGstreamerCameraFocusControl::setCustomFocusPoint(const QPointF &point)
{
    Q_UNUSED(point);
}

QList<QRectF> QGstreamerCameraFocusControl::focusZones() const
{
    return QList<QRectF> ();
}

void QGstreamerCameraFocusControl::startFocusing()
{
    m_focusStatus = QCamera::FocusRequested;
    gst_photography_set_autofocus(GST_PHOTOGRAPHY(&m_camerabin), TRUE);
}

void QGstreamerCameraFocusControl::cancelFocusing()
{
    m_focusStatus = QCamera::FocusCanceled;
    gst_photography_set_autofocus(GST_PHOTOGRAPHY(&m_camerabin), FALSE);
}

