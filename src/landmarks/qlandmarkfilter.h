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
#include "qlandmarkdatabase.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QLandmark;
class QGeoCoordinate;
class QLandmarkCategory;
class QLandmarkFilterPrivate;

class Q_LANDMARKS_EXPORT QLandmarkFilter
{
public:
    QLandmarkFilter();
    virtual ~QLandmarkFilter();
    void setMaxMatches(int maxMatches);
    int maxMatches() const;

    QLandmarkFilter operator&(const QLandmarkFilter &other);
    QLandmarkFilter operator|(const QLandmarkFilter &other);

    QLandmarkFilter &operator&=(const QLandmarkFilter &other);
    QLandmarkFilter &operator|=(const QLandmarkFilter &other);
    QLandmarkFilter operator~() const;
protected:
    virtual QList<QLandmark> filter(QList<QLandmark> &landmarks) const;
private:
    QLandmarkFilterPrivate *d;
};

class Q_LANDMARKS_EXPORT QLandmarkNameFilter : public QLandmarkFilter
{
public:
    QLandmarkNameFilter(const QString &name);
    virtual ~QLandmarkNameFilter();

protected:
    virtual QList<QLandmark> filter(const QList<QLandmark> &landmarks) const;
};

class Q_LANDMARKS_EXPORT QLandmarkProximityFilter : public QLandmarkFilter
{
public:
    QLandmarkProximityFilter(const QGeoCoordinate &coordinate, double range);
    virtual ~QLandmarkProximityFilter();

protected:
    virtual QList<QLandmark> filter(const QList<QLandmark> &landmarks) const;
};

class Q_LANDMARKS_EXPORT QLandmarkNearestFilter : public QLandmarkFilter
{
public:
    QLandmarkNearestFilter(const QGeoCoordinate &coordinate);
    virtual ~QLandmarkNearestFilter();

protected:
     virtual QList<QLandmark> filter(const QList<QLandmark> &landmarks) const;
};

class Q_LANDMARKS_EXPORT QLandmarkCategoryFilter : public QLandmarkFilter
{
public:
    QLandmarkCategoryFilter(const QLandmarkCategory &category);
    virtual ~QLandmarkCategoryFilter();

protected:
    virtual QList<QLandmark> filter(const QList<QLandmark> landmarks) const;
};

class Q_LANDMARKS_EXPORT QLandmarkBoxFilter: public QLandmarkFilter
{
public:
    QLandmarkBoxFilter(const QGeoCoordinate &NW, const QGeoCoordinate &SE);
    virtual ~QLandmarkBoxFilter();

protected:
   virtual QList<QLandmark> filter(const QList<QLandmark> &landmarks) const;
};

class Q_LANDMARKS_EXPORT QLandmarkSortOrder
{
protected:
    virtual int compare(const QLandmark &l1, const QLandmark &l2) = 0;
};

class Q_LANDMARKS_EXPORT QLandmarkNameSort : public QLandmarkSortOrder
{
public:
    enum Type {Ascending, Descending};
    QLandmarkNameSort(Type orderType);
    virtual ~QLandmarkNameSort();
protected:
    virtual int compare(const QLandmark &l1, const QLandmark &l2);
};

class Q_LANDMARKS_EXPORT QLandmarkDistanceSort : public QLandmarkSortOrder
{
public:
    virtual ~QLandmarkDistanceSort();
    enum Type {NearestFirst, FurthestFirst};
    QLandmarkDistanceSort(Type orderType);

protected:
    virtual int compare(const QLandmark &l1, const QLandmark &l2);

};

class Q_LANDMARKS_EXPORT QLandmarkFetchSet: public QList<QLandmark>
{
public:
    int newIndex() const;
    QLandmarkDatabase::FetchStatus fetchStatus();

    virtual ~QLandmarkFetchSet();
private:
    QLandmarkFetchSet();
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
