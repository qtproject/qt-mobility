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

#ifndef QCAMERAFOCUSCONTROL_H
#define QCAMERAFOCUSCONTROL_H

#include <qmediacontrol.h>
#include <qmediaobject.h>

#include <experimental/qcamera.h>

QTM_BEGIN_NAMESPACE

class Q_MEDIA_EXPORT QCameraFocusControl : public QMediaControl
{
    Q_OBJECT

public:
    ~QCameraFocusControl();

    virtual QCamera::FocusMode focusMode() const = 0;
    virtual void setFocusMode(QCamera::FocusMode mode) = 0;
    virtual QCamera::FocusModes supportedFocusModes() const = 0;
    virtual QCamera::FocusStatus focusStatus() const = 0;

    virtual bool macroFocusingEnabled() const = 0;
    virtual bool isMacroFocusingSupported() const = 0;
    virtual void setMacroFocusingEnabled(bool) = 0;

    virtual qreal maximumOpticalZoom() const = 0;
    virtual qreal maximumDigitalZoom() const = 0;
    virtual qreal opticalZoom() const = 0;
    virtual qreal digitalZoom() const = 0;

    virtual void zoomTo(qreal optical, qreal digital) = 0;

    virtual QCamera::FocusPointMode focusPointMode() const = 0;
    virtual void setFocusPointMode(QCamera::FocusPointMode mode) = 0;
    virtual QCamera::FocusPointModes supportedFocusPointModes() const = 0;
    virtual QPointF customFocusPoint() const = 0;
    virtual void setCustomFocusPoint(const QPointF &point) = 0;

    virtual QList<QRectF> focusZones() const = 0;

public Q_SLOTS:
    virtual void startFocusing() = 0;
    virtual void cancelFocusing() = 0;

Q_SIGNALS:
    void opticalZoomChanged(qreal opticalZoom);
    void digitalZoomChanged(qreal digitalZoom);
    void focusStatusChanged(QCamera::FocusStatus);
    void focusZonesChanged(const QList<QRectF> &);

protected:
    QCameraFocusControl(QObject* parent = 0);
};

#define QCameraFocusControl_iid "com.nokia.Qt.QCameraFocusingControl/1.0"
Q_MEDIA_DECLARE_CONTROL(QCameraFocusControl, QCameraFocusControl_iid)

QTM_END_NAMESPACE

#endif  // QCAMERAFOCUSCONTROL_H

