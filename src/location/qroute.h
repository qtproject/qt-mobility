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

#ifndef QLOCATION_ROUTE_H
#define QLOCATION_ROUTE_H

#include <QDateTime>
#include <QList>
#include <QRectF>

#include "qmaneuver.h"

QTM_BEGIN_NAMESPACE

/*!
* This class represents a route as contained in a \ref QRouteReply.
* A CRoute contains a collection of \ref CManeuver objects.
*/
class Q_LOCATION_EXPORT QRoute
{
    friend class QRouteXmlHandler;

public:
    //! The default constructor.
    QRoute() {};
    //! The copy constructor.
    QRoute(const QRoute& route);
    //! The assignment operator.
    QRoute& operator=(const QRoute& route);

    /*!
    * @return The departure time of this route.
    */
    QDateTime departure() const {
        return tod;
    }
    /*!
    * @return The arrival time of this route.
    */
    QDateTime arrival() const {
        return toa;
    }
    /*!
    * @return The distance covered by this route in meters.
    */
    quint32 distance() const {
        return dist;
    }

    /*!
    * @return The bounding box that completely encloses the route.
    *         The x coordinates of the corner points represent longitudes,
              the y coordinates represent latitudes.
    */
    const QRectF& boundingBox() const {
        return box;
    }
    /*!
    * @return The list of all maneuvers comprising the route.
    */
    QList<QManeuver> maneuvers() const {
        return man;
    }

protected:
    quint32 dist;
    QDateTime tod;
    QDateTime toa;

    QRectF box;
    QList<QManeuver> man;
};

QTM_END_NAMESPACE

#endif
