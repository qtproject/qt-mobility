/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactfilter.h"
#include "qcontactfilter_p.h"
#include "qcontactmanager.h"

/* ====================================================================== */

template<> QContactFilterPrivate *QSharedDataPointer<QContactFilterPrivate>::clone()
{
    return d->clone();
}

/* ====================================================================== */

//Q_IMPLEMENT_BASE_CONTACTFILTER_PRIVATE(QContactFilter)

QContactFilter::QContactFilter()
    : d_ptr(0)
{
}

QContactFilter::QContactFilter(const QContactFilter& other)
    : d_ptr(other.d_ptr)
{
}

QContactFilter& QContactFilter::operator=(const QContactFilter& other)
{
    if (this != &other) {
        d_ptr = other.d_ptr;
    }
    return *this;
}

QContactFilter::~QContactFilter()
{
}

QContactFilter::FilterType QContactFilter::type() const
{
    return d_ptr ? d_ptr->m_type : QContactFilter::Invalid;
}

bool QContactFilter::operator==(const QContactFilter& other) const
{
    /* An invalid filter is not equal to anything, even other invalid filters */
    if (!d_ptr)
        return false;

    /* Different types can't be equal */
    if (other.type() != type())
        return false;

    /* Otherwise, use the virtual op == */
    return d_ptr->compare(other.d_ptr);
}

QContactFilter::QContactFilter(QContactFilterPrivate *d)
    : d_ptr(d)
{

}

/* ====================================================================== */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactGroupMembershipFilter);

QContactGroupMembershipFilter::QContactGroupMembershipFilter()
    : QContactFilter(new QContactGroupMembershipFilterPrivate)
{
}

void QContactGroupMembershipFilter::setGroupId(const QUniqueId& id)
{
    Q_D(QContactGroupMembershipFilter);
    d->m_id = id;
}

QUniqueId QContactGroupMembershipFilter::groupId() const
{
    Q_D(const QContactGroupMembershipFilter);
    return d->m_id;
}

/* ====================================================================== */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactIntersectionFilter);

QContactIntersectionFilter::QContactIntersectionFilter()
    : QContactFilter(new QContactIntersectionFilterPrivate)
{
}

void QContactIntersectionFilter::setFilters(const QList<QContactFilter>& filters)
{
    Q_D(QContactIntersectionFilter);
    d->m_filters = filters;
}

void QContactIntersectionFilter::prepend(const QContactFilter& filter)
{
    Q_D(QContactIntersectionFilter);
    d->m_filters.prepend(filter);
}

void QContactIntersectionFilter::append(const QContactFilter& filter)
{
    Q_D(QContactIntersectionFilter);
    d->m_filters.append(filter);
}

void QContactIntersectionFilter::remove(const QContactFilter& filter)
{
    Q_D(QContactIntersectionFilter);
    d->m_filters.removeAll(filter);
}

QContactIntersectionFilter& QContactIntersectionFilter::operator<<(const QContactFilter& filter)
{
    Q_D(QContactIntersectionFilter);
    d->m_filters << filter;
    return *this;
}

QList<QContactFilter> QContactIntersectionFilter::filters() const
{
    Q_D(const QContactIntersectionFilter);
    return d->m_filters;
}

/* ====================================================================== */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactUnionFilter);

QContactUnionFilter::QContactUnionFilter()
    : QContactFilter(new QContactUnionFilterPrivate)
{
}

void QContactUnionFilter::setFilters(const QList<QContactFilter>& filters)
{
    Q_D(QContactUnionFilter);
    d->m_filters = filters;
}

void QContactUnionFilter::prepend(const QContactFilter& filter)
{
    Q_D(QContactUnionFilter);
    d->m_filters.prepend(filter);
}

void QContactUnionFilter::append(const QContactFilter& filter)
{
    Q_D(QContactUnionFilter);
    d->m_filters.append(filter);
}

void QContactUnionFilter::remove(const QContactFilter& filter)
{
    Q_D(QContactUnionFilter);
    d->m_filters.removeAll(filter);
}

QContactUnionFilter& QContactUnionFilter::operator<<(const QContactFilter& filter)
{
    Q_D(QContactUnionFilter);
    d->m_filters << filter;
    return *this;
}

QList<QContactFilter> QContactUnionFilter::filters() const
{
    Q_D(const QContactUnionFilter);
    return d->m_filters;
}

/* ====================================================================== */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactDetailFilter);

QContactDetailFilter::QContactDetailFilter()
    : QContactFilter(new QContactDetailFilterPrivate)
{
}

void QContactDetailFilter::setDetailDefinitionName(const QString& definitionName, const QString& fieldName)
{
    Q_D(QContactDetailFilter);
    d->m_defId = definitionName;
    d->m_fieldId = fieldName;
}

void QContactDetailFilter::setValue(const QVariant& value)
{
    Q_D(QContactDetailFilter);
    d->m_exactValue = value;
}

void QContactDetailFilter::setMatchFlags(Qt::MatchFlags flags)
{
    Q_D(QContactDetailFilter);
    d->m_flags = flags;
}

Qt::MatchFlags QContactDetailFilter::matchFlags() const
{
    Q_D(const QContactDetailFilter);
    return d->m_flags;
}

QString QContactDetailFilter::detailDefinitionName() const
{
    Q_D(const QContactDetailFilter);
    return d->m_defId;
}

QString QContactDetailFilter::detailFieldName() const
{
    Q_D(const QContactDetailFilter);
    return d->m_fieldId;
}

QVariant QContactDetailFilter::value() const
{
    Q_D(const QContactDetailFilter);
    return d->m_exactValue;
}

/* ====================================================================== */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactDetailRangeFilter);

QContactDetailRangeFilter::QContactDetailRangeFilter()
    : QContactFilter(new QContactDetailRangeFilterPrivate)
{
}

void QContactDetailRangeFilter::setRange(const QVariant& min, const QVariant& max, RangeFlags flags)
{
    Q_D(QContactDetailRangeFilter);
    d->m_minValue = min;
    d->m_maxValue = max;
    d->m_rangeflags = flags;
}

void QContactDetailRangeFilter::setMatchFlags(Qt::MatchFlags flags)
{
    Q_D(QContactDetailRangeFilter);
    d->m_flags = flags;
}

void QContactDetailRangeFilter::setDetailDefinitionName(const QString& definitionName, const QString& fieldName)
{
    Q_D(QContactDetailRangeFilter);
    d->m_defId = definitionName;
    d->m_fieldId = fieldName;
}

Qt::MatchFlags QContactDetailRangeFilter::matchFlags() const
{
    Q_D(const QContactDetailRangeFilter);
    return d->m_flags;
}

QString QContactDetailRangeFilter::detailDefinitionName() const
{
    Q_D(const QContactDetailRangeFilter);
    return d->m_defId;
}

QString QContactDetailRangeFilter::detailFieldName() const
{
    Q_D(const QContactDetailRangeFilter);
    return d->m_fieldId;
}

QVariant QContactDetailRangeFilter::minValue() const
{
    Q_D(const QContactDetailRangeFilter);
    return d->m_minValue;
}

QVariant QContactDetailRangeFilter::maxValue() const
{
    Q_D(const QContactDetailRangeFilter);
    return d->m_maxValue;
}

QContactDetailRangeFilter::RangeFlags QContactDetailRangeFilter::rangeFlags() const
{
    Q_D(const QContactDetailRangeFilter);
    return d->m_rangeflags;
}

/* ====================================================================== */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactChangeLogFilter);

QContactChangeLogFilter::QContactChangeLogFilter(ChangeType type)
    : QContactFilter(new QContactChangeLogFilterPrivate(type))
{
}

void QContactChangeLogFilter::setChangeType(ChangeType type)
{
    Q_D(QContactChangeLogFilter);
    d->m_changeType = type;
}

void QContactChangeLogFilter::setSince(const QDateTime& since)
{
    Q_D(QContactChangeLogFilter);
    d->m_since = since;
}

QDateTime QContactChangeLogFilter::since() const
{
    Q_D(const QContactChangeLogFilter);
    return d->m_since;
}

QContactChangeLogFilter::ChangeType QContactChangeLogFilter::changeType() const
{
    Q_D(const QContactChangeLogFilter);
    return d->m_changeType;
}

/* ====================================================================== */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactActionFilter);

QContactActionFilter::QContactActionFilter()
    : QContactFilter(new QContactActionFilterPrivate)
{
}

void QContactActionFilter::setActionName(const QString& action)
{
    Q_D(QContactActionFilter);
    d->m_action = action;
}
void QContactActionFilter::setValue(const QVariant& value)
{
    Q_D(QContactActionFilter);
    d->m_value = value;
}

QString QContactActionFilter::actionName() const
{
    Q_D(const QContactActionFilter);
    return d->m_action;
}

QVariant QContactActionFilter::value() const
{
    Q_D(const QContactActionFilter);
    return d->m_value;
}

/* ====================================================================== */

const QContactFilter operator&&(const QContactFilter& left, const QContactFilter& right)
{
    if (left.type() == QContactFilter::Intersection) {
        QContactIntersectionFilter bf(left);
        /* we can just add the right to this one */
        bf.append(right);
        return bf;
    }

    if (right.type() == QContactFilter::Intersection) {
        QContactIntersectionFilter bf(right);
        /* we can prepend the left to this one */
        bf.prepend(left);
        return bf;
    }

    /* usual fallback case */
    QContactIntersectionFilter nif;
    nif << left << right;
    return nif;
}

/* ====================================================================== */

const QContactFilter operator||(const QContactFilter& left, const QContactFilter& right)
{
    if (left.type() == QContactFilter::Union) {
        QContactUnionFilter bf(left);
        /* we can just add the right to this one */
        bf.append(right);
        return bf;
    }

    if (right.type() == QContactFilter::Union) {
        QContactUnionFilter bf(right);
        /* we can prepend the left to this one */
        bf.prepend(left);
        return bf;
    }

    /* usual fallback case */
    QContactUnionFilter nif;
    nif << left << right;
    return nif;
}

