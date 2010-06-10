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

#ifndef QGEOMAPDATA_H
#define QGEOMAPDATA_H

#include "qgeomapwidget.h"

#include <QObject>
#include <QSize>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoBoundingBox;
class QGeoMappingManagerEngine;
class QGeoMapObject;
class QGeoMapDataPrivate;

class Q_LOCATION_EXPORT QGeoMapData
{
public:
    QGeoMapData(QGeoMappingManagerEngine *engine, QGeoMapWidget *widget);
    virtual ~QGeoMapData();

    virtual void setViewportSize(const QSizeF &size);
    virtual QSizeF viewportSize() const;

    virtual void setZoomLevel(qreal zoomLevel);
    virtual qreal zoomLevel() const;

    virtual void pan(int dx, int dy);

    virtual void setCenter(const QGeoCoordinate &center);
    virtual QGeoCoordinate center() const;

    void setMapType(QGeoMapWidget::MapType mapType);
    QGeoMapWidget::MapType mapType() const;

    virtual void addMapObject(QGeoMapObject *mapObject);
    virtual void removeMapObject(QGeoMapObject *mapObject);
    virtual QList<QGeoMapObject*> mapObjects();
    virtual QList<QGeoMapObject*> visibleMapObjects();
    virtual QList<QGeoMapObject*> mapObjectsAtScreenPosition(const QPointF &screenPosition);
    virtual QList<QGeoMapObject*> mapObjectsInScreenRect(const QRectF &screenRect);

    virtual QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const = 0;
    virtual QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition) const = 0;

    void setImageChangesTriggerUpdates(bool trigger);
    bool imageChangesTriggerUpdates() const;

    void setMapImage(const QPixmap &mapImage);
    QPixmap mapImage() const;

protected:
    QGeoMapWidget* widget() const;
    QGeoMappingManagerEngine* engine() const;

    QGeoMapObject* containerObject();

private:
    QGeoMapDataPrivate* d_ptr;
    Q_DISABLE_COPY(QGeoMapData)
};

QTM_END_NAMESPACE

#endif
