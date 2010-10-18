/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
  \enum QContactActionTarget::Type
  This enumerator defines the type of a QContactActionTarget.

  \value Invalid The type is invalid.
  \value WholeContact The type is a complete contact.
  \value SingleDetail The type is only a single detail.
  \value MultipleDetails The type contains multiple details.
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

/*!
 * Returns the contact that this action target will operate on.
 * \sa details()
 */
QContact QContactActionTarget::contact() const
{
    return d->m_contact;
}

/*!
 * Returns the details that this action target will operate on.
 * \sa contact()
 */
QList<QContactDetail> QContactActionTarget::details() const
{
    return d->m_details;
}

/*!
 * Sets the contact that this action target will operate on to \a contact.
 * \sa setDetails()
 */
void QContactActionTarget::setContact(const QContact& contact)
{
    d->m_contact = contact;
}

/*!
 * Sets the details that this action target will operate on to \a details.
 * \sa setContact()
 */
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
 * Returns true if the contacts and details specified by this action target are equal to those specified by \a other
 */
bool QContactActionTarget::operator==(const QContactActionTarget& other) const
{
    return d->m_contact == other.d->m_contact
            && d->m_details == other.d->m_details;
}

/*!
 * Returns true if the contacts or details specified by this action target are different to that specified by \a other
 */
bool QContactActionTarget::operator!=(const QContactActionTarget& other) const
{
    return !(*this == other);
}

/*!
  Returns the type of this action target.

  The type is determined by the properties that have been set on this target.
 */
QContactActionTarget::Type QContactActionTarget::type() const
{
    if (d->m_contact.isEmpty())
        return QContactActionTarget::Invalid;
    switch (d->m_details.count()) {
        case 0:
            return QContactActionTarget::WholeContact;
        case 1:
            return QContactActionTarget::SingleDetail;
        default:
            return QContactActionTarget::MultipleDetails;
    }
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

#ifndef QT_NO_DATASTREAM
/*! Streams the given \a target to the datastream \a out */
QDataStream& operator<<(QDataStream& out, const QContactActionTarget& target)
{
    quint8 formatVersion = 1; // Version of QDataStream format for QContactActionTarget
    out << formatVersion;
    out << target.d->m_contact;
    out << target.d->m_details;
    return out;
}

/*! Streams \a target in from the datastream \a in */
QDataStream& operator>>(QDataStream& in, QContactActionTarget& target)
{
    QContactActionTarget retn;
    quint8 formatVersion;
    in >> formatVersion;
    if (formatVersion == 1) {
        in >> retn.d->m_contact;
        in >> retn.d->m_details;
    } else {
        in.setStatus(QDataStream::ReadCorruptData);
    }
    target = retn;
    return in;
}
#endif

#ifndef QT_NO_DEBUG_STREAM
QDebug& operator<<(QDebug dbg, const QContactActionTarget& target)
{
    dbg.nospace() << "QContactActionTarget(" << target.contact() << target.details() << ')';
    return dbg.maybeSpace();
}
#endif

QTM_END_NAMESPACE
