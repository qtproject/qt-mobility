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

    Subclasses of QGeoNavigationInstruction will provide more detailed
    instruction information.  The type() function can be used to determine
    the subclass that is being used.

    \sa QGeoNavigationInstruction::InstructionType
*/

/*!
    \enum QGeoNavigationInstruction::InstructionType

    Describes the specialization of a QGeoNavigationInstruction object.

    This is used to distinguish between different subclasses of
    QGeoNavigationInstruction when they are accessed as
    QGeoNavigationInstruction objects.

    \value NormalInstruction
        The instruction is not specialized.
    \value PrivateTransportInstruction
        The instruction is specialized for those travelling by private transport.
    \value PublicTransportInstruction
        The instruction is specialized for those travelling by public transport
    \value TruckInstruction
        The instruction is specialized for those travelling by truck.

    \sa QGeoNavigationInstruction::type()
*/

/*!
    Constructs a QGeoNavigationInstruction object.
*/
QGeoNavigationInstruction::QGeoNavigationInstruction()
        : d_ptr(new QGeoNavigationInstructionPrivate()) {}

/*
    For internal use
*/
QGeoNavigationInstruction::QGeoNavigationInstruction(QGeoNavigationInstructionPrivate *d_ptr)
        : d_ptr(d_ptr) {}

/*!
    Destroys this QGeoNavigationInstruction object.
*/
QGeoNavigationInstruction::~QGeoNavigationInstruction()
{
    Q_D(QGeoNavigationInstruction);
    delete d;
}

/*!
    Returns the type of this instruction.

    If the type is something other than
    QGeoNavigationInstruction::NormalInstruction then this
    QGeoNavigationInstruction object has been created as a subclass of
    QGeoNavigationInstruction which contains more detailed information about
    the instructions.

    \sa QGeoNavigationInstruction::InstructionType
*/
QGeoNavigationInstruction::InstructionType QGeoNavigationInstruction::type() const
{
    Q_D(const QGeoNavigationInstruction);
    return d->type;
}

/*!
    Sets the position where the instructions should be provided to \a position.
*/
void QGeoNavigationInstruction::setPosition(const QGeoCoordinate &position)
{
    Q_D(QGeoNavigationInstruction);
    d->position = position;
}

/*!
    Returns the position where the instructions should be provided.
*/
QGeoCoordinate QGeoNavigationInstruction::position() const
{
    Q_D(const QGeoNavigationInstruction);
    return d->position;
}

/*!
    Sets the textual navigation instructions to \a instructionText.
*/
void QGeoNavigationInstruction::setInstructionText(const QString &instructionText)
{
    Q_D(QGeoNavigationInstruction);
    d->text = instructionText;
}

/*!
    Returns the textual navigation instructions.
*/
QString QGeoNavigationInstruction::instructionText() const
{
    Q_D(const QGeoNavigationInstruction);
    return d->text;
}

/*!
    Sets the estimated time it will take to travel from the point at which this
    instruction was issued and the point that the next instruction should be
    issued, in seconds, to \a secs.
*/
void QGeoNavigationInstruction::setTimeToNextInstruction(int secs)
{
    Q_D(QGeoNavigationInstruction);
    d->timeToNextInstruction = secs;
}

/*!
    Returns the estimated time it will take to travel from the point at which
    this instruction was issued and the point that the next instruction should
    be issued, in seconds.
*/
int QGeoNavigationInstruction::timeToNextInstruction() const
{
    Q_D(const QGeoNavigationInstruction);
    return d->timeToNextInstruction;
}

/*!
    Sets the distance between the point at which this instruction was issued
    and the point that the next instruction should be issued to \a distance.
*/
void QGeoNavigationInstruction::setDistanceToNextInstruction(const QGeoDistance &distance)
{
    Q_D(QGeoNavigationInstruction);
    d->distanceToNextInstruction = distance;
}

/*!
    Returns the distance between the point at which this instruction was issued
    and the point that the next instruction should be issued.
*/
QGeoDistance QGeoNavigationInstruction::distanceToNextInstruction() const
{
    Q_D(const QGeoNavigationInstruction);
    return d->distanceToNextInstruction;
}

/*******************************************************************************
*******************************************************************************/

QGeoNavigationInstructionPrivate::QGeoNavigationInstructionPrivate()
{
    type = QGeoNavigationInstruction::NormalInstruction;
}

QGeoNavigationInstructionPrivate::QGeoNavigationInstructionPrivate(const QGeoNavigationInstructionPrivate &other)
        : type(other.type),
        position(other.position),
        text(other.text),
        timeToNextInstruction(other.timeToNextInstruction),
        distanceToNextInstruction(other.distanceToNextInstruction) {}

QGeoNavigationInstructionPrivate::~QGeoNavigationInstructionPrivate() {}

QGeoNavigationInstructionPrivate& QGeoNavigationInstructionPrivate::operator= (const QGeoNavigationInstructionPrivate & other)
{
    type = other.type;
    position = other.position;
    text = other.text;
    timeToNextInstruction = other.timeToNextInstruction;
    distanceToNextInstruction = other.distanceToNextInstruction;

    return *this;
}

QTM_END_NAMESPACE
