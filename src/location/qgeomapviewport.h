/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QGEOMAPVIEWPORT_H
#define QGEOMAPVIEWPORT_H

#include "qgeomapwidget.h"

#include <QObject>
#include <QSize>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoBoundingBox;
class QGeoMappingManager;
class QGeoMapViewportPrivate;

class Q_LOCATION_EXPORT QGeoMapViewport
{
public:
    QGeoMapViewport(QGeoMappingManager *manager, QGeoMapWidget *widget);
    virtual ~QGeoMapViewport();

    virtual QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const = 0;
    virtual QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition) const = 0;

    virtual void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    virtual void setCenter(const QGeoCoordinate &center);
    virtual QGeoCoordinate center() const;

    void setViewportSize(const QSizeF &size);
    QSizeF viewportSize() const;

    void setMapType(QGeoMapWidget::MapType mapType);
    QGeoMapWidget::MapType mapType() const;

    void setImageChangesTriggerUpdates(bool trigger);
    bool imageChangesTriggerUpdates() const;

    void setMapImage(const QPixmap &mapImage);
    QPixmap mapImage();

protected:
    QGeoMapViewport(QGeoMapViewportPrivate *dd, QGeoMappingManager *manager, QGeoMapWidget *widget);

    QGeoMapWidget* widget() const;
    QGeoMappingManager* manager() const;

    QGeoMapViewportPrivate* d_ptr;

private:
    Q_DECLARE_PRIVATE(QGeoMapViewport)
    Q_DISABLE_COPY(QGeoMapViewport)
};

QTM_END_NAMESPACE

#endif
