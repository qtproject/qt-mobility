/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qorganizeritemchangelogfilter.h"
#include "qorganizeritemchangelogfilter_p.h"
#include "qorganizeritemfilter_p.h"
#include "qorganizeritemmanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemChangeLogFilter
  \brief The QOrganizerItemChangeLogFilter class provides a filter based around a
  organizeritem timestamp criterion.
   \ingroup organizeritems-filters
 
  It may be used to select organizeritems which have been updated or created
  within a certain period of time.
 */

/*!
 * \enum QOrganizerItemChangeLogFilter::EventType
 * Enumerates the type of changes which a changelog filter can match against
 * \value EventAdded
 * \value EventChanged
 * \value EventRemoved
 */

Q_IMPLEMENT_ORGANIZERITEMFILTER_PRIVATE(QOrganizerItemChangeLogFilter);

/*!
 * \fn QOrganizerItemChangeLogFilter::QOrganizerItemChangeLogFilter(const QOrganizerItemFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new changelog filter
 */

/*!
 * Constructs a new changelog filter which matches changes of the specified \a type
 */
QOrganizerItemChangeLogFilter::QOrganizerItemChangeLogFilter(QOrganizerItemChangeLogFilter::EventType type)
    : QOrganizerItemFilter(new QOrganizerItemChangeLogFilterPrivate(type))
{
}

/*!
 * Sets the type of change that this filter will match against to \a type
 * \sa eventType()
 */
void QOrganizerItemChangeLogFilter::setEventType(QOrganizerItemChangeLogFilter::EventType type)
{
    Q_D(QOrganizerItemChangeLogFilter);
    d->m_eventType = type;
}

/*!
 * Sets the date and time lower-bound criterion of the filter to \a since
 * \sa since()
 */
void QOrganizerItemChangeLogFilter::setSince(const QDateTime& since)
{
    Q_D(QOrganizerItemChangeLogFilter);
    d->m_since = since;
}

/*!
 * Returns the date and time lower-bound criterion of the filter
 * \sa setSince()
 */
QDateTime QOrganizerItemChangeLogFilter::since() const
{
    Q_D(const QOrganizerItemChangeLogFilter);
    return d->m_since;
}

/*!
 * Returns the type of change that this filter will match against
 * \sa setEventType()
 */
QOrganizerItemChangeLogFilter::EventType QOrganizerItemChangeLogFilter::eventType() const
{
    Q_D(const QOrganizerItemChangeLogFilter);
    return d->m_eventType;
}

QTM_END_NAMESPACE
