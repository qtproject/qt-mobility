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

#include "qlandmarknamefilter.h"
#include "qlandmarknamefilter_p.h"
#include <QString>

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkNameFilter
    \brief The QLandmarkNameFilter class is used to search for landmarks by name.
*/

/*!
    Creates a filter that selects landmarks by \a name.
*/
QLandmarkNameFilter::QLandmarkNameFilter(const QString &name)
    : QLandmarkFilter(new QLandmarkNameFilterPrivate(name))
{
}

/*!
    Destroys the filter.
*/
QLandmarkNameFilter::~QLandmarkNameFilter()
{
    // pointer deleted in superclass descructor
}

/*!
    Returns the name that the filter will use to determine matches.
*/
QString QLandmarkNameFilter::name() const
{
    Q_D(const QLandmarkNameFilter);
    return d->name;
}

/*!
    Sets the \a name that the filter will use to determine matches.
*/
void QLandmarkNameFilter::setName(const QString &name)
{
    Q_D(QLandmarkNameFilter);
    d->name = name;
}

/*!
    Returns the case sensitivity of the filter.
*/
Qt::CaseSensitivity QLandmarkNameFilter::caseSensitivity() const
{
    Q_D(const QLandmarkNameFilter);
    return d->sensitivity;
}

/*!
    Sets the the \a caseSensitivity of the filter.
*/
void QLandmarkNameFilter::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
    Q_D(QLandmarkNameFilter);
    d->sensitivity = caseSensitivity;
}

/*******************************************************************************
*******************************************************************************/

QLandmarkNameFilterPrivate::QLandmarkNameFilterPrivate(const QString &name)
    : name(name),
    sensitivity(Qt::CaseSensitive)
{
    type = QLandmarkFilter::NameFilter;
}

QLandmarkNameFilterPrivate::QLandmarkNameFilterPrivate(const QLandmarkNameFilterPrivate &other)
    : QLandmarkFilterPrivate(other),
    name(other.name),
    sensitivity(other.sensitivity) {}

QLandmarkNameFilterPrivate::~QLandmarkNameFilterPrivate() {}

QTM_END_NAMESPACE
