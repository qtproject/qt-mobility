/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qcontactsaverequest.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactSaveRequest
  \brief The QContactSaveRequest class allows a client to asynchronously
    request that certain contacts be saved to a contacts store.

  For a QContactSaveRequest, the resultsAvailable() signal will be emitted when
  either the individual item errors (which may be retrieved by calling errorMap()), or the resultant
  contacts (which may be retrieved by calling contacts()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  Please see the class documentation of QContactAbstractRequest for more information about
  the usage of request classes and ownership semantics.

  
  \inmodule QtContacts
  
  \ingroup contacts-requests
 */

/*! Constructs a new contact save request whose parent is the specified \a parent */
QContactSaveRequest::QContactSaveRequest(QObject* parent)
    : QContactAbstractRequest(new QContactSaveRequestPrivate, parent)
{
}

/*! Frees any memory used by this request */
QContactSaveRequest::~QContactSaveRequest()
{
    QContactAbstractRequestPrivate::notifyEngine(this);
}

/*!
  Sets the contact to be saved to \a contact.
  Equivalent to calling:
  \code
      setContacts(QList<QContact>() << contact);
  \endcode
 */
void QContactSaveRequest::setContact(const QContact& contact)
{
    Q_D(QContactSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_contacts.clear();
    d->m_contacts.append(contact);
}

/*! Sets the list of contacts to be saved to \a contacts */
void QContactSaveRequest::setContacts(const QList<QContact>& contacts)
{
    Q_D(QContactSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_contacts = contacts;
}

/*! Returns the list of contacts which will be saved if called prior to calling \c start(),
    otherwise returns the list of contacts with their ids set appropriately (successfully
    saved new contacts will have an id assigned).
*/
QList<QContact> QContactSaveRequest::contacts() const
{
    Q_D(const QContactSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_contacts;
}

/*! Returns the map of input contact list indices to errors which occurred */
QMap<int, QContactManager::Error> QContactSaveRequest::errorMap() const
{
    Q_D(const QContactSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_errors;
}

/*!
    Set the list of definitions to restrict saving to \a definitionMask.  This allows you to perform
    partial save (and remove) operations on existing contacts.

    If \a definitionMask is empty (the default), no restrictions will apply, and the passed
    in contacts will be saved as is.  Otherwise, only details whose definitions are in
    the list will be saved.  If a definition name is present in the list, but there are no
    corresponding details in the contact passed into this request, any existing details in
    the manager for that contact will be removed.

    This is useful if you've used a fetch hint to fetch a partial contact from a manager
    so that you can save changes to the details you actually fetched without removing
    the details you didn't.

    Additionally, when performing synchronization operations with other managers that don't
    support the full range of details, you can restrict the update operation to only those
    details so that you don't lose the extra details that are supported in this manager.

    \note Some managers do not support partial updates natively, in which case the QtContacts
    framework will emulate the functionality (fetching the whole contact, applying the
    new restricted details, and saving the contact back).
*/
void QContactSaveRequest::setDefinitionMask(const QStringList &definitionMask)
{
    Q_D(QContactSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_definitionMask = definitionMask;
}

/*!
    Returns the list of definitions that this request will operate on.

    If the list is empty, the request will operate on all details.
 */
QStringList QContactSaveRequest::definitionMask() const
{
    Q_D(const QContactSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_definitionMask;
}

#include "moc_qcontactsaverequest.cpp"

QTM_END_NAMESPACE
