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

#include <QVariant>
#include <QStringList>

QTM_USE_NAMESPACE

/*!
    \class QLandmarkCategoryId
    \ingroup location

    \brief The QLandmarkCategoryId class provides a unique identifier for
    a category.

    It consists of a manager URI which identifies the manager which contains
    the category, and the id for that category.
*/

/*!

    Creates an invalid identifier, id() and managerUri() will return null strings
*/
QLandmarkCategoryId::QLandmarkCategoryId()
{
}

/*!
    Constructs a copy of \a other.
*/
QLandmarkCategoryId::QLandmarkCategoryId(const QLandmarkCategoryId &other)
    :d(other.d)
{
}

/*!
    Destroys the identifier.
*/
QLandmarkCategoryId::~QLandmarkCategoryId()
{
}

/*!
    Returns true if the identifier is valid, meaning that both
    a manager URI and id have been set
*/
bool QLandmarkCategoryId::isValid() const
{
}

/*!
    Returns the id of the category identifier
*/
QString QLandmarkCategoryId::id() const
{
}

/*!
    Sets the \a id of the category identifier
*/
void QLandmarkCategoryId::setId(const QString &id)
{
}

/*!
    Returns the URI of the manager which this category identifer
    belongs to.
*/
QString QLandmarkCategoryId::managerUri() const
{
}

/*!
    Sets the manager \a uri which this category identifier
    belongs to.
*/
void QLandmarkCategoryId::setManagerUri(const QString &uri)
{
}

/*!
    Assigns \a other to this category identifier and returns a reference
    to this category identifier.
*/
QLandmarkCategoryId &QLandmarkCategoryId::operator=(const QLandmarkCategoryId &other)
{
}

/*!
    Returns true if this category identifer is equal to \a other, otherwise
    returns false.

    Two QLandmarkCategoryIds are considered equal if both have the same
    manager URI and id.
*/
bool QLandmarkCategoryId::operator==(const QLandmarkCategoryId &other) const
{
}

/*!
    Returns true if this category identifier is not equal to \a other,
    otherwise returns false.

    If either the manager URIs or ids differ, then the
    QLandmarkCategoryIds are not considered equal.
*/
bool QLandmarkCategoryId::operator!=(const QLandmarkCategoryId &other) const
{
}

// ----- QLandmarkCategoryPrivate -----

QLandmarkCategoryPrivate::QLandmarkCategoryPrivate() {}

QLandmarkCategoryPrivate::QLandmarkCategoryPrivate(const QLandmarkCategoryPrivate &other)
{
    name = other.name;
    icon = other.icon;
    description = other.description;
}

QLandmarkCategoryPrivate::~QLandmarkCategoryPrivate() {}

QLandmarkCategoryPrivate& QLandmarkCategoryPrivate::operator= (const QLandmarkCategoryPrivate & other)
{
    name = other.name;
    icon = other.icon;
    description = other.description;

    return *this;
}

bool QLandmarkCategoryPrivate::operator== (const QLandmarkCategoryPrivate &other) const
{
    return ((name == other.name)
            && (icon == other.icon)
            && (description == other.description));
}

// ----- QLandmarkCategory -----
/*!
    \class QLandmarkCategory

    \brief The QLandmarkCategory class designates a grouping of
    landmarks of similar type.

    \ingroup location

    Landmarks of similar type may be grouped together into categories,
    e.g. restaurants, accommodation etc.  A QLandmarkCategory object
    represents one of these and allows access to category properties
    such it's description and icon.

    Note that more than one category can be assigned
    to a landmark.  (This is achieved by using the QLandmark and
    QLandmarkCategoryId classes).

    A QLandmarkCategory instance is an in memory representation and may
    not reflect the state of the category found in persistent storage
    until the appropriate synchronization method is called on the
    QLandmarkManager(i.e. saveCategory, removeCategory).
*/

/*!
    Constructs an empty category.
*/
QLandmarkCategory::QLandmarkCategory()
{
    d = new QLandmarkCategoryPrivate();
}

/*!
    Constructs a copy of \a other.
*/
QLandmarkCategory::QLandmarkCategory(const QLandmarkCategory &other)
{
    d = new QLandmarkCategoryPrivate(*(other.d));
}

/*!
    Destroys the category.
*/
QLandmarkCategory::~QLandmarkCategory()
{
    delete d;
}
/*!
    Assigns \a other to this category and returns a reference to this category.
*/
QLandmarkCategory& QLandmarkCategory::operator= (const QLandmarkCategory & other)
{
    *d = *(other.d);
    return *this;
}

/*!
    Returns true if this category is equal to \a other, otherwise
    returns false.
*/
bool QLandmarkCategory::operator== (const QLandmarkCategory &other) const
{
    return (*d == *(other.d));
}

/*!
    Returns the name of the category
*/
QString QLandmarkCategory::name() const
{
    return d->name;
}

/*!
    Sets the \a name of the category
*/
void QLandmarkCategory::setName(const QString &name){
    d->name = name;
}

/*!
    Returns the filepath of the category icon
*/
QString QLandmarkCategory::icon() const
{
    return d->icon;
}

/*!
    Sets the \a filePath of the category icon
*/
void QLandmarkCategory::setIcon(const QString &filePath)
{
    d->icon = filePath;
}

/*!
    Returns the description of the category
*/
QString QLandmarkCategory::description() const
{
    return d->description;
}

/*!
    Sets the \a description of the category
*/
void QLandmarkCategory::setDescription(const QString &description)
{
    d->description = description;
}

/*!
    Returns whether the category is read-only.
*/
bool QLandmarkCategory::isReadOnly() const
{
}

/*!
    Returns the category identifier of the category.
*/
QLandmarkCategoryId QLandmarkCategory::categoryId() const
{
}

/*!
    Returns the value of the attribute corresponding to \a attributeName.
*/
QVariant QLandmarkCategory::attribute(const QString &attributeName) const
{
    return QVariant();
}

/*!
    Sets the \a value of the attribute corresponding to \a attributeName.
*/
void QLandmarkCategory::setAttribute(const QString &attributeName, const QVariant &value)
{

}

/*!
    Returns a list of attribute names.
*/
QStringList QLandmarkCategory::attributes() const
{
    return QStringList();
}

// ---- QLandmarkId ----
/*!
    \class QLandmarkId
    \ingroup location

    \brief The QLandmarkId class provides a unique identifier for a
    landmark.

    It consists of a manager URI which identifies the manager which
    contains the landmark, and the id for that landmark.
*/

/*!
    Creates an invalid identifier, id() and managerUri() will return null
    strings.
*/
QLandmarkId::QLandmarkId()
{
}

/*!
    Constructs a copy of \a other.
*/
QLandmarkId::QLandmarkId(const QLandmarkId &other)
{
}

/*!
    Destroys the identifier.
*/
QLandmarkId::~QLandmarkId()
{
}

/*!
    Returns true if the identifier is valid, meaning that both
    a mangaer URI and id have been set.
*/
bool QLandmarkId::isValid() const
{
}

/*!
    Returns the id of the landmark identifier.
*/
QString QLandmarkId::id()
{
}

/*!
    Sets the \a id of the landmark identifier.
*/
void QLandmarkId::setId(const QString &id)
{
}

/*!
    Returns the URI of the manager which this landmark identifier
    belongs to.
*/
QString QLandmarkId::managerUri() const
{
}

/*!
    Sets the manager \a uri which this landmark identifier
    belongs to.
*/
void QLandmarkId::setManagerUri(const QString &uri)
{
}

/*!
    Assigns \a other to this landmark identifier and returns a
    reference to this landmark identifier.
*/
QLandmarkId &QLandmarkId::operator=(const QLandmarkId &other)
{
}

/*!
    Returns true if this landmark identifier is equal to \a other,
    otherwise returnse false.

    Two QLandmarks are considered equal if both have the same
    manager URI and id.
*/
bool QLandmarkId::operator==(const QLandmarkId &other) const
{
}

/*!
    Returns true if this landmark identifier is not equal to \a other,
    otherwise returns false.

    If either the manager URIs or ids differ, then the
    QLandmarkIds are not considered equal.
*/
bool QLandmarkId::operator!=(const QLandmarkId &other) const
{
}

// ----- QLandmarkPrivate -----

QLandmarkPrivate::QLandmarkPrivate() {}

QLandmarkPrivate::QLandmarkPrivate(const QLandmarkPrivate &other)
{
    name = other.name;
    coordinate = other.coordinate;
    description = other.description;
    icon = other.icon;
    radius = other.radius;
    street = other.street;
    locality = other.locality;
    region = other.region;
    country = other.country;
    postcode = other.postcode;
    phone = other.phone;
    url = other.url;
    categories = other.categories;
    attributes = other.attributes;
}

QLandmarkPrivate::~QLandmarkPrivate() {}

QLandmarkPrivate& QLandmarkPrivate::operator= (const QLandmarkPrivate & other)
{
    name = other.name;
    coordinate = other.coordinate;
    description = other.description;
    icon = other.icon;
    radius = other.radius;
    street = other.street;
    locality = other.locality;
    region = other.region;
    country = other.country;
    postcode = other.postcode;
    phone = other.phone;
    url = other.url;
    categories = other.categories;
    attributes = other.attributes;

    return *this;
}

bool QLandmarkPrivate::operator== (const QLandmarkPrivate &other) const
{
    return ((name == other.name)
            && (coordinate == other.coordinate)
            && (description == other.description)
            && (icon == other.icon)
            && (radius == other.radius)
            && (street == other.street)
            && (locality == other.locality)
            && (region == other.region)
            && (country == other.country)
            && (postcode == other.postcode)
            && (phone == other.phone)
            && (url == other.url)
            && (categories == other.categories)
            && (attributes == other.attributes));
}

/*!
    \class QLandmark
    \ingroup location

    \brief The QLandmark class describes location or point of interest

    It consists of a number of set properties such as name and coordinates
    as well as a set of generic attributes.

    Each QLandmark may be associated with zero or more categories.
    Categories are manipulated by using QLandmarkCategoryId objects via
    the setCategories(), addCategory() and removeCategory() functions.
*/

/*!
    Constructs an empty landmark.
*/
QLandmark::QLandmark()
{
    d = new QLandmarkPrivate();
}

/*!
    Constructs a copy of \a other.
*/
QLandmark::QLandmark(const QLandmark &other)
{
    d = new QLandmarkPrivate(*(other.d));
}

/*!
    Destroys the landmark.
*/
QLandmark::~QLandmark()
{
    delete d;
}

/*!
    Assigns \a other to this landmark and returns a reference to this landmark.
*/
QLandmark& QLandmark::operator= (const QLandmark & other)
{
    *d = *(other.d);
    return *this;
}

/*!
    Returns true if this landmark is equal to \a other, otherwise
    returns false.
*/
bool QLandmark::operator== (const QLandmark &other) const
{
    return (*d == *(other.d));
}

/*!
    Returns the name of the landmark.
*/
QString QLandmark::name() const
{
    return d->name;
}

/*!
    Sets the \a name of the landmark.
*/
void QLandmark::setName(const QString &name)
{
    d->name = name;
}

/*!
    Returns the coordinate of the landmark.
*/
QGeoCoordinate QLandmark::coordinate() const
{
    return d->coordinate;
}

/*!
    Sets the \a coordinate of the landmark.

    The coordinates should be specified in the WGS84 datum.
*/
void QLandmark::setCoordinate(const QGeoCoordinate& coordinate)
{
    d->coordinate = coordinate;
}


/*!
    Returns a of list identifiers of categories that this landmark
    belongs to.
*/
QList<QLandmarkCategoryId> QLandmark::categories() const
{
    return d->categories;
}

/*!
    Sets the categories that this landmark belongs to via
    a list of \a categoryIds.

*/
void QLandmark::setCategories(const QList<QLandmarkCategoryId> &categoryIds)
{
    // remove duplicates
    d->categories.clear();
    for (int i = 0; i < categoryIds.size(); ++i) {
        if (!d->categories.contains(categoryIds.at(i)))
            d->categories.append(categoryIds.at(i));
    }
}

/*!
    Adds another category that this landmark will be associated
    with via its \a categoryId.
*/
void QLandmark::addCategory(const QLandmarkCategoryId &categoryId)
{
    if (!d->categories.contains(categoryId))
        d->categories.append(categoryId);
}

/*!
    Removes a category that this landmark will no longer
    be associated with via its \a categoryId.
*/
void QLandmark::removeCategory(const QLandmarkCategoryId &categoryId)
{
    d->categories.removeAll(categoryId);
}

/*!
    Returns a description of the landmark.
*/
QString QLandmark::description() const
{
    return d->description;
}

/*!
    Sets the \a description of the landmark.
*/
void QLandmark::setDescription(const QString &description)
{
    d->description = description;
}

/*!
    Returns the file path of the landmark's icon.
*/
QString QLandmark::icon() const
{
    return d->icon;
}

/*!
    Sets the \a filePath of the landmark's icon.
*/
void QLandmark::setIcon(const QString &filePath)
{
    d->icon = filePath;
}

/*!
    Returns the coverage radius of the landmark.

    The coverage radius is relevant for large landmarks
    such as cities.
*/
double QLandmark::radius() const
{
    return d->radius;
}

/*!
    Sets the coverage \a radius of the landmark.
*/
void QLandmark::setRadius(double radius)
{
    d->radius = radius;
}

/*!
    Returns the value of the attribute corresponding to \a attributeName.
*/
QVariant QLandmark::attribute(const QString &attributeName) const
{
    return d->attributes.value(attributeName);
}

/*!
    Sets the \a value of the attribute corresponding to \a attributeName.
*/
void QLandmark::setAttribute(const QString &attributeName, const QVariant &value)
{
    d->attributes[attributeName] = value;
}

/*!
    Returns a list of attribute names.
*/
QStringList QLandmark::attributes() const
{
    return d->attributes.keys();
}

/*!
    Returns the street of the landmark.
*/
QString QLandmark::street() const
{
    return d->street;
}

/*!
    Sets the \a street of the landmark.
*/
void QLandmark::setStreet(const QString &street)
{
    d->street = street;
}

/*!
    Returns the locality of the landmark.
*/
QString QLandmark::locality() const
{
    return d->locality;
}

/*!
    Sets the \a locality of the landmark.
*/
void QLandmark::setLocality(const QString &locality)
{
    d->locality = locality;
}

/*!
    Returns the region of the landmark.
*/
QString QLandmark::region() const
{
    return d->region;
}

/*!
    Sets the \a region of the landmark
*/
void QLandmark::setRegion(const QString &region)
{
    d->region = region;
}

/*!
    Returns the country of the landmark.
*/
QString QLandmark::country() const
{
    return d->country;
}

/*!
    Sets the \a country of the landmark.
*/
void QLandmark::setCountry(const QString &country)
{
    d->country = country;
}

/*!
    Returns the postcode of the landmark.
*/
QString QLandmark::postcode() const
{
    return d->postcode;
}

/*!
    Sets the \a postcode of the landmark.
*/
void QLandmark::setPostcode(const QString &postcode)
{
    d->postcode = postcode;
}

/*!
    Returns the post office box of the landmark.
*/
QString QLandmark::postOfficeBox() const
{
}

/*!
    Sets the \a postOfficeBox of the landmark.
*/
void QLandmark::setPostOfficeBox(const QString &postOfficeBox)
{
}

/*!
    Returns the phone number of the landmark.
*/
QString QLandmark::phone() const
{
    return d->phone;
}

/*!
    Sets the \a phone number of the landmark.
*/
void QLandmark::setPhone(const QString &phone)
{
    d->phone = phone;
}
/*!
    Returns the url of the landmark.
*/
QString QLandmark::url() const
{
    return d->url;
}

/*!
    Sets the \a url of the landmark.
*/
void QLandmark::setUrl(const QString &url)
{
    d->url = url;
}

/*!
    Returns the landmark identifier for the landmark.
*/
QLandmarkId QLandmark::landmarkId()
{
}
