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

#include "qcontactactiontarget.h"
#include "qcontactactiontarget_p.h"

#include "qcontact.h"

#include <QHash>

QTM_BEGIN_NAMESPACE

/*!
  \class QContactActionTarget
  \brief The QContactActionTarget class provides information about the
  target of an action.  It may be either a contact, a contact and a detail
  of that contact, or a contact and a list of the details of the contact,
  which together should be used by the action.
  \ingroup contacts-actions
*/

/*!
 * Constructs a new action target from the given \a contact and the given list of that contact's \a details.
 * If no \a contact is specified, the target will be invalid.  If a \a contact but no \a details are specified,
 * the target will be valid, but the action which operates on the target may fail (for example, if it requires
 * a certain detail to be specified in order to perform the action).
 */
QContactActionTarget::QContactActionTarget(const QContact& contact, const QList<QContactDetail>& details)
        : d(new QContactActionTargetPrivate(contact, details))
{
}

/*!
 * Constructs a new action target from the given \a contact and a specific \a detail of that contact
 */
QContactActionTarget::QContactActionTarget(const QContact& contact, const QContactDetail& detail)
        : d(new QContactActionTargetPrivate(contact, QList<QContactDetail>() << detail))
{
}

/*!
 * Constructs a copy of the \a other action target
 */
QContactActionTarget::QContactActionTarget(const QContactActionTarget& other)
        : d(other.d)
{
}

/*!
 * Assigns this action target to be equal to \a other
 */
QContactActionTarget& QContactActionTarget::operator=(const QContactActionTarget& other)
{
    d = other.d;
    return *this;
}

/*!
 * Cleans up any memory in use by the action target
 */
QContactActionTarget::~QContactActionTarget()
{
}

QContact QContactActionTarget::contact() const
{
    return d->m_contact;
}

QList<QContactDetail> QContactActionTarget::details() const
{
    return d->m_details;
}

void QContactActionTarget::setContact(const QContact& contact)
{
    d->m_contact = contact;
}

void QContactActionTarget::setDetails(const QList<QContactDetail>& details)
{
    d->m_details = details;
}

/*!
 * Returns true if the target contact is not the default constructed contact.
 * The validity of any specified details is not checked by this function.
 */
bool QContactActionTarget::isValid() const
{
    return (d->m_contact != QContact());
}

/*!
 * Returns true if the action name, service name and service-specified implementation version
 * specified by this action target are equal to those specified by \a other
 */
bool QContactActionTarget::operator==(const QContactActionTarget& other) const
{
    return d->m_contact == other.d->m_contact
            && d->m_details == other.d->m_details;
}

/*!
 * Returns true if the action name, service name or service-specified implementation version
 * specified by this action target are different to that specified by \a other
 */
bool QContactActionTarget::operator!=(const QContactActionTarget& other) const
{
    return !(*this == other);
}

/*!
 * Returns true if the action target is less than the \a other action target.  The
 * comparison is performed first on the local id of the contact, then on the size of the list of details
 * specified as part of the target.
 */
bool QContactActionTarget::operator<(const QContactActionTarget& other) const
{
    if (d->m_contact.localId() < other.d->m_contact.localId())
        return true;
    if (d->m_contact.localId() == other.d->m_contact.localId())
        return d->m_details.size() < other.d->m_details.size();
    return false;
}

/*! Returns the hash value for \a key. */
uint qHash(const QContactActionTarget& key)
{
    uint ret = qHash(key.contact());
    foreach (const QContactDetail& det, key.details()) {
        ret += qHash(det);
    }
    return ret;
}

QTM_END_NAMESPACE
