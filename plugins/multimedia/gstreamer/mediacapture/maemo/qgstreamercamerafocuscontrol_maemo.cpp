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

QCGstreamerCameraFocusControl::QCGstreamerCameraFocusControl(QGstreamerCaptureSession *session)
{
}

QCGstreamerCameraFocusControl::~QCGstreamerCameraFocusControl()
{
}

QCamera::FocusMode QCGstreamerCameraFocusControl::focusMode()
{
}

void QCGstreamerCameraFocusControl::setFocusMode(QCamera::FocusMode mode)
{
}
QCamera::FocusModes QCGstreamerCameraFocusControl::supportedFocusModes() const
{
}

QCamera::FocusStatus QCGstreamerCameraFocusControl::focusStatus() const
{
}

bool QCGstreamerCameraFocusControl::macroFocusingEnabled() const
{
}

bool QCGstreamerCameraFocusControl::isMacroFocusingSupported() const
{
}

void QCGstreamerCameraFocusControl::setMacroFocusingEnabled(bool)
{
}

qreal QCGstreamerCameraFocusControl::maximumOpticalZoom() const
{
}

qreal QCGstreamerCameraFocusControl::maximumDigitalZoom() const
{
}

qreal QCGstreamerCameraFocusControl::opticalZoom() const
{
}

qreal QCGstreamerCameraFocusControl::digitalZoom() const
{
}

void QCGstreamerCameraFocusControl::zoomTo(qreal optical, qreal digital)
{
}

QCamera::FocusPointMode QCGstreamerCameraFocusControl::focusPointMode() const
{
}

void QCGstreamerCameraFocusControl::setFocusPointMode(QCamera::FocusPointMode mode)
{
}

QCamera::FocusPointModes QCGstreamerCameraFocusControl::supportedFocusPointModes() const
{
}

QPointF QCGstreamerCameraFocusControl::customFocusPoint() const
{
}

void QCGstreamerCameraFocusControl::setCustomFocusPoint(const QPointF &point)
{
}

QList<QRectF> QCGstreamerCameraFocusControl::focusZones() const
{
}

void QCGstreamerCameraFocusControl::startFocusing()
{
}

void QCGstreamerCameraFocusControl::cancelFocusing()
{
}

