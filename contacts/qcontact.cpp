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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include "qcontact.h"
#include "qcontact_p.h"
#include "qcontactdetail_p.h"
#include "qcontactname.h"

/*!
 * \class QContact
 *
 * \brief The QContact class provides an addressbook contact.
 *
 * A QContact consists of zero or more details.
 *
 * An instance of the QContact class represents an in-memory contact,
 * and may not reflect the state of that contact found in persistent
 * storage until the appropriate synchronisation method is called
 * on the QContactManager (i.e., saveContact, removeContact).
 *
 * \sa QContactManager, QContactDetail
 */

/*!
 * \fn QList<T> QContact::details() const
 * Returns a list of details of the template type
 */

/*!
 * \fn T QContact::detail() const
 * Returns the first detail of the template type
 */

/*!
 * \fn QContact::operator!=(const QContact &other) const
 * Returns true if this contacts Id or details are different to those of the \a other contact
 */

/*! Construct an empty contact. */
QContact::QContact()
    : d(new QContactData)
{
    // insert the contact's name field.
    QContactName contactName;
    contactName.d->m_id = 1;
    d->m_details.insert(0, QContactName());
}

/*! Initializes this QContact from \a other */
QContact::QContact(const QContact& other)
    : d(other.d)
{
}

/*! Replace the contents of this QContact with \a other */
QContact& QContact::operator=(const QContact& other)
{
    d = other.d;
    return *this;
}

/*! Frees the memory used by this QContact */
QContact::~QContact()
{
}

/*!
 * \enum QContact::Error
 *
 * This enum specifies an error that occurred during the most recent operation:
 *
 * \value NoError The most recent operation was successful
 * \value OutOfMemoryError The most recent operation failed due to running out of memory
 * \value DetailDoesNotExistError The most recent operation failed because the requested detail does not exist
 * \value DetailAlreadyExistsError The most recent operation failed because the specified detail already exists
 * \value DetailLimitReachedError The most recent operation failed because no more details may be stored in this contact
 * \value DetailOfThisTypeLimitReachedError The most recent operation failed because no more details of this definition type may be stored in this contact
 * \value PermissionsError The most recent operation failed because the caller does not have permission to perform the operation
 * \value UnspecifiedError The most recent operation failed for an undocumented reason
 */

/*! Returns the QUniqueId that identifies this contact */
QUniqueId QContact::id() const
{
    QContactData::setError(d, QContact::NoError);
    return d->m_id;
}

/*!
 * Sets the id of this contact to \a id.  Note that this only affects
 * this structure, not any corresponding structures stored
 * by a QContactManager.
 *
 * If you change the id of a contact and save the contact
 * in a manager, the previously existing contact will still
 * exist.  You can do this to create copies (possibly modified)
 * of an existing contact.
 *
 * Returns true if the \a id was set successfully, otherwise
 * returns false.
 */
bool QContact::setId(const QUniqueId& id)
{
    d->m_id = id;
    d->m_error = QContact::NoError;
    return true;
}

/*! Returns the name of the contact */
QContactName& QContact::name()
{
    QContactData::setError(d, QContact::NoError);
    return (QContactName&)d->m_details[0];
}

/*! Returns the name of the contact */
const QContactName& QContact::name() const
{
    QContactData::setError(d, QContact::NoError);
    return (QContactName&)d->m_details[0];
}

/*! Returns the first detail stored in the contact which is of the given \a definitionId */
QContactDetail QContact::detail(const QString& definitionId) const
{
    // build the sub-list of matching details.
    for (int i = 0; i < d->m_details.size(); i++) {
        const QContactDetail& existing = d->m_details.at(i);
        if (definitionId.isEmpty() || definitionId == existing.definitionId()) {
            QContactData::setError(d, QContact::NoError);
            return existing;
        }
    }

    QContactData::setError(d, QContact::DetailDoesNotExistError);
    return QContactDetail();
}

/*! Returns a list of details of the given \a definitionId */
QList<QContactDetail> QContact::details(const QString& definitionId) const
{
    // build the sub-list of matching details.
    QList<QContactDetail> sublist;

    // special case
    if (definitionId.isEmpty()) {
        sublist = d->m_details;
    } else {
        for (int i = 0; i < d->m_details.size(); i++) {
            const QContactDetail& existing = d->m_details.at(i);
            if (definitionId == existing.definitionId()) {
                sublist.append(existing);
                QContactData::setError(d, QContact::NoError);
            }
        }
    }

    if(sublist.count() == 0)
        QContactData::setError(d, QContact::DetailDoesNotExistError);
    return sublist;
}

/*! Returns the code of the error of the most recent operation */
QContact::Error QContact::error() const
{
    return d->m_error;
}

/*! Saves the given \a detail in the list of stored details, and sets its Id.  If another detail of the same type and Id has been previously saved in this contact, that detail is overwritten.  Otherwise, a new Id is generated and set in the detail, and the detail is added to the list.  Returns true if the detail was saved successfully, otherwise returns false */
bool QContact::saveDetail(QContactDetail* detail)
{
    // unique details are a special case; there is only and always one unique field.
    // ### name is always unique... but we don't know what store we're in for other checking XXX
    if (detail->definitionId() == "Name") {
        QContactData::setError(d, QContact::NoError);
        d->m_details.removeAt(0);
        QContactName name = *detail;
        name.d->m_id = 1; // the name field's id
        d->m_details.insert(0, name); // replace the name.
        return true;
    }

    // now we try to find the "old version" of this field
    // ie, the one with the same type and id, but different value or attributes.
    for (int i = 0; i < d->m_details.size(); i++) {
        QContactDetail curr = d->m_details.at(i);
        if (detail->d->m_definitionId == curr.d->m_definitionId && detail->d->m_id == curr.d->m_id) {
            // Found the old version.  Replace it with this one.
            d->m_details[i] = *detail;
            QContactData::setError(d, QContact::NoError);
            return true;
        }
    }

    // this is a new detail!  add it to the contact.
    QContactData::setError(d, QContact::NoError);
    detail->d->m_id = ++d->m_nextDetailId;
    d->m_details.append(*detail);
    return true;
}

/*! Removes the \a detail from the contact.  Any preference for the given field is removed, also.  The Id of the \a detail is removed, to signify that it is no longer part of the contact.  Returns true if the detail was removed successfully, false if an error occurred */
bool QContact::removeDetail(QContactDetail* detail)
{
    // name details are a special case; there is only and always one name field.
    if (detail->definitionId() == "Name") {
        QContactData::setError(d, QContact::PermissionsError);
        return false;
    }

    if (!d->m_details.contains(*detail)) {
        QContactData::setError(d, QContact::DetailDoesNotExistError);
        return false;
    }

    // TODO: remove preference if exists - relies on QSFW code.

    d->m_details.removeOne(*detail);
    detail->d->m_id = 0;
    QContactData::setError(d, QContact::NoError);
    return true;
}

/*! Returns true if this contact is equal to the \a other contact, false if either the id or stored details are not the same */
bool QContact::operator==(const QContact& other) const
{
    return other.d->m_id == d->m_id &&
        other.d->m_details == d->m_details;
}


/*! Retrieve the first detail for which the given \a actionId is available */
QContactDetail QContact::detailWithAction(const QString& actionId) const
{
    // dummy implementation - actions aren't implemented (requires QServiceFramework integration)
    Q_UNUSED(actionId);
    QContactData::setError(d, QContact::DetailDoesNotExistError);
    return QContactDetail();
}

/*! Retrieve any details for which the given \a actionId is available */
QList<QContactDetail> QContact::detailsWithAction(const QString& actionId)
{
    // dummy implementation - actions aren't implemented (requires QServiceFramework integration)
    Q_UNUSED(actionId);
    QContactData::setError(d, QContact::DetailDoesNotExistError);
    return QList<QContactDetail>();
}

/*! Return a list of actions available to be performed on this contact */
QStringList QContact::availableActions() const
{
    // dummy implementation - actions aren't implemented (requires QServiceFramework integration)
    QContactData::setError(d, QContact::UnspecifiedError);
    return QStringList();
}

/*! Set a particular detail as the \a preferredDetail for a given \a actionId.  Returns true if the detail was successfully set as the preferred detail for the action identified by \a actionId, otherwise returns false  */
bool QContact::setPreferredDetail(const QString& actionId, const QContactDetail& preferredDetail)
{
    // dummy implementation - actions aren't implemented (requires QServiceFramework integration)
    Q_UNUSED(actionId);
    Q_UNUSED(preferredDetail);
    QContactData::setError(d, QContact::UnspecifiedError);
    return false;
}

/*! Returns true if the given \a detail is a preferred detail for the given \a actionId, or for any action if the \a actionId is empty */
bool QContact::isPreferredDetail(const QString& actionId, const QContactDetail& detail) const
{
    // dummy implementation - actions aren't implemented (requires QServiceFramework integration)
    Q_UNUSED(detail);
    Q_UNUSED(actionId);
    QContactData::setError(d, QContact::UnspecifiedError);
    return false;
}

/*! Returns the preferred detail for a given \a actionId */
QContactDetail QContact::preferredDetail(const QString& actionId) const
{
    // dummy implementation - actions aren't implemented (requires QServiceFramework integration)
    Q_UNUSED(actionId);
    QContactData::setError(d, QContact::UnspecifiedError);
    return QContactDetail();
}
