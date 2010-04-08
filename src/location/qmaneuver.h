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

class QManeuverPrivate;
class Q_LOCATION_EXPORT QManeuver
{
public:
    QManeuver();
    QManeuver(const QManeuver& maneuver);
    QManeuver& operator=(const QManeuver& maneuver);
    ~QManeuver();

    QString description() const;
    void setDescription(const QString &description);

    QString action() const;
    void setAction(const QString &action);

    quint32 distance() const;
    void setDistance(quint32 distance);

    qint32 duration() const;
    void setDuration(qint32 duration);

    QString turn() const;
    void setTurn(const QString &turn);

    QString streetName() const;
    void setStreetName(const QString &streetName);

    QString routeName() const;
    void setRouteName(const QString &routeName);

    QString nextStreetName() const;
    void setNextStreetName(const QString &nextStreetName);

    QString signPost() const;
    void setSignPost(const QString &signPost);

    qint64 trafficDirection() const;
    void setTrafficDirection(qint64 trafficDirection);

    qint64 icon() const;
    void setIcon(qint64 icon);

    const QList<QGeoCoordinate> wayPoints() const;
    void setWaypoints(const QList<QGeoCoordinate> &wayPoints);

    const QList<QGeoCoordinate> maneuverPoints() const;
    void setManeuverPoints(const QList<QGeoCoordinate> &maneuverPoints);

private:
    QManeuverPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QManeuver);
};

QTM_END_NAMESPACE

#endif
