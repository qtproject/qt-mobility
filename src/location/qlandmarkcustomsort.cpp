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

#include "qlandmarkcustomsort.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCustomSort
    \brief The QLandmarkCustomSort class is the interface which all
    custom sort orders should inherit.
    \ingroup landmarks-sort

    All custom sort orders should inherit this class and implement the
    compare() function.

    Custom landmark sort orders inherently have no native support
    and thus may be subject to performance issues.
*/

/*!
    Constructs a custom landmark sort order.
*/
QLandmarkCustomSort::QLandmarkCustomSort()
{
}

/*!
    Destroys the sort order.
*/
QLandmarkCustomSort::~QLandmarkCustomSort()
{
}

/*!
    \fn int QLandmarkCustomSort::compare(const QLandmark &l1, const QLandmark &l2) const
    Compares \a l1 and \a l2 and returns an integer less than, equal to, or greater than
    zero if \a l1 is less than, equal to, or greater than \a l2.
*/

QTM_END_NAMESPACE
