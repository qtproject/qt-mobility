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

#include "qgeoinstruction.h"
#include "qgeoinstruction_p.h"

#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoInstruction
    \brief The QGeoInstruction class represents a navigation instruction.

    \inmodule QtLocation

    \ingroup maps-routing

    A QGeoInstruction instance has a position at which the
    instruction applies and a textual instruction to provide at that position.
*/

/*!
\enum QGeoInstruction::InstructionDirection

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
QGeoInstruction::QGeoInstruction()
        : d_ptr(new QGeoInstructionPrivate()) {}


/*!
    Constructs a navigation instruction object from the contents of \a other.
*/
QGeoInstruction::QGeoInstruction(const QGeoInstruction &other)
        : d_ptr(other.d_ptr) {}

/*!
    Destroys this navigation instruction object.
*/
QGeoInstruction::~QGeoInstruction() {}

/*!
    Assigns \a other to this navigation instruction object and then returns
    a reference to this navigation instruction object.
*/
QGeoInstruction& QGeoInstruction::operator= (const QGeoInstruction & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Returns whether this navigation instruction is equal to \a other.
*/
bool QGeoInstruction::operator== (const QGeoInstruction &other) const
{
    return (*(d_ptr.constData()) == *(other.d_ptr.constData()));
}

/*!
    Returns whether this navigation instruction is not equal to \a other.
*/
bool QGeoInstruction::operator!= (const QGeoInstruction &other) const
{
    return !(operator==(other));
}

/*!
    Sets the position where the instructions should be provided to \a position.
*/
void QGeoInstruction::setPosition(const QGeoCoordinate &position)
{
    d_ptr->position = position;
}

/*!
    Returns the position where the instructions should be provided.
*/
QGeoCoordinate QGeoInstruction::position() const
{
    return d_ptr->position;
}

/*!
    Sets the textual navigation instructions to \a instructionText.
*/
void QGeoInstruction::setInstructionText(const QString &instructionText)
{
    d_ptr->text = instructionText;
}

/*!
    Returns the textual navigation instructions.
*/
QString QGeoInstruction::instructionText() const
{
    return d_ptr->text;
}

/*!
    Sets the direction associated with this instruction to \a direction.
*/
void QGeoInstruction::setDirection(QGeoInstruction::InstructionDirection direction)
{
    d_ptr->direction = direction;
}

/*!
    Returns the direction associated with this instruction.
*/
QGeoInstruction::InstructionDirection QGeoInstruction::direction() const
{
    return d_ptr->direction;
}

/*!
    Sets the estimated time it will take to travel from the point at which this
    instruction was issued and the point that the next instruction should be
    issued, in seconds, to \a secs.
*/
void QGeoInstruction::setTimeToNextInstruction(int secs)
{
    d_ptr->timeToNextInstruction = secs;
}

/*!
    Returns the estimated time it will take to travel from the point at which
    this instruction was issued and the point that the next instruction should
    be issued, in seconds.
*/
int QGeoInstruction::timeToNextInstruction() const
{
    return d_ptr->timeToNextInstruction;
}

/*!
    Sets the distance, in metres, between the point at which this instruction was issued
    and the point that the next instruction should be issued to \a distance.
*/
void QGeoInstruction::setDistanceToNextInstruction(qreal distance)
{
    d_ptr->distanceToNextInstruction = distance;
}

/*!
    Returns the distance, in metres, between the point at which this instruction was issued
    and the point that the next instruction should be issued.
*/
qreal QGeoInstruction::distanceToNextInstruction() const
{
    return d_ptr->distanceToNextInstruction;
}

/*******************************************************************************
*******************************************************************************/

QGeoInstructionPrivate::QGeoInstructionPrivate()
        : direction(QGeoInstruction::NoDirection),
        timeToNextInstruction(0),
        distanceToNextInstruction(0.0) {}

QGeoInstructionPrivate::QGeoInstructionPrivate(const QGeoInstructionPrivate &other)
        : QSharedData(other),
        position(other.position),
        text(other.text),
        direction(other.direction),
        timeToNextInstruction(other.timeToNextInstruction),
        distanceToNextInstruction(other.distanceToNextInstruction) {}

QGeoInstructionPrivate::~QGeoInstructionPrivate() {}

bool QGeoInstructionPrivate::operator ==(const QGeoInstructionPrivate &other) const
{
    return ((position == other.position)
            && (text == other.text)
            && (direction == other.direction)
            && (timeToNextInstruction == other.timeToNextInstruction)
            && (distanceToNextInstruction == other.distanceToNextInstruction));
}

QTM_END_NAMESPACE
