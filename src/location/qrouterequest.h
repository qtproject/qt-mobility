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

#ifndef QLOCATION_ROUTEREQUEST_H
#define QLOCATION_ROUTEREQUEST_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>

#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

class QRouteRequestPrivate;

class Q_LOCATION_EXPORT QRouteRequest
{
    friend class QGeoNetworkManager;

public:

    enum RouteType {
        Fastest,
        Shortest,
        Economic
    };

    enum RouteMode {
        Car,
        Pedestrian,
        PublicTransport
    };

    enum RouteAvoid {
        Highways,
        Tollroads,
        Ferries,
        Tunnels,
        Dirtroads,
        RailFerries
    };

public:
    QRouteRequest();
    ~QRouteRequest();

    QString version() const;

    void setSource(const QGeoCoordinate& source);
    QGeoCoordinate source() const;

    void setDestination(const QGeoCoordinate& destination);
    QGeoCoordinate destination() const;

    void setTotalResults(quint32 totalResults);
    quint32 totalResults() const;

    void setAlternatives(quint16 nAlternatives);
    quint16 alternatives() const;

    void setLanguage(const QString& code);
    QString language() const;

    void setDepartureTime(const QDateTime& departureTime);
    QDateTime departureTime() const;

    void setArrivalTime(const QDateTime& arrivalTime);
    QDateTime arrivalTime() const;

    void setType(RouteType type);
    RouteType type() const;

    void setMode(RouteMode mode);
    RouteMode mode() const;

    void setAvoid(QList<RouteAvoid> avoid);
    QList<RouteAvoid> avoid() const;

    void addStopOver(const QGeoCoordinate& stopOver);
    void removeStopOver(const QGeoCoordinate& stopOver);
    const QList<QGeoCoordinate>& stopOvers() const;

    QString requestString(const QString &host) const;

private:
    QRouteRequestPrivate *d_ptr;

    Q_DECLARE_PRIVATE(QRouteRequest)
    Q_DISABLE_COPY(QRouteRequest)
};

QTM_END_NAMESPACE

#endif
