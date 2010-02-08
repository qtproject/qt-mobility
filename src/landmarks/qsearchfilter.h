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

#ifndef QSEARCHFILTER_H
#define QSERACHFILTER_H

#include "qmobilityglobal.h"

#include <QObject>
#include <QPair>
#include "qlandmarkdatabase.h"

class QLandmark;
class QGeoCoordinate;
class QLandmarkCategory;

class QSearchFilter : public QObject
{
    Q_OBJECT
public:
    QSearchFilter(int maxMatches = -1);
    virtual ~QSearchFilter() {}
    void setMaxMatches(int maxMatches);
    int maxMatches() const;
};

class QLandmarkNameFilter : public QSearchFilter
{
public:
    QLandmarkNameFilter(const QString &name);
    virtual ~QLandmarkNameFilter();
    QString name();
    void setName(const QString &name);
};

class QLandmarkProximityFilter : public QSearchFilter
{
public:
    QLandmarkProximityFilter(const QGeoCoordinate &coordinate, double range);
    virtual ~QLandmarkProximityFilter();

    QGeoCoordinate centralCoordinate() const;
    void setCentralCoordinate(const QGeoCoordinate &coordinate);

    double range();
    void setRange(double range);
};

class QLandmarkNearestFilter : public QSearchFilter
{
public:
    QLandmarkNearestFilter(const QGeoCoordinate &coordinate);
    virtual ~QLandmarkNearestFilter();

    QGeoCoordinate centralCoordinate() const;
    void setCentralCoordinate(const QGeoCoordinate &centralCoordinate);
};

class QLandmarkCategoryFilter : public QSearchFilter
{
public:
    QLandmarkCategoryFilter(const QLandmarkCategory &category);
    virtual ~QLandmarkCategoryFilter();

    QLandmarkCategory category() const;
    void setCategory(const QLandmarkCategory &category);
};

class QLandmarkRegionFilter: public QSearchFilter
{
public:
    QLandmarkRegionFilter(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight);
    virtual ~QLandmarkRegionFilter();

    QPair<QGeoCoordinate, QGeoCoordinate> searchRegion() const;
    void setSearchRegion(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight);
};

class QLandmarkSortOrder : public QObject
{
};

class QLandmarkNameSort : public QLandmarkSortOrder
{
public:
    enum Type {Ascending, Descending};
    QLandmarkNameSort(Type orderType);
    virtual ~QLandmarkNameSort();
};

class QLandmarkDistanceSort : public QLandmarkSortOrder
{
public:
    enum Type {NearestFirst, FurthestFirst};
    QLandmarkDistanceSort(Type orderType);
    virtual ~QLandmarkDistanceSort();
};

class QLandmarkSearchResult: public QList<QLandmark>
{
public:
    int newIndex() const;
    QLandmarkDatabase::SearchStatus status();

    virtual ~QLandmarkSearchResult();
private:
    QLandmarkSearchResult();
};

#endif
