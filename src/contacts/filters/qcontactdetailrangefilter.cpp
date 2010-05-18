/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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

#include "qcontactdetailrangefilter.h"
#include "qcontactdetailrangefilter_p.h"
#include "qcontactfilter_p.h"
#include "qcontactmanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactDetailRangeFilter
  \brief The QContactDetailRangeFilter class provides a filter based around
  a detail value range criterion.
  \ingroup contacts-filters
 
  It may be used to select contacts which contain a detail of a particular definition with a particular value that lies in
  a range (either open or closed).
 */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactDetailRangeFilter);

/*!
 * \fn QContactDetailRangeFilter::QContactDetailRangeFilter(const QContactFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new detail range filter
 */

/*!
 * \enum QContactDetailRangeFilter::RangeFlag
 * Enumerates the semantics of the boundary conditions of the detail range filter
 * \value IncludeLower The filter will match if the field value is equal to or greater than the minimum value
 * \value IncludeUpper The filter will match if the field value is equal to or less than the maximum value
 * \value ExcludeLower The filter will match if the field value is greater than the minimum value (but not equal)
 * \value ExcludeUpper The filter will match if the field value is less than the maximum value (but not equal)
 */

/*!
 * Constructs a new detail range filter
 */
QContactDetailRangeFilter::QContactDetailRangeFilter()
    : QContactFilter(new QContactDetailRangeFilterPrivate)
{
}

/*!
 * Sets the value range criterion of the filter to within \a min and \a max, with boundary conditions specified in the given \a flags.
 * If \a min or \a max is a null variant, that condition will not be checked.  For example, to check for
 * fields with a value greater than 7, you would specify:
 * \code
 * filter.setRange(7, QVariant(), QContactDetailRangeFilter::ExcludeLower);
 * \endcode
 * \sa minValue(), maxValue()
 */
void QContactDetailRangeFilter::setRange(const QVariant& min, const QVariant& max, RangeFlags flags)
{
    Q_D(QContactDetailRangeFilter);
    d->m_minValue = min;
    d->m_maxValue = max;
    d->m_rangeflags = flags;
}

/*!
 * Sets the match flags of the filter criterion to \a flags
 *
 * Not all flags are supported by a range filter.  The supported flags include:
 *
 * \list
 * \o QContactDetail::MatchExactly
 * \o QContactDetail::MatchFixedString
 * \o QContactDetail::MatchCaseSensitive
 * \endlist
 * \sa matchFlags()
 */
void QContactDetailRangeFilter::setMatchFlags(QContactFilter::MatchFlags flags)
{
    Q_D(QContactDetailRangeFilter);
    d->m_flags = flags;
}

/*!
 * Sets the name of the detail definition of which type details will be inspected for matching values to \a definitionName,
 * and the name of the field which will be inspected in details of that definition to \a fieldName.
 * \sa detailDefinitionName(), detailFieldName()
 */
void QContactDetailRangeFilter::setDetailDefinitionName(const QString& definitionName, const QString& fieldName)
{
    Q_D(QContactDetailRangeFilter);
    d->m_defId = definitionName;
    d->m_fieldId = fieldName;
}

/*!
 * Returns the match flags of the criterion, which define semantics such as case sensitivity, and exact matching.
 * \sa setMatchFlags()
 */
QContactFilter::MatchFlags QContactDetailRangeFilter::matchFlags() const
{
    Q_D(const QContactDetailRangeFilter);
    return d->m_flags;
}

/*!
 * Returns the definition name of the details which will be inspected for matching values
 * \sa setDetailDefinitionName()
 */
QString QContactDetailRangeFilter::detailDefinitionName() const
{
    Q_D(const QContactDetailRangeFilter);
    return d->m_defId;
}

/*!
 * Returns the name of the field which contains the value which will be matched against the value criterion
 * \sa setDetailDefinitionName()
 */
QString QContactDetailRangeFilter::detailFieldName() const
{
    Q_D(const QContactDetailRangeFilter);
    return d->m_fieldId;
}

/*!
 * Returns the lower bound of the value range criterion.
 * If this value is null, there is no lower bound.
 * If it is valid, the \l rangeFlags() determines whether this value is included in the valid values.
 * \sa setRange()
 */
QVariant QContactDetailRangeFilter::minValue() const
{
    Q_D(const QContactDetailRangeFilter);
    return d->m_minValue;
}

/*!
 * Returns the upper bound of the value range criterion
 * If this value is null, there is no upper bound.
 * If it is valid, the \l rangeFlags() determines whether this value is included in the valid values.
 * \sa setRange()
 */
QVariant QContactDetailRangeFilter::maxValue() const
{
    Q_D(const QContactDetailRangeFilter);
    return d->m_maxValue;
}

/*!
 * Returns a set of flags which defines the boundary condition semantics of the value range criterion
 * \sa setRange()
 */
QContactDetailRangeFilter::RangeFlags QContactDetailRangeFilter::rangeFlags() const
{
    Q_D(const QContactDetailRangeFilter);
    return d->m_rangeflags;
}

QTM_END_NAMESPACE
