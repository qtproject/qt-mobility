/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMobility module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QCONTACTFILTER_H
#define QCONTACTFILTER_H

#include <QVariant>
#include <QList>
#include <QDateTime>
#include <QSharedData>

#include "qtcontactsglobal.h"

/* Manual Q_DECLARE_CONTACTFILTER_PRIVATE macro */

#define Q_DECLARE_CONTACTFILTER_PRIVATE(Class) \
    inline Class##Private* d_func(); \
    inline const Class##Private* d_func() const; \
    friend class Class##Private;

class QContactFilterPrivate;
class QTCONTACTS_EXPORT QContactFilter
{
public:
    virtual ~QContactFilter();
    QContactFilter(const QContactFilter& other);
    QContactFilter& operator=(const QContactFilter& other);

    enum FilterType {
        Invalid,
        ContactDetail,
        ContactDetailRange,
        ChangeLog,
        Action,
        GroupMembership,
        Intersection,
        Union
    };

    FilterType type() const;

    bool operator==(const QContactFilter& other) const;
    bool operator!=(const QContactFilter& other) const {return !operator==(other);}

protected:
    QContactFilter();
    QContactFilter(QContactFilterPrivate* d);

protected:
    QSharedDataPointer<QContactFilterPrivate> d_ptr;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactFilter);
};

class QContactDetailFilterPrivate;
class QTCONTACTS_EXPORT QContactDetailFilter : public QContactFilter
{
public:
    QContactDetailFilter();
    QContactDetailFilter(const QContactFilter& other);

    /* Mutators */
    void setDetailDefinitionName(const QString& definition);
    void setDetailFieldName(const QString& field);
    void setMatchFlags(Qt::MatchFlags flags);

    /* Filter Criterion */
    void setValue(const QVariant& value);

    /* Accessors */
    QString detailDefinitionName() const;
    QString detailFieldName() const;
    Qt::MatchFlags matchFlags() const;

    QVariant value() const;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactDetailFilter);
};

class QContactDetailRangeFilterPrivate;
class QTCONTACTS_EXPORT QContactDetailRangeFilter : public QContactFilter
{
public:
    QContactDetailRangeFilter();
    QContactDetailRangeFilter(const QContactFilter& other);

    enum RangeFlag {
        IncludeLower = 0, // [
        IncludeUpper = 1, // ]
        ExcludeLower = 2, // (
        ExcludeUpper = 0  // ) - Default is [)
    };
    Q_DECLARE_FLAGS(RangeFlags, RangeFlag);

    /* Mutators */
    void setDetailDefinitionName(const QString& definition);
    void setDetailFieldName(const QString& field);
    void setMatchFlags(Qt::MatchFlags flags);

    /* Filter Criterion */
    void setRange(const QVariant& min, const QVariant& max, RangeFlags flags = 0);

    /* Accessors */
    QString detailDefinitionName() const;
    QString detailFieldName() const;
    Qt::MatchFlags matchFlags() const;

    QVariant minValue() const;
    QVariant maxValue() const;
    RangeFlags rangeFlags() const;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactDetailRangeFilter);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QContactDetailRangeFilter::RangeFlags)

class QContactChangeLogFilterPrivate;
class QTCONTACTS_EXPORT QContactChangeLogFilter: public QContactFilter
{
public:
    enum ChangeType {
        Added,
        Changed,
        Removed
    };

    QContactChangeLogFilter(ChangeType type);
    QContactChangeLogFilter(const QContactFilter& other);

    void setChangeType(ChangeType type);
    void setSince(const QDateTime& since);

    QDateTime since() const;
    ChangeType type() const;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactChangeLogFilter);
};

class QContactActionFilterPrivate;
class QTCONTACTS_EXPORT QContactActionFilter : public QContactFilter
{
public:
    QContactActionFilter();
    QContactActionFilter(const QContactFilter& other);

    void setActionId(const QString& action);
    void setValue(const QVariant& value);

    /* Accessors */
    QString actionId() const;
    QVariant value() const;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactActionFilter);
};

class QContactGroupMembershipFilterPrivate;
class QTCONTACTS_EXPORT QContactGroupMembershipFilter : public QContactFilter
{
public:
    QContactGroupMembershipFilter();
    QContactGroupMembershipFilter(const QContactFilter& other);

    void setGroupId(const QUniqueId& id);
    QUniqueId groupId() const;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactGroupMembershipFilter);
};

class QContactIntersectionFilterPrivate;
class QTCONTACTS_EXPORT QContactIntersectionFilter : public QContactFilter
{
public:

    QContactIntersectionFilter();
    QContactIntersectionFilter(const QContactFilter& other);

    void setFilters(const QList<QContactFilter>& filters);

    void prepend(const QContactFilter& filter);
    void append(const QContactFilter& filter);
    void remove(const QContactFilter& filter);

    QContactIntersectionFilter& operator<<(const QContactFilter& filter);

    /* Accessors */
    QList<QContactFilter> filters() const;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactIntersectionFilter);
};

const QTCONTACTS_EXPORT QContactFilter operator&&(const QContactFilter& left, const QContactFilter& right);

class QContactUnionFilterPrivate;
class QTCONTACTS_EXPORT QContactUnionFilter : public QContactFilter
{
public:

    QContactUnionFilter();
    QContactUnionFilter(const QContactFilter& other);

    void setFilters(const QList<QContactFilter>& filters);

    void prepend(const QContactFilter& filter);
    void append(const QContactFilter& filter);
    void remove(const QContactFilter& filter);

    QContactUnionFilter& operator<<(const QContactFilter& filter);

    /* Accessors */
    QList<QContactFilter> filters() const;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactUnionFilter);
};

const QTCONTACTS_EXPORT QContactFilter operator||(const QContactFilter& left, const QContactFilter& right);

#endif
