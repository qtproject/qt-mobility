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

#include "qlandmarkattributefilter.h"
#include "qlandmarkfilter_p.h"
#include <QHash>

QTM_BEGIN_NAMESPACE

class QLandmarkAttributeFilterPrivate : public QLandmarkFilterPrivate
{
public:
    QLandmarkAttributeFilterPrivate();
    virtual ~QLandmarkAttributeFilterPrivate();

    virtual bool compare(const QLandmarkFilterPrivate *other) const
    {
        //it is assumed that the types and superclass member variables already match
        const QLandmarkAttributeFilterPrivate *od = static_cast<const QLandmarkAttributeFilterPrivate*>(other);
        return attributes == od->attributes;
    }

    Q_IMPLEMENT_LANDMARKFILTER_VIRTUALCTORS(QLandmarkAttributeFilter, QLandmarkFilter::AttributeFilter)

    QHash<QString, QVariant> attributes;
};


QLandmarkAttributeFilterPrivate::QLandmarkAttributeFilterPrivate()
    : QLandmarkFilterPrivate(),
      attributes(QHash<QString, QVariant>())
{
    type = QLandmarkFilter::AttributeFilter;
}

QLandmarkAttributeFilterPrivate::~QLandmarkAttributeFilterPrivate()
{
}

/*!
    \class QLandmarkAttributeFilter
    \brief The QLandmarkAttributeFilter class provides filtering based on generic landmark attributes.
    \ingroup landmarks-filter

    The QLandmarkAttributeFilter class may be used to filter landmarks whose attributes match certain values.
    If an invalid QVariant is provided as the value for an attribute, then any landmark that has that attribute is
    returned, regardless of its value.  More than one attribute may be set in the filter, a landmark will
    only match if all attributes are matched.
*/
Q_IMPLEMENT_LANDMARKFILTER_PRIVATE(QLandmarkAttributeFilter);

/*!
    Creates an attribute filter.
*/
QLandmarkAttributeFilter::QLandmarkAttributeFilter()
    : QLandmarkFilter(new QLandmarkAttributeFilterPrivate)
{
}

/*!
    \fn QLandmarkAttributeFilter::QLandmarkAttributeFilter(const QLandmarkFilter &other)
    Constructs a copy of \a other if possible, otherwise constructs a new attribute filter.
*/

/*!
    Destroys the filter.
*/
QLandmarkAttributeFilter::~QLandmarkAttributeFilter()
{
}

/*!
    Returns the value of the attribute corresponding to \a key.

    If the attribute isn't set an invalid QVariant is returned.
*/
QVariant QLandmarkAttributeFilter::attribute(const QString &key) const
{
    Q_D(const QLandmarkAttributeFilter);
    return d->attributes.value(key);
}

/*!
    Sets the \a value of the attribute corresponding to \a key.

    Setting an invalid QVariant to an attribute means that the filter
    will match any landmark that has that attribute set, regardless of its value.
    Setting a valid QVariant means that the filter will match only landmarks
    whose values exactly match that of \a value.
*/
void QLandmarkAttributeFilter::setAttribute(const QString &key, const QVariant &value)
{
    Q_D(QLandmarkAttributeFilter);
    d->attributes[key] = value;
}

/*!
    Removes the attribute corresponding to \a key from the filter.
*/
void QLandmarkAttributeFilter::removeAttribute(const QString &key)
{
    Q_D(QLandmarkAttributeFilter);
    d->attributes.remove(key);
}

/*!
    Clears all attributes from the filter.
*/
void QLandmarkAttributeFilter::clearAttributes()
{
    Q_D(QLandmarkAttributeFilter);
    d->attributes.clear();
}

/*!
    Returns the keys of all attributes set in the filter.
*/
QStringList QLandmarkAttributeFilter::attributeKeys() const
{
    Q_D(const QLandmarkAttributeFilter);
    return d->attributes.keys();
}

QTM_END_NAMESPACE
