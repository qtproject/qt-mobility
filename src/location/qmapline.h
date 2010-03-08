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

#ifndef QLOCATION_MAPLINE_H
#define QLOCATION_MAPLINE_H

#include <QPen>
#include <QPointF>
#include <QPainter>

#include "qmapobject.h"
#include "qgeocoordinatemaps.h"

QTM_BEGIN_NAMESPACE

/*!
* Internal representation of a line that has been added to the map.
*/
class QMapLine : public QMapObject
{
    friend class QMapView;

public:
    /*!
    * The contructor.
    * @param point1 The first end point (as geo coordinate) of the line.
    * @param point2 The second end point (as geo coordinate) of the line.
    * @param pen The pen used for drawing the line.
    * @param layerIndex The map layer index of the pixmap.
    */
    QMapLine(const QMapView& mapView, const QGeoCoordinateMaps& point1, const QGeoCoordinateMaps& point2,
             const QPen& pen = QPen(), quint16 layerIndex = 0);

    /*!
    * @return The pen used for drawing this line.
    */
    QPen pen() const {
        return p;
    }
    /*!
    * @return The first end point (as geo coordinate) of the line.
    */
    QGeoCoordinateMaps point1() const {
        return pt1;
    }
    /*!
    * @return The second end point (as geo coordinate) of the line.
    */
    QGeoCoordinateMaps point2() const {
        return pt2;
    }

protected:
    virtual bool intersects(const QRectF& tileRect) const;
    virtual void compMapCoords();
    virtual void paint(QPainter* painter, const QRectF& viewPort);

private:
    QGeoCoordinateMaps pt1; //!< The first end point of the line.
    QGeoCoordinateMaps pt2; //!< The second end point of the line.
    QPen p; //!< The pen used for drawing this line.
    QLineF line;
};

QTM_END_NAMESPACE

#endif
