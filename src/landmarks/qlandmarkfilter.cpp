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

#include "qlandmarkfilter.h"

QTM_BEGIN_NAMESPACE

// ----- QLandmarkFilter -----

QLandmarkFilter::QLandmarkFilter()
{
    //TODO: implement
}

QLandmarkFilter::~QLandmarkFilter()
{
    //TODO: implement
}

void QLandmarkFilter::setMaxMatches(int maxMatches)
{
    //TODO: implement
}

QLandmarkFilter QLandmarkFilter::operator&(const QLandmarkFilter &other)
{
    return QLandmarkFilter(); //TODO: implement
}

QLandmarkFilter QLandmarkFilter::operator|(const QLandmarkFilter &other)
{
    return QLandmarkFilter(); //TODO: implement
}

QLandmarkFilter& QLandmarkFilter::operator&=(const QLandmarkFilter &other)
{
    return *this; //TODO: implement
}

QLandmarkFilter& QLandmarkFilter::operator|=(const QLandmarkFilter &other)
{
    return *this; //TODO: implement
}

QLandmarkFilter QLandmarkFilter::operator~() const
{
    return QLandmarkFilter(); //TODO: implement
}

// ----- QLandmarkNameFilter -----

QLandmarkNameFilter::QLandmarkNameFilter(const QString &name)
{
    //TODO: implement
}

QLandmarkNameFilter::~QLandmarkNameFilter()
{
    //TODO: implement
}

QList<QLandmark> QLandmarkNameFilter::filter(const QList<QLandmark> &landmarks) const
{
    return QList<QLandmark>(); //TODO: implement
}

// ----- QLandmarkProximityFilter -----

QLandmarkProximityFilter::QLandmarkProximityFilter(const QGeoCoordinate &coordinate, double range)
{
    //TODO: implement
}

QLandmarkProximityFilter::~QLandmarkProximityFilter()
{
    //TODO: implement
}

QList<QLandmark> QLandmarkProximityFilter::filter(const QList<QLandmark> &landmarks) const
{
    return QList<QLandmark>();  //TODO: implement
}

// ----- QLandmarkNearestFilter -----

QLandmarkNearestFilter::QLandmarkNearestFilter(const QGeoCoordinate &coordinate)
{
    //TODO: implement
}

QLandmarkNearestFilter::~QLandmarkNearestFilter()
{
    //TODO: implement
}

QList<QLandmark> QLandmarkNearestFilter::filter(const QList<QLandmark> &landmarks) const
{
    return QList<QLandmark>(); //TODO: implement
}

// ----- QLandmarkCategoryFilter -----

QLandmarkCategoryFilter::QLandmarkCategoryFilter(const QLandmarkCategory &category)
{
    //TODO: implement
}

QLandmarkCategoryFilter::~QLandmarkCategoryFilter()
{
    //TODO: implement
}

QList<QLandmark> QLandmarkCategoryFilter::filter(const QList<QLandmark> landmarks) const
{
    return QList<QLandmark>();  //TODO: implement
}

QLandmarkBoxFilter::QLandmarkBoxFilter(const QGeoCoordinate &NW, const QGeoCoordinate &SE)
{
    //TODO: implement
}

// ----- QLandmarkBoxFilter -----

QLandmarkBoxFilter::~QLandmarkBoxFilter()
{
    //TODO: implement
}

QList<QLandmark> QLandmarkBoxFilter::filter(const QList<QLandmark> &landmarks) const
{
    return QList<QLandmark>(); //TODO: implement
}

// ----- QLandmarkNameSort -----

QLandmarkNameSort::QLandmarkNameSort(Type orderType)
{
    //TODO: implement
}

QLandmarkNameSort::~QLandmarkNameSort()
{
    //TODO: implement
}

int QLandmarkNameSort::compare(const QLandmark &l1, const QLandmark &l2)
{
    return 0; //TODO: implement
}

// ----- QLandmarkDistanceSort -----

QLandmarkDistanceSort::QLandmarkDistanceSort(Type orderType)
{
    //TODO: implement
}

QLandmarkDistanceSort::~QLandmarkDistanceSort()
{
    //TODO: implement
}

int QLandmarkDistanceSort::compare(const QLandmark &l1, const QLandmark &l2)
{
    return 0;  //TODO: implement
}

// ----- QLandmarkFetchSet -----

QLandmarkFetchSet::~QLandmarkFetchSet()
{
    //TODO: implement
}

int QLandmarkFetchSet::newIndex() const
{
    return -1;  //TODO: implement
}

QLandmarkDatabase::FetchStatus QLandmarkFetchSet::fetchStatus()
{
    return QLandmarkDatabase::Active;  //TODO: implement
}

QTM_END_NAMESPACE
