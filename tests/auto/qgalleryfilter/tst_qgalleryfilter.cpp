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

#include <QtTest/QtTest>

#include <qgalleryfilter.h>
#include <qgalleryproperty.h>

Q_DECLARE_METATYPE(Qt::MatchFlags);
Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QGalleryFilter))

QTM_USE_NAMESPACE

class tst_QGalleryFilter : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void nullFilter();
    void metaDataFilter_data();
    void metaDataFilter();
    void metaDataRangeFilter_data();
    void metaDataRangeFilter();
    void unionFilter();
    void intersectionFilter();
    void assignment();
    void copy();
    void cast();
    void intersectionOperator();
    void unionOperator();
    void propertyOperators();
    void equality_data();
    void equality();
    void inequality_data();
    void inequality();
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
    QTest::addColumn<Qt::MatchFlags>("flags");

    QTest::newRow("album title")
            << QString::fromLatin1("albumTitle")
            << QVariant(QString::fromLatin1("Greatest Hits"))
            << Qt::MatchFlags(Qt::MatchCaseSensitive);
}

void tst_QGalleryFilter::metaDataFilter()
{
    QFETCH(QString, propertyName);
    QFETCH(QVariant, value);
    QFETCH(Qt::MatchFlags, flags);

    {
        QGalleryMetaDataFilter filter;

        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.fieldName(), QString());
        QCOMPARE(filter.value(), QVariant());
        QCOMPARE(filter.matchFlags(), Qt::MatchExactly);

        filter.setFieldName(propertyName);
        filter.setValue(value);
        filter.setMatchFlags(flags);

        QCOMPARE(filter.fieldName(), propertyName);
        QCOMPARE(filter.value(), value);
        QCOMPARE(filter.matchFlags(), flags);
    }

    {
        QGalleryMetaDataFilter filter(propertyName, value, flags);

        QCOMPARE(filter.isValid(), true);

        QCOMPARE(filter.fieldName(), propertyName);
        QCOMPARE(filter.value(), value);
        QCOMPARE(filter.matchFlags(), flags);
    }
}

void tst_QGalleryFilter::metaDataRangeFilter_data()
{
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVariant>("minimum");
    QTest::addColumn<QVariant>("maximum");

    QTest::newRow("track number")
            << QString::fromLatin1("trackNumber")
            << QVariant(2)
            << QVariant(9);
}

void tst_QGalleryFilter::metaDataRangeFilter()
{
    QFETCH(QString, propertyName);
    QFETCH(QVariant, minimum);
    QFETCH(QVariant, maximum);

    QGalleryMetaDataRangeFilter filter;

    QCOMPARE(filter.isValid(), true);

    QCOMPARE(filter.fieldName(), QString());
    QCOMPARE(filter.minimumValue(), QVariant());
    QCOMPARE(filter.maximumValue(), QVariant());
    QCOMPARE(filter.rangeFlags(), QGalleryFilter::RangeFlags());

    filter.setFieldName(propertyName);
    QCOMPARE(filter.fieldName(), propertyName);

    filter.setExclusiveRange(minimum, maximum);
    QCOMPARE(filter.minimumValue(), minimum);
    QCOMPARE(filter.maximumValue(), maximum);
    QCOMPARE(filter.rangeFlags(), QGalleryFilter::ExclusiveRange);

    filter.setLessThan(maximum);
    QCOMPARE(filter.minimumValue(), QVariant());
    QCOMPARE(filter.maximumValue(), maximum);
    QCOMPARE(filter.rangeFlags(), QGalleryFilter::LessThanMaximum);

    filter.setGreaterThan(minimum);
    QCOMPARE(filter.minimumValue(), minimum);
    QCOMPARE(filter.maximumValue(), QVariant());
    QCOMPARE(filter.rangeFlags(), QGalleryFilter::GreaterThanMinimum);

    filter.setLessThanEquals(maximum);
    QCOMPARE(filter.minimumValue(), QVariant());
    QCOMPARE(filter.maximumValue(), maximum);
    QCOMPARE(filter.rangeFlags(), QGalleryFilter::LessThanEqualsMaximum);

    filter.setGreaterThanEquals(minimum);
    QCOMPARE(filter.minimumValue(), minimum);
    QCOMPARE(filter.maximumValue(), QVariant());
    QCOMPARE(filter.rangeFlags(), QGalleryFilter::GreaterThanEqualsMinimum);

    filter.setInclusiveRange(minimum, maximum);
    QCOMPARE(filter.minimumValue(), minimum);
    QCOMPARE(filter.maximumValue(), maximum);
    QCOMPARE(filter.rangeFlags(), QGalleryFilter::InclusiveRange);
}

void tst_QGalleryFilter::unionFilter()
{
    QList<QGalleryFilter> filters;

    QGalleryMetaDataFilter metaDataFilter;
    QGalleryMetaDataRangeFilter metaDataRangeFilter;

    QGalleryUnionFilter unionFilter;

    QCOMPARE(unionFilter.isValid(), true);
    QCOMPARE(unionFilter.isEmpty(), true);
    QCOMPARE(unionFilter.filterCount(), 0);

    unionFilter.append(metaDataFilter);
    unionFilter.append(metaDataFilter);
    unionFilter.append(metaDataRangeFilter);
    unionFilter.append(metaDataFilter);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 4);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 4);

    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);

    unionFilter.append(unionFilter);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 8);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 8);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(6).type(), QGalleryFilter::MetaDataRange);

    unionFilter.insert(1, metaDataRangeFilter);
    unionFilter.insert(2, metaDataFilter);

    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 10);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 10);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::MetaDataRange);

    unionFilter.insert(0, QGalleryUnionFilter(metaDataRangeFilter));
    unionFilter.insert(4, QGalleryUnionFilter(metaDataFilter));

    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 12);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 12);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(5).type(), QGalleryFilter::MetaData);

    unionFilter.replace(0, metaDataFilter);
    unionFilter.replace(4, metaDataRangeFilter);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 12);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 12);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(5).type(), QGalleryFilter::MetaData);

    unionFilter.removeAt(0);
    unionFilter.removeAt(3);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 10);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 10);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::MetaDataRange);

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
    QGalleryMetaDataRangeFilter metaDataRangeFilter;
    QGalleryUnionFilter unionFilter;

    QGalleryIntersectionFilter  intersectionFilter;

    QCOMPARE(intersectionFilter.isValid(), true);
    QCOMPARE(intersectionFilter.isEmpty(), true);
    QCOMPARE(intersectionFilter.filterCount(), 0);

    intersectionFilter.append(metaDataFilter);
    intersectionFilter.append(metaDataRangeFilter);
    intersectionFilter.append(unionFilter);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 3);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 3);

    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::Union);

    intersectionFilter.append(intersectionFilter);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 6);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 6);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(5).type(), QGalleryFilter::Union);

    intersectionFilter.insert(0, unionFilter);
    intersectionFilter.insert(1, metaDataRangeFilter);
    intersectionFilter.insert(2, metaDataFilter);

    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 9);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 9);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);

    intersectionFilter.insert(2, QGalleryIntersectionFilter(metaDataFilter));
    intersectionFilter.insert(3, QGalleryIntersectionFilter(metaDataRangeFilter));
    intersectionFilter.insert(4, QGalleryIntersectionFilter(unionFilter));
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 12);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 12);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(4).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(5).type(), QGalleryFilter::MetaData);

    intersectionFilter.replace(2, unionFilter);
    intersectionFilter.replace(3, metaDataFilter);
    intersectionFilter.replace(5, metaDataRangeFilter);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 12);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 12);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(5).type(), QGalleryFilter::MetaDataRange);

    intersectionFilter.removeAt(0);
    intersectionFilter.removeAt(3);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 10);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 10);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaDataRange);

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
    QGalleryMetaDataRangeFilter metaDataRangeFilter;
    QGalleryUnionFilter unionFilter;
    QGalleryIntersectionFilter intersectionFilter;

    QCOMPARE(filter.isValid(), false);
    QCOMPARE(filter.type(), QGalleryFilter::Invalid);

    filter = metaDataFilter;
    QCOMPARE(filter.isValid(), true);
    QCOMPARE(filter.type(), QGalleryFilter::MetaData);

    filter = metaDataRangeFilter;
    QCOMPARE(filter.isValid(), true);
    QCOMPARE(filter.type(), QGalleryFilter::MetaDataRange);

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
    QGalleryMetaDataRangeFilter metaDataRangeFilter;
    QGalleryUnionFilter unionFilter;
    QGalleryIntersectionFilter intersectionFilter;

    {
        QGalleryFilter filter(metaDataFilter);
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.type(), QGalleryFilter::MetaData);
    } {
        QGalleryFilter filter(metaDataRangeFilter);
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.type(), QGalleryFilter::MetaDataRange);
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

void tst_QGalleryFilter::cast()
{
    QGalleryFilter metaDataFilter = QGalleryMetaDataFilter();
    QGalleryFilter metaDataRangeFilter = QGalleryMetaDataRangeFilter();
    QGalleryFilter unionFilter = QGalleryUnionFilter();
    QGalleryFilter intersectionFilter = QGalleryIntersectionFilter();

    QCOMPARE(metaDataFilter.type(), QGalleryFilter::MetaData);
    QCOMPARE(metaDataRangeFilter.type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(unionFilter.type(), QGalleryFilter::Union);
    QCOMPARE(intersectionFilter.type(), QGalleryFilter::Intersection);

    QCOMPARE(metaDataFilter.isValid(), true);
    QCOMPARE(metaDataRangeFilter.isValid(), true);
    QCOMPARE(unionFilter.isValid(), true);
    QCOMPARE(intersectionFilter.isValid(), true);

    QCOMPARE(metaDataFilter.toMetaDataFilter().isValid(), true);
    QCOMPARE(metaDataFilter.toMetaDataRangeFilter().isValid(), false);
    QCOMPARE(metaDataFilter.toUnionFilter().isValid(), false);
    QCOMPARE(metaDataFilter.toIntersectionFilter().isValid(), false);

    QCOMPARE(metaDataRangeFilter.toMetaDataFilter().isValid(), false);
    QCOMPARE(metaDataRangeFilter.toMetaDataRangeFilter().isValid(), true);
    QCOMPARE(metaDataRangeFilter.toUnionFilter().isValid(), false);
    QCOMPARE(metaDataRangeFilter.toIntersectionFilter().isValid(), false);

    QCOMPARE(unionFilter.toMetaDataFilter().isValid(), false);
    QCOMPARE(unionFilter.toMetaDataRangeFilter().isValid(), false);
    QCOMPARE(unionFilter.toUnionFilter().isValid(), true);
    QCOMPARE(unionFilter.toIntersectionFilter().isValid(), false);

    QCOMPARE(intersectionFilter.toMetaDataFilter().isValid(), false);
    QCOMPARE(intersectionFilter.toMetaDataRangeFilter().isValid(), false);
    QCOMPARE(intersectionFilter.toUnionFilter().isValid(), false);
    QCOMPARE(intersectionFilter.toIntersectionFilter().isValid(), true);
}

void tst_QGalleryFilter::intersectionOperator()
{
    QList<QGalleryFilter> filters;

    QGalleryMetaDataFilter metaDataFilter;
    QGalleryMetaDataRangeFilter metaDataRangeFilter;
    QGalleryUnionFilter unionFilter;
    QGalleryIntersectionFilter intersectionFilter;

    intersectionFilter = metaDataFilter || intersectionFilter;
    QCOMPARE(intersectionFilter.isValid(), true);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 1);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 1);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);

    intersectionFilter = metaDataRangeFilter || unionFilter;
    QCOMPARE(intersectionFilter.isValid(), true);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 2);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 2);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::Union);

    intersectionFilter = unionFilter || metaDataRangeFilter;
    QCOMPARE(intersectionFilter.isValid(), true);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 2);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 2);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaDataRange);

    intersectionFilter = intersectionFilter || metaDataFilter;
    QCOMPARE(intersectionFilter.isValid(), true);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 3);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 3);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaData);
}

void tst_QGalleryFilter::unionOperator()
{
    QList<QGalleryFilter> filters;

    QGalleryMetaDataFilter metaDataFilter;
    QGalleryMetaDataRangeFilter metaDataRangeFilter;
    QGalleryUnionFilter unionFilter;

    unionFilter = metaDataFilter && unionFilter;
    QCOMPARE(unionFilter.isValid(), true);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 1);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 1);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);

    unionFilter = metaDataRangeFilter && metaDataFilter;
    QCOMPARE(unionFilter.isValid(), true);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 2);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 2);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);

    unionFilter = unionFilter && metaDataRangeFilter;
    QCOMPARE(unionFilter.isValid(), true);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 3);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 3);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaDataRange);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaDataRange);
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
        QCOMPARE(filter.fieldName(), albumProperty.name());
        QCOMPARE(filter.value(), albumTitle);
        QCOMPARE(filter.matchFlags(), Qt::MatchExactly);
    } {
        QGalleryMetaDataRangeFilter filter = trackProperty >= 3;
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.fieldName(), trackProperty.name());
        QCOMPARE(filter.minimumValue(), track);
        QCOMPARE(filter.maximumValue(), QVariant());
        QCOMPARE(filter.rangeFlags(), QGalleryFilter::GreaterThanEqualsMinimum);
    } {
        QGalleryMetaDataRangeFilter filter = trackProperty > 3;
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.fieldName(), trackProperty.name());
        QCOMPARE(filter.minimumValue(), track);
        QCOMPARE(filter.maximumValue(), QVariant());
        QCOMPARE(filter.rangeFlags(), QGalleryFilter::GreaterThanMinimum);
    } {
        QGalleryMetaDataRangeFilter filter = trackProperty <= 3;
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.fieldName(), trackProperty.name());
        QCOMPARE(filter.minimumValue(), QVariant());
        QCOMPARE(filter.maximumValue(), track);
        QCOMPARE(filter.rangeFlags(), QGalleryFilter::LessThanEqualsMaximum);
    } {
        QGalleryMetaDataRangeFilter filter = trackProperty < 3;
        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.fieldName(), trackProperty.name());
        QCOMPARE(filter.minimumValue(), QVariant());
        QCOMPARE(filter.maximumValue(), track);
        QCOMPARE(filter.rangeFlags(), QGalleryFilter::LessThanMaximum);
    }
}

void tst_QGalleryFilter::equality_data()
{
    const QGalleryProperty albumProperty("albumTitle");
    const QGalleryProperty artistProperty("artistTitle");
    const QGalleryProperty durationProperty("duration");
    const QGalleryProperty trackProperty("trackNumber");

    QGalleryMetaDataFilter metaDataFilter = albumProperty == QLatin1String("Greatest Hits");
    QGalleryMetaDataRangeFilter metaDataRangeFilter = durationProperty < 12000;
    QGalleryUnionFilter unionFilter
            = metaDataFilter
            && artistProperty == QLatin1String("Them")
            && metaDataRangeFilter;
    QGalleryIntersectionFilter intersectionFilter
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
    QTest::newRow("null meta-data range filters")
            << QGalleryFilter(QGalleryMetaDataRangeFilter())
            << QGalleryFilter(QGalleryMetaDataRangeFilter())
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
    QTest::newRow("same meta-data range filter")
            << QGalleryFilter(metaDataRangeFilter)
            << QGalleryFilter(metaDataRangeFilter)
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
            << QGalleryFilter(durationProperty > 10000 && metaDataRangeFilter)
            << QGalleryFilter(durationProperty > 10000 && metaDataRangeFilter)
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
                    albumProperty, QLatin1String("Self Titled"), Qt::MatchContains))
            << QGalleryFilter(albumProperty == QLatin1String("Self Titled"))
            << false;
    QTest::newRow("unequal meta-data filters")
            << QGalleryFilter(QGalleryMetaDataFilter(
                    albumProperty, QLatin1String("Greatest Hits"), Qt::MatchContains))
            << QGalleryFilter(artistProperty == QLatin1String("Self Titled"))
            << false;

    QTest::newRow("uneuqal meta-data range filter property names (less than)")
            << QGalleryFilter(trackProperty < 15)
            << QGalleryFilter(durationProperty < 15)
            << false;
    QTest::newRow("uneuqal meta-data range filter property names (greater than")
            << QGalleryFilter(trackProperty > 15)
            << QGalleryFilter(durationProperty > 15)
            << false;
    QTest::newRow("unequal meta-data range filter values (less than)")
            << QGalleryFilter(trackProperty < 16)
            << QGalleryFilter(trackProperty < 4)
            << false;
    QTest::newRow("unequal meta-data range filter values (greater than)")
            << QGalleryFilter(trackProperty > 15)
            << QGalleryFilter(trackProperty > 3)
            << false;
    QTest::newRow("unequal meta-data range filter comparison (less than")
            << QGalleryFilter(trackProperty < 15)
            << QGalleryFilter(trackProperty <= 15)
            << false;
    QTest::newRow("unequal meta-data range filter comparison (greater than")
            << QGalleryFilter(trackProperty > 15)
            << QGalleryFilter(trackProperty >= 15)
            << false;
    QTest::newRow("unequal meta-data range filter comparison (mixed")
            << QGalleryFilter(trackProperty > 15)
            << QGalleryFilter(trackProperty <= 15)
            << false;
    QTest::newRow("unequal meta-data range filter")
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
    QTest::newRow("null range filter != null meta data range filter")
            << QGalleryFilter()
            << QGalleryFilter(QGalleryMetaDataRangeFilter())
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
    QTest::newRow("null range filter != populated meta data range filter")
            << QGalleryFilter()
            << QGalleryFilter(metaDataRangeFilter)
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
    QTest::newRow("null meta-data range filter != populated meta data range filter")
            << QGalleryFilter(QGalleryMetaDataRangeFilter())
            << QGalleryFilter(metaDataRangeFilter)
            << false;
    QTest::newRow("null union filter != populated union filter")
            << QGalleryFilter(QGalleryUnionFilter())
            << QGalleryFilter(unionFilter)
            << false;
    QTest::newRow("null intersection filter != populated intersection filter")
            << QGalleryFilter(QGalleryIntersectionFilter())
            << QGalleryFilter(intersectionFilter)
            << false;

    QTest::newRow("null meta-data filter != null meta-data range filter")
            << QGalleryFilter(QGalleryMetaDataFilter())
            << QGalleryFilter(QGalleryMetaDataRangeFilter())
            << false;
    QTest::newRow("null meta-data filter != null union filter")
            << QGalleryFilter(QGalleryMetaDataFilter())
            << QGalleryFilter(QGalleryUnionFilter())
            << false;
    QTest::newRow("null meta-data filter != null intersection filter")
            << QGalleryFilter(QGalleryMetaDataFilter())
            << QGalleryFilter(QGalleryIntersectionFilter())
            << false;
    QTest::newRow("null meta-data range filter != null union filter")
            << QGalleryFilter(QGalleryMetaDataRangeFilter())
            << QGalleryFilter(QGalleryUnionFilter())
            << false;
    QTest::newRow("null meta-data range filter != null intersection filter")
            << QGalleryFilter(QGalleryMetaDataRangeFilter())
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

QTEST_MAIN(tst_QGalleryFilter)

#include "tst_qgalleryfilter.moc"
