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

#include "qgeomaprequestoptions.h"
#include "qgeomaprequestoptions_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QGeoMapRequestOptions
  \brief The QGeoMapRequestOptions class is.
  \ingroup maps


*/

/*!
    Constructs a QGeoMapRequestOptions object.
*/
QGeoMapRequestOptions::QGeoMapRequestOptions()
    : d_ptr(new QGeoMapRequestOptionsPrivate()) {}

/*!
    Constructs a QGeoMapRequestOptions object form \a other.
*/
QGeoMapRequestOptions::QGeoMapRequestOptions(const QGeoMapRequestOptions &other)
    : d_ptr(new QGeoMapRequestOptionsPrivate(*(other.d_ptr))) {}

/*!
    Destroys this QGeoMapRequestOptions object.
*/
QGeoMapRequestOptions::~QGeoMapRequestOptions()
{
    Q_D(QGeoMapRequestOptions);
    delete d;
}

/*!
    Assigns \a other to this QGeoMapRequestOptions object and returns a
    reference to this QGeoMapRequestOptions object.
*/
QGeoMapRequestOptions& QGeoMapRequestOptions::operator= (const QGeoMapRequestOptions &other)
{
    *d_ptr = *(other.d_ptr);

    return *this;
}

/*!
    Sets the map type for the request to \a mapType.

    \sa QGeoMapWidget::MapType
    \sa QGeoMapRequestOptions::mapType()
*/
void QGeoMapRequestOptions::setMapType(QGeoMapWidget::MapType mapType)
{
    Q_D(QGeoMapRequestOptions);
    d->mapType = mapType;
}

/*!
    Returns the map type which will be requested.

    \sa QGeoMapWidget::MapType
    \sa QGeoMapRequestOptions::setMapType()
*/
QGeoMapWidget::MapType QGeoMapRequestOptions::mapType() const
{
    Q_D(const QGeoMapRequestOptions);
    return d->mapType;
}

/*!
    Sets the image format for the request to \a imageFormat.

    \sa QGeoMapRequestOptions::imageFormat()
*/
void QGeoMapRequestOptions::setImageFormat(const QString &imageFormat)
{
    Q_D(QGeoMapRequestOptions);
    d->imageFormat = imageFormat;
}

/*!
    Returns the image format which will be requested.

    \sa QGeoMapRequestOptions::setImageFormat()
*/
QString QGeoMapRequestOptions::imageFormat() const
{
    Q_D(const QGeoMapRequestOptions);
    return d->imageFormat;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapRequestOptionsPrivate::QGeoMapRequestOptionsPrivate() :
        mapType(QGeoMapWidget::StreetMap),
        imageFormat("jpg") {}

QGeoMapRequestOptionsPrivate::QGeoMapRequestOptionsPrivate(const QGeoMapRequestOptionsPrivate &other)
    : mapType(other.mapType),
    imageFormat(other.imageFormat) {}

QGeoMapRequestOptionsPrivate::~QGeoMapRequestOptionsPrivate() {}

QGeoMapRequestOptionsPrivate& QGeoMapRequestOptionsPrivate::operator= (const QGeoMapRequestOptionsPrivate &other)
{
    mapType = other.mapType;
    imageFormat = other.imageFormat;

    return *this;
}

QTM_END_NAMESPACE
