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

#include "qlandmark.h"
#include "qlandmark_p.h"

#include "qlandmarkcategory_p.h"
#include "qlandmarkcategoryid.h"
#include "qlandmarkid.h"
#include "qlandmarkid_p.h"

#include "qgeoaddress.h"
#include "qgeocoordinate.h"

#include "qgeolocation.h"
#include "qgeolocation_p.h"

#include <QVariant>
#include <QStringList>

QTM_USE_NAMESPACE

// ----- QLandmarkPrivate -----

QLandmarkPrivate::QLandmarkPrivate()
        : QGeoLocationPrivate()
{
    type = QGeoLocation::LandmarkType;
    radius = -1.0;
}

QLandmarkPrivate::QLandmarkPrivate(const QLandmarkPrivate &other)
        : QGeoLocationPrivate(other),
        name(other.name),
        categoryIds(other.categoryIds),
        description(other.description),
        iconUrl(other.iconUrl),
        radius(other.radius),
        attributes(other.attributes),
        phone(other.phone),
        url(other.url),
        id(other.id)
{
}

QLandmarkPrivate::~QLandmarkPrivate() {}

QLandmarkPrivate& QLandmarkPrivate::operator= (const QLandmarkPrivate & other)
{
    QGeoLocationPrivate::operator =(other);
    name = other.name;
    description = other.description;
    iconUrl = other.iconUrl;
    radius = other.radius;
    phone = other.phone;
    url = other.url;
    categoryIds = other.categoryIds;
    attributes = other.attributes;
    id = other.id;

    return *this;
}

bool QLandmarkPrivate::operator== (const QLandmarkPrivate &other) const
{
    return (QGeoLocationPrivate::operator== (other)
            && (name == other.name)
            && (description == other.description)
            && (iconUrl == other.iconUrl)
            && (radius == other.radius)
            && (phone == other.phone)
            && (url == other.url)
            && (categoryIds == other.categoryIds)
            && (attributes == other.attributes)
           && (id == other.id));
}

/*!
    \class QLandmark
    \ingroup landmarks-main

    \brief The QLandmark class represents a location or point of interest
           of some significance.


    Each landmark consists of a number of properties such as name,
    coordinates, descriptoin etc.  Landmarks may also be assigned a set of
    generic attributes which may be accessed and modified by using the attribute()
    and setAttribute() functions.

    Each QLandmark may be associated with zero or more categories.
    A category  defines a type of landmark such as restaurant or
    cinema.  To set the category that a landmark belongs to, use
    the setCategoryId() or addCategoryId() functions.  A landmark may
    be removed from a category by using the removeCategoryId() function.

    Some landmarks may be designated as read-only, e.g. a publically accessible
    landmark server may not want some of its content to be editable.
    Localization is only possible for landmarks that are read-only.  If the
    landmark store supports localization, the locale may be set through a
    QLandmarkManager's parameters and whenever landmarks are retrieved,
    the translated names are used.  The \c {QLandmarkManager::isReadOnly(const QLandmarkyId &)}
    function may be used to determine if a category is read-only.

    Each QLandmark is an in memory representation of a landmark;
    it does not reflect the actual landmark state in persistent
    storage until the appropriate synchronization method is called
    on the QLandmarkManager(e.g. \l {QLandmarkManager::saveLandmark()} {saveLandmark()},
    \l {QLandmarkManager::removeLandmark()} {removeLandmark()}).
*/

/*!
    Constructs an new landmark.

    A new landmark will be assigned with invalid QLandmarkId.
*/
QLandmark::QLandmark()
        : QGeoLocation(new QLandmarkPrivate)
{
}

/*!
    Constructs a copy of \a other.
*/
QLandmark::QLandmark(const QLandmark &other)
        : QGeoLocation(other)
{
}

/*!
    Destroys the landmark.
*/
QLandmark::~QLandmark()
{
}

/*!
    Assigns \a other to this landmark and returns a reference to this landmark.
*/
QLandmark &QLandmark::operator= (const QLandmark & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

inline QLandmarkPrivate* QLandmark::d_func()
{
    return reinterpret_cast<QLandmarkPrivate*>(d_ptr.data());
}

inline const QLandmarkPrivate* QLandmark::d_func() const
{
    return reinterpret_cast<const QLandmarkPrivate*>(d_ptr.constData());
}

/*!
    Returns true if this landmark is equal to \a other, otherwise
    returns false.

    Two landmarks are considered equal if both the landmark details
    and identifiers are equal.

    \sa operator!=()
*/
bool QLandmark::operator== (const QLandmark &other) const
{
    Q_D(const QLandmark);
    return *d == *(other.d_func());
}

/*!
    \fn bool QLandmark::operator!= (const QLandmark &other) const

    Returns true if this landmark not is equal to \a other, otherwise
    returns false.

    \sa operator==()
*/

/*!
    Returns the name of the landmark.
*/
QString QLandmark::name() const
{
    Q_D(const QLandmark);
    return d->name;
}

/*!
    Sets the \a name of the landmark.
*/
void QLandmark::setName(const QString &name)
{
    Q_D(QLandmark);
    d->name = name;
}

/*!
    Returns a of list identifiers of categories that this landmark
    belongs to.

    \sa setCategoryIds()
*/
QList<QLandmarkCategoryId> QLandmark::categoryIds() const
{
    Q_D(const QLandmark);
    return d->categoryIds;
}

/*!
    Sets the categories that this landmark belongs to via
    a list of \a categoryIds.

    \sa addCategoryId(), removeCategoryId()

*/
void QLandmark::setCategoryIds(const QList<QLandmarkCategoryId> &categoryIds)
{
    Q_D(QLandmark);
    d->categoryIds.clear();

    // remove duplicates
    for (int i = 0; i < categoryIds.size(); ++i) {
        if (!d->categoryIds.contains(categoryIds.at(i)))
            d->categoryIds.append(categoryIds.at(i));
    }
}

/*!
    Adds another category that this landmark will be associated
    with via its \a categoryId.

    \sa setCategoryIds(), removeCategoryId()
*/
void QLandmark::addCategoryId(const QLandmarkCategoryId &categoryId)
{
    Q_D(QLandmark);
    if (!d->categoryIds.contains(categoryId))
        d->categoryIds.append(categoryId);
}

/*!
    Removes a category from a landmark, by using its \a categoryId.

    \sa addCategoryId(), categoryIds()
*/
void QLandmark::removeCategoryId(const QLandmarkCategoryId &categoryId)
{
    Q_D(QLandmark);
    d->categoryIds.removeAll(categoryId);
}

/*!
    Returns a description of the landmark.
*/
QString QLandmark::description() const
{
    Q_D(const QLandmark);
    return d->description;
}

/*!
    Sets the \a description of the landmark.
*/
void QLandmark::setDescription(const QString &description)
{
    Q_D(QLandmark);
    d->description = description;
}

/*!
    Returns the url of the landmark's icon.
*/
QUrl QLandmark::iconUrl() const
{
    Q_D(const QLandmark);
    return d->iconUrl;
}

/*!
    Sets the \a url of the landmark's icon.
*/
void QLandmark::setIconUrl(const QUrl &url)
{
    Q_D(QLandmark);
    d->iconUrl = url;
}

/*!
    Returns the coverage radius of the landmark.

    The coverage radius is relevant for large landmarks
    such as cities.  Note that landmark searches over a given area
    do not factor in the coverage radius.
*/
double QLandmark::radius() const
{
    Q_D(const QLandmark);
    return d->radius;
}

/*!
    Sets the coverage \a radius of the landmark.
*/
void QLandmark::setRadius(double radius)
{
    Q_D(QLandmark);
    d->radius = radius;
}

/*!
    Returns the value of the attribute corresponding to \a key.
    If the attribute doest exist, returns \a defaultValue.

    If no default value is specified, a default QVariant is returned.
*/
QVariant QLandmark::attribute(const QString &key, const QVariant &defaultValue) const
{
    Q_D(const QLandmark);
    return d->attributes.value(key, defaultValue);
}

/*!
    Sets the \a value of the attribute corresponding to \a key.
*/
void QLandmark::setAttribute(const QString &key, const QVariant &value)
{
    Q_D(QLandmark);
    d->attributes[key] = value;
}

/*!
    Returns a list of attribute keys.

    \sa attribute(), setAttribute()
*/
QStringList QLandmark::attributeKeys() const
{
    Q_D(const QLandmark);
    return d->attributes.keys();
}

/*!
    Returns the phone number of the landmark.
*/
QString QLandmark::phone() const
{
    Q_D(const QLandmark);
    return d->phone;
}

/*!
    Sets the \a phone number of the landmark.
*/
void QLandmark::setPhone(const QString &phone)
{
    Q_D(QLandmark);
    d->phone = phone;
}
/*!
    Returns the url of the landmark.
*/
QUrl QLandmark::url() const
{
    Q_D(const QLandmark);
    return d->url;
}

/*!
    Sets the \a url of the landmark.
*/
void QLandmark::setUrl(const QUrl &url)
{
    Q_D(QLandmark);
    d->url = url;
}

/*!
    Returns the identifier of the landmark.
*/
QLandmarkId QLandmark::landmarkId() const
{
    Q_D(const QLandmark);
    return d->id;
}

/*!
    Sets the \a id of the landmark.

    Note that saving a new landmark using a QLandmarkManager
    will automatically assign the landmark a valid identifier.
*/
void QLandmark::setLandmarkId(const QLandmarkId &id)
{
    Q_D(QLandmark);
    d->id = id;
}
