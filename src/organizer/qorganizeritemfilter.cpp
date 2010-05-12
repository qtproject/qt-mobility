/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** OrganizerItem: Nokia Corporation (qt-info@nokia.com)
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

#include "qorganizeritemfilter.h"
#include "qorganizeritemfilter_p.h"

#include "qorganizeritemintersectionfilter.h"
#include "qorganizeritemunionfilter.h"

#include "qorganizeritemmanager.h"



/*!
  \class QOrganizerItemFilter
  \brief The QOrganizerItemFilter class is used to select organizeritems made available
  through a QOrganizerItemManager.

  \ingroup organizeritems-main

  This class is used as a parameter to various functions offered by QOrganizerItemManager, to allow
  selection of organizeritems which have certain details or properties.
 */

/*!
  \enum QOrganizerItemFilter::FilterType
  Describes the type of the filter
  \value InvalidFilter An invalid filter which matches nothing
  \value OrganizerItemDetailFilter A filter which matches organizeritems containing one or more details of a particular definition with a particular value
  \value OrganizerItemDetailRangeFilter A filter which matches organizeritems containing one or more details of a particular definition whose values are within a particular range
  \value ChangeLogFilter A filter which matches organizeritems whose timestamps have been updated since some particular date and time
  \omitvalue ActionFilter A filter which matches organizeritems for which a particular action is available, or which contain a detail with a particular value for which a particular action is available
  \value IntersectionFilter A filter which matches all organizeritems that are matched by all filters it includes
  \value UnionFilter A filter which matches any organizeritem that is matched by any of the filters it includes
  \value LocalIdFilter A filter which matches any organizeritem whose local id is contained in a particular list of organizeritem local ids
  \value DefaultFilter A filter which matches everything
 */

/*!
  \enum QOrganizerItemFilter::MatchFlag
  Describes the semantics of matching followed by the filter
  \value MatchExactly Performs QVariant-based matching
  \value MatchContains The search term is contained in the item
  \value MatchStartsWith The search term matches the start of the item
  \value MatchEndsWith The search term matches the end of the item
  \value MatchFixedString Performs string-based matching. String-based comparisons are case-insensitive unless the \c MatchCaseSensitive flag is also specified
  \value MatchCaseSensitive The search is case sensitive
  \value MatchPhoneNumber The search term is considered to be in the form of a phone number, and special processing (removing dialing prefixes, non significant
         characters like '-'. ')' etc). may be performed when matching the item.
  \value MatchKeypadCollation The search term is in the form of text entered by a numeric phone keypad (such as ITU-T E.161 compliant keypads).  Each digit in the
         search term can represent a number of alphanumeric symbols.  For example, the search string "43556" would match items "HELLO", "GEKKO", "HELL6" and "43556" among others.
         Accented characters and other punctuation characters may additionally be matched by the QOrganizerItemManager in a way consistent with the platform.
 */

/*!
  \fn QOrganizerItemFilter::operator!=(const QOrganizerItemFilter& other) const
  Returns true if this filter is not identical to the \a other filter.
  \sa operator==()
 */

#if !defined(Q_CC_MWERKS)
template<> QTM_PREPEND_NAMESPACE(QOrganizerItemFilterPrivate) *QSharedDataPointer<QTM_PREPEND_NAMESPACE(QOrganizerItemFilterPrivate)>::clone()
{
    return d->clone();
}
#endif

QTM_BEGIN_NAMESPACE

/*! Constructs an empty filter */
QOrganizerItemFilter::QOrganizerItemFilter()
    : d_ptr(0)
{
}

/*! Constructs a new copy of \a other */
QOrganizerItemFilter::QOrganizerItemFilter(const QOrganizerItemFilter& other)
    : d_ptr(other.d_ptr)
{
}

/*! Assigns this filter to be \a other */
QOrganizerItemFilter& QOrganizerItemFilter::operator=(const QOrganizerItemFilter& other)
{
    if (this != &other) {
        d_ptr = other.d_ptr;
    }
    return *this;
}

/*! Cleans up the memory used by this filter */
QOrganizerItemFilter::~QOrganizerItemFilter()
{
}

/*! Returns the type of the filter */
QOrganizerItemFilter::FilterType QOrganizerItemFilter::type() const
{
    if (!d_ptr)
        return QOrganizerItemFilter::DefaultFilter;
    return d_ptr->type();
}

/*! Returns true if the filter has the same type and criteria as \a other */
bool QOrganizerItemFilter::operator==(const QOrganizerItemFilter& other) const
{
    /* A default filter is only equal to other default filters */
    if (!d_ptr)
        return !other.d_ptr;

    /* Different types can't be equal */
    if (other.type() != type())
        return false;

    /* Otherwise, use the virtual op == */
    return d_ptr->compare(other.d_ptr);
}

/*!
  \internal
  Constructs a new filter from the given data pointer \a d
 */
QOrganizerItemFilter::QOrganizerItemFilter(QOrganizerItemFilterPrivate *d)
    : d_ptr(d)
{

}

/*!
 \relates QOrganizerItemFilter
 Returns a filter which is the intersection of the \a left and \a right filters
 \sa QOrganizerItemIntersectionFilter
 */
const QOrganizerItemFilter operator&(const QOrganizerItemFilter& left, const QOrganizerItemFilter& right)
{
    // XXX TODO: empty intersection/union operations are not well defined yet.
    //if (left.type() == QOrganizerItemFilter::Intersection) {
    //    QOrganizerItemIntersectionFilter bf(left);
    //    /* we can just add the right to this one */
    //    bf.append(right);
    //    return bf;
    //}

    //if (right.type() == QOrganizerItemFilter::Intersection) {
    //    QOrganizerItemIntersectionFilter bf(right);
    //    /* we can prepend the left to this one */
    //    bf.prepend(left);
    //    return bf;
    //}

    /* usual fallback case */
    QOrganizerItemIntersectionFilter nif;
    nif << left << right;
    return nif;
}

/*!
 \relates QOrganizerItemFilter
 Returns a filter which is the union of the \a left and \a right filters
 \sa QOrganizerItemUnionFilter
 */
const QOrganizerItemFilter operator|(const QOrganizerItemFilter& left, const QOrganizerItemFilter& right)
{
    if (left.type() == QOrganizerItemFilter::UnionFilter) {
        QOrganizerItemUnionFilter bf(left);
        /* we can just add the right to this one */
        bf.append(right);
        return bf;
    }

    if (right.type() == QOrganizerItemFilter::UnionFilter) {
        QOrganizerItemUnionFilter bf(right);
        /* we can prepend the left to this one */
        bf.prepend(left);
        return bf;
    }

    /* usual fallback case */
    QOrganizerItemUnionFilter nif;
    nif << left << right;
    return nif;
}
QTM_END_NAMESPACE
