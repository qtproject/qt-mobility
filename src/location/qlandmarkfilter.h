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

#ifndef QLANDMARKFILTER_H
#define QLANDMARKFILTER_H

#include "qmobilityglobal.h"

#include <QObject>
#include "qlandmarkmanager.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QLandmark;
class QGeoCoordinate;
class QLandmarkCategory;
class QLandmarkFilterPrivate;
class QLandmarkAbstractCustomFilter;

class QLandmarkFilterPrivate;
class Q_LOCATION_EXPORT QLandmarkFilter
{
public:
    enum FilterType{InvalidFilter, LandmarkNameFilter, LandmarkProximityFilter,
                    LandmarkNearestFilter, LandmarkCategoryFilter, LandmarkBoxFilter,
                    LandmarkCustomFilter =500};
    QLandmarkFilter();
    ~QLandmarkFilter();

    FilterType type() const;

    int maxMatches() const;
    void setMaxMatches(int maxMatches);

    bool operator!=(const QLandmarkFilter &other) const;
    bool operator==(const QLandmarkFilter &other) const;
    QLandmarkFilter &operator=(const QLandmarkFilter &other);
private:
    QLandmarkFilterPrivate *d;
};


class QLandmarkNameFilterPrivate;
class Q_LOCATION_EXPORT QLandmarkNameFilter : public QLandmarkFilter
{
public:
    QLandmarkNameFilter(const QString &name);
    virtual ~QLandmarkNameFilter();

    QString name();
    void setName(const QString &name);
private:
    QLandmarkNameFilterPrivate *d;
};

class QLandmarkProximityFilterPrivate;
class Q_LOCATION_EXPORT QLandmarkProximityFilter : public QLandmarkFilter
{
public:
    QLandmarkProximityFilter(const QGeoCoordinate &coordinate, double range);
    virtual ~QLandmarkProximityFilter();

    QGeoCoordinate coordinate();
    void setCoordinate(const QGeoCoordinate &coordinate);

    double radius();
    void setRadius(double radius);
private:
    QLandmarkProximityFilterPrivate *d;
};

class QLandmarkNearestFilterPrivate;
class Q_LOCATION_EXPORT QLandmarkNearestFilter : public QLandmarkFilter
{
public:
    QLandmarkNearestFilter(const QGeoCoordinate &coordinate);
    virtual ~QLandmarkNearestFilter();

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &coordinate);

private:
    QLandmarkNearestFilter *d;
};

class QLandmarkCategoryFilterPrivate;
class Q_LOCATION_EXPORT QLandmarkCategoryFilter : public QLandmarkFilter
{
public:
    QLandmarkCategoryFilter(const QLandmarkCategoryId &categoryId);
    virtual ~QLandmarkCategoryFilter();

    QLandmarkCategoryId category() const;
    void setCategoryId(const QLandmarkCategoryId &categoryId);

private:
    QLandmarkCategoryFilterPrivate *d;
};

class QLandmarkCustomFilterPrivate;
class Q_LOCATION_EXPORT QLandmarkCustomFilter : public QLandmarkFilter
{
public:
    virtual ~QLandmarkCustomFilter();

protected:
    QLandmarkCustomFilter();
    virtual bool isMatch(const QLandmarkId &landmark)=0;
private:
    QLandmarkCustomFilterPrivate *d;
};

class QLandmarkSortOrderPrivate;
class Q_LOCATION_EXPORT QLandmarkSortOrder
{

public:
    enum SortType{LandmarkNameFilter, LandmarkDistanceFilter, LandmarkCustomFilter=500};

    SortType type() const;
    Qt::SortOrder direction() const;
    void setDirection(Qt::SortOrder direction);
    virtual int compare(const QLandmark &l1, const QLandmark &l2) const= 0;
    virtual QList<QLandmarkId> sort(const QList<QLandmarkId> &landmarkIds) const= 0;
protected:
    QLandmarkSortOrder();
private:
    QLandmarkSortOrderPrivate *d;
};

class QLandmarkNameSortPrivate;
class Q_LOCATION_EXPORT QLandmarkNameSort : public QLandmarkSortOrder
{
public:
    QLandmarkNameSort(Qt::SortOrder direction);
    virtual ~QLandmarkNameSort();

    virtual int compare(const QLandmark &l1, const QLandmark &l2) const;
    virtual QList<QLandmarkId> sort(const QList<QLandmarkId> &landmarkIds) const;
private:
    QLandmarkNameSortPrivate *d;
};

class QLandmarkDistanceSortPrivate;
class Q_LOCATION_EXPORT QLandmarkDistanceSort : public QLandmarkSortOrder
{
public:
    QLandmarkDistanceSort(QGeoCoordinate coordinate, Qt::SortOrder direction);
    virtual ~QLandmarkDistanceSort();

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &coordinate);
    virtual int compare(const QLandmark &l1, const QLandmark &l2) const;
    virtual QList<QLandmarkId> sort(const QList<QLandmarkId> &landmarkIds) const;
private:
    QLandmarkDistanceSortPrivate *d;
};

class QLandmarkCustomSortPrivate;
class Q_LOCATION_EXPORT QLandmarkCustomSort : public QLandmarkSortOrder
{
    virtual ~QLandmarkCustomSort();
protected:
    QLandmarkCustomSort();

private:
    QLandmarkCustomSortPrivate *d;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
