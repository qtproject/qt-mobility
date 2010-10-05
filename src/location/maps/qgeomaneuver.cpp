/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeomaneuver.h"
#include "qgeomaneuver_p.h"

#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoManeuver
    \brief The QGeoManeuver class represents a navigation instruction.

    \inmodule QtLocation

    \ingroup maps-routing

    A QGeoManeuver instance has a position at which the
    instruction applies and a textual instruction to provide at that position.
*/

/*!
\enum QGeoManeuver::InstructionDirection

Describes the change in direction associated with an instruction.

\value NoDirection
There is no direction associated with the instruction.

\value DirectionForward
The instruction indicates that the direction of travel does not need to change.

\value DirectionBearRight
The instruction indicates that the direction of travel should bear to the right.

\value DirectionLightRight
The instruction indicates that a light turn to the right is required.

\value DirectionRight
The instruction indicates that a turn to the right is required.

\value DirectionHardRight
The instruction indicates that a hard turn to the right is required.

\value DirectionUTurnRight
The instruction indicates that a u-turn to the right is required.

\value DirectionUTurnLeft
The instruction indicates that a u-turn to the left is required.

\value DirectionHardLeft
The instruction indicates that a hard turn to the left is required.

\value DirectionLeft
The instruction indicates that a turn to the left is required.

\value DirectionLightLeft
The instruction indicates that a light turn to the left is required.

\value DirectionBearLeft
The instruction indicates that the direction of travel should bear to the left.

*/

/*!
    Constructs a navigation instruction object.
*/
QGeoManeuver::QGeoManeuver()
        : d_ptr(new QGeoManeuverPrivate()) {}

/*!
    Constructs a navigation instruction object from the contents of \a other.
*/
QGeoManeuver::QGeoManeuver(const QGeoManeuver &other)
        : d_ptr(other.d_ptr) {}

/*!
    Destroys this navigation instruction object.
*/
QGeoManeuver::~QGeoManeuver() {}

/*!
    Assigns \a other to this navigation instruction object and then returns
    a reference to this navigation instruction object.
*/
QGeoManeuver& QGeoManeuver::operator= (const QGeoManeuver & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Returns whether this navigation instruction is equal to \a other.
*/
bool QGeoManeuver::operator== (const QGeoManeuver &other) const
{
    return (*(d_ptr.constData()) == *(other.d_ptr.constData()));
}

/*!
    Returns whether this navigation instruction is not equal to \a other.
*/
bool QGeoManeuver::operator!= (const QGeoManeuver &other) const
{
    return !(operator==(other));
}

/*!
    Sets the position where the instructions should be provided to \a position.
*/
void QGeoManeuver::setPosition(const QGeoCoordinate &position)
{
    d_ptr->position = position;
}

/*!
    Returns the position where the instructions should be provided.
*/
QGeoCoordinate QGeoManeuver::position() const
{
    return d_ptr->position;
}

/*!
    Sets the textual navigation instructions to \a instructionText.
*/
void QGeoManeuver::setInstructionText(const QString &instructionText)
{
    d_ptr->text = instructionText;
}

/*!
    Returns the textual navigation instructions.
*/
QString QGeoManeuver::instructionText() const
{
    return d_ptr->text;
}

/*!
    Sets the direction associated with this instruction to \a direction.
*/
void QGeoManeuver::setDirection(QGeoManeuver::InstructionDirection direction)
{
    d_ptr->direction = direction;
}

/*!
    Returns the direction associated with this instruction.
*/
QGeoManeuver::InstructionDirection QGeoManeuver::direction() const
{
    return d_ptr->direction;
}

/*!
    Sets the estimated time it will take to travel from the point at which this
    instruction was issued and the point that the next instruction should be
    issued, in seconds, to \a secs.
*/
void QGeoManeuver::setTimeToNextInstruction(int secs)
{
    d_ptr->timeToNextInstruction = secs;
}

/*!
    Returns the estimated time it will take to travel from the point at which
    this instruction was issued and the point that the next instruction should
    be issued, in seconds.
*/
int QGeoManeuver::timeToNextInstruction() const
{
    return d_ptr->timeToNextInstruction;
}

/*!
    Sets the distance, in metres, between the point at which this instruction was issued
    and the point that the next instruction should be issued to \a distance.
*/
void QGeoManeuver::setDistanceToNextInstruction(qreal distance)
{
    d_ptr->distanceToNextInstruction = distance;
}

/*!
    Returns the distance, in metres, between the point at which this instruction was issued
    and the point that the next instruction should be issued.
*/
qreal QGeoManeuver::distanceToNextInstruction() const
{
    return d_ptr->distanceToNextInstruction;
}

/*!
*/
void QGeoManeuver::setWaypoint(const QGeoCoordinate &coordinate)
{
    d_ptr->waypoint = coordinate;
}

/*!
*/
QGeoCoordinate QGeoManeuver::waypoint() const
{
    return d_ptr->waypoint;
}

/*******************************************************************************
*******************************************************************************/

QGeoManeuverPrivate::QGeoManeuverPrivate()
        : direction(QGeoManeuver::NoDirection),
        timeToNextInstruction(0),
        distanceToNextInstruction(0.0) {}

QGeoManeuverPrivate::QGeoManeuverPrivate(const QGeoManeuverPrivate &other)
        : QSharedData(other),
        position(other.position),
        text(other.text),
        direction(other.direction),
        timeToNextInstruction(other.timeToNextInstruction),
        distanceToNextInstruction(other.distanceToNextInstruction),
        waypoint(other.waypoint) {}

QGeoManeuverPrivate::~QGeoManeuverPrivate() {}

bool QGeoManeuverPrivate::operator ==(const QGeoManeuverPrivate &other) const
{
    return ((position == other.position)
            && (text == other.text)
            && (direction == other.direction)
            && (timeToNextInstruction == other.timeToNextInstruction)
            && (distanceToNextInstruction == other.distanceToNextInstruction)
            && (waypoint == other.waypoint));
}

QTM_END_NAMESPACE
