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

#include "qdeclarativegeomapobject_p.h"
#include "qdeclarativegeomapmousearea_p.h"

#include <QDeclarativeParserStatus>
#include <QDebug>

QTM_BEGIN_NAMESPACE

QDeclarativeGeoMapObject::QDeclarativeGeoMapObject(QDeclarativeItem *parent)
    : QDeclarativeItem(parent),
      object_(0),
      visible_(true) {}

QDeclarativeGeoMapObject::~QDeclarativeGeoMapObject() {}

void QDeclarativeGeoMapObject::componentComplete()
{
    QDeclarativeItem::componentComplete();

    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.size(); ++i) {
        QDeclarativeGeoMapMouseArea *mouseArea
                = qobject_cast<QDeclarativeGeoMapMouseArea*>(children.at(i));
        if (mouseArea)
            mouseAreas_.append(mouseArea);
    }
}

void QDeclarativeGeoMapObject::clickEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (event->accepted())
        return;

    for (int i = 0; i < mouseAreas_.size(); ++i) {
        mouseAreas_.at(i)->clickEvent(event);
        if (event->accepted())
            return;
    }
}

void QDeclarativeGeoMapObject::doubleClickEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (event->accepted())
        return;

    for (int i = 0; i < mouseAreas_.size(); ++i) {
        mouseAreas_.at(i)->doubleClickEvent(event);
        if (event->accepted())
            return;
    }
}

void QDeclarativeGeoMapObject::pressEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (event->accepted())
        return;

    for (int i = 0; i < mouseAreas_.size(); ++i) {
        mouseAreas_.at(i)->pressEvent(event);
        if (event->accepted())
            return;
    }
}

void QDeclarativeGeoMapObject::releaseEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (event->accepted())
        return;

    for (int i = 0; i < mouseAreas_.size(); ++i) {
        mouseAreas_.at(i)->releaseEvent(event);
        if (event->accepted())
            return;
    }
}

void QDeclarativeGeoMapObject::enterEvent()
{
    for (int i = 0; i < mouseAreas_.size(); ++i)
        mouseAreas_.at(i)->enterEvent();
}

void QDeclarativeGeoMapObject::exitEvent()
{
    for (int i = 0; i < mouseAreas_.size(); ++i)
        mouseAreas_.at(i)->exitEvent();
}

void QDeclarativeGeoMapObject::moveEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (event->accepted())
        return;

    for (int i = 0; i < mouseAreas_.size(); ++i) {
        mouseAreas_.at(i)->moveEvent(event);
        if (event->accepted())
            return;
    }
}

void QDeclarativeGeoMapObject::setMapObject(QGeoMapObject *object)
{
    if (!object)
        return;

    object_ = object;
    object_->setVisible(visible_);

    connect(this,
            SIGNAL(zChanged()),
            this,
            SLOT(parentZChanged()));

    object_->setZValue(zValue());
}

QGeoMapObject* QDeclarativeGeoMapObject::mapObject()
{
    return object_;
}

void QDeclarativeGeoMapObject::parentZChanged()
{
    object_->setZValue(zValue());
}

void QDeclarativeGeoMapObject::setVisible(bool visible)
{
    if (visible_ == visible)
        return;

    visible_ = visible;

    if (object_)
        object_->setVisible(visible);

    emit visibleChanged(visible_);
}

bool QDeclarativeGeoMapObject::isVisible() const
{
    return visible_;
}

#include "moc_qdeclarativegeomapobject_p.cpp"

QTM_END_NAMESPACE
