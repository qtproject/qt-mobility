/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlandmarkcategoryfilter.h"
#include "qlandmarkcategoryfilter_p.h"
#include "qlandmarkcategoryid.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCategoryFilter
    \brief The QLandmarkCategoryFilter class is used to search for landmarks that
    belong to a certain category.

    \inmodule QtLocation
    \since 1.1

    \ingroup landmarks-filter
*/

Q_IMPLEMENT_LANDMARKFILTER_PRIVATE(QLandmarkCategoryFilter)

/*!
    Creates a filter that searches for landmarks that belong to the category
    identified by \a categoryId.
    \since 1.1
*/
QLandmarkCategoryFilter::QLandmarkCategoryFilter(const QLandmarkCategoryId &categoryId)
        : QLandmarkFilter(new QLandmarkCategoryFilterPrivate(categoryId))
{
}

/*!
    \fn QLandmarkCategoryFilter::QLandmarkCategoryFilter(const QLandmarkFilter &other)
    Constructs a copy of \a other if possible, otherwise constructs a new category filter.
    \since 1.1
*/

/*!
    Destroys the filter.
*/
QLandmarkCategoryFilter::~QLandmarkCategoryFilter()
{
    // pointer deleted in superclass destructor
}

/*!
    Returns the category identifier that this filter matches by.
    \since 1.1
*/
QLandmarkCategoryId QLandmarkCategoryFilter::categoryId() const
{
    Q_D(const QLandmarkCategoryFilter);
    return d->id;
}

/*!
    Sets the category identifier that this filter matches by
    to \a categoryId.
    \since 1.1
*/
void QLandmarkCategoryFilter::setCategoryId(const QLandmarkCategoryId &categoryId)
{
    Q_D(QLandmarkCategoryFilter);
    d->id = categoryId;
}

/*!
    Convenience function to set the \a category that this filter matches by.
    It is equivalent to calling setCategory(category.categoryId()).
    \since 1.1
*/
void QLandmarkCategoryFilter::setCategory(const QLandmarkCategory &category)
{
    Q_D(QLandmarkCategoryFilter);
    d->id = category.categoryId();
}

/*******************************************************************************
*******************************************************************************/

QLandmarkCategoryFilterPrivate::QLandmarkCategoryFilterPrivate(const QLandmarkCategoryId &id)
        : id(id)
{
    type = QLandmarkFilter::CategoryFilter;
}

QLandmarkCategoryFilterPrivate::QLandmarkCategoryFilterPrivate(const QLandmarkCategoryFilterPrivate &other)
        : QLandmarkFilterPrivate(other),
        id(other.id) {}

QLandmarkCategoryFilterPrivate::~QLandmarkCategoryFilterPrivate() {}

QTM_END_NAMESPACE
