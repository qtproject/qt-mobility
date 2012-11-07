/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOMANEUVER_H
#define QDECLARATIVEGEOMANEUVER_H

#include <qgeomaneuver.h>

#include "qdeclarativecoordinate_p.h"

#include <QObject>

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoManeuver : public QObject
{
    Q_OBJECT
    Q_ENUMS(Direction)

    Q_PROPERTY(bool valid READ valid)
    Q_PROPERTY(QDeclarativeCoordinate* position READ position CONSTANT)
    Q_PROPERTY(QString instructionText READ instructionText CONSTANT)
    Q_PROPERTY(Direction direction READ direction CONSTANT)
    Q_PROPERTY(int timeToNextInstruction READ timeToNextInstruction CONSTANT)
    Q_PROPERTY(qreal distanceToNextInstruction READ distanceToNextInstruction CONSTANT)
    Q_PROPERTY(QDeclarativeCoordinate* waypoint READ waypoint CONSTANT)

public:
    enum Direction {
        NoDirection = QGeoManeuver::NoDirection,
        DirectionForward = QGeoManeuver::DirectionForward,
        DirectionBearRight = QGeoManeuver::DirectionBearRight,
        DirectionLightRight = QGeoManeuver::DirectionLightRight,
        DirectionRight = QGeoManeuver::DirectionRight,
        DirectionHardRight = QGeoManeuver::DirectionHardRight,
        DirectionUTurnRight = QGeoManeuver::DirectionUTurnRight,
        DirectionUTurnLeft = QGeoManeuver::DirectionUTurnLeft,
        DirectionHardLeft = QGeoManeuver::DirectionHardLeft,
        DirectionLeft = QGeoManeuver::DirectionLeft,
        DirectionLightLeft = QGeoManeuver::DirectionLightLeft,
        DirectionBearLeft = QGeoManeuver::DirectionBearLeft
    };

    QDeclarativeGeoManeuver(QObject *parent = 0);
    QDeclarativeGeoManeuver(const QGeoManeuver &maneuver, QObject *parent = 0);
    ~QDeclarativeGeoManeuver();

    bool valid() const;

    QDeclarativeCoordinate* position() const;
    QString instructionText() const;
    Direction direction() const;
    int timeToNextInstruction() const;
    qreal distanceToNextInstruction() const;
    QDeclarativeCoordinate* waypoint() const;

private:
    QGeoManeuver maneuver_;
    QDeclarativeCoordinate* position_;
    QDeclarativeCoordinate* waypoint_;
};

QTM_END_NAMESPACE

#endif
