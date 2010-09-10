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

#include "qgeoplace.h"
#include "qgeoplace_p.h"

#include <QVariant>
#include <QStringList>
#include <QSet>
#include <QHash>

#ifdef LANDMARKPRIVATE_DEBUG
#include <QDebug>
#endif


QTM_BEGIN_NAMESPACE
uint qHash(const QLandmarkCategoryId& key) {
   return qHash(key.localId()) + qHash(key.managerUri());
}
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

// ----- QLandmarkPrivate -----

QStringList QLandmarkPrivate::commonKeys = QStringList() << "name"
                                                         << "description"
                                                         << "iconUrl"
                                                         << "radius"
                                                         << "phoneNumber"
                                                         << "url"
                                                         << "latitude"
                                                         << "longitude"
                                                         << "altitude"
                                                         << "country"
                                                         << "countryCode"
                                                         << "state"
                                                         << "county"
                                                         << "city"
                                                         << "district"
                                                         << "street"
                                                         << "streetNumber"
                                                         << "postCode";

QLandmarkPrivate::QLandmarkPrivate()
        : QGeoPlacePrivate()
{
    type = QGeoPlacePrivate::LandmarkType;
    radius = -1.0;
}

QLandmarkPrivate::QLandmarkPrivate(const QGeoPlacePrivate &other)
        : QGeoPlacePrivate(other)
{
    type = QGeoPlacePrivate::LandmarkType;
    radius = -1.0;
}

QLandmarkPrivate::QLandmarkPrivate(const QLandmarkPrivate &other)
        : QGeoPlacePrivate(other),
        name(other.name),
        categoryIds(other.categoryIds),
        description(other.description),
        iconUrl(other.iconUrl),
        radius(other.radius),
        managerAttributes(other.managerAttributes),
        customAttributes(other.customAttributes),
        phoneNumber(other.phoneNumber),
        url(other.url),
        id(other.id)
{
}

QLandmarkPrivate::~QLandmarkPrivate() {}

QLandmarkPrivate& QLandmarkPrivate::operator= (const QLandmarkPrivate & other)
{
    QGeoPlacePrivate::operator =(other);
    name = other.name;
    description = other.description;
    iconUrl = other.iconUrl;
    radius = other.radius;
    phoneNumber = other.phoneNumber;
    url = other.url;
    categoryIds = other.categoryIds;
    managerAttributes = other.managerAttributes;
    customAttributes = other.customAttributes;
    id = other.id;

    return *this;
}

bool QLandmarkPrivate::operator== (const QLandmarkPrivate &other) const
{

#ifdef LANDMARKPRIVATE_DEBUG
    qDebug() << "==" << (QGeoPlacePrivate::operator== (other));
    qDebug() << "name:" << (name == other.name);
    qDebug() << "description:" <<  (description == other.description);
    qDebug() << "iconUrl:" << (iconUrl == other.iconUrl);
    qDebug() << "radius:" <<  (radius == other.radius);
    qDebug() << "phoneNumber:" << (phoneNumber == other.phoneNumber);
    qDebug() << "url:" << (url == other.url);
    qDebug() << "categoryIds:" << (categoryIds.toSet() == other.categoryIds.toSet());
    qDebug() << "managerAttributes:" << (managerAttributes == other.managerAttributes);
    qDebug() << "customAttributes:" << (customAttributes == other.customAttributes);
    qDebug() << "id" << (id == other.id);
#endif

    return (QGeoPlacePrivate::operator== (other)
            && (name == other.name)
            && (description == other.description)
            && (iconUrl == other.iconUrl)
            && (radius == other.radius)
            && (phoneNumber == other.phoneNumber)
            && (url == other.url)
            && (categoryIds.toSet() == other.categoryIds.toSet())
            && (managerAttributes == other.managerAttributes)
            && (customAttributes == other.customAttributes)
           && (id == other.id));
}

/*!
    \class QLandmark

    \inmodule QtLocation

    \ingroup landmarks-main

    \brief The QLandmark class represents a location or point of interest
           of some significance.


    Each landmark consists of a number of core properties such as name, coordinates,
    description etc accessible through standard member functions.  For
    information on accessing landmark properties through the attributes function
    see \l {Landmark Attributes}.

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
        : QGeoPlace(new QLandmarkPrivate)
{
}

/*!
    Constructs a new landmark from \a other.

    If other is a QLandmark instance this is equivalent to
    QLandmark(const QLandmark &other).

    If other::type() is QGeoPlace instance this will initialize just the
    coordinate and address of this landmark.

    Otherwise this is equivalent to QLandmark().
*/
QLandmark::QLandmark(const QGeoPlace &other)
    : QGeoPlace(other)
{
    switch (other.d_ptr->type) {
        case QGeoPlacePrivate::GeoPlaceType:
        d_ptr = new QLandmarkPrivate(*(other.d_ptr.constData()));
        break;
    case QGeoPlacePrivate::LandmarkType:
        // nothing extra to do here
        break;
    default:
        d_ptr = new QLandmarkPrivate();
        break;
    }
}

/*!
    Constructs a copy of \a other.
*/
QLandmark::QLandmark(const QLandmark &other)
        : QGeoPlace(other)
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
    If the key doest exist, an invalid QVariant is returned.
*/
QVariant QLandmark::attribute(const QString &key) const
{
    Q_D(const QLandmark);

    if (key.compare("name",Qt::CaseInsensitive) == 0) {
        return name();
    } else if (key.compare("description", Qt::CaseInsensitive) == 0) {
        return description();
    } else if (key.compare("iconUrl",Qt::CaseInsensitive) ==0) {
        return iconUrl();
    } else if (key.compare("radius", Qt::CaseInsensitive) == 0) {
        return radius();
    } else if (key.compare("phoneNumber", Qt::CaseInsensitive) == 0) {
        return phoneNumber();
    } else if (key.compare("url", Qt::CaseInsensitive) ==0 ) {
        return url();
    } else if (key.compare("latitude", Qt::CaseSensitive)== 0) {
        return d->coordinate.latitude();
    } else if (key.compare("longitude", Qt::CaseSensitive) ==0) {
        return d->coordinate.longitude();
    } else if (key.compare("altitude", Qt::CaseSensitive) ==0) {
        return d->coordinate.altitude();
    } else if (key.compare("country", Qt::CaseSensitive) ==0 ){
        return d->address.country();
    }  else if (key.compare("countryCode", Qt::CaseSensitive) ==0 ){
        return d->address.countryCode();
    } else if (key.compare("state", Qt::CaseSensitive) ==0 ){
        return d->address.state();
    } else if (key.compare("county", Qt::CaseSensitive) ==0 ){
        return d->address.county();
    } else if (key.compare("city", Qt::CaseSensitive) ==0 ){
        return d->address.city();
    } else if (key.compare("district", Qt::CaseSensitive) ==0 ){
        return d->address.district();
    } else if (key.compare("street", Qt::CaseSensitive) ==0 ){
        return d->address.street();
    } else if (key.compare("streetNumber", Qt::CaseSensitive) ==0 ){
        return d->address.streetNumber();
    } else if (key.compare("postCode", Qt::CaseSensitive) ==0 ){
        return d->address.postCode();
    }

    return d->managerAttributes.value(key);
}

/*!
    Sets the \a value of the attribute corresponding to \a key.
    If the \a key does not already exist this function has no effect.
*/
void QLandmark::setAttribute(const QString &key, const QVariant &value)
{
    Q_D(QLandmark);

    if (key.compare("name",Qt::CaseInsensitive) == 0) {
        setName(value.toString());
        return;
    } else if (key.compare("description", Qt::CaseInsensitive) == 0) {
        setDescription(value.toString());
        return;
    } else if (key.compare("iconUrl",Qt::CaseInsensitive) ==0) {
        setIconUrl(QUrl(value.toUrl()));
        return;
    } else if (key.compare("radius", Qt::CaseInsensitive) == 0) {
        setRadius(value.toDouble());
        return;
    } else if (key.compare("phoneNumber", Qt::CaseInsensitive) == 0) {
        setPhoneNumber(value.toString());
        return;
    } else if (key.compare("url", Qt::CaseInsensitive) ==0 ) {
        setUrl(QUrl(value.toUrl()));
        return;
    } else if (key.compare("latitude", Qt::CaseSensitive)== 0) {
        d->coordinate.setLatitude(value.toDouble());
        return;
    } else if (key.compare("longitude", Qt::CaseSensitive) ==0) {
        d->coordinate.setLongitude(value.toDouble());
        return;
    } else if (key.compare("altitude", Qt::CaseSensitive) ==0) {
        d->coordinate.setAltitude(value.toDouble());
        return;
    } else if (key.compare("country", Qt::CaseSensitive) ==0 ){
        d->address.setCountry(value.toString());
        return;
    }  else if (key.compare("countryCode", Qt::CaseSensitive) ==0 ){
        d->address.setCountryCode(value.toString());
        return;
    } else if (key.compare("state", Qt::CaseSensitive) ==0 ){
        d->address.setState(value.toString());
        return;
    } else if (key.compare("county", Qt::CaseSensitive) ==0 ){
        d->address.setCounty(value.toString());
        return;
    } else if (key.compare("city", Qt::CaseSensitive) == 0 ){
        d->address.setCity(value.toString());
        return;
    } else if (key.compare("district", Qt::CaseSensitive) ==0 ){
        d->address.setDistrict(value.toString());
        return;
    } else if (key.compare("street", Qt::CaseSensitive) ==0 ){
        d->address.setStreet(value.toString());
        return;
    } else if (key.compare("streetNumber", Qt::CaseSensitive) ==0 ){
        d->address.setStreetNumber(value.toString());
        return;
    } else if (key.compare("postCode", Qt::CaseSensitive) ==0 ){
        d->address.setPostCode(value.toString());
        return;
    }

    d->managerAttributes.insert(key, value);
}

/*!
    Returns a list of attribute keys.

    \sa attribute(), setAttribute()
*/
QStringList QLandmark::attributeKeys() const
{
    Q_D(const QLandmark);
    return d->commonKeys + d->managerAttributes.keys();
}

/*!
    Removes the attribute corresponding to \a key.
    Common cross platform attributes cannot be removed,
    only extended attributes may be removed using this function.
*/
void QLandmark::removeAttribute(const QString &key)
{
    Q_D(QLandmark);
    if (d->commonKeys.contains(key))
        return;
     else
        d->managerAttributes.remove(key);
}

/*!
    Returns the value of the custom attribute corresponding to \a key.
    If the custom attribute doest exist, returns \a defaultValue.

    If no default value is specified, a default QVariant is returned.
*/
QVariant QLandmark::customAttribute(const QString &key, const QVariant &defaultValue) const
{
    Q_D(const QLandmark);

    return d->customAttributes.value(key, defaultValue);
}

/*!
    Sets the \a value of the custom attribute corresponding to \a key.
    Setting an invalid QVariant removes the key.
*/
void QLandmark::setCustomAttribute(const QString &key, const QVariant &value)
{
    Q_D(QLandmark);

    if (!value.isValid())
        d->customAttributes.remove(key);
    else
        d->customAttributes[key] = value;
}

/*!
    Returns a list of custom Attribute keys.

    \sa customAttribute(), setCustomAttribute()
*/
QStringList QLandmark::customAttributeKeys() const
{
    Q_D(const QLandmark);

    return d->customAttributes.keys();
}

/*!
    Removes the custom attribute corresponding to \a key.
*/
void QLandmark::removeCustomAttribute(const QString &key)
{
    Q_D(QLandmark);
     d->customAttributes.remove(key);
}

/*!
    Returns the phone number of the landmark.
*/
QString QLandmark::phoneNumber() const
{
    Q_D(const QLandmark);
    return d->phoneNumber;
}

/*!
    Sets the \a phoneNumber of the landmark.
*/
void QLandmark::setPhoneNumber(const QString &phoneNumber)
{
    Q_D(QLandmark);
    d->phoneNumber = phoneNumber;
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

/*!
    Clears all the data fields of the landmark.
*/
void QLandmark::clear()
{
    Q_D(QLandmark);
    d->address.clear();
    d->coordinate = QGeoCoordinate();
    d->viewport = QGeoBoundingBox();
    d->name.clear();
    d->categoryIds.clear();
    d->description.clear();
    d->iconUrl.clear();
    d->radius = 0.0;
    d->managerAttributes.clear();
    d->customAttributes.clear();
    d->phoneNumber.clear();
    d->url.clear();
    d->id = QLandmarkId();
}

