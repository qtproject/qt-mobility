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

#ifndef QGEOTILEDMAPVIEWPORT_H
#define QGEOTILEDMAPVIEWPORT_H

#include "qgeomapviewport.h"

QTM_BEGIN_NAMESPACE

class QGeoTiledMapViewportPrivate;

class Q_LOCATION_EXPORT QGeoTiledMapViewport : public QGeoMapViewport
{
public:
    QGeoTiledMapViewport(QGeoMappingManagerEngine *engine, QGeoMapWidget *widget);
    virtual ~QGeoTiledMapViewport();

    QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const;
    QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition) const;

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    void setZoomLevel(qreal zoomLevel);
    void setViewportSize(const QSizeF &size);
    void pan(int dx, int dy);

    // TODO WorldPixel a better name?
    void setTopLeftMapPixelX(qulonglong x);
    qulonglong topLeftMapPixelX() const;

    void setTopLeftMapPixelY(qulonglong y);
    qulonglong topLeftMapPixelY() const;

    qulonglong width() const;
    qulonglong height() const;

    qulonglong zoomFactor() const;

    QRectF protectedRegion() const;
    void clearProtectedRegion();

protected:
    virtual void coordinateToWorldPixel(const QGeoCoordinate &coordinate, qulonglong *x, qulonglong *y) const;
    virtual QGeoCoordinate worldPixelToCoordinate(qulonglong x, qulonglong y) const;

private:
    Q_DECLARE_PRIVATE(QGeoTiledMapViewport);
    Q_DISABLE_COPY(QGeoTiledMapViewport)
};

QTM_END_NAMESPACE

#endif
