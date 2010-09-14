/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QDECLARATIVEORGANIZERITEMFILTER_P_H
#define QDECLARATIVEORGANIZERITEMFILTER_P_H

#include <qdeclarative.h>
#include <QDeclarativeExtensionPlugin>
#include <qorganizeritemfilters.h>
#include <QDeclarativeListProperty>

QTM_USE_NAMESPACE


class QDeclarativeOrganizerItemFilter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(FilterType type READ type NOTIFY typeChanged)

    Q_ENUMS(FilterType);
    Q_FLAGS(MatchFlags);
public:
    QDeclarativeOrganizerItemFilter(QObject *parent=0)
        :QObject(parent)
    {
    }

    enum FilterType {
        InvalidFilter = QOrganizerItemFilter::InvalidFilter,
        OrganizerItemDetailFilter = QOrganizerItemFilter::OrganizerItemDetailFilter,
        OrganizerItemDetailRangeFilter = QOrganizerItemFilter::OrganizerItemDetailRangeFilter,
        OrganizerItemDateTimePeriodFilter = QOrganizerItemFilter::OrganizerItemDateTimePeriodFilter,
        ChangeLogFilter = QOrganizerItemFilter::ChangeLogFilter,
        ActionFilter = QOrganizerItemFilter::ActionFilter,
        IntersectionFilter = QOrganizerItemFilter::IntersectionFilter,
        UnionFilter = QOrganizerItemFilter::UnionFilter,
        LocalIdFilter = QOrganizerItemFilter::LocalIdFilter,
        CollectionFilter = QOrganizerItemFilter::CollectionFilter,
        DefaultFilter = QOrganizerItemFilter::DefaultFilter
    };

    FilterType type() const {
        return filter().type();
    }


    enum MatchFlag {
        MatchExactly = QOrganizerItemFilter::MatchExactly,
        MatchContains = QOrganizerItemFilter::MatchContains,
        MatchStartsWith = QOrganizerItemFilter::MatchStartsWith,
        MatchEndsWith = QOrganizerItemFilter::MatchEndsWith,
        MatchFixedString = QOrganizerItemFilter::MatchFixedString,
        MatchCaseSensitive = QOrganizerItemFilter::MatchCaseSensitive
    };
    Q_DECLARE_FLAGS(MatchFlags, MatchFlag)

    virtual QOrganizerItemFilter filter() const
    {
        return QOrganizerItemFilter();
    }

signals:
    void typeChanged();
    void valueChanged();
};

QML_DECLARE_TYPE(QDeclarativeOrganizerItemFilter)


//changelog filter
class QDeclarativeOrganizerItemChangelogFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
    Q_PROPERTY(QDateTime since READ since WRITE setSince NOTIFY valueChanged)
    Q_PROPERTY(EventType eventType READ eventType WRITE setEventType NOTIFY valueChanged)

    Q_ENUMS(EventType)

public:
    enum EventType {
        EventAdded = QOrganizerItemChangeLogFilter::EventAdded,
        EventChanged = QOrganizerItemChangeLogFilter::EventChanged,
        EventRemoved = QOrganizerItemChangeLogFilter::EventRemoved
    };

    QDeclarativeOrganizerItemChangelogFilter(QObject *parent)
        :QDeclarativeOrganizerItemFilter(parent)
    {
    }

    QDateTime since() const { return d.since(); }
    void setSince(const QDateTime& since) { d.setSince(since); }

    EventType eventType() const { return d.eventType(); }
    void setEventType(EventType type) { d.setEventType(type); }

    QDeclarativeOrganizerItemFilter filter() const
    {
        return d;
    }

signals:
    void valueChanged();

private:
    QOrganizerItemChangeLogFilter d;

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemChangelogFilter)

//collection filter
class QDeclarativeOrganizerItemCollectionFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QOrganizerCollectionLocalId> ids READ ids NOTIFY valueChanged)
    Q_CLASSINFO("DefaultProperty", "ids")
public:
    QDeclarativeOrganizerItemCollectionFilter(QObject *parent)
        :QDeclarativeOrganizerItemFilter(parent)
    {
    }

    QDeclarativeListProperty<QOrganizerCollectionLocalId> ids()
    {
        return QDeclarativeListProperty<QOrganizerCollectionLocalId>(this, m_ids);
    }

    QDeclarativeOrganizerItemFilter filter() const
    {
        d.setCollectionIds(m_ids.toSet());
        return d;
    }

signals:
    void valueChanged();

private:
    QList<QOrganizerCollectionLocalId> m_ids;
    QOrganizerItemCollectionFilter d;

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemCollectionFilter)

//date time period filter
class QDeclarativeOrganizerItemDateTimePeriodFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
    Q_PROPERTY(QDateTime start READ start WRITE setStart NOTIFY valueChanged)
    Q_PROPERTY(QDateTime end READ end WRITE setEnd NOTIFY valueChanged)
public:
    QDeclarativeOrganizerItemDateTimePeriodFilter(QObject *parent)
        :QDeclarativeOrganizerItemFilter(parent)
    {
    }

    QDateTime start() const { return d.startPeriod(); }
    void setStart(const QDateTime& dateTime) { d.setStartPeriod(dateTime); }

    QDateTime end() const { return d.endPeriod(); }
    void setEnd(const QDateTime& dateTime) { d.setEndPeriod(dateTime); }

    QDeclarativeOrganizerItemFilter filter() const
    {
        return d;
    }

signals:
    void valueChanged();

private:
    QOrganizerItemDateTimePeriodFilter d;

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemDateTimePeriodFilter)


//detail filter
class QDeclarativeOrganizerItemDetailFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(MatchFlags matchFlags READ matchFlags WRITE setMatchFlags NOTIFY valueChanged)
    Q_PROPERTY(QString detailFieldName READ detailFieldName WRITE setDetailFieldName NOTIFY valueChanged)
    Q_PROPERTY(QString detailDefinitionName READ detailDefinitionName WRITE setDetailDefinitionName NOTIFY valueChanged)

public:
    QDeclarativeOrganizerItemDetailFilter(QObject *parent)
        :QDeclarativeOrganizerItemFilter(parent)
    {
    }


    QString detailDefinitionName() const { return d.detailDefinitionName(); }
    void setDetailDefinitionName(const QString& definition) { d.setDetailDefinitionName(definition, d.detailFieldName()); }

    QString detailFieldName() const { return d.detailFieldName(); }
    void setDetailFieldName(const QString& fieldName) { d.setDetailDefinitionName(d.detailDefinitionName(), fieldName); }

    void setMatchFlags(QOrganizerItemFilter::MatchFlags flags) { d.setMatchFlags(flags); }
    QOrganizerItemFilter::MatchFlags matchFlags() const { return d.matchFlags(); }


    void setValue(const QVariant& value) { d.setValue(value); }
    QVariant value() const { return d.value(); }

    QDeclarativeOrganizerItemFilter filter() const
    {
        return d;
    }

signals:
    void valueChanged();

private:
    QOrganizerItemDetailFilter d;

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemDetailFilter)


//detail range filter
class QDeclarativeOrganizerItemDetailRangeFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
    Q_PROPERTY(QVariant min READ minValue WRITE setMinValue NOTIFY valueChanged)
    Q_PROPERTY(QVariant max READ maxValue WRITE setMaxValue NOTIFY valueChanged)
    Q_PROPERTY(MatchFlags matchFlags READ matchFlags WRITE setMatchFlags NOTIFY valueChanged)
    Q_PROPERTY(RangeFlags rangeFlags READ rangeFlags WRITE setRangeFlags NOTIFY valueChanged)
    Q_PROPERTY(QString detailFieldName READ detailFieldName WRITE setDetailFieldName NOTIFY valueChanged)
    Q_PROPERTY(QString detailDefinitionName READ detailDefinitionName WRITE setDetailDefinitionName NOTIFY valueChanged)

    Q_ENUMS(RangeFlag)
    Q_FLAGS(RangeFlags)
public:

    enum RangeFlag {
        IncludeLower = QOrganizerItemDetailRangeFilter::IncludeLower,
        IncludeUpper = QOrganizerItemDetailRangeFilter::IncludeUpper,
        ExcludeLower = QOrganizerItemDetailRangeFilter::ExcludeLower,
        ExcludeUpper = QOrganizerItemDetailRangeFilter::ExcludeUpper
    };
    Q_DECLARE_FLAGS(RangeFlags, RangeFlag)

    QDeclarativeOrganizerItemDetailRangeFilter(QObject *parent)
        :QDeclarativeOrganizerItemFilter(parent)
    {
    }


    QString detailDefinitionName() const { return d.detailDefinitionName(); }
    void setDetailDefinitionName(const QString& definition) { d.setDetailDefinitionName(definition, d.detailFieldName()); }

    QString detailFieldName() const { return d.detailFieldName(); }
    void setDetailFieldName(const QString& fieldName) { d.setDetailDefinitionName(d.detailDefinitionName(), fieldName); }

    void setMatchFlags(QOrganizerItemFilter::MatchFlags flags) { d.setMatchFlags(flags); }
    QOrganizerItemFilter::MatchFlags matchFlags() const { return d.matchFlags(); }

    void setRangeFlags(RangeFlags flags) { d.setRange(d.minValue(), d.maxValue(), flags); }
    RangeFlags rangeFlags() const { return d.rangeFlags(); }


    void setMinValue(const QVariant& value) { d.setRange(value, d.maxValue(), d.rangeFlags()); }
    QVariant minValue() const { return d.minValue(); }

    void setMaxValue(const QVariant& value) { d.setRange(d.minValue(), value, d.rangeFlags()); }
    QVariant maxValue() const { return d.maxValue(); }

    QDeclarativeOrganizerItemFilter filter() const
    {
        return d;
    }

signals:
    void valueChanged();

private:
    QOrganizerItemDetailRangeFilter d;

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemDetailRangeFilter)


//localid filter
class QDeclarativeOrganizerItemLocalIdFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QOrganizerItemLocalId> ids READ ids NOTIFY valueChanged)
    Q_CLASSINFO("DefaultProperty", "ids")

public:
    QDeclarativeOrganizerItemLocalIdFilter(QObject *parent)
        :QDeclarativeOrganizerItemFilter(parent)
    {
    }

    QDeclarativeOrganizerItemFilter filter() const
    {
        d.setIds(m_ids);
        return d;
    }

    QDeclarativeListProperty<QOrganizerItemLocalId> ids()
    {
        return QDeclarativeListProperty<QOrganizerItemLocalId>(this, m_ids);
    }

signals:
    void valueChanged();

private:
    QList<QOrganizerItemLocalId> m_ids;
    QOrganizerItemLocalIdFilter d;

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemLocalIdFilter)


//intersection filter
class QDeclarativeOrganizerItemIntersectionFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemFilter> subFilters READ subFilters NOTIFY valueChanged)
    Q_CLASSINFO("DefaultProperty", "subFilters")

public:
    QDeclarativeOrganizerItemIntersectionFilter(QObject *parent)
        :QDeclarativeOrganizerItemFilter(parent)
    {
    }

    QDeclarativeOrganizerItemFilter filter() const
    {
        QList<QDeclarativeOrganizerItemFilter> filters;
        foreach (const QDeclarativeOrganizerItemFilter* filter, m_filters) {
            filters.append(filter->filter());
        }
        d.setFilters(filters);
        return d;
    }

    QDeclarativeListProperty<QDeclarativeOrganizerItemFilter> subFilters()
    {
        return QDeclarativeListProperty<QOrganizerItemLocalId>(this, m_filters);
    }

signals:
    void valueChanged();

private:
    QList<QDeclarativeOrganizerItemFilter*> m_filters;
    QOrganizerItemIntersectionFilter d;

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemIntersectionFilter)


//union filter
class QDeclarativeOrganizerItemUnionFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemFilter> subFilters READ subFilters NOTIFY valueChanged)
    Q_CLASSINFO("DefaultProperty", "subFilters")

public:
    QDeclarativeOrganizerItemUnionFilter(QObject *parent)
        :QDeclarativeOrganizerItemFilter(parent)
    {
    }

    QDeclarativeOrganizerItemFilter filter() const
    {
        QList<QDeclarativeOrganizerItemFilter> filters;
        foreach (const QDeclarativeOrganizerItemFilter* filter, m_filters) {
            filters.append(filter->filter());
        }
        d.setFilters(filters);
        return d;
    }

    QDeclarativeListProperty<QDeclarativeOrganizerItemFilter> subFilters()
    {
        return QDeclarativeListProperty<QOrganizerItemLocalId>(this, m_filters);
    }

signals:
    void valueChanged();

private:
    QList<QDeclarativeOrganizerItemFilter*> m_filters;
    QOrganizerItemUnionFilter d;

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemUnionFilter)

//invalid filter
class QDeclarativeOrganizerItemInvalidFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
public:
    QDeclarativeOrganizerItemInvalidFilter(QObject *parent)
        :QDeclarativeOrganizerItemFilter(parent)
    {
    }

    QDeclarativeOrganizerItemFilter filter() const
    {
        return QOrganizerItemInvalidFilter();
    }

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemInvalidFilter)

#endif
