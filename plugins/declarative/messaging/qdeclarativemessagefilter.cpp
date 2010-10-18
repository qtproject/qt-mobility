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

#include "qdeclarativemessagefilter.h"
#include <qdeclarativeinfo.h>
#include <QDebug>

QTM_USE_NAMESPACE

QDeclarativeMessageFilterBase::QDeclarativeMessageFilterBase(QObject *parent)
    : QObject(parent), m_negated(false)
{
}

bool QDeclarativeMessageFilterBase::negated() const
{
    return m_negated;
}

void QDeclarativeMessageFilterBase::setNegated(bool n)
{
    if (m_negated == n)
        return;
    m_negated = n;
    emit negatedChanged();
}

//===========================================================================


/*!
    \qmlclass MessageIntersectionFilter QDeclarativeMessageIntersectionFilter
    \brief The MessageIntersectionFilter element specifies an insection of MessageFilter

    \ingroup qml-messaging

    This element is part of the \bold{QtMobility.messaging 1.1} module.

    Logical AND combinations of MessageFilters can be formed using MessageIntersectionFilter.

    Setting \l negated to true will negate the filter.

    The following example creates a filter which filters for messages
    less than 1024 bytes in size from \c martin.

    \qml
    model: MessageModel {
        sortBy: MessageModel.Timestamp
        sortOrder: MessageModel.DescendingOrder
        filter: MessageIntersectionFilter {
            MessageFilter {
                type: MessageFilter.Size
                value: 1024
                comparator: MessageFilter.LessThan
            }
            MessageFilter {
                type: MessageFilter.Sender
                value: "martin"
                comparator: MessageFilter.Includes
            }
        }
    }
    \endqml

    \sa MessageUnionFilter
*/

/*!
    \qmlproperty bool MessageIntersectionFilter::negated
    Setting negated to true will result in the filter being locially negated.
*/

/*!
    \qmlproperty list<MessageFilter> MessageIntersectionFilter::filters
    \default

    The filters to AND together.
*/

QDeclarativeMessageIntersectionFilter::QDeclarativeMessageIntersectionFilter(QObject *parent)
    : QDeclarativeMessageFilterBase(parent)
{
}

QDeclarativeListProperty<QDeclarativeMessageFilterBase> QDeclarativeMessageIntersectionFilter::filters()
{
    return QDeclarativeListProperty<QDeclarativeMessageFilterBase>(this, m_filters);
}

QMessageFilter QDeclarativeMessageIntersectionFilter::filter()
{
    if (m_filters.count() == 0)
        return QMessageFilter();

    QMessageFilter filter = m_filters.first()->filter();
    for (int i = 1; i < m_filters.count(); ++i)
        filter &= m_filters.at(i)->filter();

    if (m_negated)
        return ~filter;
    else
        return filter;
}


/*!
    \qmlclass MessageUnionFilter QDeclarativeMessageUnionFilter
    \brief The MessageUnionFilter element specifies a union of MessageFilter

    \ingroup qml-messaging

    This element is part of the \bold{QtMobility.messaging 1.1} module.

    Logical OR combinations of MessageFilters can be formed using MessageUnionFilter.

    Setting \l negated to true will negate the filter.

    The following example creates a filter which filters for messages
    from \c martin or \c don.

    \qml
    model: MessageModel {
        sortBy: MessageModel.Timestamp
        sortOrder: MessageModel.DescendingOrder
        filter: MessageUnionFilter {
            MessageFilter {
                type: MessageFilter.Sender
                value: "martin"
                comparator: MessageFilter.Includes
            }
            MessageFilter {
                type: MessageFilter.Sender
                value: "don"
                comparator: MessageFilter.Includes
            }
        }
    }
    \endqml

    \sa MessageIntersectionFilter
*/

/*!
    \qmlproperty bool MessageUnionFilter::negated
    Setting negated to true will result in the filter being locially negated.
*/

/*!
    \qmlproperty list<MessageFilter> MessageUnionFilter::filters
    \default

    The filters to OR together.
*/
QDeclarativeMessageUnionFilter::QDeclarativeMessageUnionFilter(QObject *parent)
    : QDeclarativeMessageFilterBase(parent)
{
}

QDeclarativeListProperty<QDeclarativeMessageFilterBase> QDeclarativeMessageUnionFilter::filters()
{
    return QDeclarativeListProperty<QDeclarativeMessageFilterBase>(this, m_filters);
}

QMessageFilter QDeclarativeMessageUnionFilter::filter()
{
    if (m_filters.count() == 0)
        return QMessageFilter();

    QMessageFilter filter = m_filters.first()->filter();
    for (int i = 1; i < m_filters.count(); ++i)
        filter |= m_filters.at(i)->filter();

    if (m_negated)
        return ~filter;
    else
        return filter;
}

//===========================================================================

class QDeclarativeMessageFilterPrivate
{
public:
    QDeclarativeMessageFilterPrivate()
        : type(QDeclarativeMessageFilter::Sender)
        , comparator(QDeclarativeMessageFilter::Includes)
    {
    }

    QDeclarativeMessageFilter::FilterType type;
    QVariant value;
    QDeclarativeMessageFilter::Comparator comparator;
};

/*!
    \qmlclass MessageFilter QDeclarativeMessageFilter
    \brief The MessageFilter element specifies a message filter for MessageModel

    \ingroup qml-messaging

    This element is part of the \bold{QtMobility.messaging 1.1} module.

    Logical combinations of MessageFilters can be formed using MessageIntersectionFilter
    and MessageUnionFilter.

    Setting \l negated to true will negate the filter.

    The following example creates a filter which filters for messages
    less than 1024 bytes in size from
    \c martin or \c don, excluding replies from \c martin.  The
    messages will be sorted by descending timestamp.

    \qml
    model: MessageModel {
        sortBy: MessageModel.Timestamp
        sortOrder: MessageModel.DescendingOrder
        filter: MessageIntersectionFilter {
            MessageFilter {
                type: MessageFilter.Size
                value: 1024
                comparator: MessageFilter.LessThan
            }
            MessageUnionFilter {
                MessageIntersectionFilter {
                    MessageFilter {
                        type: MessageFilter.Sender
                        value: "martin"
                        comparator: MessageFilter.Includes
                    }
                    MessageFilter {
                        negated: true
                        type: MessageFilter.Subject
                        value: "re:"
                        comparator: MessageFilter.Includes
                    }
                }
                MessageFilter {
                    type: MessageFilter.Sender
                    value: "don"
                    comparator: MessageFilter.Includes
                }
            }
        }
    }
    \endqml
*/
QDeclarativeMessageFilter::QDeclarativeMessageFilter(QObject *parent)
    : QDeclarativeMessageFilterBase(parent), d(new QDeclarativeMessageFilterPrivate)
{
}

/*!
    \qmlproperty Variant MessageFilter::value
    Holds the value to filter on.
*/
QVariant QDeclarativeMessageFilter::value() const
{
    return d->value;
}

void QDeclarativeMessageFilter::setValue(const QVariant &value)
{
    if (d->value == value)
        return;

    d->value = value;
    emit valueChanged();
}

/*!
    \qmlproperty enumeration MessageFilter::type
    Holds the field to filter on.

    type may be one of the following:
    \table
    \header \o Filter type \o value type
    \row \o MessageFilter.AncestorFolder
        \o string
    \row \o MessageFilter.ParentFolder
        \o string
    \row \o MessageFilter.Priority
        \o enumeration
            \list
            \o MessageFilter.HighPriority
            \o MessageFilter.NormalPriority
            \o MessageFilter.LowPriority
            \endlist
    \row \o MessageFilter.Recipients
        \o string
    \row \o MessageFilter.Sender
        \o string
    \row \o MessageFilter.Size
        \o int
    \row \o MessageFilter.StandardFolder
        \o enumeration
            \list
            \o MessageFilter.InboxFolder
            \o MessageFilter.DraftsFolder
            \o MessageFilter.OutboxFolder
            \o MessageFilter.SentFolder
            \o MessageFilter.TrashFolder
            \endlist
    \row \o MessageFilter.Status
        \o enumeration
            \list
            \o MessageFilter.Read
            \o MessageFilter.HasAttachments
            \o MessageFilter.Incoming
            \o MessageFilter.Removed 
            \endlist
    \row \o MessageFilter.Subject
        \o string
    \row \o MessageFilter.Timestamp
        \o Date
    \row \o MessageFilter.ReceptionTimestamp
        \o Date
    \row \o MessageFilter.Type
        \o enumeration
            \list
            \o MessageFilter.Mms
            \o MessageFilter.Sms
            \o MessageFilter.Email
            \o MessageFilter.InstantMessage
            \endlist
    \endtable
*/
QDeclarativeMessageFilter::FilterType QDeclarativeMessageFilter::type() const
{
    return d->type;
}

void QDeclarativeMessageFilter::setType(FilterType type)
{
    if (type == d->type)
        return;
    d->type = type;
    emit typeChanged();
}

/*!
    \qmlproperty enumeration MessageFilter::comparator
    Holds the type of comparison to apply.

    comparator may be one of the following:
    \list
    \o MessageFilter.Includes
    \o MessageFilter.Excludes
    \o MessageFilter.Equality
    \o MessageFilter.Equal
    \o MessageFilter.NotEqual
    \o MessageFilter.LessThan
    \o MessageFilter.LessThanEqual
    \o MessageFilter.GreaterThan
    \o MessageFilter.GreaterThanEqual
    \endlist

    Note that not all comparators make sense for all filter types.  For example
    the relational comparators (LessThan, GreaterThan, etc.) make sense for Size
    filters, but the inclusion comprators (Includes, Excludes) do not.
*/
QDeclarativeMessageFilter::Comparator QDeclarativeMessageFilter::comparator() const
{
    return d->comparator;
}

void QDeclarativeMessageFilter::setComparator(QDeclarativeMessageFilter::Comparator comparator)
{
    if (d->comparator == comparator)
        return;

    d->comparator = comparator;
    emit comparatorChanged();
}

/*!
    \qmlproperty bool MessageFilter::negated
    Setting negated to true will result in the filter being locially negated.
*/


QMessageFilter QDeclarativeMessageFilter::filter()
{
    QMessageDataComparator::InclusionComparator inclusion = QMessageDataComparator::Includes;
    QMessageDataComparator::EqualityComparator equality = QMessageDataComparator::Equal;
    QMessageDataComparator::RelationComparator relation;

    enum ComparatorType { Inclusion, Equality, Relation };
    ComparatorType compType = Inclusion;

    switch (d->comparator) {
    case Includes:
        compType = Inclusion;
        inclusion = QMessageDataComparator::Includes;
        break;
    case Excludes:
        compType = Inclusion;
        inclusion = QMessageDataComparator::Excludes;
        break;
    case Equal:
        compType = Equality;
        equality = QMessageDataComparator::Equal;
        break;
    case NotEqual:
        compType = Equality;
        equality = QMessageDataComparator::NotEqual;
        break;
    case LessThan:
        compType = Relation;
        relation = QMessageDataComparator::LessThan;
        break;
    case LessThanEqual:
        compType = Relation;
        relation = QMessageDataComparator::LessThanEqual;
        break;
    case GreaterThan:
        compType = Relation;
        relation = QMessageDataComparator::GreaterThan;
        break;
    case GreaterThanEqual:
        compType = Relation;
        relation = QMessageDataComparator::GreaterThanEqual;
        break;
    }

    QMessageFilter filter;
    switch (d->type) {
    case AncestorFolder:
        if (compType == Equality)
            filter = QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byPath(d->value.toString(), equality));
        else if (compType == Inclusion)
            filter = QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byPath(d->value.toString(), inclusion));
        else
            qmlInfo(this) << "Relational comparators not valid for AncestorFolder filter";
        break;
    case ParentFolder:
        if (compType == Equality)
            filter = QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath(d->value.toString(), equality));
        else if (compType == Inclusion)
            filter = QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath(d->value.toString(), inclusion));
        else
            qmlInfo(this) << "Relational comparators not valid for ParentFolder filter";
        break;
    case Priority:
        if (compType == Equality)
            filter = QMessageFilter::byPriority(QMessage::Priority(d->value.toInt()), equality);
        else
            qmlInfo(this) << "Only Equal and NotEqual comparators supported by Priority filter";
        break;
    case Recipients:
        if (compType == Inclusion)
            filter = QMessageFilter::byRecipients(d->value.toString(), inclusion);
        else
            qmlInfo(this) << "Only Inclusion comparators are valid for Recipients filter";
        break;
    case Sender:
        if (compType == Equality)
            filter = QMessageFilter::bySender(d->value.toString(), equality);
        else if (compType == Inclusion)
            filter = QMessageFilter::bySender(d->value.toString(), inclusion);
        else
            qmlInfo(this) << "Relational comparators not valid for Sender filter";
        break;
    case Size:
        if (compType == Equality)
            filter = QMessageFilter::bySize(d->value.toInt(), equality);
        else if (compType == Relation)
            filter = QMessageFilter::bySize(d->value.toInt(), relation);
        else
            qmlInfo(this) << "Includes and Excludes comparators are not valid for Size filter";
        break;
    case StandardFolder:
        if (compType == Equality)
            filter = QMessageFilter::byStandardFolder(QMessage::StandardFolder(d->value.toInt()), equality);
        else
            qmlInfo(this) << "Only Equal and NotEqual comparators supported by StandardFolder filter";
        break;
    case Status:
        if (compType == Equality)
            filter = QMessageFilter::byStatus(QMessage::Status(d->value.toInt()), equality);
        else if (compType == Inclusion)
            filter = QMessageFilter::byStatus(QMessage::Status(d->value.toInt()), inclusion);
        else
            qmlInfo(this) << "Relational comparators not valid for Status filter";
        break;
    case Subject:
        if (compType == Equality)
            filter = QMessageFilter::bySubject(d->value.toString(), equality);
        else if (compType == Inclusion)
            filter = QMessageFilter::bySubject(d->value.toString(), inclusion);
        else
            qmlInfo(this) << "Relational comparators not valid for Subject filter";
        break;
    case Timestamp:
        if (compType == Equality)
            filter = QMessageFilter::byTimeStamp(d->value.toDateTime(), equality);
        else if (compType == Relation)
            filter = QMessageFilter::byTimeStamp(d->value.toDateTime(), relation);
        else
            qmlInfo(this) << "Includes and Excludes comparators are not valid for Timestamp filter";
        break;
    case ReceptionTimestamp:
        if (compType == Equality)
            filter = QMessageFilter::byReceptionTimeStamp(d->value.toDateTime(), equality);
        else if (compType == Relation)
            filter = QMessageFilter::byReceptionTimeStamp(d->value.toDateTime(), relation);
        else
            qmlInfo(this) << "Includes and Excludes comparators are not valid for Timestamp filter";
        break;
    case Type:
        if (compType == Equality)
            filter = QMessageFilter::byType(QMessage::Type(d->value.toInt()), equality);
        else if (compType == Inclusion)
            filter = QMessageFilter::byType(QMessage::Type(d->value.toInt()), inclusion);
        else
            qmlInfo(this) << "Relational comparators not valid for Type filter";
        break;
    default:
        qmlInfo(this) << "filter not supported" << d->type;
        break;
    }

    if (m_negated)
        return ~filter;

    return filter;
}


