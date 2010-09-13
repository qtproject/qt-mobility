/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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

QStringList QLandmarkCategoryPrivate::commonKeys = QStringList() << "name"
                                                                 << "iconUrl";

QLandmarkCategoryPrivate::QLandmarkCategoryPrivate()
    : QSharedData(),
      name(QString()),
      iconUrl(QUrl()),
      description(QString()),
      managerAttributes(QHash<QString, QVariant>()),
      customAttributes(QHash<QString, QVariant>()),
      id(QLandmarkCategoryId())
{
}

QLandmarkCategoryPrivate::QLandmarkCategoryPrivate(const QLandmarkCategoryPrivate &other)
    : QSharedData(other),
      name(other.name),
      iconUrl(other.iconUrl),
      description(other.description),
      managerAttributes(other.managerAttributes),
      customAttributes(other.customAttributes),
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
    managerAttributes = other.managerAttributes;
    customAttributes = other.customAttributes;
    id = other.id;

    return *this;
}

bool QLandmarkCategoryPrivate::operator == (const QLandmarkCategoryPrivate &other) const
{
    return ((name == other.name)
            && (iconUrl == other.iconUrl)
            && (description == other.description)
            && (managerAttributes == other.managerAttributes)
            && (customAttributes == other.customAttributes)
            && (id == other.id));
}

/*!
    \class QLandmarkCategory

    \brief The QLandmarkCategory class designates a grouping of
    landmarks of similar type.

    \inmodule QtLocation

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
    are read-only.  If the landmark store supports localization, the locale may
    be set through a QLandmarkManager's parameters and whenever categories are
    retrieved, the translated names are used.
    The \c {QLandmarkManager::isReadOnly(const QLandmarkCategoryId &)}
    function may be used to determine if a category is read-only.

    A QLandmarkCategory instance is an in memory representation and may
    not reflect the state of the category found in persistent storage,
    until the appropriate synchronization method is called on the
    QLandmarkManager(e.g.\l {QLandmarkManager::saveCategory()} {saveCategory()},
    \l {QLandmarkManager::removeCategory()} {removeCategory()}).
*/

/*!
    Constructs an empty category.

    A new category will be assigned an invalid QLandmarkCategoryId.
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
    Returns the identifier of the category.
*/
QLandmarkCategoryId QLandmarkCategory::categoryId() const
{
    return d->id;
}

/*!
    Sets the \a id of the category.

    Note that saving a new category using a QLandmarkManager
    will automatically assign the category a valid identifier.
*/
void QLandmarkCategory::setCategoryId(const QLandmarkCategoryId &id)
{
    d->id = id;
}

/*!
    Returns the value of the attribute corresponding to \a key.
    If the key doest exist, an invalid QVariant is returned.
*/
QVariant QLandmarkCategory::attribute(const QString &key) const
{

    if (key.compare("name",Qt::CaseInsensitive) == 0) {
        return name();
    } else if (key.compare("iconUrl",Qt::CaseInsensitive) ==0) {
        return iconUrl();
    }

    return d->managerAttributes.value(key);
}

/*!
    Sets the \a value of the attribute corresponding to \a key.
    If the \a key does not already exist this function has no effect.
*/
void QLandmarkCategory::setAttribute(const QString &key, const QVariant &value)
{

    if (key.compare("name",Qt::CaseInsensitive) == 0) {
        setName(value.toString());
        return;
    } else if (key.compare("iconUrl",Qt::CaseInsensitive) ==0) {
        setIconUrl(QUrl(value.toUrl()));
        return;
    }

    if (d->managerAttributes.contains(key))
        d->managerAttributes[key] = value;
}

/*!
    Returns a list of attribute keys.

    \sa attribute(), setAttribute()
*/
QStringList QLandmarkCategory::attributeKeys() const
{
    return d->commonKeys + d->managerAttributes.keys();
}

/*!
    Removes the attribute corresponding to \a key.
    Common cross platform attributes cannot be removed,
    only extended attributes may be removed using this function.
*/
void QLandmarkCategory::removeAttribute(const QString &key)
{
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
QVariant QLandmarkCategory::customAttribute(const QString &key, const QVariant &defaultValue) const
{
    return d->customAttributes.value(key,defaultValue);
}

/*!
    Sets the \a value of the custom attribute corresponding to \a key.
    Setting an invalid QVariant removes the key.
*/
void QLandmarkCategory::setCustomAttribute(const QString &key, const QVariant &value)
{
    if (!value.isValid())
        d->customAttributes.remove(key);
    else
        d->customAttributes[key] = value;
}

/*!
    Returns a list of custom Attribute keys.

    \sa customAttribute(), setCustomAttribute()
*/
QStringList QLandmarkCategory::customAttributeKeys() const
{
    return d->customAttributes.keys();
}

/*!
    Removes the custom attribute corresponding to \a key.
*/
void QLandmarkCategory::removeCustomAttribute(const QString &key)
{
    d->customAttributes.remove(key);
}

/*!
    Clears all the data fields of the category.
*/
void QLandmarkCategory::clear()
{
    d->name.clear();
    d->iconUrl.clear();
    d->description.clear();
    d->managerAttributes.clear();
    d->customAttributes.clear();
    d->id = QLandmarkCategoryId();
}
