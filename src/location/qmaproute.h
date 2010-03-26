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

#ifndef QLOCATION_MAPROUTE_H
#define QLOCATION_MAPROUTE_H

#include <QPen>
#include <QMap>
#include <QList>
#include <QPixmap>
#include <QLineF>

#include "qgeoroute.h"
#include "qmapobject.h"

QTM_BEGIN_NAMESPACE

class QMapRoutePrivate;
class Q_LOCATION_EXPORT QMapRoute : public QMapObject
{
    friend class QMapView;

public:
    QMapRoute( const QGeoRoute& route,
              const QPen& pen = QPen(),
              const QPixmap& endpointMarker = QPixmap(),
              quint16 layerIndex = 0);

    QPen pen() const;

protected:
    virtual void compMapCoords();
    virtual void compIntersectingTiles(const QRectF& box);
    virtual bool intersects(const QRectF& rect) const;
    virtual void paint(QPainter* painter, const QRectF& viewPort);

    QMapRoute(QMapRoutePrivate &dd,
              const QGeoRoute& route,
              const QPen& pen = QPen(),
              const QPixmap& endpointMarker = QPixmap(),
              quint16 layerIndex = 0);

private:
    void addSegment(const QLineF& line);
    Q_DECLARE_PRIVATE(QMapRoute)
};

QTM_END_NAMESPACE

#endif
