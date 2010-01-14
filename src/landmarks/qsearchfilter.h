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

class QSearchFilter : public QObject
{
    Q_OBJECT
public:
    SearchFilter(int maxMatches=-1);
    void setMaxMatches(int maxMatches);
    int maxMatches() const;
};

class NameFilter : public QSearchFilter {
public:
    NameFilter(const QString &searchString);
    QString searchString();
    setSearchString(const QString &searchString);
};

class ProximityFilter : public QSeachFilter
{
public:
    ProximityFilter(const QGeoCoordinate &coordinate, double range);
    setCentralCoordinate(const QGeoCoordinate &coordinate);
    QGeoCooordinate centralCoordinate() const;

    double range();
    void setRange(double range);
};

class NearestFilter : public QSearchFilter
{
public:
    NearestFilter( const QGeoCoordinate &coordinate);

    QGeoCoordinate centralCoordinate() const;
    void setCentralCoordinate(const QGeoCoordinate &centralCoordinate);
};

class CategoryFilter : public QSearchFilter
{
public:
    CategoryFilter (const QString &category);

    QString category() const;
    void setCategory(const QString &category);
};

class RegionFilter: public QSearchFilter
{
public:
    RegionFilter(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight);

    QPair<QGeoCooordinate, QGeoCoordinate> searchRegion() const;
    void setSearchRegion(const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight);
};

#endif
