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
  \brief The QGeoNavigationInstruction class is
  \ingroup maps

*/

/*!
  \enum QGeoNavigationInstruction::InstructionType

description

  \value NormalInstruction description
  \value PrivateTransportInstruction description
  \value PublicTransportInstruction description
  \value TruckInstruction description
*/

/*!
*/
QGeoNavigationInstruction::QGeoNavigationInstruction()
    : d_ptr(new QGeoNavigationInstructionPrivate()) {}

/*!
*/
QGeoNavigationInstruction::QGeoNavigationInstruction(QGeoNavigationInstructionPrivate *d_ptr)
    : d_ptr(d_ptr) {}

/*!
*/
QGeoNavigationInstruction::~QGeoNavigationInstruction()
{
    Q_D(QGeoNavigationInstruction);
    delete d;
}

/*!
*/
QGeoNavigationInstruction::InstructionType QGeoNavigationInstruction::type() const
{
    Q_D(const QGeoNavigationInstruction);
    return d->type;
}

/*!
*/
void QGeoNavigationInstruction::setPosition(const QGeoCoordinate &position)
{
    Q_D(QGeoNavigationInstruction);
    d->position = position;
}

/*!
*/
QGeoCoordinate QGeoNavigationInstruction::position() const
{
    Q_D(const QGeoNavigationInstruction);
    return d->position;
}

/*!
*/
void QGeoNavigationInstruction::setInstructionText(const QString &instructionText)
{
    Q_D(QGeoNavigationInstruction);
    d->text = instructionText;
}

/*!
*/
QString QGeoNavigationInstruction::instructionText() const
{
    Q_D(const QGeoNavigationInstruction);
    return d->text;
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
    text(other.text) {}

QGeoNavigationInstructionPrivate::~QGeoNavigationInstructionPrivate() {}

QGeoNavigationInstructionPrivate& QGeoNavigationInstructionPrivate::operator= (const QGeoNavigationInstructionPrivate &other)
{
    type = other.type;
    position = other.position;
    text = other.text;

    return *this;
}

QTM_END_NAMESPACE
