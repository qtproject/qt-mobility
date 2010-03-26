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

#include "qlandmarkcustomfilter.h"
#include "qlandmarkid.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCustomFilter
    \brief The QLandmarkCustomFilter class provides the interface which
    all custom filters should inherit.
    \ingroup location

    All custom landmark filters should inherit off the QLandmarkCustomFilter
    and must implement the isMatch() function.

    Custom filters inherently do not have native support and are thus
    may be subject to performance issues.
*/

/*!
    Creates a custom landmark filter.
*/
QLandmarkCustomFilter::QLandmarkCustomFilter()
{
}

/*!
    Destroys the filter.
*/
QLandmarkCustomFilter::~QLandmarkCustomFilter()
{
}

/*!
    Returns true if the \a landmark
    should be matched by the filter.  All custom landmark filters
    need to implement this function.
*/
bool QLandmarkCustomFilter::isMatch(const QLandmark &landmark)
{
    return false;
}

QTM_END_NAMESPACE
