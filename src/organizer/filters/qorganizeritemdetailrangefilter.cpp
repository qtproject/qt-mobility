/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qorganizeritemdetailrangefilter.h"
#include "qorganizeritemdetailrangefilter_p.h"
#include "qorganizeritemfilter_p.h"
#include "qorganizermanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemDetailRangeFilter
  \brief The QOrganizerItemDetailRangeFilter class provides a filter based around
  a detail value range criterion.
  \inmodule QtOrganizer
  \ingroup organizer-filters
  \since 1.1

  It may be used to select organizeritems which contain a detail of a particular definition with a particular value
 */

Q_IMPLEMENT_ORGANIZERITEMFILTER_PRIVATE(QOrganizerItemDetailRangeFilter);

/*!
 * \fn QOrganizerItemDetailRangeFilter::QOrganizerItemDetailRangeFilter(const QOrganizerItemFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new detail range filter
   \since 1.1
 */

/*!
 * \enum QOrganizerItemDetailRangeFilter::RangeFlag
 * Enumerates the semantics of the boundary conditions of the detail range filter
 * \value IncludeLower
 * \value IncludeUpper
 * \value ExcludeLower
 * \value ExcludeUpper
 */

/*!
 * Constructs a new detail range filter
   \since 1.1
 */
QOrganizerItemDetailRangeFilter::QOrganizerItemDetailRangeFilter()
    : QOrganizerItemFilter(new QOrganizerItemDetailRangeFilterPrivate)
{
}

/*!
 * Sets the value range criterion of the filter to within \a min and \a max, with boundary conditions specified in the given \a flags
 * \sa minValue(), maxValue()
   \since 1.1
 */
void QOrganizerItemDetailRangeFilter::setRange(const QVariant& min, const QVariant& max, RangeFlags flags)
{
    Q_D(QOrganizerItemDetailRangeFilter);
    d->m_minValue = min;
    d->m_maxValue = max;
    d->m_rangeflags = flags;
}

/*!
  Sets the match flags of the filter criterion to \a flags

  Not all flags are supported by a range filter.  The supported flags include:

  \list
  \o QOrganizerItemFilter::MatchExactly
  \o QOrganizerItemFilter::MatchFixedString
  \o QOrganizerItemFilter::MatchCaseSensitive
  \endlist

  Unsupported flags will be ignored.

  \sa matchFlags()
  \since 1.1
 */
void QOrganizerItemDetailRangeFilter::setMatchFlags(QOrganizerItemFilter::MatchFlags flags)
{
    Q_D(QOrganizerItemDetailRangeFilter);
    flags &= (QOrganizerItemFilter::MatchExactly | QOrganizerItemFilter::MatchFixedString | QOrganizerItemFilter::MatchCaseSensitive);
    d->m_flags = flags;
}

/*!
 * Sets the name of the detail definition of which type details will be inspected for matching values to \a definitionName,
 * and the name of the field which will be inspected in details of that definition to \a fieldName.
 * \sa detailDefinitionName(), detailFieldName()
   \since 1.1
 */
void QOrganizerItemDetailRangeFilter::setDetailDefinitionName(const QString& definitionName, const QString& fieldName)
{
    Q_D(QOrganizerItemDetailRangeFilter);
    d->m_defId = definitionName;
    d->m_fieldId = fieldName;
}

/*!
 * Returns the match flags of the criterion, which define semantics such as case sensitivity, prefix matching, exact matching, etc.
 * \sa setMatchFlags()
   \since 1.1
 */
QOrganizerItemFilter::MatchFlags QOrganizerItemDetailRangeFilter::matchFlags() const
{
    Q_D(const QOrganizerItemDetailRangeFilter);
    return d->m_flags;
}

/*!
 * Returns the definition name of the details which will be inspected for matching values
 * \sa setDetailDefinitionName()
   \since 1.1
 */
QString QOrganizerItemDetailRangeFilter::detailDefinitionName() const
{
    Q_D(const QOrganizerItemDetailRangeFilter);
    return d->m_defId;
}

/*!
 * Returns the name of the field which contains the value which will be matched against the value criterion
 * \sa setDetailDefinitionName()
   \since 1.1
 */
QString QOrganizerItemDetailRangeFilter::detailFieldName() const
{
    Q_D(const QOrganizerItemDetailRangeFilter);
    return d->m_fieldId;
}

/*!
 * Returns the lower bound of the value range criterion
 * \sa setRange()
   \since 1.1
 */
QVariant QOrganizerItemDetailRangeFilter::minValue() const
{
    Q_D(const QOrganizerItemDetailRangeFilter);
    return d->m_minValue;
}

/*!
 * Returns the upper bound of the value range criterion
 * \sa setRange()
   \since 1.1
 */
QVariant QOrganizerItemDetailRangeFilter::maxValue() const
{
    Q_D(const QOrganizerItemDetailRangeFilter);
    return d->m_maxValue;
}

/*!
 * Returns a set of flags which defines the boundary condition semantics of the value range criterion
 * \sa setRange()
   \since 1.1
 */
QOrganizerItemDetailRangeFilter::RangeFlags QOrganizerItemDetailRangeFilter::rangeFlags() const
{
    Q_D(const QOrganizerItemDetailRangeFilter);
    return d->m_rangeflags;
}

QTM_END_NAMESPACE
