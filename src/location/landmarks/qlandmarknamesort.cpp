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

#include "qlandmarknamesort.h"
#include "qlandmarknamesort_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkNameSort
    \brief The QLandmarkNameSort class is used to sort landmarks by name.

    \inmodule QtLocation

    \ingroup landmarks-sort

    Note that some manager's may not support case sensitive sorting.
*/

Q_IMPLEMENT_LANDMARKSORTORDER_PRIVATE(QLandmarkNameSort)

/*!
    Creates a sort order that sorts by name according to the given \a direction and
    case \a sensitivity.
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
*/

/*!
    Destroys the sort order.
*/
QLandmarkNameSort::~QLandmarkNameSort()
{
}

/*!
    Returns the case sensitivity of the sort order.
*/
Qt::CaseSensitivity QLandmarkNameSort::caseSensitivity() const
{
    Q_D(const QLandmarkNameSort);
    return d->sensitivity;
}

/*!
    Sets the the \a caseSensitivity of the sort order.
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
