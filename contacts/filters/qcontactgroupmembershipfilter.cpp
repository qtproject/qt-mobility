/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactgroupmembershipfilter.h"
#include "qcontactgroupmembershipfilter_p.h"
#include "qcontactfilter_p.h"
#include "qcontactmanager.h"

/*!
  \class QContactGroupMembershipFilter
  \brief The QContactGroupMembershipFilter class provides a filter based around a group membership criterion.
   \ingroup contacts-filters
 
  It may be used to select contacts which are members of a particular group
 */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactGroupMembershipFilter);

/*!
 * \fn QContactGroupMembershipFilter::QContactGroupMembershipFilter(const QContactFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new group membership filter
 */

/*!
 * Constructs a new group membership filter
 */
QContactGroupMembershipFilter::QContactGroupMembershipFilter()
    : QContactFilter(new QContactGroupMembershipFilterPrivate)
{
}

/*!
 * Sets the id of the group to which a contact must belong in order to match this filter to \a id
 * \sa groupId()
 */
void QContactGroupMembershipFilter::setGroupId(const QUniqueId& id)
{
    Q_D(QContactGroupMembershipFilter);
    d->m_id = id;
}

/*!
 * Returns the id of the group to which a contact must belong in order to match this filter
 * \sa setGroupId()
 */
QUniqueId QContactGroupMembershipFilter::groupId() const
{
    Q_D(const QContactGroupMembershipFilter);
    return d->m_id;
}
