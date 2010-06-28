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

#include "qgeonavigationinstruction.h"
#include "qgeonavigationinstruction_p.h"

#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoNavigationInstruction
    \brief The QGeoNavigationInstruction class represents a navigation instruction.

    \inmodule QtLocation
    
    \ingroup maps-routing

    A QGeoNavigationInstruction instance has a position at which the
    instruction applies and a textual instruction to provide at that position.
*/

/*!
    Constructs a navigation instruction object.
*/
QGeoNavigationInstruction::QGeoNavigationInstruction()
        : d_ptr(new QGeoNavigationInstructionPrivate()) {}


/*!
    Constructs a navigation instruction object from the contents of \a other.
*/
QGeoNavigationInstruction::QGeoNavigationInstruction(const QGeoNavigationInstruction &other)
    : d_ptr(other.d_ptr) {}

/*!
    Destroys this navigation instruction object.
*/
QGeoNavigationInstruction::~QGeoNavigationInstruction() {}

/*!
    Assigns \a other to this navigation instruction object and then returns
    a reference to this navigation instruction object.
*/
QGeoNavigationInstruction& QGeoNavigationInstruction::operator= (const QGeoNavigationInstruction &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Returns whether this navigation instruction is equal to \a other.
*/
bool QGeoNavigationInstruction::operator== (const QGeoNavigationInstruction &other) const
{
    return (*(d_ptr.constData()) == *(other.d_ptr.constData()));
}

/*!
    Returns whether this navigation instruction is not equal to \a other.
*/
bool QGeoNavigationInstruction::operator!= (const QGeoNavigationInstruction &other) const
{
    return !(operator==(other));
}

/*!
    Sets the position where the instructions should be provided to \a position.
*/
void QGeoNavigationInstruction::setPosition(const QGeoCoordinate &position)
{
    d_ptr->position = position;
}

/*!
    Returns the position where the instructions should be provided.
*/
QGeoCoordinate QGeoNavigationInstruction::position() const
{
    return d_ptr->position;
}

/*!
    Sets the textual navigation instructions to \a instructionText.
*/
void QGeoNavigationInstruction::setInstructionText(const QString &instructionText)
{
    d_ptr->text = instructionText;
}

/*!
    Returns the textual navigation instructions.
*/
QString QGeoNavigationInstruction::instructionText() const
{
    return d_ptr->text;
}

/*!
    Sets the estimated time it will take to travel from the point at which this
    instruction was issued and the point that the next instruction should be
    issued, in seconds, to \a secs.
*/
void QGeoNavigationInstruction::setTimeToNextInstruction(int secs)
{
    d_ptr->timeToNextInstruction = secs;
}

/*!
    Returns the estimated time it will take to travel from the point at which
    this instruction was issued and the point that the next instruction should
    be issued, in seconds.
*/
int QGeoNavigationInstruction::timeToNextInstruction() const
{
    return d_ptr->timeToNextInstruction;
}

/*!
    Sets the distance, in metres, between the point at which this instruction was issued
    and the point that the next instruction should be issued to \a distance.
*/
void QGeoNavigationInstruction::setDistanceToNextInstruction(qreal distance)
{
    d_ptr->distanceToNextInstruction = distance;
}

/*!
    Returns the distance, in metres, between the point at which this instruction was issued
    and the point that the next instruction should be issued.
*/
qreal QGeoNavigationInstruction::distanceToNextInstruction() const
{
    return d_ptr->distanceToNextInstruction;
}

/*******************************************************************************
*******************************************************************************/

QGeoNavigationInstructionPrivate::QGeoNavigationInstructionPrivate()
    : timeToNextInstruction(0),
    distanceToNextInstruction(0.0) {}

QGeoNavigationInstructionPrivate::QGeoNavigationInstructionPrivate(const QGeoNavigationInstructionPrivate &other)
        : QSharedData(other),
        position(other.position),
        text(other.text),
        timeToNextInstruction(other.timeToNextInstruction),
        distanceToNextInstruction(other.distanceToNextInstruction) {}

QGeoNavigationInstructionPrivate::~QGeoNavigationInstructionPrivate() {}

bool QGeoNavigationInstructionPrivate::operator ==(const QGeoNavigationInstructionPrivate &other) const
{
    return ((position == other.position)
            && (text == other.text)
            && (timeToNextInstruction == other.timeToNextInstruction)
            && (distanceToNextInstruction == other.distanceToNextInstruction));
}

QTM_END_NAMESPACE
