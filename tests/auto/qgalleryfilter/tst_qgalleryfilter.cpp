/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=src/documentgallery

#include <qgalleryfilter.h>
#include <qgalleryproperty.h>

#include <QtTest/QtTest>

Q_DECLARE_METATYPE(QVariant);
Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QGalleryFilter))
Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QGalleryFilter::Comparator))
Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QGalleryMetaDataFilter))

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
    void intersectionStreamOperator();
    void unionStreamOperator();
    void propertyOperators_data();
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
    QTest::addColumn<bool>("negated");

    QTest::newRow("album title")
            << QString::fromLatin1("albumTitle")
            << QVariant(QString::fromLatin1("Greatest"))
            << QGalleryFilter::StartsWith
            << false;

    QTest::newRow("not album title")
            << QString::fromLatin1("albumTitle")
            << QVariant(QString::fromLatin1("Greatest"))
            << QGalleryFilter::StartsWith
            << false;
}

void tst_QGalleryFilter::metaDataFilter()
{
    QFETCH(QString, propertyName);
    QFETCH(QVariant, value);
    QFETCH(QGalleryFilter::Comparator, comparator);
    QFETCH(bool, negated);

    {
        QGalleryMetaDataFilter filter;

        QCOMPARE(filter.isValid(), true);
        QCOMPARE(filter.propertyName(), QString());
        QCOMPARE(filter.value(), QVariant());
        QCOMPARE(filter.comparator(), QGalleryFilter::Equals);
        QCOMPARE(filter.isNegated(), false);

        filter.setPropertyName(propertyName);
        filter.setValue(value);
        filter.setComparator(comparator);
        filter.setNegated(negated);

        QCOMPARE(filter.propertyName(), propertyName);
        QCOMPARE(filter.value(), value);
        QCOMPARE(filter.comparator(), comparator);
        QCOMPARE(filter.isNegated(), negated);
    }

    {
        QGalleryMetaDataFilter filter(propertyName, value, comparator);

        if (negated)
            filter = !filter;

        QCOMPARE(filter.isValid(), true);

        QCOMPARE(filter.propertyName(), propertyName);
        QCOMPARE(filter.value(), value);
        QCOMPARE(filter.comparator(), comparator);
        QCOMPARE(filter.isNegated(), negated);
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

    unionFilter.remove(0);
    unionFilter.remove(3);
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

    unionFilter.prepend(metaDataFilter);
    unionFilter.prepend(metaDataFilter);
    unionFilter.prepend(intersectionFilter);
    unionFilter.prepend(metaDataFilter);
    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 4);

    filters = unionFilter.filters();
    QCOMPARE(filters.count(), 4);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::Intersection);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);
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

    intersectionFilter.remove(0);
    intersectionFilter.remove(3);
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

    intersectionFilter.prepend(metaDataFilter);
    intersectionFilter.prepend(metaDataFilter);
    intersectionFilter.prepend(unionFilter);
    intersectionFilter.prepend(metaDataFilter);
    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 4);

    filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 4);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);
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
        filter.setNegated(true);

        metaDataFilter = filter;

        QGalleryMetaDataFilter filterCopy(filter);
        filter.setPropertyName(QLatin1String("artist"));
        filter.setValue(QLatin1String("Self Titled"));
        filter.setComparator(QGalleryFilter::StartsWith);

        QCOMPARE(filterCopy.propertyName(), QLatin1String("albumTitle"));
        QCOMPARE(filterCopy.value(), QVariant(QLatin1String("Greatest Hits")));
        QCOMPARE(filterCopy.comparator(), QGalleryFilter::EndsWith);
        QCOMPARE(filterCopy.isNegated(), true);

        QCOMPARE(filter.propertyName(), QLatin1String("artist"));
        QCOMPARE(filter.value(), QVariant(QLatin1String("Self Titled")));
        QCOMPARE(filter.comparator(), QGalleryFilter::StartsWith);
        QCOMPARE(filter.isNegated(), true);
    } {
        QGalleryUnionFilter filter;
        filter.append(QGalleryMetaDataFilter());

        unionFilter = filter;

        QGalleryUnionFilter filterCopy(filter);
        filter.append(QGalleryIntersectionFilter());
        filter.append(filterCopy);
        filter.remove(0);

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
        filter.remove(0);

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
    QCOMPARE(metaDataFilter.isNegated(), true);

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

void tst_QGalleryFilter::intersectionStreamOperator()
{
    QGalleryMetaDataFilter metaDataFilter;
    QGalleryUnionFilter unionFilter;

    QGalleryIntersectionFilter intersectionFilter = QGalleryIntersectionFilter()
            << metaDataFilter
            << metaDataFilter
            << unionFilter
            << metaDataFilter;

    QCOMPARE(intersectionFilter.isEmpty(), false);
    QCOMPARE(intersectionFilter.filterCount(), 4);

    QList<QGalleryFilter> filters = intersectionFilter.filters();
    QCOMPARE(filters.count(), 4);
    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::Union);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);
}

void tst_QGalleryFilter::unionStreamOperator()
{
    QGalleryMetaDataFilter metaDataFilter;
    QGalleryIntersectionFilter intersectionFilter;

    QGalleryUnionFilter unionFilter = QGalleryUnionFilter()
            <<metaDataFilter
            <<metaDataFilter
            <<intersectionFilter
            <<metaDataFilter;

    QCOMPARE(unionFilter.isEmpty(), false);
    QCOMPARE(unionFilter.filterCount(), 4);

    QList<QGalleryFilter> filters = unionFilter.filters();
    QCOMPARE(filters.count(), 4);

    QCOMPARE(filters.at(0).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(1).type(), QGalleryFilter::MetaData);
    QCOMPARE(filters.at(2).type(), QGalleryFilter::Intersection);
    QCOMPARE(filters.at(3).type(), QGalleryFilter::MetaData);
}

void tst_QGalleryFilter::propertyOperators_data()
{
    QTest::addColumn<QGalleryMetaDataFilter>("filter");
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QGalleryFilter::Comparator>("comparator");

    const QGalleryProperty albumTitle = {"albumTitle", sizeof("albumTitle")};
    const QGalleryProperty trackNumber = {"trackNumber", sizeof("trackNumber")};

    QTest::newRow("albumTitle == Self Titled")
            << (albumTitle == QLatin1String("Self Titled"))
            << "albumTitle"
            << QVariant(QLatin1String("Self Titled"))
            << QGalleryFilter::Equals;

    QTest::newRow("trackNumber >= 3")
            << (trackNumber >= 3)
            << "trackNumber"
            << QVariant(3)
            << QGalleryFilter::GreaterThanEquals;

    QTest::newRow("trackNumber > 3")
            << (trackNumber > 3)
            << "trackNumber"
            << QVariant(3)
            << QGalleryFilter::GreaterThan;

    QTest::newRow("trackNumber <= 3")
            << (trackNumber <= 3)
            << "trackNumber"
            << QVariant(3)
            << QGalleryFilter::LessThanEquals;

    QTest::newRow("trackNumber < 3")
            << (trackNumber < 3)
            << "trackNumber"
            << QVariant(3)
            << QGalleryFilter::LessThan;

    QTest::newRow("albumTitle.contains(lf Titl)")
            << albumTitle.contains(QLatin1String("lf Titl"))
            << "albumTitle"
            << QVariant(QLatin1String("lf Titl"))
            << QGalleryFilter::Contains;

    QTest::newRow("albumTitle.startsWith(Self)")
            << albumTitle.startsWith(QLatin1String("Self"))
            << "albumTitle"
            << QVariant(QLatin1String("Self"))
            << QGalleryFilter::StartsWith;

    QTest::newRow("albumTitle.endsWith(Self)")
            << albumTitle.endsWith(QLatin1String("Titled"))
            << "albumTitle"
            << QVariant(QLatin1String("Titled"))
            << QGalleryFilter::EndsWith;

    QTest::newRow("albumTitle.wildcard(S*f T*d)")
            << albumTitle.wildcard(QLatin1String("S*f T*d"))
            << "albumTitle"
            << QVariant(QLatin1String("S*f T*d"))
            << QGalleryFilter::Wildcard;

    QTest::newRow("albumTitle.regExp((Self Titled|Greatest Hits))")
            << albumTitle.regExp(QLatin1String("(Self Titled|Greatest Hits)"))
            << "albumTitle"
            << QVariant(QLatin1String("(Self Titled|Greatest Hits)"))
            << QGalleryFilter::RegExp;

    QTest::newRow("albumTitle.regExp(QRegExp((Self Titled|Greatest Hits)))")
            << albumTitle.regExp(QRegExp(QLatin1String("(Self Titled|Greatest Hits)")))
            << "albumTitle"
            << QVariant(QRegExp(QLatin1String("(Self Titled|Greatest Hits)")))
            << QGalleryFilter::RegExp;
}

void tst_QGalleryFilter::propertyOperators()
{
    QFETCH(QGalleryMetaDataFilter, filter);
    QFETCH(QString, propertyName);
    QFETCH(QVariant, value);
    QFETCH(QGalleryFilter::Comparator, comparator);

    QCOMPARE(filter.isValid(), true);
    QCOMPARE(filter.propertyName(), propertyName);
    QCOMPARE(filter.value(), value);
    QCOMPARE(filter.comparator(), comparator);
}

void tst_QGalleryFilter::equality_data()
{
    const QGalleryProperty albumProperty = {"albumTitle", sizeof("albumTitle")};
    const QGalleryProperty artistProperty = {"artistTitle", sizeof("artistTitle>")};
    const QGalleryProperty durationProperty = {"duration", sizeof("duration")};
    const QGalleryProperty trackProperty = {"trackNumber", sizeof("trackNumber")};

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
            << QGalleryFilter(albumProperty.contains(QLatin1String("Self Titled")))
            << QGalleryFilter(albumProperty == QLatin1String("Self Titled"))
            << false;
    QTest::newRow("unequal meta-data filters")
            << QGalleryFilter(albumProperty.contains(QLatin1String("Greatest Hits")))
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
    QTest::newRow("unequal meta-data filter (negated")
            << QGalleryFilter(metaDataFilter)
            << QGalleryFilter(!metaDataFilter)
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
            << QByteArray("QGalleryMetaDataFilter(comparator: 0)");
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
