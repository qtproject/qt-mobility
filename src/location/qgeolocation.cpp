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

#include "qgeolocation.h"
#include "qgeolocation_p.h"


QTM_BEGIN_NAMESPACE

/*!
    \class QGeoLocation
    \brief The QGeoLocation class represents a geolocation.
    \ingroup location

    This class represent a geo location as returned by QGeocodingReply::places().
*/

/*!
    Default constructor.
*/
QGeoLocation::QGeoLocation()
    : d_ptr(new QGeoLocationPrivate()) {}

/*!
    Copy constructor.
*/

QGeoLocation::QGeoLocation(const QGeoLocation &gl)
    : d_ptr(new QGeoLocationPrivate(*(gl.d_ptr))) {}

/*!
    Assignment operator.
*/
QGeoLocation& QGeoLocation::operator=(const QGeoLocation &gl)
{
    *d_ptr = *(gl.d_ptr);
    return *this;
}

/*!
    Destructor.
*/
QGeoLocation::~QGeoLocation()
{
    Q_D(QGeoLocation);
    delete d;
}

/*!
    Returns the bounding box that completely encloses the location.

    The x coordinates of the corner points represent longitudes,
    the y coordinates represent latitudes.
*/
QRectF QGeoLocation::boundingBox() const
{
    Q_D(const QGeoLocation);
    return d->boundingBox;
}

void QGeoLocation::setBoundingBox(const QRectF &boundingBox)
{
    Q_D(QGeoLocation);
    d->boundingBox = boundingBox;
}

/*!
    Returns the geocoordinate of this location.
*/
QGeoCoordinate QGeoLocation::position() const
{
    Q_D(const QGeoLocation);
    return d->position;
}

void QGeoLocation::setPosition(const QGeoCoordinate &position)
{
    Q_D(QGeoLocation);
    d->position = position;
}

/*!
    Returns a description of the location.
*/
QString QGeoLocation::title() const
{
    Q_D(const QGeoLocation);
    return d->title;
}

void QGeoLocation::setTitle(const QString &title)
{
    Q_D(QGeoLocation);
    d->title = title;
}

/*!
    Returns the MARC language used in the description of this location.
*/
QString QGeoLocation::language() const
{
    Q_D(const QGeoLocation);
    return d->language;
}

void QGeoLocation::setLanguage(const QString &language)
{
    Q_D(QGeoLocation);
    d->language = language;
}

/*!
    Returns the address found.
*/
QGeoAddress QGeoLocation::address() const
{
    Q_D(const QGeoLocation);
    return d->address;
}

void QGeoLocation::setAddress(const QGeoAddress &address)
{
    Q_D(QGeoLocation);
    d->address = address;
}

/*******************************************************************************
*******************************************************************************/

QGeoLocationPrivate::QGeoLocationPrivate() {}

QGeoLocationPrivate::QGeoLocationPrivate(const QGeoLocationPrivate &glp)
    : boundingBox(glp.boundingBox),
    position(glp.position),
    title(glp.title),
    language(glp.language),
    address(glp.address) {}

QGeoLocationPrivate& QGeoLocationPrivate::operator=(const QGeoLocationPrivate &glp)
{
    boundingBox = glp.boundingBox;
    position = glp.position;
    title = glp.title;
    language = glp.language;
    address = glp.address;
    return *this;
}

QTM_END_NAMESPACE

