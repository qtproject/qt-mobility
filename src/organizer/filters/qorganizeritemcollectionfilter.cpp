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

#include "qorganizeritemcollectionfilter.h"
#include "qorganizeritemcollectionfilter_p.h"
#include "qorganizeritemfilter_p.h"
#include "qorganizermanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemCollectionFilter
  \brief The QOrganizerItemCollectionFilter class provides a filter based around a list of organizeritem ids
  \since 1.1

  \ingroup organizeritems-filters

  \inmodule QtOrganizer

  It may be used to select organizeritems whose ids are contained in the given list of ids.
 */

Q_IMPLEMENT_ORGANIZERITEMFILTER_PRIVATE(QOrganizerItemCollectionFilter);

/*!
 * \fn QOrganizerItemCollectionFilter::QOrganizerItemCollectionFilter(const QOrganizerItemFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new organizeritem id filter
 \since 1.1
 */

/*!
 * Constructs a new organizeritem id filter
   \since 1.1
 */
QOrganizerItemCollectionFilter::QOrganizerItemCollectionFilter()
    : QOrganizerItemFilter(new QOrganizerItemCollectionFilterPrivate)
{
}

/*!
 * Clears the list which contains the ids of possible matching organizeritems, and sets it to the list
 * which contains a single id \a id.
   \since 1.1
 */
void QOrganizerItemCollectionFilter::setCollectionId(const QOrganizerCollectionId &id)
{
    Q_D(QOrganizerItemCollectionFilter);
    d->m_ids.clear();
    d->m_ids.insert(id);
}

/*!
 * Sets the list which contains the ids of possible matching organizeritems to \a ids
   \since 1.1
 */
void QOrganizerItemCollectionFilter::setCollectionIds(const QSet<QOrganizerCollectionId> &ids)
{
    Q_D(QOrganizerItemCollectionFilter);
    d->m_ids = ids;
}

/*!
 * Returns the list of ids of organizeritems which match this filter
   \since 1.1
 */
QSet<QOrganizerCollectionId> QOrganizerItemCollectionFilter::collectionIds() const
{
    Q_D(const QOrganizerItemCollectionFilter);
    return d->m_ids;
}

QTM_END_NAMESPACE
