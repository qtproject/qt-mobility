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
*/
QGeoMapObject::QGeoMapObject()
    : d_ptr(new QGeoMapObjectPrivate()) {}

/*!
*/
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
void QGeoMapObject::setZIndex(int zIndex)
{
    Q_D(QGeoMapObject);
    d->zIndex = zIndex;
}

/*!
*/
int QGeoMapObject::zIndex() const
{
    Q_D(const QGeoMapObject);
    return d->zIndex;
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
void QGeoMapObject::setBoundingBox(const QGeoBoundingBox& boundingBox)
{
    Q_D(QGeoMapObject);
    d->boundingBox = boundingBox;
}

/*!
*/
QGeoBoundingBox QGeoMapObject::boundingBox() const
{
    Q_D(const QGeoMapObject);
    return d->boundingBox;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapObjectPrivate::QGeoMapObjectPrivate() {}

QGeoMapObjectPrivate::QGeoMapObjectPrivate(const QGeoMapObjectPrivate &other)
    : type(other.type),
    zIndex(other.zIndex),
    isVisible(other.isVisible),
    boundingBox(other.boundingBox) {}

QGeoMapObjectPrivate::~QGeoMapObjectPrivate() {}

QGeoMapObjectPrivate& QGeoMapObjectPrivate::operator= (const QGeoMapObjectPrivate &other)
{
    type = other.type;
    zIndex = other.zIndex;
    isVisible = other.isVisible;
    boundingBox = other.boundingBox;

    return *this;
}

QTM_END_NAMESPACE
