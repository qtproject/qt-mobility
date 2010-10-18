/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** OrganizerItem: Nokia Corporation (qt-info@nokia.com)
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

#include "qorganizeritemfilter.h"
#include "qorganizeritemfilter_p.h"
#include "qorganizeritemfilters.h"

#include "qorganizeritemintersectionfilter.h"
#include "qorganizeritemunionfilter.h"

#include "qorganizermanager.h"



/*!
  \class QOrganizerItemFilter
  \brief The QOrganizerItemFilter class is used to select organizer items made available
  through a QOrganizerManager.

  \inmodule QtOrganizer

  \ingroup organizer-main

  This class is used as a parameter to various functions offered by QOrganizerManager, to allow
  selection of items which have certain details or properties.
 */

/*!
  \enum QOrganizerItemFilter::FilterType
  Describes the type of the filter
  \value InvalidFilter An invalid filter which matches nothing
  \value OrganizerItemDetailFilter A filter which matches items containing one or more details of a particular definition with a particular value
  \value OrganizerItemDetailRangeFilter A filter which matches items containing one or more details of a particular definition whose values are within a particular range
  \value ChangeLogFilter A filter which matches items whose timestamps have been updated since some particular date and time
  \omitvalue ActionFilter A filter which matches items for which a particular action is available, or which contain a detail with a particular value for which a particular action is available
  \value IntersectionFilter A filter which matches all items that are matched by all filters it includes
  \value UnionFilter A filter which matches any organizer item that is matched by any of the filters it includes
  \value IdFilter A filter which matches any organizer item whose id is contained in a particular list of organizer item ids
  \value CollectionFilter A filter which matches any organizer item that is matched by collection.
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

#ifndef QT_NO_DATASTREAM
/*!
 * Writes \a filter to the stream \a out.
 *
 * A QOrganizerItemIdFilter will not be preserved if streamed to a QDataStream.
 */
QDataStream& operator<<(QDataStream& out, const QOrganizerItemFilter& filter)
{
    quint8 formatVersion = 1; // Version of QDataStream format for QOrganizerItemDetailFilter
    out << formatVersion << static_cast<quint32>(filter.type());
    if (filter.d_ptr)
        filter.d_ptr->outputToStream(out, formatVersion);
    return out;
}

/*!
 * Reads an organizer item filter from stream \a in into \a filter.
 *
 * A QOrganizerItemIdFilter will not be preserved if streamed from a QDataStream.
 */
QDataStream& operator>>(QDataStream& in, QOrganizerItemFilter& filter)
{
    quint8 formatVersion;
    in >> formatVersion;
    if (formatVersion == 1) {
        quint32 type;
        in >> type;
        switch (type) {
            case QOrganizerItemFilter::InvalidFilter:
                filter = QOrganizerItemInvalidFilter();
                break;
            case QOrganizerItemFilter::OrganizerItemDetailFilter:
                filter = QOrganizerItemDetailFilter();
                break;
            case QOrganizerItemFilter::OrganizerItemDetailRangeFilter:
                filter = QOrganizerItemDetailRangeFilter();
                break;
            case QOrganizerItemFilter::ChangeLogFilter:
                filter = QOrganizerItemChangeLogFilter();
                break;
            case QOrganizerItemFilter::IntersectionFilter:
                filter = QOrganizerItemIntersectionFilter();
                break;
            case QOrganizerItemFilter::UnionFilter:
                filter = QOrganizerItemUnionFilter();
                break;
            case QOrganizerItemFilter::IdFilter:
                filter = QOrganizerItemIdFilter();
                break;
            case QOrganizerItemFilter::DefaultFilter:
                filter = QOrganizerItemFilter();
                break;
        }

        if (filter.d_ptr) {
            filter.d_ptr->inputFromStream(in, formatVersion);
        }
    } else {
        in.setStatus(QDataStream::ReadCorruptData);
    }
    return in;
}
#endif


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
