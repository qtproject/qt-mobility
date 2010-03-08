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

#ifndef QLOCATION_MANEUVER_H
#define QLOCATION_MANEUVER_H

#include <QString>
#include <QList>

#include "qgeocoordinatemaps.h"

QTM_BEGIN_NAMESPACE

/*!
* This class represents a maneuver as part of a route reply
* \see QRouteReply
*/
class QManeuver
{
    friend class QRouteXmlHandler;

public:
    /*!
    * Default constructor.
    */
    QManeuver() : dur(0), dist(0), traffDir(0), icn(0) {}
    /*!
    * Copy constructor.
    * @param maneuver The maneuver to be copied.
    */
    QManeuver(const QManeuver& maneuver);
    /*!
    * Assignment operator.
    * @param maneuver The maneuver to be assigned from.
    * @return A reference to this maneuver.
    */
    QManeuver& operator=(const QManeuver& maneuver);

    /*!
    * @return A textual representation of the maneuver that has to be performed to follow the route.
    */
    QString description() const {
        return descr;
    }
    /*!
    * @return A string constant that describes the essential action
    *         that has to be taken on this maneuver.
    */
    QString action() const {
        return act;
    }
    /*!
    * @return The distance that is covered by this single maneuver in meters.
    */
    quint32 distance() const {
        return dist;
    }
    /*!
    * @return The estimated duration that performing the whole maneuver will take
    *         in the xsd:duration format.
    */
    qint32 duration() const {
        return dur;
    }
    /*!
    * @return The turn to be taken in this maneuver.
    */
    QString turn() const {
        return trn;
    }
    /*!
    * @return The name of the street involved.
    */
    QString streetName() const {
        return stName;
    }
    /*!
    * @return The name of the route.
    */
    QString routeName() const {
        return rtName;
    }
    /*!
    * @return The name of the next street.
    */
    QString nextStreetName() const {
        return nxtStreetName;
    }
    /*!
    * @return A sign post
    */
    QString signPost() const {
        return sgnPost;
    }
    /*!
    * @return A value representing the traffic direction.
    */
    qint64  trafficDirection() const {
        return traffDir;
    }
    /*!
    * @return A value representing an icon.
    */
    qint64  icon() const {
        return icn;
    }

    /*!
    * Returns the geographical coordinates that define
    * this part of the route in form of a 2D-polyline.
    * The geo coordinates in the way points are less detailed then
    * those in the <i>maneuver points</i>, and primarily designed for displaying on screen.
    * \see maneuverPoints()
    * @return A list of way points.
    */
    const QList<QGeoCoordinateMaps> wayPoints() const {
        return wPoints;
    }
    /*!
    * Returns the geographical coordinates that define
    * this part of the route in form of a 2D-polyline.
    * Maneuver points describe the actual maneuver to take with a much higher detail
    * than the <i>ways points</i> do.
    * \see wayPoints()
    * @return A list of maneuver points.
    */
    const QList<QGeoCoordinateMaps> maneuverPoints() const {
        return mPoints;
    }

private:
    QString descr; //!< description
    QString act; //!< action
    qint32  dur; //!< duration
    quint32 dist; //!< distance
    QString trn; //!< turn
    QString stName; //!< street name
    QString rtName; //!< route name
    QString nxtStreetName; //!< next street name
    QString sgnPost; //!< sign post
    qint64  traffDir; //!< traffic directions
    qint64  icn; //!< icon

    QList<QGeoCoordinateMaps> wPoints; //!< way points
    QList<QGeoCoordinateMaps> mPoints; //!< maneuver points
};

QTM_END_NAMESPACE

#endif
