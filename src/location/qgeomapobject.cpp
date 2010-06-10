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

#include "qgeomapobject.h"
#include "qgeomapobject_p.h"

QTM_BEGIN_NAMESPACE

/*!
        enum Type {
            ContainerType,
            RectangleType,
            CircleType,
            PolylineType,
            PolygonType,
            MarkerType,
        };
*/

/*!
*/
QGeoMapObject::QGeoMapObject(QGeoMapObject *parent)
    : d_ptr(new QGeoMapObjectPrivate(this, parent)) {}

QGeoMapObject::QGeoMapObject(QGeoMapObjectPrivate *dd)
    : d_ptr(dd) {}

/*!
*/
QGeoMapObject::~QGeoMapObject()
{
    Q_D(QGeoMapObject);
    delete d;
}

/*!
*/
QGeoMapObject::Type QGeoMapObject::type() const
{
    Q_D(const QGeoMapObject);
    return d->type;
}

/*!
*/
void QGeoMapObject::setZValue(int zValue)
{
    Q_D(QGeoMapObject);
    d->zValue = zValue;
}

/*!
*/
int QGeoMapObject::zValue() const
{
    Q_D(const QGeoMapObject);
    return d->zValue;
}

/*!
*/
void QGeoMapObject::setVisible(bool visible)
{
    Q_D(QGeoMapObject);
    d->isVisible = visible;
}

/*!
*/
bool QGeoMapObject::isVisible() const
{
    Q_D(const QGeoMapObject);
    return d->isVisible;
}

/*!
*/
QGeoBoundingBox QGeoMapObject::boundingBox() const
{
    Q_D(const QGeoMapObject);

    QGeoBoundingBox bounds;

    if (d->children.size() == 0)
        return bounds;

    bounds = d->children.at(0)->boundingBox();

    for (int i = 1; i < d->children.size(); ++i)
        bounds = bounds.united(d->children.at(i)->boundingBox());

    return bounds;
}

/*!
*/
bool QGeoMapObject::contains(const QGeoCoordinate &coordinate) const
{
    Q_D(const QGeoMapObject);

    if (d->children.size() == 0)
        return false;

    for (int i = 0; i < d->children.size(); ++i)
        if (d->children.at(i)->contains(coordiante))
            return true;

    return false;
}

/*!
*/
QGeoMapObject* QGeoMapObject::parentObject() const
{
    Q_D(const QGeoMapObject);
    return d->parent;
}

/*!
*/
void QGeoMapObject::addChildObject(QGeoMapObject *childObject)
{
    Q_D(QGeoMapObject);

    // TODO check if already added?

    if (childObject)
        d->children.append(childObject);
}

/*!
*/
void QGeoMapObject::removeChildObject(QGeoMapObject *childObject)
{
    Q_D(QGeoMapObject);
    if (childObject)
        d->children.removeAll(childObject);
}

/*!
*/
QList<QGeoMapObject*> QGeoMapObject::childObjects() const
{
    Q_D(const QGeoMapObject);
    return d->children;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapObjectPrivate::QGeoMapObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
    : type(QGeoMapObject::ContainerType),
    parent(parent),
    q_ptr(impl) {}

QGeoMapObjectPrivate::QGeoMapObjectPrivate(const QGeoMapObjectPrivate &other)
    : type(other.type),
    parent(other.parent),
    children(other.children),
    zValue(other.zValue),
    isVisible(other.isVisible),
    boundingBox(other.boundingBox),
    q_ptr(other.q_ptr) {}

QGeoMapObjectPrivate::~QGeoMapObjectPrivate() {
    if (parent) {
        Q_Q(QGeoMapObject);
        parent->removeChildObject(q);
    }

    for (int i = 0; i < children.size(); ++i) {
        children.at(i)->d_ptr->parent = 0;
        delete children.at(i);
    }

    children.clear();
}

QGeoMapObjectPrivate& QGeoMapObjectPrivate::operator= (const QGeoMapObjectPrivate &other)
{
    type = other.type;
    parent = other.parent;
    children = other.children;
    zValue = other.zValue;
    isVisible = other.isVisible;
    boundingBox = other.boundingBox;
    q_ptr = other.q_ptr;

    return *this;
}

QTM_END_NAMESPACE
