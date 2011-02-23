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

#include "qdeclarativegeomapmousearea_p.h"

#include <QDebug>

#include "qdeclarativegeomapobject_p.h"

QTM_BEGIN_NAMESPACE

QDeclarativeGeoMapMouseArea::QDeclarativeGeoMapMouseArea(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
}

QDeclarativeGeoMapMouseArea::~QDeclarativeGeoMapMouseArea()
{
}

void QDeclarativeGeoMapMouseArea::setContainsMouse(bool containsMouse)
{
    if (containsMouse_ == containsMouse)
        return;

    containsMouse_ = containsMouse;

    emit containsMouseChanged(containsMouse_);
}

bool QDeclarativeGeoMapMouseArea::containsMouse() const
{
    return containsMouse_;
}

void QDeclarativeGeoMapMouseArea::setEnabled(bool enabled)
{
    if (enabled_ == enabled)
        return;

    enabled_ = enabled;

    emit enabledChanged(enabled_);
}

bool QDeclarativeGeoMapMouseArea::enabled() const
{
    return enabled_;
}

void QDeclarativeGeoMapMouseArea::setHoverEnabled(bool hoverEnabled)
{
    if (hoverEnabled_ == hoverEnabled)
        return;

    hoverEnabled_ = hoverEnabled;

    emit hoverEnabledChanged(hoverEnabled_);
}

bool QDeclarativeGeoMapMouseArea::hoverEnabled() const
{
    return hoverEnabled_;
}

void QDeclarativeGeoMapMouseArea::setMouseX(qreal mouseX)
{
    if (mouseX_ == mouseX)
        return;

    mouseX_ = mouseX;

    emit mouseXChanged(mouseX);
}

qreal QDeclarativeGeoMapMouseArea::mouseX() const
{
    return mouseX_;
}

void QDeclarativeGeoMapMouseArea::setMouseY(qreal mouseY)
{
    if (mouseY_ == mouseY)
        return;

    mouseY_ = mouseY;

    emit mouseYChanged(mouseY_);
}

qreal QDeclarativeGeoMapMouseArea::mouseY() const
{
    return mouseY_;
}

void QDeclarativeGeoMapMouseArea::setPressed(bool pressed)
{
    if (pressed_ == pressed)
        return;

    pressed_ = pressed;

    emit pressedChanged(pressed_);
}

bool QDeclarativeGeoMapMouseArea::pressed() const
{
    return pressed_;
}

void QDeclarativeGeoMapMouseArea::setAcceptedButtons(Qt::MouseButtons acceptedButtons)
{
    if (acceptedButtons_ == acceptedButtons)
        return;

    acceptedButtons_ = acceptedButtons;

    emit acceptedButtonsChanged(acceptedButtons_);
}

Qt::MouseButtons QDeclarativeGeoMapMouseArea::acceptedButtons() const
{
    return acceptedButtons_;
}

void QDeclarativeGeoMapMouseArea::setPressedButtons(Qt::MouseButtons pressedButtons)
{
    if (pressedButtons_ == pressedButtons)
        return;

    pressedButtons_ = pressedButtons;

    emit pressedButtonsChanged(pressedButtons_);
}

Qt::MouseButtons QDeclarativeGeoMapMouseArea::pressedButtons() const
{
    return pressedButtons_;
}

void QDeclarativeGeoMapMouseArea::clickEvent(QDeclarativeGeoMapMouseEvent *event)
{
    emit clicked(event);
}

void QDeclarativeGeoMapMouseArea::doubleClickEvent(QDeclarativeGeoMapMouseEvent *event)
{
    emit doubleClicked(event);
}

void QDeclarativeGeoMapMouseArea::pressEvent(QDeclarativeGeoMapMouseEvent *event)
{
    emit pressed(event);
}

void QDeclarativeGeoMapMouseArea::releaseEvent(QDeclarativeGeoMapMouseEvent *event)
{
    emit released(event);
}

void QDeclarativeGeoMapMouseArea::enterEvent()
{
    emit entered();
}

void QDeclarativeGeoMapMouseArea::exitEvent()
{
    emit exited();
}

void QDeclarativeGeoMapMouseArea::moveEvent(QDeclarativeGeoMapMouseEvent *event)
{
    emit positionChanged(event);
}

#include "moc_qdeclarativegeomapmousearea_p.cpp"

QTM_END_NAMESPACE
