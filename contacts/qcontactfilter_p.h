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

#ifndef QCONTACTFILTER_P_H
#define QCONTACTFILTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

/* Boiler plate code */
#define Q_IMPLEMENT_BASE_CONTACTFILTER_PRIVATE(Class) \
    Class##Private* Class::d_func() { return reinterpret_cast<Class##Private *>(d_ptr.data()); } \
    const Class##Private* Class::d_func() const { return reinterpret_cast<const Class##Private *>(d_ptr.constData()); } \

#define Q_IMPLEMENT_CONTACTFILTER_PRIVATE(Class) \
    Q_IMPLEMENT_BASE_CONTACTFILTER_PRIVATE(Class) \
    Class::Class(const QContactFilter& other) : QContactFilter() { Class##Private::copyIfPossible(d_ptr, other); }

#define Q_IMPLEMENT_CONTACTFILTER_VIRTUALCTORS(Class, Type) \
    QContactFilterPrivate* clone() const { return new Class##Private(*this); } \
    static void copyIfPossible(QSharedDataPointer<QContactFilterPrivate>& d_ptr, const QContactFilter& other) \
    { \
        if (other.type() == Type) \
            d_ptr = extract_d(other); \
        else \
            d_ptr = new Class##Private; \
    }

class QContactFilterPrivate : public QSharedData
{
public:
    QContactFilterPrivate(QContactFilter::FilterType type)
        : m_type(type)
    {
    }

    QContactFilter::FilterType m_type;

    virtual bool compare(const QContactFilterPrivate* other) const = 0;
    virtual QContactFilterPrivate* clone() const = 0;

    /* Helper functions for C++ protection rules */
    static const QSharedDataPointer<QContactFilterPrivate>& extract_d(const QContactFilter& other) {return other.d_ptr;}
};



class QContactGroupMembershipFilterPrivate : public QContactFilterPrivate
{
public:
    QContactGroupMembershipFilterPrivate()
        : QContactFilterPrivate(QContactFilter::GroupMembership)
        , m_id(0)
    {

    }

    QContactGroupMembershipFilterPrivate(const QContactGroupMembershipFilterPrivate& other)
        : QContactFilterPrivate(other),
        m_id(other.m_id)
    {
    }

    virtual bool compare(const QContactFilterPrivate* other) const
    {
        const QContactGroupMembershipFilterPrivate *od = static_cast<const QContactGroupMembershipFilterPrivate*>(other);
        if (m_id != od->m_id)
            return false;
        return true;
    }

    Q_IMPLEMENT_CONTACTFILTER_VIRTUALCTORS(QContactGroupMembershipFilter, QContactFilter::GroupMembership)

    QUniqueId m_id;
};

class QContactIntersectionFilterPrivate : public QContactFilterPrivate
{
public:
    QContactIntersectionFilterPrivate()
       : QContactFilterPrivate(QContactFilter::Intersection)
    {

    }

    QContactIntersectionFilterPrivate(const QContactIntersectionFilterPrivate& other)
       : QContactFilterPrivate(other),
       m_filters(other.m_filters)
    {

    }

    virtual bool compare(const QContactFilterPrivate* other) const
    {
        const QContactIntersectionFilterPrivate *od = static_cast<const QContactIntersectionFilterPrivate*>(other);
        if (m_filters != od->m_filters)
            return false;
        return true;
    }

    Q_IMPLEMENT_CONTACTFILTER_VIRTUALCTORS(QContactIntersectionFilter, QContactFilter::Intersection)

    QList<QContactFilter> m_filters;
};

class QContactUnionFilterPrivate : public QContactFilterPrivate
{
public:
    QContactUnionFilterPrivate()
       : QContactFilterPrivate(QContactFilter::Union)
    {

    }

    QContactUnionFilterPrivate(const QContactUnionFilterPrivate& other)
       : QContactFilterPrivate(other),
       m_filters(other.m_filters)
    {

    }

    virtual bool compare(const QContactFilterPrivate* other) const
    {
        const QContactUnionFilterPrivate *od = static_cast<const QContactUnionFilterPrivate*>(other);
        if (m_filters != od->m_filters)
            return false;
        return true;
    }

    Q_IMPLEMENT_CONTACTFILTER_VIRTUALCTORS(QContactUnionFilter, QContactFilter::Union)

    QList<QContactFilter> m_filters;
};

class QContactDetailFilterPrivate : public QContactFilterPrivate
{
public:
    QContactDetailFilterPrivate()
        : QContactFilterPrivate(QContactFilter::ContactDetail)
        , m_flags(0)
    {
    }

    QContactDetailFilterPrivate(const QContactDetailFilterPrivate& other)
        : QContactFilterPrivate(other)
        , m_defId(other.m_defId)
        , m_fieldId(other.m_fieldId)
        , m_exactValue(other.m_exactValue)
        , m_flags(other.m_flags)
    {
    }

    virtual bool compare(const QContactFilterPrivate* other) const
    {
        const QContactDetailFilterPrivate *od = static_cast<const QContactDetailFilterPrivate*>(other);
        if (m_defId != od->m_defId)
            return false;
        if (m_fieldId != od->m_fieldId)
            return false;
        if (m_exactValue != od->m_exactValue)
            return false;
        if (m_flags != od->m_flags)
            return false;
        return true;
    }

    Q_IMPLEMENT_CONTACTFILTER_VIRTUALCTORS(QContactDetailFilter, QContactFilter::ContactDetail)

    QString m_defId;
    QString m_fieldId;
    QVariant m_exactValue;
    Qt::MatchFlags m_flags;
};

class QContactDetailRangeFilterPrivate : public QContactFilterPrivate
{
public:
    QContactDetailRangeFilterPrivate()
        : QContactFilterPrivate(QContactFilter::ContactDetailRange)
        , m_flags(0)
        , m_rangeflags(0)
    {
    }

    QContactDetailRangeFilterPrivate(const QContactDetailRangeFilterPrivate& other)
        : QContactFilterPrivate(other)
        , m_defId(other.m_defId)
        , m_fieldId(other.m_fieldId)
        , m_minValue(other.m_minValue)
        , m_maxValue(other.m_maxValue)
        , m_flags(other.m_flags)
        , m_rangeflags(other.m_rangeflags)
    {
    }

    virtual bool compare(const QContactFilterPrivate* other) const
    {
        const QContactDetailRangeFilterPrivate *od = static_cast<const QContactDetailRangeFilterPrivate*>(other);
        if (m_defId != od->m_defId)
            return false;
        if (m_fieldId != od->m_fieldId)
            return false;
        if (m_minValue != od->m_minValue)
            return false;
        if (m_maxValue!= od->m_maxValue)
            return false;
        if (m_flags != od->m_flags)
            return false;
        if (m_rangeflags != od->m_rangeflags)
            return false;
        return true;
    }

    Q_IMPLEMENT_CONTACTFILTER_VIRTUALCTORS(QContactDetailRangeFilter, QContactFilter::ContactDetailRange)

    QString m_defId;
    QString m_fieldId;
    QVariant m_minValue;
    QVariant m_maxValue;
    Qt::MatchFlags m_flags;
    QContactDetailRangeFilter::RangeFlags m_rangeflags;
};

class QContactChangeLogFilterPrivate : public QContactFilterPrivate
{
public:
    QContactChangeLogFilterPrivate(QContactChangeLogFilter::ChangeType type = QContactChangeLogFilter::Added)
        : QContactFilterPrivate(QContactFilter::ChangeLog)
        , m_changeType(type)
    {

    }

    QContactChangeLogFilterPrivate(const QContactChangeLogFilterPrivate& other)
        : QContactFilterPrivate(other)
        , m_changeType(other.m_changeType)
        , m_since(other.m_since)
    {

    }

    virtual bool compare(const QContactFilterPrivate* other) const
    {
        const QContactChangeLogFilterPrivate *od = static_cast<const QContactChangeLogFilterPrivate*>(other);
        if (m_changeType != od->m_changeType)
            return false;
        if (m_since != od->m_since)
            return false;
        return true;
    }

    Q_IMPLEMENT_CONTACTFILTER_VIRTUALCTORS(QContactChangeLogFilter, QContactFilter::ChangeLog)

    QContactChangeLogFilter::ChangeType m_changeType;
    QDateTime m_since;
};

class QContactActionFilterPrivate : public QContactFilterPrivate
{
public:
    QContactActionFilterPrivate()
        : QContactFilterPrivate(QContactFilter::Action)
    {
    }

    QContactActionFilterPrivate(const QContactActionFilterPrivate& other)
        : QContactFilterPrivate(other)
        , m_action(other.m_action)
        , m_value(other.m_value)
    {
    }

    virtual bool compare(const QContactFilterPrivate* other) const
    {
        const QContactActionFilterPrivate *od = static_cast<const QContactActionFilterPrivate*>(other);
        if (m_action != od->m_action)
            return false;
        if (m_value != od->m_value)
            return false;
        return true;
    }

    Q_IMPLEMENT_CONTACTFILTER_VIRTUALCTORS(QContactActionFilter, QContactFilter::Action)

    QString m_action;
    QVariant m_value;
};

#endif
