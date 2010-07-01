/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>

#include <qgalleryfilter.h>
#include <qgalleryproperty.h>

Q_DECLARE_METATYPE(Qt::CaseSensitivity);
Q_DECLARE_METATYPE(QVariant);
Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QGalleryFilter))
Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QGalleryFilter::Comparator))

QTM_USE_NAMESPACE

class tst_QGalleryFilter : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void nullFilter();
    void metaDataFilter_data();
    void metaDataFilter();
    void unionFilter();
    void intersectionFilter();
    void assignment();
    void copy();
    void copyOnWrite();
    void cast();
    void intersectionOperator();
    void unionOperator();
    void propertyOperators();
    void equality_data();
    void equality();
    void inequality_data();
    void inequality();

#ifndef QT_NO_DEBUG_STREAM
    void debugMessage_data();
    void debugMessage();
#endif
};

void tst_QGalleryFilter::nullFilter()
{
    QGalleryFilter filter;

    QCOMPARE(filter.type(), QGalleryFilter::Invalid);
    QCOMPARE(filter.isValid(), false);
}

void tst_QGalleryFilter::metaDataFilter_data()
{
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QGalleryFilter::Comparator>("comparator");
    QTest::addColumn<Qt::CaseSensitivity>("caseSensitivity");
    QTest::addColumn<bool>("inverted");

    QTest::newRow("album title")
            << QString::fromLatin1("albumTitle")
            << QVariant(QString::fromLatin1("Greatest"))
            << QGalleryFilter::StartsWith
            << Qt::CaseSensitive
            << false;

    QTest::newRow("not album title")
            << QString::fromLatin1("albumTitle")
            << QVariant(QString::fromLatin1("Greatest"))
            << QGalleryFilter::StartsWith
            << Qt::CaseInsensitive
            << false;
}

void tst_QGalleryFilter::metaDataFilter()
{
    QFETCH(QString, propertyName);
    QFETCH(QVariant, value);
    QFETCH(QGalleryFilter::Comparator, comparator);
    QFETCH(Qt::CaseSensitivity, caseSensitivity);
    QFETCH(bool, inverted);

    {
        QGalleryMetaDataFilter filter;

        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.propertyName(), QString());
        QCOMPARE(filter.value(), QVariant());
        QCOMPARE(filter.comparator(), QGalleryFilter::Equals);
        QCOMPARE(filter.caseSensitivity(), Qt::CaseSensitive);
        QCOMPARE(filter.isInverted(), false);

        filter.setPropertyName(propertyName);
        filter.setValue(value);
        filter.setComparator(comparator);
        filter.setCaseSensitivity(caseSensitivity);
        filter.setInverted(inverted);

        QCOMPARE(filter.propertyName(), propertyName);
        QCOMPARE(filter.value(), value);
        QCOMPARE(filter.comparator(), comparator);
        QCOMPARE(filter.caseSensitivity(), caseSensitivity);
        QCOMPARE(filter.isInverted(), inverted);
    }

    {
        QGalleryMetaDataFilter filter(propertyName, value, comparator, caseSensitivity);

        if (inverted)
            filter = !filter;

        QCOMPARE(filter.isValid(), true);

        QCOMPARE(filter.propertyName(), propertyName);
        QCOMPARE(filter.value(), value);
        QCOMPARE(filter.comparator(), comparator);
        QCOMPARE(filter.caseSensitivity(), caseSensitivity);
        QCOMPARE(filter.isInverted(), inverted);
    }
}

void tst_QGalleryFilter::unionFilter()
{
    QList<QGalleryFilter> filters;

    QGalleryMetaDataFilter metaDataFilter;
    QGalleryIntersectionFilter intersectionFilter;

    QGalleryUnionFilter unionFilter;

    QCOMPARE(unionFilter.isValid(), true);
    QCOMPARE(unionFilter.isEmpty(), true);
    QCOMPARE(unionFilter.filterCount(), 0);

    unionFilter.append(metaDataFilter);
    unionFilter.append(metaDataFilter);
    unionFilter.append(intersectionFilter);
    unionFilter.append(metaDataFilter);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 4);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 4);

    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::Intersection);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);

    unionFilter.append(unionFilter);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 8);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 8);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::Intersection);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(6).type(), QGalleryFilter::Intersection);

    unionFilter.insert(1, intersectionFilter);
    unionFilter.insert(2, metaDataFilter);

    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 10);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 10);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::Intersection);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::Intersection);

    unionFilter.insert(0, QGalleryUnionFilter(intersectionFilter));
    unionFilter.insert(4, QGalleryUnionFilter(metaDataFilter));

    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 12);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 12);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::Intersection);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(5).type(), QGalleryFilter::MetaData);

    unionFilter.replace(0, metaDataFilter);
    unionFilter.replace(4, intersectionFilter);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 12);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 12);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::Intersection);
    QCOMPARE(filters.at(5).type(), QGalleryFilter::MetaData);

    unionFilter.removeAt(0);
    unionFilter.removeAt(3);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 10);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 10);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::Intersection);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::Intersection);

    unionFilter.clear();
    QCOMPARE(unionFilter.isEmpty(), true);
    QCOMPARE(unionFilter.filterCount(), 0);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 0);
}

void tst_QGalleryFilter::intersectionFilter()
{
    QList<QGalleryFilter> filters;

    QGalleryMetaDataFilter metaDataFilter;
    QGalleryUnionFilter unionFilter;

    QGalleryIntersectionFilter intersectionFilter;

    QCOMPARE(intersectionFilter.isValid(), true);
    QCOMPARE(intersectionFilter.isEmpty(), true);
    QCOMPARE(intersectionFilter.filterCount(), 0);

    intersectionFilter.append(metaDataFilter);
    intersectionFilter.append(metaDataFilter);
    intersectionFilter.append(unionFilter);
    intersectionFilter.append(metaDataFilter);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 4);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 4);

    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);

    intersectionFilter.append(intersectionFilter);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 8);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 8);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(6).type(), QGalleryFilter::Union);

    intersectionFilter.insert(1, unionFilter);
    intersectionFilter.insert(2, metaDataFilter);

    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 10);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 10);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::Union);

    intersectionFilter.insert(0, QGalleryIntersectionFilter(unionFilter));
    intersectionFilter.insert(4, QGalleryIntersectionFilter(metaDataFilter));

    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 12);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 12);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(5).type(), QGalleryFilter::MetaData);

    intersectionFilter.replace(0, metaDataFilter);
    intersectionFilter.replace(4, unionFilter);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 12);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 12);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(5).type(), QGalleryFilter::MetaData);

    intersectionFilter.removeAt(0);
    intersectionFilter.removeAt(3);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 10);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 10);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::Union);

    intersectionFilter.clear();
    QCOMPARE(intersectionFilter.isEmpty(), true);
    QCOMPARE(intersectionFilter.filterCount(), 0);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 0);
}

void tst_QGalleryFilter::assignment()
{
    QGalleryFilter filter;

    QGalleryMetaDataFilter metaDataFilter;
    QGalleryUnionFilter unionFilter;
    QGalleryIntersectionFilter intersectionFilter;

    QCOMPARE(filter.isValid(), false);
    QCOMPARE(filter.type(), QGalleryFilter::Invalid);

    filter = metaDataFilter;
    QCOMPARE(filter.isValid(), true);
    QCOMPARE(filter.type(), QGalleryFilter::MetaData);

    filter = unionFilter;
    QCOMPARE(filter.isValid(), true);
    QCOMPARE(filter.type(), QGalleryFilter::Union);

    filter = intersectionFilter;
    QCOMPARE(filter.isValid(), true);
    QCOMPARE(filter.type(), QGalleryFilter::Intersection);
}

void tst_QGalleryFilter::copy()
{
    QGalleryMetaDataFilter metaDataFilter;
    QGalleryUnionFilter unionFilter;
    QGalleryIntersectionFilter intersectionFilter;

    {
        QGalleryFilter filter(metaDataFilter);
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.type(), QGalleryFilter::MetaData);
    } {
        QGalleryFilter filter(unionFilter);
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.type(), QGalleryFilter::Union);
    } {
        QGalleryFilter filter(intersectionFilter);
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.type(), QGalleryFilter::Intersection);
    }
}

void tst_QGalleryFilter::copyOnWrite()
{
    QList<QGalleryFilter> filters;

    QGalleryMetaDataFilter metaDataFilter;
    QGalleryUnionFilter unionFilter;
    QGalleryIntersectionFilter intersectionFilter;

    {
        QGalleryMetaDataFilter filter;
        filter.setPropertyName(QLatin1String("albumTitle"));
        filter.setValue(QLatin1String("Greatest Hits"));
        filter.setComparator(QGalleryFilter::EndsWith);
        filter.setCaseSensitivity(Qt::CaseInsensitive);
        filter.setInverted(true);

        metaDataFilter = filter;

        QGalleryMetaDataFilter filterCopy(filter);
        filter.setPropertyName(QLatin1String("artist"));
        filter.setValue(QLatin1String("Self Titled"));
        filter.setComparator(QGalleryFilter::StartsWith);

        QCOMPARE(filterCopy.propertyName(), QLatin1String("albumTitle"));
        QCOMPARE(filterCopy.value(), QVariant(QLatin1String("Greatest Hits")));
        QCOMPARE(filterCopy.comparator(), QGalleryFilter::EndsWith);
        QCOMPARE(filterCopy.caseSensitivity(), Qt::CaseInsensitive);
        QCOMPARE(filterCopy.isInverted(), true);

        QCOMPARE(filter.propertyName(), QLatin1String("artist"));
        QCOMPARE(filter.value(), QVariant(QLatin1String("Self Titled")));
        QCOMPARE(filter.comparator(), QGalleryFilter::StartsWith);
        QCOMPARE(filter.caseSensitivity(), Qt::CaseInsensitive);
        QCOMPARE(filter.isInverted(), true);
    } {
        QGalleryUnionFilter filter;
        filter.append(QGalleryMetaDataFilter());

        unionFilter = filter;

        QGalleryUnionFilter filterCopy(filter);
        filter.append(QGalleryIntersectionFilter());
        filter.append(filterCopy);
        filter.removeAt(0);

        filters = filterCopy.filters();
        QCOMPARE(filters.count(), 1);
        QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);

        filters = filter.filters();
        QCOMPARE(filters.count(), 2);
        QCOMPARE(filters.at(0).type(), QGalleryFilter::Intersection);
        QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    } {
        QGalleryIntersectionFilter filter;
        filter.append(QGalleryMetaDataFilter());

        intersectionFilter = filter;

        QGalleryIntersectionFilter filterCopy(filter);
        filter.append(QGalleryUnionFilter());
        filter.append(filterCopy);
        filter.removeAt(0);

        filters = filterCopy.filters();
        QCOMPARE(filters.count(), 1);
        QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);

        filters = filter.filters();
        QCOMPARE(filters.count(), 2);
        QCOMPARE(filters.at(0).type(), QGalleryFilter::Union);
        QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    }

    QCOMPARE(metaDataFilter.propertyName(), QLatin1String("albumTitle"));
    QCOMPARE(metaDataFilter.value(), QVariant(QLatin1String("Greatest Hits")));
    QCOMPARE(metaDataFilter.comparator(), QGalleryFilter::EndsWith);
    QCOMPARE(metaDataFilter.caseSensitivity(), Qt::CaseInsensitive);
    QCOMPARE(metaDataFilter.isInverted(), true);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 1);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 1);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
}

void tst_QGalleryFilter::cast()
{
    QGalleryFilter metaDataFilter = QGalleryMetaDataFilter();
    QGalleryFilter unionFilter = QGalleryUnionFilter();
    QGalleryFilter intersectionFilter = QGalleryIntersectionFilter();

    QCOMPARE(metaDataFilter.type(), QGalleryFilter::MetaData);
    QCOMPARE(unionFilter.type(), QGalleryFilter::Union);
    QCOMPARE(intersectionFilter.type(), QGalleryFilter::Intersection);

    QCOMPARE(metaDataFilter.isValid(), true);
    QCOMPARE(unionFilter.isValid(), true);
    QCOMPARE(intersectionFilter.isValid(), true);

    QCOMPARE(metaDataFilter.toMetaDataFilter().isValid(), true);
    QCOMPARE(metaDataFilter.toUnionFilter().isValid(), false);
    QCOMPARE(metaDataFilter.toIntersectionFilter().isValid(), false);

    QCOMPARE(unionFilter.toMetaDataFilter().isValid(), false);
    QCOMPARE(unionFilter.toUnionFilter().isValid(), true);
    QCOMPARE(unionFilter.toIntersectionFilter().isValid(), false);

    QCOMPARE(intersectionFilter.toMetaDataFilter().isValid(), false);
    QCOMPARE(intersectionFilter.toUnionFilter().isValid(), false);
    QCOMPARE(intersectionFilter.toIntersectionFilter().isValid(), true);

    QCOMPARE(QGalleryFilter().toMetaDataFilter().isValid(), false);
    QCOMPARE(QGalleryFilter().toUnionFilter().isValid(), false);
    QCOMPARE(QGalleryFilter().toIntersectionFilter().isValid(), false);
}


void tst_QGalleryFilter::intersectionOperator()
{
    QList<QGalleryFilter> filters;

    QGalleryMetaDataFilter metaDataFilter;
    QGalleryUnionFilter unionFilter;
    QGalleryIntersectionFilter intersectionFilter;

    intersectionFilter = metaDataFilter && intersectionFilter;
    QCOMPARE(intersectionFilter.isValid(), true);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 1);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 1);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);

    intersectionFilter = metaDataFilter && unionFilter;
    QCOMPARE(intersectionFilter.isValid(), true);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 2);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 2);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::Union);

    intersectionFilter = unionFilter && metaDataFilter;
    QCOMPARE(intersectionFilter.isValid(), true);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 2);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 2);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);

    intersectionFilter = intersectionFilter && metaDataFilter;
    QCOMPARE(intersectionFilter.isValid(), true);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 3);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 3);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaData);
}

void tst_QGalleryFilter::unionOperator()
{
    QList<QGalleryFilter> filters;

    QGalleryMetaDataFilter metaDataFilter;
    QGalleryIntersectionFilter intersectionFilter;
    QGalleryUnionFilter unionFilter;

    unionFilter = metaDataFilter || unionFilter;
    QCOMPARE(unionFilter.isValid(), true);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 1);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 1);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);

    unionFilter = metaDataFilter || intersectionFilter;
    QCOMPARE(unionFilter.isValid(), true);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 2);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 2);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::Intersection);

    unionFilter = intersectionFilter || metaDataFilter;
    QCOMPARE(unionFilter.isValid(), true);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 2);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 2);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::Intersection);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);

    unionFilter = unionFilter || metaDataFilter;
    QCOMPARE(unionFilter.isValid(), true);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 3);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 3);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::Intersection);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaData);
}

void tst_QGalleryFilter::propertyOperators()
{
    const QGalleryProperty albumProperty("albumTitle");
    const QGalleryProperty trackProperty("trackNumber");

    const QVariant albumTitle = QLatin1String("Self Titled");
    const QVariant track = 3;

    {
        QGalleryMetaDataFilter filter = albumProperty == QLatin1String("Self Titled");
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.propertyName(), albumProperty.name());
        QCOMPARE(filter.value(), albumTitle);
        QCOMPARE(filter.comparator(), QGalleryFilter::Equals);
    } {
        QGalleryMetaDataFilter filter = trackProperty >= 3;
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.propertyName(), trackProperty.name());
        QCOMPARE(filter.value(), track);
        QCOMPARE(filter.comparator(), QGalleryFilter::GreaterThanEquals);
    } {
        QGalleryMetaDataFilter filter = trackProperty > 3;
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.propertyName(), trackProperty.name());
        QCOMPARE(filter.value(), track);
        QCOMPARE(filter.comparator(), QGalleryFilter::GreaterThan);
    } {
        QGalleryMetaDataFilter filter = trackProperty <= 3;
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.propertyName(), trackProperty.name());
        QCOMPARE(filter.value(), track);
        QCOMPARE(filter.comparator(), QGalleryFilter::LessThanEquals);
    } {
        QGalleryMetaDataFilter filter = trackProperty < 3;
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.propertyName(), trackProperty.name());
        QCOMPARE(filter.value(), track);
        QCOMPARE(filter.comparator(), QGalleryFilter::LessThan);
    }
}


void tst_QGalleryFilter::equality_data()
{
    const QGalleryProperty albumProperty("albumTitle");
    const QGalleryProperty artistProperty("artistTitle");
    const QGalleryProperty durationProperty("duration");
    const QGalleryProperty trackProperty("trackNumber");

    QGalleryMetaDataFilter metaDataFilter = albumProperty == QLatin1String("Greatest Hits");
    QGalleryMetaDataFilter metaDataRangeFilter = durationProperty < 12000;
    QGalleryIntersectionFilter unionFilter
            = metaDataFilter
            && artistProperty == QLatin1String("Them")
            && metaDataRangeFilter;
    QGalleryUnionFilter intersectionFilter
            = unionFilter || albumProperty == QLatin1String("Self Titled");

    QTest::addColumn<QGalleryFilter>("filter1");
    QTest::addColumn<QGalleryFilter>("filter2");
    QTest::addColumn<bool>("isEqual");

    QTest::newRow("null filters")
            << QGalleryFilter()
            << QGalleryFilter()
            << true;
    QTest::newRow("null meta-data filters")
            << QGalleryFilter(QGalleryMetaDataFilter())
            << QGalleryFilter(QGalleryMetaDataFilter())
            << true;
    QTest::newRow("null union filters")
            << QGalleryFilter(QGalleryUnionFilter())
            << QGalleryFilter(QGalleryUnionFilter())
            << true;
    QTest::newRow("null intersection filters")
            << QGalleryFilter(QGalleryIntersectionFilter())
            << QGalleryFilter(QGalleryIntersectionFilter())
            << true;

    QTest::newRow("same meta-data filter")
            << QGalleryFilter(metaDataFilter)
            << QGalleryFilter(metaDataFilter)
            << true;
    QTest::newRow("same union filter")
            << QGalleryFilter(unionFilter)
            << QGalleryFilter(unionFilter)
            << true;
    QTest::newRow("same intersection filter")
            << QGalleryFilter(intersectionFilter)
            << QGalleryFilter(intersectionFilter)
            << true;

    QTest::newRow("equal meta-data filters")
            << QGalleryFilter(albumProperty == QLatin1String("Self Titled"))
            << QGalleryFilter(albumProperty == QLatin1String("Self Titled"))
            << true;
    QTest::newRow("equal meta-data range filters")
            << QGalleryFilter(trackProperty < 14)
            << QGalleryFilter(trackProperty < 14)
            << true;
    QTest::newRow("equal union filters")
            << QGalleryFilter(durationProperty > 10000 && metaDataFilter)
            << QGalleryFilter(durationProperty > 10000 && metaDataFilter)
            << true;
    QTest::newRow("equal intersection filters")
            << QGalleryFilter(metaDataFilter || albumProperty == QLatin1String("Self Titled"))
            << QGalleryFilter(metaDataFilter || albumProperty == QLatin1String("Self Titled"))
            << true;

    QTest::newRow("unequal meta-data filter property names")
            << QGalleryFilter(albumProperty == QLatin1String("Self Titled"))
            << QGalleryFilter(artistProperty == QLatin1String("Self Titled"))
            << false;
    QTest::newRow("unequal meta-data filter values")
            << QGalleryFilter(albumProperty == QLatin1String("Self Titled"))
            << QGalleryFilter(albumProperty == QLatin1String("Greatest Hits"))
            << false;
    QTest::newRow("unequal meta-data filter match flags")
            << QGalleryFilter(QGalleryMetaDataFilter(
                    albumProperty, QLatin1String("Self Titled"), QGalleryFilter::Contains))
            << QGalleryFilter(albumProperty == QLatin1String("Self Titled"))
            << false;
    QTest::newRow("unequal meta-data filters")
            << QGalleryFilter(QGalleryMetaDataFilter(
                    albumProperty, QLatin1String("Greatest Hits"), QGalleryFilter::Contains))
            << QGalleryFilter(artistProperty == QLatin1String("Self Titled"))
            << false;

    QTest::newRow("uneuqal meta-data filter property names (less than)")
            << QGalleryFilter(trackProperty < 15)
            << QGalleryFilter(durationProperty < 15)
            << false;
    QTest::newRow("uneuqal meta-data filter property names (greater than")
            << QGalleryFilter(trackProperty > 15)
            << QGalleryFilter(durationProperty > 15)
            << false;
    QTest::newRow("unequal meta-data filter values (less than)")
            << QGalleryFilter(trackProperty < 16)
            << QGalleryFilter(trackProperty < 4)
            << false;
    QTest::newRow("unequal meta-data filter values (greater than)")
            << QGalleryFilter(trackProperty > 15)
            << QGalleryFilter(trackProperty > 3)
            << false;
    QTest::newRow("unequal meta-data filter comparison (less than")
            << QGalleryFilter(trackProperty < 15)
            << QGalleryFilter(trackProperty <= 15)
            << false;
    QTest::newRow("unequal meta-data filter comparison (greater than")
            << QGalleryFilter(trackProperty > 15)
            << QGalleryFilter(trackProperty >= 15)
            << false;
    QTest::newRow("unequal meta-data filter comparison (mixed")
            << QGalleryFilter(trackProperty > 15)
            << QGalleryFilter(trackProperty <= 15)
            << false;
    QTest::newRow("unequal meta-data filter")
            << QGalleryFilter(trackProperty < 3)
            << QGalleryFilter(durationProperty >= 23004)
            << false;

    QTest::newRow("unequal union filter count")
            << QGalleryFilter(unionFilter)
            << QGalleryFilter(unionFilter && metaDataFilter)
            << false;
    QTest::newRow("unequal union filter")
            << QGalleryFilter(albumProperty == QLatin1String("Self Titled")
                    && artistProperty == QLatin1String("Self Titled"))
            << QGalleryFilter(albumProperty == QLatin1String("Greatest Hits")
                    && artistProperty == QLatin1String("Self Titled"))
            << false;

    QTest::newRow("unequal intersection filter count")
            << QGalleryFilter(intersectionFilter)
            << QGalleryFilter(intersectionFilter || metaDataFilter)
            << false;
    QTest::newRow("unequal intersection filter")
            << QGalleryFilter(albumProperty == QLatin1String("Self Titled")
                    || artistProperty == QLatin1String("Self Titled"))
            << QGalleryFilter(albumProperty == QLatin1String("Greatest Hits")
                    || artistProperty == QLatin1String("Self Titled"))
            << false;

    QTest::newRow("null filter != null meta data filter")
            << QGalleryFilter()
            << QGalleryFilter(QGalleryMetaDataFilter())
            << false;
    QTest::newRow("null filter != null union filter")
            << QGalleryFilter()
            << QGalleryFilter(QGalleryUnionFilter())
            << false;
    QTest::newRow("null filter != null intersection filter")
            << QGalleryFilter()
            << QGalleryFilter(QGalleryIntersectionFilter())
            << false;

    QTest::newRow("null filter != populated meta data filter")
            << QGalleryFilter()
            << QGalleryFilter(metaDataFilter)
            << false;
    QTest::newRow("null filter != populated union filter")
            << QGalleryFilter()
            << QGalleryFilter(unionFilter)
            << false;
    QTest::newRow("null filter != populated intersection filter")
            << QGalleryFilter()
            << QGalleryFilter(intersectionFilter)
            << false;

    QTest::newRow("null meta-data filter != populated meta data filter")
            << QGalleryFilter(QGalleryMetaDataFilter())
            << QGalleryFilter(metaDataFilter)
            << false;
    QTest::newRow("null union filter != populated union filter")
            << QGalleryFilter(QGalleryUnionFilter())
            << QGalleryFilter(unionFilter)
            << false;
    QTest::newRow("null intersection filter != populated intersection filter")
            << QGalleryFilter(QGalleryIntersectionFilter())
            << QGalleryFilter(intersectionFilter)
            << false;
    QTest::newRow("null meta-data filter != null union filter")
            << QGalleryFilter(QGalleryMetaDataFilter())
            << QGalleryFilter(QGalleryUnionFilter())
            << false;
    QTest::newRow("null meta-data filter != null intersection filter")
            << QGalleryFilter(QGalleryMetaDataFilter())
            << QGalleryFilter(QGalleryIntersectionFilter())
            << false;
    QTest::newRow("null union filter != null intersection filter")
            << QGalleryFilter(QGalleryUnionFilter())
            << QGalleryFilter(QGalleryIntersectionFilter())
            << false;
}


void tst_QGalleryFilter::equality()
{
    QFETCH(QGalleryFilter, filter1);
    QFETCH(QGalleryFilter, filter2);
    QFETCH(bool, isEqual);

    QCOMPARE(filter1 == filter2, isEqual);
    QCOMPARE(filter2 == filter1, isEqual);
}

void tst_QGalleryFilter::inequality_data()
{
    equality_data();
}

void tst_QGalleryFilter::inequality()
{
    QFETCH(QGalleryFilter, filter1);
    QFETCH(QGalleryFilter, filter2);
    QFETCH(bool, isEqual);

    QCOMPARE(filter1 != filter2, !isEqual);
    QCOMPARE(filter2 != filter1, !isEqual);
}

#ifndef QT_NO_DEBUG_STREAM

#define TST_QGALLERYMETADATAFILTER_DEBUG_TEXT "QGalleryMetaDataFilter(" \
        "propertyName: \"title\" " \
        "comparator: 6 "  \
        "case-sensitive: 1 " \
        "value: QVariant(QString, \"Greatest\") ) "

void tst_QGalleryFilter::debugMessage_data()
{
    QTest::addColumn<QGalleryFilter>("filter");
    QTest::addColumn<QByteArray>("message");

    QTest::newRow("null QGalleryFilter")
            << QGalleryFilter(QGalleryFilter())
            << QByteArray("QGalleryFilter()");
    QTest::newRow("null QGalleryMetaDataFilter")
            << QGalleryFilter(QGalleryMetaDataFilter())
            << QByteArray("QGalleryMetaDataFilter(comparator: 0 case-sensitive: 1)");
    QTest::newRow("null QGalleryUnionFilter")
            << QGalleryFilter(QGalleryUnionFilter())
            << QByteArray("QGalleryUnionFilter()");
    QTest::newRow("null QGalleryIntersectionFilter")
            << QGalleryFilter(QGalleryIntersectionFilter())
            << QByteArray("QGalleryIntersectionFilter()");

    QGalleryMetaDataFilter metaDataFilter;
    metaDataFilter.setPropertyName(QLatin1String("title"));
    metaDataFilter.setValue(QLatin1String("Greatest"));
    metaDataFilter.setComparator(QGalleryFilter::StartsWith);

    QTest::newRow("Populated QGalleryMetaDataFilter")
            << QGalleryFilter(metaDataFilter)
            << QByteArray(TST_QGALLERYMETADATAFILTER_DEBUG_TEXT);

    QTest::newRow("Inverse Populated QGallleryMetaDataFilter")
            << QGalleryFilter(!metaDataFilter)
            << QByteArray("!"TST_QGALLERYMETADATAFILTER_DEBUG_TEXT);

    QGalleryUnionFilter unionFilter;
    unionFilter.append(metaDataFilter);

    QTest::newRow("Single Child QGalleryUnionFilter")
            << QGalleryFilter(unionFilter)
            << QByteArray("QGalleryUnionFilter("
                TST_QGALLERYMETADATAFILTER_DEBUG_TEXT
                ") ");

    QGalleryIntersectionFilter intersectionFilter;
    intersectionFilter.append(metaDataFilter);

    QTest::newRow("Single Child QGalleryIntersectionFilter")
            << QGalleryFilter(intersectionFilter)
            << QByteArray("QGalleryIntersectionFilter("
                TST_QGALLERYMETADATAFILTER_DEBUG_TEXT
                ") ");

    unionFilter.append(!metaDataFilter);

    QTest::newRow("Multiple Child QGalleryUnionFilter")
            << QGalleryFilter(unionFilter)
            << QByteArray("QGalleryUnionFilter("
                TST_QGALLERYMETADATAFILTER_DEBUG_TEXT
                " && "
                "!"TST_QGALLERYMETADATAFILTER_DEBUG_TEXT
                ") ");

    intersectionFilter.append(!metaDataFilter);

    QTest::newRow("Multiple Child QGalleryIntersectionFilter")
            << QGalleryFilter(intersectionFilter)
            << QByteArray("QGalleryIntersectionFilter("
                TST_QGALLERYMETADATAFILTER_DEBUG_TEXT
                " || "
                "!"TST_QGALLERYMETADATAFILTER_DEBUG_TEXT
                ") ");
}

#undef TST_QGALLERYMETADATAFILTER_DEBUG_TEXT

void tst_QGalleryFilter::debugMessage()
{
    QFETCH(QGalleryFilter, filter);
    QFETCH(QByteArray, message);

    QTest::ignoreMessage(QtDebugMsg, message.constData());
    qDebug() << filter;
}
#endif

QTEST_MAIN(tst_QGalleryFilter)

#include "tst_qgalleryfilter.moc"
