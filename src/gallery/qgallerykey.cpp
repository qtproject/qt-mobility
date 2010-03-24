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

#include "qgallerykey.h"


/*!
    \class QGalleryKey

    \brief The QGalleryKey class provides an identifier for a gallery
    property key.
*/

/*!
    \fn QGalleryKey::QGalleryKey(const char (&id)[N])

    Constructs a property key from a string literal \a id.
*/

/*!
    Returns the id string of a propery key.
*/

QString QGalleryKey::toString() const
{
    return QString::fromLatin1(m_id, m_length);
}

/*!
    \fn QGalleryType::operator QString() const

    Returns the id string of a property key.
*/

/*!
    Returns a gallery filter which tests if a property matches a \a value.
    The match \a flags identify the criteria for a match.
*/

QGalleryMetaDataFilter QGalleryKey::matches(const QVariant &value, Qt::MatchFlags flags)
{
    return QGalleryMetaDataFilter(toString(), value, flags);
}

/*!
    Returns a gallery filter which tests if a property is in a range between
    a \a minimum and \a maximum value.
*/

QGalleryMetaDataRangeFilter QGalleryKey::isInRange(
        const QVariant &minimum, const QVariant &maximum) const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setFieldName(toString());
    filter.setInclusiveRange(minimum, maximum);
    return filter;
}

/*!
    Returns a gallery filter which tests if  a property is less than a
    \a value.
*/

QGalleryMetaDataRangeFilter QGalleryKey::operator <(const QVariant &value) const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setFieldName(toString());
    filter.setLessThan(value);
    return filter;
}

/*!
    Returns a gallery filter which tests if a property is less than or equal to
    a \a value.
*/

QGalleryMetaDataRangeFilter QGalleryKey::operator <=(const QVariant &value) const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setFieldName(toString());
    filter.setLessThanEquals(value);
    return filter;
}

/*!
    Returns a gallery filter which tests if a property is equal to a \a value.
*/

QGalleryMetaDataFilter QGalleryKey::operator ==(const QVariant &value) const
{
    return QGalleryMetaDataFilter(toString(), value);
}

/*!
    Returns a gallery filter which tests if a property is greater than or equal
    to a \a value.
*/

QGalleryMetaDataRangeFilter QGalleryKey::operator >=(const QVariant &value) const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setFieldName(toString());
    filter.setGreaterThanEquals(value);
    return filter;
}

/*!
    Returns a gallery filter which tests if a property is greater than a
    \a value.
*/

QGalleryMetaDataRangeFilter QGalleryKey::operator >(const QVariant &value) const
{
    QGalleryMetaDataRangeFilter filter;
    filter.setFieldName(toString());
    filter.setGreaterThan(value);
    return filter;
}
