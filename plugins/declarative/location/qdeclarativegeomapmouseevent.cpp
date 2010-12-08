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

#include "qdeclarativegeomapmouseevent_p.h"

QTM_BEGIN_NAMESPACE

QDeclarativeGeoMapMouseEvent::QDeclarativeGeoMapMouseEvent(QObject *parent)
    : QObject(parent),
      accepted_(false)
{
}

QDeclarativeGeoMapMouseEvent::~QDeclarativeGeoMapMouseEvent()
{
}

void QDeclarativeGeoMapMouseEvent::setAccepted(bool accepted)
{
    if (accepted_ == accepted)
        return;

    accepted_ = accepted;

    emit acceptedChanged(accepted_);
}

bool QDeclarativeGeoMapMouseEvent::accepted() const
{
    return accepted_;
}

void QDeclarativeGeoMapMouseEvent::setButtons(int buttons)
{
    if (buttons_ == buttons)
        return;

    buttons_ = buttons;

    emit buttonsChanged(buttons_);
}

int QDeclarativeGeoMapMouseEvent::buttons() const
{
    return buttons_;
}

void QDeclarativeGeoMapMouseEvent::setModifiers(int modifiers)
{
    if (modifiers == modifiers_)
        return;

    modifiers_ = modifiers;

    emit modifiersChanged(modifiers_);
}

int QDeclarativeGeoMapMouseEvent::modifiers() const
{
    return modifiers_;
}

void QDeclarativeGeoMapMouseEvent::setWasHeld(bool wasHeld)
{
    if (wasHeld_ == wasHeld)
        return;

    wasHeld_ = wasHeld;

    emit wasHeldChanged(wasHeld_);
}

bool QDeclarativeGeoMapMouseEvent::wasHeld() const
{
    return wasHeld_;
}

void QDeclarativeGeoMapMouseEvent::setX(int x)
{
    if (x_ == x)
        return;

    x_ = x;

    emit xChanged(x_);
}

int QDeclarativeGeoMapMouseEvent::x() const
{
    return x_;
}

void QDeclarativeGeoMapMouseEvent::setY(int y)
{
    if (y_ == y)
        return;

    y_ = y;

    emit yChanged(y_);
}

int QDeclarativeGeoMapMouseEvent::y() const
{
    return y_;
}

void QDeclarativeGeoMapMouseEvent::setCoordinate(QDeclarativeCoordinate *coordinate)
{
    if (coordinate_.coordinate() == coordinate->coordinate())
        return;

    coordinate_.setCoordinate(coordinate->coordinate());

    emit coordinateChanged(&coordinate_);
}

QDeclarativeCoordinate* QDeclarativeGeoMapMouseEvent::coordinate()
{
    return &coordinate_;
}


void QDeclarativeGeoMapMouseEvent::coordinateLatitudeChanged(double /*latitude*/)
{
    emit coordinateChanged(&coordinate_);
}

void QDeclarativeGeoMapMouseEvent::coordinateLongitudeChanged(double /*longitude*/)
{
    emit coordinateChanged(&coordinate_);
}

void QDeclarativeGeoMapMouseEvent::coordinateAltitudeChanged(double /*altitude*/)
{
    emit coordinateChanged(&coordinate_);
}

#include "moc_qdeclarativegeomapmouseevent_p.cpp"

QTM_END_NAMESPACE
