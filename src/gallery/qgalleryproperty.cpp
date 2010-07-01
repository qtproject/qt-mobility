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

#include "qgalleryproperty.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGalleryProperty

    \brief The QGalleryProperty class provides an identifier for a gallery
    property.
*/

/*!
    \enum QGalleryProperty::Attribute

    This enumeration describes the attributes of a property.

    \value CanRead The value of this property can be read.
    \value CanWrite The value of this property can be written.
    \value CanSort The value of this property can be used to sort items.
    \value CanFilter The value of this property can be used in a filter.
*/

/*!
    \fn QGalleryProperty::QGalleryProperty(const char *name)

    Constructs a property from a string literal \a name.
*/

/*!
    Returns the name of a property.
*/

QString QGalleryProperty::name() const
{
    return QString::fromLatin1(m_name, m_length);
}

/*!
    \fn QGalleryProperty::operator QString() const

    Returns the name of a property.
*/

/*!
    Returns a gallery filter which tests if  a property is less than a
    \a value.
*/

QGalleryMetaDataFilter QGalleryProperty::operator <(const QVariant &value) const
{
    return QGalleryMetaDataFilter(name(), value, QGalleryFilter::LessThan);
}

/*!
    Returns a gallery filter which tests if a property is less than or equal to
    a \a value.
*/

QGalleryMetaDataFilter QGalleryProperty::operator <=(const QVariant &value) const
{
    return QGalleryMetaDataFilter(name(), value, QGalleryFilter::LessThanEquals);
}

/*!
    Returns a gallery filter which tests if a property is equal to a \a value.
*/

QGalleryMetaDataFilter QGalleryProperty::operator ==(const QVariant &value) const
{
    return QGalleryMetaDataFilter(name(), value);
}

/*!
    Returns a gallery filter which tests if a property is greater than or equal
    to a \a value.
*/

QGalleryMetaDataFilter QGalleryProperty::operator >=(const QVariant &value) const
{
    return QGalleryMetaDataFilter(name(), value, QGalleryFilter::GreaterThanEquals);
}

/*!
    Returns a gallery filter which tests if a property is greater than a
    \a value.
*/

QGalleryMetaDataFilter QGalleryProperty::operator >(const QVariant &value) const
{
    return QGalleryMetaDataFilter(name(), value, QGalleryFilter::GreaterThan);
}

/*!
    Returns a sort key which indicating items should be sorted a property in
    ascending order.
*/

QString QGalleryProperty::ascending() const
{
    return QLatin1Char('+') + name();
}

/*!
    Returns a sort key which indicating items should be sorted a property in
    descending order.
*/

QString QGalleryProperty::descending() const
{
    return QLatin1Char('-') + name();
}

QTM_END_NAMESPACE
