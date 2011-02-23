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

#include <QSet>

#include <qdeclarative.h>
#include <QDeclarativeExtensionPlugin>
#include <qorganizeritemfilters.h>
#include <QDeclarativeListProperty>
#include <QDeclarativeParserStatus>

#include "qdeclarativeorganizeritemdetail_p.h"
QTM_USE_NAMESPACE


class QDeclarativeOrganizerItemFilter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(FilterType type READ type)

    Q_ENUMS(FilterType)
    Q_FLAGS(MatchFlags)
public:
    QDeclarativeOrganizerItemFilter(QObject *parent=0)
        :QObject(parent)
    {
        //for grouped filter: intersect /union filters
        if (parent && qobject_cast<QDeclarativeOrganizerItemFilter*>(parent)) {
            connect(this, SIGNAL(filterChanged()), parent, SIGNAL(filterChanged()));
        }
    }

    enum FilterType {
        InvalidFilter = QOrganizerItemFilter::InvalidFilter,
        DetailFilter = QOrganizerItemFilter::OrganizerItemDetailFilter,
        DetailRangeFilter = QOrganizerItemFilter::OrganizerItemDetailRangeFilter,
        ChangeLogFilter = QOrganizerItemFilter::ChangeLogFilter,
        ActionFilter = QOrganizerItemFilter::ActionFilter,
        IntersectionFilter = QOrganizerItemFilter::IntersectionFilter,
        UnionFilter = QOrganizerItemFilter::UnionFilter,
        IdFilter = QOrganizerItemFilter::IdFilter,
        CollectionFilter = QOrganizerItemFilter::CollectionFilter,
        DefaultFilter = QOrganizerItemFilter::DefaultFilter
    };

    FilterType type() const {
        return static_cast<QDeclarativeOrganizerItemFilter::FilterType>(filter().type());
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
    void filterChanged();
};

QML_DECLARE_TYPE(QDeclarativeOrganizerItemFilter)


//changelog filter
class QDeclarativeOrganizerItemChangeLogFilter : public QDeclarativeOrganizerItemFilter
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

    QDeclarativeOrganizerItemChangeLogFilter(QObject *parent = 0)
        :QDeclarativeOrganizerItemFilter(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(filterChanged()));
    }

    QDateTime since() const { return d.since(); }
    void setSince(const QDateTime& datetime)
    {
        if (datetime != since()) {
            emit valueChanged();
            d.setSince(datetime);
        }
    }

    EventType eventType() const { return static_cast<QDeclarativeOrganizerItemChangeLogFilter::EventType>(d.eventType()); }
    void setEventType(EventType type)
    {
        if (type != eventType()) {
            d.setEventType(static_cast<QOrganizerItemChangeLogFilter::EventType>(type));
            emit valueChanged();
        }
    }

    QOrganizerItemFilter filter() const
    {
        return d;
    }

signals:
    void valueChanged();

private:
    QOrganizerItemChangeLogFilter d;

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemChangeLogFilter)

//collection filter
class QDeclarativeOrganizerItemCollectionFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
    Q_PROPERTY(QStringList ids READ ids WRITE setIds NOTIFY valueChanged)
public:
    QDeclarativeOrganizerItemCollectionFilter(QObject *parent = 0)
        :QDeclarativeOrganizerItemFilter(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(filterChanged()));
    }

    QStringList ids() const
    {
        return m_ids;
    }

    void setIds(const QStringList& ids)
    {
        foreach (const QString& id, ids) {
            if (!m_ids.contains(id)) {
                m_ids = ids;
                emit valueChanged();
                return;
            }
        }

        foreach (const QString& id, m_ids) {
            if (!ids.contains(id)) {
                m_ids = ids;
                emit valueChanged();
                return;
            }
        }
    }

    QOrganizerItemFilter filter() const;

signals:
    void valueChanged();

private:
    QStringList m_ids;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemCollectionFilter)

//detail filter
class QDeclarativeOrganizerItemDetailFilter : public QDeclarativeOrganizerItemFilter, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemFilter::MatchFlags matchFlags READ matchFlags WRITE setMatchFlags NOTIFY valueChanged)
    Q_PROPERTY(QVariant field READ field WRITE setField NOTIFY valueChanged)
    Q_PROPERTY(QVariant detail READ detail WRITE setDetail NOTIFY valueChanged)
    Q_INTERFACES(QDeclarativeParserStatus)
public:
    QDeclarativeOrganizerItemDetailFilter(QObject *parent = 0)
        :QDeclarativeOrganizerItemFilter(parent),
          m_componentCompleted(false)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(filterChanged()));
    }

    //from QDeclarativeParserStatus
    void classBegin() {}
    void componentComplete()
    {
        setDetailDefinitionName();
        m_componentCompleted = true;
    }
    void setDetailDefinitionName()
    {
        QString ddn;
        if (m_detail.type() != QVariant::String) {
            ddn = QDeclarativeOrganizerItemDetail::definitionName(static_cast<QDeclarativeOrganizerItemDetail::ItemDetailType>(m_detail.toInt()));
        } else {
            ddn = m_detail.toString();
        }

        QString dfn;
        if (m_field.type() != QVariant::String) {
           QDeclarativeOrganizerItemDetail::ItemDetailType dt = static_cast<QDeclarativeOrganizerItemDetail::ItemDetailType>(QDeclarativeOrganizerItemDetail::detailTypeByDefinitionName(ddn));
           dfn = QDeclarativeOrganizerItemDetail::fieldName(dt, m_field.toInt());
        } else {
            dfn = m_field.toString();
        }
        d.setDetailDefinitionName(ddn, dfn);
        m_detail = ddn;
        m_field = dfn;
    }

    QVariant detail() const { return m_detail; }
    void setDetail(const QVariant& v)
    {
        if (v != m_detail || m_componentCompleted) {
            m_detail = v;
            if (m_componentCompleted)
                setDetailDefinitionName();
            emit filterChanged();
        }
    }

    QVariant field() const { return m_field; }
    void setField(const QVariant& v)
    {
        if (v != m_field || m_componentCompleted) {
            m_field = v;
            if (m_componentCompleted)
                setDetailDefinitionName();
            emit filterChanged();
        }
    }


    void setMatchFlags(QDeclarativeOrganizerItemFilter::MatchFlags flags)
    {
        QOrganizerItemFilter::MatchFlags newFlags;
        newFlags = ~newFlags & (int)flags;
        if (newFlags != d.matchFlags()) {
            d.setMatchFlags(newFlags);
            emit valueChanged();
        }
    }
    QDeclarativeOrganizerItemFilter::MatchFlags matchFlags() const
    {
        QDeclarativeOrganizerItemFilter::MatchFlags newFlags;
        newFlags = ~newFlags & (int)d.matchFlags();
        return newFlags;
    }


    void setValue(const QVariant& v)
    {
        if (v != value()) {
            d.setValue(v);
            emit valueChanged();
        }
    }
    QVariant value() const { return d.value(); }

    QOrganizerItemFilter filter() const
    {
        return d;
    }

signals:
    void valueChanged();

private:
    QVariant m_field;
    QVariant m_detail;
    bool m_componentCompleted;
    QOrganizerItemDetailFilter d;

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemDetailFilter)


//detail range filter
class QDeclarativeOrganizerItemDetailRangeFilter : public QDeclarativeOrganizerItemFilter, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QVariant min READ minValue WRITE setMinValue NOTIFY valueChanged)
    Q_PROPERTY(QVariant max READ maxValue WRITE setMaxValue NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemFilter::MatchFlags matchFlags READ matchFlags WRITE setMatchFlags NOTIFY valueChanged)
    Q_PROPERTY(RangeFlags rangeFlags READ rangeFlags WRITE setRangeFlags NOTIFY valueChanged)
    Q_PROPERTY(QVariant detail READ detail WRITE setDetail NOTIFY valueChanged)
    Q_PROPERTY(QVariant field READ field WRITE setField NOTIFY valueChanged)
    Q_INTERFACES(QDeclarativeParserStatus)
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

    QDeclarativeOrganizerItemDetailRangeFilter(QObject *parent = 0)
        :QDeclarativeOrganizerItemFilter(parent),
          m_componentCompleted(false)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(filterChanged()));
    }

    //from QDeclarativeParserStatus
    void classBegin() {}
    void componentComplete()
    {
        setDetailDefinitionName();
        m_componentCompleted = true;
    }
    void setDetailDefinitionName()
    {
        QString ddn;
        if (m_detail.type() != QVariant::String) {
            ddn = QDeclarativeOrganizerItemDetail::definitionName(static_cast<QDeclarativeOrganizerItemDetail::ItemDetailType>(m_detail.toInt()));
        } else {
            ddn = m_detail.toString();
        }

        QString dfn;
        if (m_field.type() != QVariant::String) {
           QDeclarativeOrganizerItemDetail::ItemDetailType dt = static_cast<QDeclarativeOrganizerItemDetail::ItemDetailType>(QDeclarativeOrganizerItemDetail::detailTypeByDefinitionName(ddn));
           dfn = QDeclarativeOrganizerItemDetail::fieldName(dt, m_field.toInt());
        } else {
            dfn = m_field.toString();
        }
        d.setDetailDefinitionName(ddn, dfn);
        m_detail = ddn;
        m_field = dfn;
    }

    QVariant detail() const { return m_detail; }
    void setDetail(const QVariant& v)
    {
        if (v != m_detail || m_componentCompleted) {
            m_detail = v;
            if (m_componentCompleted)
                setDetailDefinitionName();
            emit filterChanged();
        }
    }

    QVariant field() const { return m_field; }
    void setField(const QVariant& v)
    {
        if (v != m_field || m_componentCompleted) {
            m_field = v;
            if (m_componentCompleted)
                setDetailDefinitionName();
            emit filterChanged();
        }
    }

    void setMatchFlags(QDeclarativeOrganizerItemFilter::MatchFlags flags)
    {
        QOrganizerItemFilter::MatchFlags newFlags;
        newFlags = ~newFlags & (int)flags;
        if (newFlags != d.matchFlags()) {
            d.setMatchFlags(newFlags);
            emit valueChanged();
        }
    }
    QDeclarativeOrganizerItemFilter::MatchFlags matchFlags() const
    {
        QDeclarativeOrganizerItemFilter::MatchFlags newFlags;
        newFlags = ~newFlags & (int)d.matchFlags();
        return newFlags;
    }

    void setRangeFlags(RangeFlags flags)
    {
        QOrganizerItemDetailRangeFilter::RangeFlags newFlags;
        newFlags = ~newFlags & (int)flags;
        if (newFlags != d.rangeFlags()) {
            d.setRange(d.minValue(), d.maxValue(), newFlags);
            emit valueChanged();
        }
    }
    RangeFlags rangeFlags() const
    {
        QDeclarativeOrganizerItemDetailRangeFilter::RangeFlags newFlags;
        newFlags = ~newFlags & (int)d.rangeFlags();
        return newFlags;
    }


    void setMinValue(const QVariant& value)
    {
        if (value != d.minValue()) {
            d.setRange(value, d.maxValue(), d.rangeFlags());
            emit valueChanged();
        }
    }
    QVariant minValue() const { return d.minValue(); }

    void setMaxValue(const QVariant& value)
    {
        if (value != d.maxValue()) {
            d.setRange(d.minValue(), value, d.rangeFlags());
            emit valueChanged();
        }
    }
    QVariant maxValue() const { return d.maxValue(); }

    QOrganizerItemFilter filter() const
    {
        return d;
    }

signals:
    void valueChanged();

private:
    QVariant m_field;
    QVariant m_detail;
    bool m_componentCompleted;
    QOrganizerItemDetailRangeFilter d;

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemDetailRangeFilter)


//id filter
class QDeclarativeOrganizerItemIdFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
    Q_PROPERTY(QVariantList ids READ ids WRITE setIds NOTIFY valueChanged)

public:
    QDeclarativeOrganizerItemIdFilter(QObject *parent = 0)
        :QDeclarativeOrganizerItemFilter(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(filterChanged()));
    }

    QOrganizerItemFilter filter() const;

    QVariantList ids() const
    {
        return m_ids;
    }

    void setIds(const QVariantList& ids)
    {
        foreach (const QVariant& id, ids) {
            if (!m_ids.contains(id)) {
                m_ids = ids;
                emit valueChanged();
                return;
            }
        }

        foreach (const QVariant& id, m_ids) {
            if (!ids.contains(id)) {
                m_ids = ids;
                emit valueChanged();
            }
        }
    }

signals:
    void valueChanged();

private:
    QVariantList m_ids;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemIdFilter)


//intersection filter
class QDeclarativeOrganizerItemIntersectionFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemFilter> filters READ filters NOTIFY valueChanged)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    QDeclarativeOrganizerItemIntersectionFilter(QObject *parent = 0)
        :QDeclarativeOrganizerItemFilter(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(filterChanged()));
    }

    QOrganizerItemFilter filter() const
    {
        QList<QOrganizerItemFilter> filters;
        foreach (const QDeclarativeOrganizerItemFilter* filter, m_filters) {
            filters << filter->filter();
        }
        QOrganizerItemIntersectionFilter f;
        f.setFilters(filters);
        return f;
    }

    QDeclarativeListProperty<QDeclarativeOrganizerItemFilter> filters()
    {
        return QDeclarativeListProperty<QDeclarativeOrganizerItemFilter>(this, m_filters);
    }

signals:
    void valueChanged();

private:
    QList<QDeclarativeOrganizerItemFilter*> m_filters;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemIntersectionFilter)


//union filter
class QDeclarativeOrganizerItemUnionFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemFilter> filters READ filters NOTIFY valueChanged)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    QDeclarativeOrganizerItemUnionFilter(QObject *parent = 0)
        :QDeclarativeOrganizerItemFilter(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(filterChanged()));
    }

    QOrganizerItemFilter filter() const
    {
        QList<QOrganizerItemFilter> filters;
        foreach (const QDeclarativeOrganizerItemFilter* filter, m_filters) {
            filters << filter->filter();
        }
        QOrganizerItemUnionFilter f;
        f.setFilters(filters);
        return f;
    }

    QDeclarativeListProperty<QDeclarativeOrganizerItemFilter> filters()
    {
        return QDeclarativeListProperty<QDeclarativeOrganizerItemFilter>(this, m_filters);
    }

signals:
    void valueChanged();

private:
    QList<QDeclarativeOrganizerItemFilter*> m_filters;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemUnionFilter)

//invalid filter
class QDeclarativeOrganizerItemInvalidFilter : public QDeclarativeOrganizerItemFilter
{
    Q_OBJECT
public:
    QDeclarativeOrganizerItemInvalidFilter(QObject *parent = 0)
        :QDeclarativeOrganizerItemFilter(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(filterChanged()));
    }

    QOrganizerItemFilter filter() const
    {
        return QOrganizerItemInvalidFilter();
    }

};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemInvalidFilter)

#endif
