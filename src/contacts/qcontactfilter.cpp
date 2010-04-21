/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qcontactfilter.h"
#include "qcontactfilter_p.h"

#include "qcontactintersectionfilter.h"
#include "qcontactunionfilter.h"

#include "qcontactmanager.h"



/*!
  \class QContactFilter
  \brief The QContactFilter class is used to select contacts made available
  through a QContactManager.

  \ingroup contacts-main
 
  This class is used as a parameter to various functions offered by QContactManager, to allow
  selection of contacts which have certain details or properties.
 */

/*!
  \enum QContactFilter::FilterType
  Describes the type of the filter
  \value InvalidFilter An invalid filter which matches nothing
  \value ContactDetailFilter A filter which matches contacts containing one or more details of a particular definition with a particular value
  \value ContactDetailRangeFilter A filter which matches contacts containing one or more details of a particular definition whose values are within a particular range
  \value ChangeLogFilter A filter which matches contacts whose timestamps have been updated since some particular date and time
  \omitvalue ActionFilter A filter which matches contacts for which a particular action is available, or which contain a detail with a particular value for which a particular action is available
  \value RelationshipFilter A filter which matches contacts which participate in a particular type of relationship, or relationship with a specified contact
  \value IntersectionFilter A filter which matches all contacts that are matched by all filters it includes
  \value UnionFilter A filter which matches any contact that is matched by any of the filters it includes
  \value LocalIdFilter A filter which matches any contact whose local id is contained in a particular list of contact local ids
  \value DefaultFilter A filter which matches everything
 */

/*!
  \enum QContactFilter::MatchFlag
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
         Accented characters and other punctuation characters may additionally be matched by the QContactManager in a way consistent with the platform.
 */

/*!
  \fn QContactFilter::operator!=(const QContactFilter& other) const
  Returns true if this filter is not identical to the \a other filter.
  \sa operator==()
 */

#if !defined(Q_CC_MWERKS)
template<> QTM_PREPEND_NAMESPACE(QContactFilterPrivate) *QSharedDataPointer<QTM_PREPEND_NAMESPACE(QContactFilterPrivate)>::clone()
{
    return d->clone();
}
#endif

QTM_BEGIN_NAMESPACE

/*! Constructs an empty filter */
QContactFilter::QContactFilter()
    : d_ptr(0)
{
}

/*! Constructs a new copy of \a other */
QContactFilter::QContactFilter(const QContactFilter& other)
    : d_ptr(other.d_ptr)
{
}

/*! Assigns this filter to be \a other */
QContactFilter& QContactFilter::operator=(const QContactFilter& other)
{
    if (this != &other) {
        d_ptr = other.d_ptr;
    }
    return *this;
}

/*! Cleans up the memory used by this filter */
QContactFilter::~QContactFilter()
{
}

/*! Returns the type of the filter */
QContactFilter::FilterType QContactFilter::type() const
{
    if (!d_ptr)
        return QContactFilter::DefaultFilter;
    return d_ptr->type();
}

/*! Returns true if the filter has the same type and criteria as \a other */
bool QContactFilter::operator==(const QContactFilter& other) const
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
QContactFilter::QContactFilter(QContactFilterPrivate *d)
    : d_ptr(d)
{

}

/*!
 \relates QContactFilter
 Returns a filter which is the intersection of the \a left and \a right filters
 \sa QContactIntersectionFilter
 */
const QContactFilter operator&(const QContactFilter& left, const QContactFilter& right)
{
    // XXX TODO: empty intersection/union operations are not well defined yet.
    //if (left.type() == QContactFilter::Intersection) {
    //    QContactIntersectionFilter bf(left);
    //    /* we can just add the right to this one */
    //    bf.append(right);
    //    return bf;
    //}

    //if (right.type() == QContactFilter::Intersection) {
    //    QContactIntersectionFilter bf(right);
    //    /* we can prepend the left to this one */
    //    bf.prepend(left);
    //    return bf;
    //}

    /* usual fallback case */
    QContactIntersectionFilter nif;
    nif << left << right;
    return nif;
}

/*!
 \relates QContactFilter
 Returns a filter which is the union of the \a left and \a right filters
 \sa QContactUnionFilter
 */
const QContactFilter operator|(const QContactFilter& left, const QContactFilter& right)
{
    if (left.type() == QContactFilter::UnionFilter) {
        QContactUnionFilter bf(left);
        /* we can just add the right to this one */
        bf.append(right);
        return bf;
    }

    if (right.type() == QContactFilter::UnionFilter) {
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
QTM_END_NAMESPACE
