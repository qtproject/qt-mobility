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

#include "qorganizeritemlocalidfilter.h"
#include "qorganizeritemlocalidfilter_p.h"
#include "qorganizeritemfilter_p.h"
#include "qorganizeritemmanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemLocalIdFilter
  \brief The QOrganizerItemLocalIdFilter class provides a filter based around a list of organizeritem ids
  \inmodule QtOrganizer
  \ingroup organizer-filters
  
  It may be used to select organizeritems whose ids are contained in the given list of ids.
 */

Q_IMPLEMENT_ORGANIZERITEMFILTER_PRIVATE(QOrganizerItemLocalIdFilter);

/*!
 * \fn QOrganizerItemLocalIdFilter::QOrganizerItemLocalIdFilter(const QOrganizerItemFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new organizeritem local id filter
 */

/*!
 * Constructs a new organizeritem local id filter
 */
QOrganizerItemLocalIdFilter::QOrganizerItemLocalIdFilter()
    : QOrganizerItemFilter(new QOrganizerItemLocalIdFilterPrivate)
{
}

/*!
 * Sets the list which contains the ids of possible matching organizeritems to \a ids
 * \sa ids()
 */
void QOrganizerItemLocalIdFilter::setIds(const QList<QOrganizerItemLocalId>& ids)
{
    Q_D(QOrganizerItemLocalIdFilter);
    d->m_ids = ids;
}

/*!
 * Returns the list of ids of organizeritems which match this filter
 * \sa setIds()
 */
QList<QOrganizerItemLocalId> QOrganizerItemLocalIdFilter::ids() const
{
    Q_D(const QOrganizerItemLocalIdFilter);
    return d->m_ids;
}

QTM_END_NAMESPACE
