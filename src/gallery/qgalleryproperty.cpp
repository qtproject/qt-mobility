/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


/*!
    \class QGalleryProperty

    \brief The QGalleryProperty class provides an identifier for a gallery
    property.
*/

/*!
    \fn QGalleryProperty::QGalleryProperty(const char (&name)[N])

    Constructs a property from a string literal \a name.
*/

/*!
    Returns the name of a propery.
*/

QString QGalleryProperty::name() const
{
    return QString::fromLatin1(m_name, m_length);
}

/*!
    \fn QGalleryType::operator QString() const

    Returns the name of a property.
*/

/*!
    Returns a gallery filter which tests if a property matches a \a value.
    The match \a flags identify the criteria for a match.
*/

QGalleryMetaDataFilter QGalleryProperty::matches(const QVariant &value, Qt::MatchFlags flags)
{
    return QGalleryMetaDataFilter(name(), value, flags);
}

/*!
    Returns a gallery filter which tests if a property is in a range between
    a \a minimum and \a maximum value.
*/

QGalleryMetaDataRangeFilter QGalleryProperty::isInRange(
        const QVariant &minimum, const QVariant &maximum) const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setFieldName(name());
    filter.setInclusiveRange(minimum, maximum);
    return filter;
}

/*!
    Returns a gallery filter which tests if  a property is less than a
    \a value.
*/

QGalleryMetaDataRangeFilter QGalleryProperty::operator <(const QVariant &value) const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setFieldName(name());
    filter.setLessThan(value);
    return filter;
}

/*!
    Returns a gallery filter which tests if a property is less than or equal to
    a \a value.
*/

QGalleryMetaDataRangeFilter QGalleryProperty::operator <=(const QVariant &value) const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setFieldName(name());
    filter.setLessThanEquals(value);
    return filter;
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

QGalleryMetaDataRangeFilter QGalleryProperty::operator >=(const QVariant &value) const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setFieldName(name());
    filter.setGreaterThanEquals(value);
    return filter;
}

/*!
    Returns a gallery filter which tests if a property is greater than a
    \a value.
*/

QGalleryMetaDataRangeFilter QGalleryProperty::operator >(const QVariant &value) const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setFieldName(name());
    filter.setGreaterThan(value);
    return filter;
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

