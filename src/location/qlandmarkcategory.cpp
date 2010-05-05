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

#include "qlandmarkcategory.h"
#include "qlandmarkcategory_p.h"
#include "qlandmarkcategoryid.h"

#include <QStringList>
#include <QVariant>

QTM_USE_NAMESPACE

// ----- QLandmarkCategoryPrivate -----

QLandmarkCategoryPrivate::QLandmarkCategoryPrivate()
    : QSharedData(),
      name(QString()),
      iconUrl(QUrl()),
      description(QString()),
      readOnly(false),
      attributes(QHash<QString, QVariant>()),
      id(QLandmarkCategoryId())
{
}

QLandmarkCategoryPrivate::QLandmarkCategoryPrivate(const QLandmarkCategoryPrivate &other)
    : QSharedData(other),
      name(other.name),
      iconUrl(other.iconUrl),
      description(other.description),
      readOnly(false),
      attributes(other.attributes),
      id(other.id)
{
}

QLandmarkCategoryPrivate::~QLandmarkCategoryPrivate()
{
}

QLandmarkCategoryPrivate& QLandmarkCategoryPrivate::operator= (const QLandmarkCategoryPrivate & other)
{
    name = other.name;
    iconUrl = other.iconUrl;
    description = other.description;
    readOnly = other.readOnly;
    attributes = other.attributes;
    id = other.id;

    return *this;
}

bool QLandmarkCategoryPrivate::operator == (const QLandmarkCategoryPrivate &other) const
{
    return ((name == other.name)
            && (iconUrl == other.iconUrl)
            && (description == other.description)
            && (readOnly == other.readOnly)
            && (attributes == other.attributes)
            && (id == other.id));
}

/*!
    \class QLandmarkCategory

    \brief The QLandmarkCategory class designates a grouping of
    landmarks of similar type.

    \ingroup landmarks-main

    Landmarks of similar type may be grouped together into categories,
    e.g. restaurants, accommodation etc.  A QLandmarkCategory object
    represents one of these and allows access to category properties
    such as description and icon url.

    More than one category can be assigned to a landmark.  Assignment
    of a category to a landmark is achieved by using the QLandmark class
    in conjunction with the QLandmarkCategoryId class.

    Some categories may be designated as read-only.  Typically these are
    intended to be "global" categories that are the same across landmark stores.
    In practicality, not all landmark stores may necessarily have the same set of
    global categories.  Localization is only possible for categories that
    are read-only.  If the landmark store supports it localization, the locale may
    be set through a QLandmarkManager and whenever categories are retrieved,
    the translated names are used.

    A QLandmarkCategory instance is an in memory representation and may
    not reflect the state of the category found in persistent storage,
    until the appropriate synchronization method is called on the
    QLandmarkManager(e.g.\l {QLandmarkManager::saveCategory()} {saveCategory()},
    \l {QLandmarkManager::removeCategory()} {removeCategory()}).
*/

/*!
    Constructs an empty category.

    A new category will return an invalid QLandmarkCategoryId when
    id() is called.
*/
QLandmarkCategory::QLandmarkCategory()
    : d(new QLandmarkCategoryPrivate)
{
}

/*!
    Constructs a copy of \a other.
*/
QLandmarkCategory::QLandmarkCategory(const QLandmarkCategory &other)
    : d(other.d)
{
}

/*!
    Destroys the category.
*/
QLandmarkCategory::~QLandmarkCategory()
{
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

    \sa operator!=()
*/
bool QLandmarkCategory::operator== (const QLandmarkCategory &other) const
{
    return (*d == *(other.d));
}

/*!
    Returns true if this category is not equal to \a other, otherwise
    returns false.

    \sa operator==()
*/
bool QLandmarkCategory::operator!= (const QLandmarkCategory &other) const
{
    return !(*this == other);
}

/*!
    Returns the name of the category.
*/
QString QLandmarkCategory::name() const
{
    return d->name;
}

/*!
    Sets the \a name of the category.
*/
void QLandmarkCategory::setName(const QString &name)
{
    d->name = name;
}

/*!
    Returns the url of the category's icon.
*/
QUrl QLandmarkCategory::iconUrl() const
{
    return d->iconUrl;
}

/*!
    Sets the \a url of the category's icon
*/
void QLandmarkCategory::setIconUrl(const QUrl &url)
{
    d->iconUrl = url;
}

/*!
    Returns the description of the category.
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
    return d->readOnly;
}

/*!
    Returns the identifier of the category.
*/
QLandmarkCategoryId QLandmarkCategory::id() const
{
    return d->id;
}

/*!
    Sets the \a id of the category.

    Note that saving a new category using a QLandmarkManager
    will automatically assign the category a valid identifier.
*/
void QLandmarkCategory::setId(const QLandmarkCategoryId &id)
{
    d->id = id;
}

/*!
    Returns the value of the attribute corresponding to \a key.
    If the attribute does not exist, returns \a defaultValue.

    If no default value is specified, a default QVariant is returned.
*/
QVariant QLandmarkCategory::attribute(const QString &key, const QVariant &defaultValue) const
{
    return d->attributes.value(key, defaultValue);
}

/*!
    Sets the \a value of the attribute corresponding to \a key.
*/
void QLandmarkCategory::setAttribute(const QString &key, const QVariant &value)
{
    d->attributes[key] = value;
}

/*!
    Returns a list of attribute keys.

    \sa attribute(), setAttribute()
*/
QStringList QLandmarkCategory::attributes() const
{
    return d->attributes.keys();
}
