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

#include "qcontactgroup.h"
#include "qcontactgroup_p.h"

QTM_BEGIN_NAMESPACE

/*! \class QContactGroup
    \brief The QContactGroup class acts as a container for a list of contacts.
    \ingroup contacts-main
 
  The QContactGroup class provides a collection of contacts that share some
  commonality.  A QContactGroup has a name and consists of a list of ids
  of contacts who are a member of the group.
 */

/*!
 * \fn QContactGroup::operator!=(const QContactGroup& other) const
 * Returns true if this group is not identical to the \a other group.
 * \sa operator==()
 */

/*! Create a new QContactGroup */
QContactGroup::QContactGroup()
    : d(new QContactGroupData)
{
}

/*! Create a new QContactGroup from \a other */
QContactGroup::QContactGroup(const QContactGroup &other)
    : d(other.d)
{
}

/*! Assign this QContactGroup to \a other */
QContactGroup& QContactGroup::operator=(const QContactGroup &other)
{
    d = other.d;
    return *this;
}

/*! Free any memory used by this QContactGroup */
QContactGroup::~QContactGroup()
{
}

/*! Returns true if the id, name and member list of this group is equal to that of \a other; otherwise, returns false */
bool QContactGroup::operator==(const QContactGroup& other) const
{
    if (d->m_id != other.d->m_id)
        return false;
    if (d->m_name != other.d->m_name)
        return false;
    if (d->m_members != other.d->m_members)
        return false;
    return true;
}

/*! Returns true if this QContactGroup has no id or name set and no members, or false otherwise. */
bool QContactGroup::isEmpty() const
{
    if (d->m_id != 0)
        return false;
    if (!d->m_name.isEmpty())
        return false;
    if (d->m_members.count() > 0)
        return false;

    return true;
}

/*! Return the id of this QContactGroup */
QUniqueId QContactGroup::id() const
{
    return d->m_id;
}

/*! Set the id of this QContactGroup to \a id.  Returns true if the id was set successfully, otherwise returns false */
void QContactGroup::setId(const QUniqueId& id)
{
    d->m_id = id;
}

/*! Return the name of the group */
QString QContactGroup::name() const
{
    return d->m_name;
}

/*! Set the name of the group to the given \a groupName.  Returns true if the name was set successfully, otherwise returns false */
void QContactGroup::setName(const QString& groupName)
{
    d->m_name = groupName;
}

/*! Add the contact identified by \a contactId to this QContactGroup.  Returns true if the contact was successfully added to the group, and false if the group already contains the contact, or the operation failed. */
bool QContactGroup::addMember(const QUniqueId& contactId)
{
    if (d->m_members.contains(contactId))
        return false;

    d->m_members.append(contactId);
    return true;
}

/*! Remove the contact identified by \a contactId from this QContactGroup.  Returns true if the contact was successfully removed from the group, and false if the group did not contain the contact, or the operation failed. */
bool QContactGroup::removeMember(const QUniqueId& contactId)
{
    if (!d->m_members.contains(contactId))
        return false;

    d->m_members.removeOne(contactId);
    return true;
}

/*! Returns true if the contact identified by \a contactId is a member of the group */
bool QContactGroup::hasMember(const QUniqueId& contactId) const
{
    return d->m_members.contains(contactId);
}

/*! Returns an iterator over the contactIds of the entire membership of the group */
QList<QUniqueId> QContactGroup::members() const
{
    return d->m_members;
}

/*!
 * Sets the list of ids of contacts which are members of this group to \a members.
 * If the order of contact ids in the group is important, this function should be used instead of \l addMember().
 * Returns true if the member list is set successfully, otherwise returns false.
 * \sa members(), addMember(), hasMember()
 */
void QContactGroup::setMembers(const QList<QUniqueId>& members)
{
    d->m_members = members;
}


// the following four functions are under consideration
// /*! Returns the position in the list of the contact with the given \a contactId, or -1 if no such contact is a member of the group */
//int QContactGroup::position(const QUniqueId& contactId) const
//{
//    d->m_error = QContactGroup::NoError;
//    int retn = d->m_members.indexOf(contactId);
//    if (retn == -1)
//        d->m_error = QContactGroup::ContactNotInGroupError;
//    return retn;
//}
//
// /*! Sets the position contact with the given \a contactId in the list to \a position.  Returns true on success, false if no such contact is a member of the group */
//bool QContactGroup::setPosition(int position, const QUniqueId& contactId)
//{
//    int currPosn = d->m_members.indexOf(contactId);
//    d->m_error = QContactGroup::ContactNotInGroupError;
//    if (currPosn == -1)
//        return false;
//
//    d->m_members.removeOne(contactId);
//    d->m_members.insert(position, contactId);
//    d->m_error = QContactGroup::NoError;
//    return true;    
//}
//QList<QContactDetail> QContactGroup::groupDetails() const
//{
//    d->m_error = QContactGroup::NoError;    
//    return d->m_details;
//}
//bool QContactGroup::setGroupDetails(const QList<QContactDetail>& details)
//{
//    d->m_details = details;
//    d->m_error = QContactGroup::NoError;
//    return true;
//}

QTM_END_NAMESPACE
