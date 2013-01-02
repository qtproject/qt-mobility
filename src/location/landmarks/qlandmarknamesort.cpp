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

#include "qlandmarknamesort.h"
#include "qlandmarknamesort_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkNameSort
    \brief The QLandmarkNameSort class is used to sort landmarks by name.

    \inmodule QtLocation

    \ingroup landmarks-sort
    \since 1.1

    Note that some manager's may not support case sensitive sorting.
*/

Q_IMPLEMENT_LANDMARKSORTORDER_PRIVATE(QLandmarkNameSort)

/*!
    Creates a sort order that sorts by name according to the given \a direction and
    case \a sensitivity.
    \since 1.1
*/
QLandmarkNameSort::QLandmarkNameSort(Qt::SortOrder direction, Qt::CaseSensitivity sensitivity)
        : QLandmarkSortOrder(new QLandmarkNameSortPrivate())
{
    Q_D(QLandmarkNameSort);
    d->order = direction;
    d->sensitivity = sensitivity;
}

/*!
    \fn QLandmarkNameSort::QLandmarkNameSort(const QLandmarkSortOrder &other)
    Constructs a copy of \a other if possible, otherwise constructs a new name sort order.
    \since 1.1
*/

/*!
    Destroys the sort order.
*/
QLandmarkNameSort::~QLandmarkNameSort()
{
}

/*!
    Returns the case sensitivity of the sort order.
    \since 1.1
*/
Qt::CaseSensitivity QLandmarkNameSort::caseSensitivity() const
{
    Q_D(const QLandmarkNameSort);
    return d->sensitivity;
}

/*!
    Sets the the \a caseSensitivity of the sort order.
    \since 1.1
*/
void QLandmarkNameSort::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
    Q_D(QLandmarkNameSort);
    d->sensitivity = caseSensitivity;
}

/*******************************************************************************
*******************************************************************************/
QLandmarkNameSortPrivate::QLandmarkNameSortPrivate()
        :  QLandmarkSortOrderPrivate(),
           sensitivity(Qt::CaseInsensitive)
{
    type = QLandmarkSortOrder::NameSort;
}

QLandmarkNameSortPrivate::QLandmarkNameSortPrivate(const QLandmarkNameSortPrivate &other)
        : QLandmarkSortOrderPrivate(other),
        sensitivity(other.sensitivity) {}

QLandmarkNameSortPrivate::~QLandmarkNameSortPrivate() {}


QTM_END_NAMESPACE
