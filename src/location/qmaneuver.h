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

#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

class Q_LOCATION_EXPORT QManeuver
{
    friend class QRouteXmlParser;

public:
    QManeuver();
    QManeuver(const QManeuver& maneuver);
    QManeuver& operator=(const QManeuver& maneuver);

    QString description() const;
    QString action() const;
    quint32 distance() const;
    qint32 duration() const;
    QString turn() const;
    QString streetName() const;
    QString routeName() const;
    QString nextStreetName() const;
    QString signPost() const;
    qint64  trafficDirection() const;
    qint64  icon() const;

    const QList<QGeoCoordinate> wayPoints() const;
    const QList<QGeoCoordinate> maneuverPoints() const;

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

    QList<QGeoCoordinate> wPoints; //!< way points
    QList<QGeoCoordinate> mPoints; //!< maneuver points
};

QTM_END_NAMESPACE

#endif
