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

QLandmarkCategory::QLandmarkCategory()
{
    d = new QLandmarkCategoryPrivate();
}

QLandmarkCategory::QLandmarkCategory(const QLandmarkCategory &other)
{
    d = new QLandmarkCategoryPrivate(*(other.d));
}

QLandmarkCategory::~QLandmarkCategory()
{
    delete d;
}

QLandmarkCategory& QLandmarkCategory::operator= (const QLandmarkCategory & other)
{
    *d = *(other.d);
    return *this;
}

bool QLandmarkCategory::operator== (const QLandmarkCategory &other) const
{
    return (*d == *(other.d));
}

QString QLandmarkCategory::name() const
{
    return d->name;
}

void QLandmarkCategory::setName(const QString &name)
{
    d->name = name;
}

QString QLandmarkCategory::icon() const
{
    return d->icon;
}

void QLandmarkCategory::setIcon(const QString &fileName)
{
    d->icon = fileName;
}

QString QLandmarkCategory::description() const
{
    return d->description;
}

void QLandmarkCategory::setDescription(const QString &description)
{
    d->description = description;
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

// ----- QLandmark -----

QLandmark::QLandmark()
{
    d = new QLandmarkPrivate();
}

QLandmark::QLandmark(const QLandmark &other)
{
    d = new QLandmarkPrivate(*(other.d));
}

QLandmark::~QLandmark()
{
    delete d;
}

QLandmark& QLandmark::operator= (const QLandmark & other)
{
    *d = *(other.d);
    return *this;
}

bool QLandmark::operator== (const QLandmark &other) const
{
    return (*d == *(other.d));
}

QString QLandmark::name() const
{
    return d->name;
}

void QLandmark::setName(const QString &name)
{
    d->name = name;
}

QGeoCoordinate QLandmark::coordinate() const
{
    return d->coordinate;
}

void QLandmark::setCoordinate(const QGeoCoordinate& coordinate)
{
    d->coordinate = coordinate;
}

QList<QLandmarkCategoryId> QLandmark::categories() const
{
    return d->categories;
}

void QLandmark::setCategories(const QList<QLandmarkCategoryId> &categories)
{
    // remove duplicates
    d->categories.clear();
    for (int i = 0; i < categories.size(); ++i) {
        if (!d->categories.contains(categories.at(i)))
            d->categories.append(categories.at(i));
    }
}

void QLandmark::addCategory(const QLandmarkCategoryId &category)
{
    if (!d->categories.contains(category))
        d->categories.append(category);
}

void QLandmark::removeCategory(const QLandmarkCategoryId &category)
{
    d->categories.removeAll(category);
}

QString QLandmark::description() const
{
    return d->description;
}

void QLandmark::setDescription(const QString &description)
{
    d->description = description;
}

QString QLandmark::icon() const
{
    return d->icon;
}

void QLandmark::setIcon(const QString &iconFileName)
{
    d->icon = iconFileName;
}

double QLandmark::radius() const
{
    return d->radius;
}

void QLandmark::setRadius(double radius)
{
    d->radius = radius;
}

QVariant QLandmark::attribute(const QString &attributeName) const
{
    return d->attributes.value(attributeName);
}

void QLandmark::setAttribute(const QString &attributeName, const QVariant &value)
{
    d->attributes[attributeName] = value;
}

QStringList QLandmark::attributes() const
{
    return d->attributes.keys();
}

QString QLandmark::street() const
{
    return d->street;
}

void QLandmark::setStreet(const QString &street)
{
    d->street = street;
}

QString QLandmark::locality() const
{
    return d->locality;
}

void QLandmark::setLocality(const QString &locality)
{
    d->locality = locality;
}

QString QLandmark::region() const
{
    return d->region;
}

void QLandmark::setRegion(const QString &region)
{
    d->region = region;
}

QString QLandmark::country() const
{
    return d->country;
}

void QLandmark::setCountry(const QString &country)
{
    d->country = country;
}

QString QLandmark::postcode() const
{
    return d->postcode;
}

void QLandmark::setPostcode(const QString &postCode)
{
    d->postcode = postCode;
}

QString QLandmark::phone() const
{
    return d->phone;
}

void QLandmark::setPhone(const QString &phone)
{
    d->phone = phone;
}

QString QLandmark::url() const
{
    return d->url;
}

void QLandmark::setUrl(const QString &url)
{
    d->url = url;
}

