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

#include "qcontactchangelogfilter.h"
#include "qcontactchangelogfilter_p.h"
#include "qcontactfilter_p.h"
#include "qcontactmanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactChangeLogFilter
  \brief The QContactChangeLogFilter class provides a filter based around a
  contact timestamp criterion.

  \inmodule QtContacts
   \since 1.0

   \ingroup contacts-filters

  It may be used to select contacts which have been updated or created
  within a certain period of time.
 */

/*!
 * \enum QContactChangeLogFilter::EventType
 * Enumerates the type of changes which a changelog filter can match against
 * \value EventAdded
 * \value EventChanged
 * \value EventRemoved
 */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactChangeLogFilter);

/*!
 * \fn QContactChangeLogFilter::QContactChangeLogFilter(const QContactFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new changelog filter
 * \since 1.0
 */

/*!
 * Constructs a new changelog filter which matches changes of the specified \a type
 */
QContactChangeLogFilter::QContactChangeLogFilter(QContactChangeLogFilter::EventType type)
    : QContactFilter(new QContactChangeLogFilterPrivate(type))
{
}

/*!
 * Sets the type of change that this filter will match against to \a type
 * \sa eventType()
 * \since 1.0
 */
void QContactChangeLogFilter::setEventType(QContactChangeLogFilter::EventType type)
{
    Q_D(QContactChangeLogFilter);
    d->m_eventType = type;
}

/*!
 * Sets the date and time lower-bound criterion of the filter to \a since
 * \sa since()
 * \since 1.0
 */
void QContactChangeLogFilter::setSince(const QDateTime& since)
{
    Q_D(QContactChangeLogFilter);
    d->m_since = since;
}

/*!
 * Returns the date and time lower-bound criterion of the filter
 * \sa setSince()
 * \since 1.0
 */
QDateTime QContactChangeLogFilter::since() const
{
    Q_D(const QContactChangeLogFilter);
    return d->m_since;
}

/*!
 * Returns the type of change that this filter will match against
 * \sa setEventType()
 * \since 1.0
 */
QContactChangeLogFilter::EventType QContactChangeLogFilter::eventType() const
{
    Q_D(const QContactChangeLogFilter);
    return d->m_eventType;
}

QTM_END_NAMESPACE
