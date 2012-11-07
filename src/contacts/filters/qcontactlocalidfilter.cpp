/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "qcontactlocalidfilter.h"
#include "qcontactlocalidfilter_p.h"
#include "qcontactfilter_p.h"
#include "qcontactmanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactLocalIdFilter
  \brief The QContactLocalIdFilter class provides a filter based around a list of contact ids


  \inmodule QtContacts
   \since 1.0

  \ingroup contacts-filters

  It may be used to select contacts whose ids are contained in the given list of ids.
 */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactLocalIdFilter);

/*!
 * \fn QContactLocalIdFilter::QContactLocalIdFilter(const QContactFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new contact local id filter
 * \since 1.0
 */

/*!
 * Constructs a new contact local id filter
 */
QContactLocalIdFilter::QContactLocalIdFilter()
    : QContactFilter(new QContactLocalIdFilterPrivate)
{
}

/*!
 * Sets the list which contains the ids of possible matching contacts to \a ids
 * \sa ids()
 * \since 1.0
 */
void QContactLocalIdFilter::setIds(const QList<QContactLocalId>& ids)
{
    Q_D(QContactLocalIdFilter);
    d->m_ids = ids;
}

/*!
 * Adds the id \a id into the list which contains the ids of possible matching contacts
 * \sa setIds()
 */
void QContactLocalIdFilter::add(const QContactLocalId& id)
{
    Q_D(QContactLocalIdFilter);
    if (!d->m_ids.contains(id))
        d->m_ids.append(id);
}

/*!
 * Removes the id \a id from the list which contains the ids of possible matching contacts,
 * if it is contained in the list, otherwise has no effect.
 * \sa clear()
 */
void QContactLocalIdFilter::remove(const QContactLocalId& id)
{
    Q_D(QContactLocalIdFilter);
    d->m_ids.removeAll(id);
}

/*!
 * Clears the list which contains the ids of possible matching contacts.
 * A local id filter with a cleared list will match no contacts.
 * \sa setIds()
 */
void QContactLocalIdFilter::clear()
{
    Q_D(QContactLocalIdFilter);
    d->m_ids.clear();
}

/*!
 * Returns the list of ids of contacts which match this filter
 * \sa setIds()
 * \since 1.0
 */
QList<QContactLocalId> QContactLocalIdFilter::ids() const
{
    Q_D(const QContactLocalIdFilter);
    return d->m_ids;
}

QTM_END_NAMESPACE
