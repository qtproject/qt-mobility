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

#include "qorganizeritemsaverequest.h"
#include "qorganizeritemrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemSaveRequest
  \brief The QOrganizerItemSaveRequest class allows a client to asynchronously
    request that certain organizer items be saved to an organizer item store.
  \inmodule QtOrganizer

  For a QOrganizerItemSaveRequest, the resultsAvailable() signal will be emitted when
  either the individual item errors (which may be retrieved by calling errorMap()), or the resultant
  organizer items (which may be retrieved by calling items()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup organizer-requests
 */

/*! Constructs a new organizer item save request whose parent is the specified \a parent */
QOrganizerItemSaveRequest::QOrganizerItemSaveRequest(QObject* parent)
    : QOrganizerAbstractRequest(new QOrganizerItemSaveRequestPrivate, parent)
{
}

/*! Frees memory in use by this request */
QOrganizerItemSaveRequest::~QOrganizerItemSaveRequest()
{
    QOrganizerAbstractRequestPrivate::notifyEngine(this);
}

/*!
  Sets the organizer item to be saved to \a organizeritem.
  Equivalent to calling:
  \code
      setOrganizerItems(QList<QOrganizerItem>() << organizeritem);
  \endcode
 */
void QOrganizerItemSaveRequest::setItem(const QOrganizerItem& organizeritem)
{
    Q_D(QOrganizerItemSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_organizeritems.clear();
    d->m_organizeritems.append(organizeritem);
}

/*! Sets the list of organizer items to be saved to \a organizeritems */
void QOrganizerItemSaveRequest::setItems(const QList<QOrganizerItem>& organizeritems)
{
    Q_D(QOrganizerItemSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_organizeritems = organizeritems;
}

/*! Returns the list of organizer items which will be saved if called prior to calling \c start(),
    otherwise returns the list of organizer items as they were saved in the organizer item store */
QList<QOrganizerItem> QOrganizerItemSaveRequest::items() const
{
    Q_D(const QOrganizerItemSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_organizeritems;
}

/*! Returns the map of input definition list indices to errors which occurred */
QMap<int, QOrganizerManager::Error> QOrganizerItemSaveRequest::errorMap() const
{
    Q_D(const QOrganizerItemSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_errors;
}

/*!
    Set the list of definitions to restrict saving to \a definitionMask.  This allows you to perform
    partial save (and remove) operations on existing items.

    If \a definitionMask is empty (the default), no restrictions will apply, and the passed
    in items will be saved as is.  Otherwise, only details whose definitions are in
    the list will be saved.  If a definition name is present in the list, but there are no
    corresponding details in the item passed into this request, any existing details in
    the manager for that item will be removed.

    This is useful if you've used a fetch hint to fetch a partial item from a manager
    so that you can save changes to the details you actually fetched without removing
    the details you didn't.

    Additionally, when performing synchronization operations with other managers that don't
    support the full range of details, you can restrict the update operation to only those
    details so that you don't lose the extra details that are supported in this manager.

    \note Some managers do not support partial updates natively, in which case the QtOrganizer
    framework will emulate the functionality (fetching the whole item, applying the
    new restricted details, and saving the item back).
*/
void QOrganizerItemSaveRequest::setDefinitionMask(const QStringList &definitionMask)
{
    Q_D(QOrganizerItemSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_definitionMask = definitionMask;
}

/*!
    Returns the list of definitions that this request will operate on.

    If the list is empty, the request will operate on all details.
 */
QStringList QOrganizerItemSaveRequest::definitionMask() const
{
    Q_D(const QOrganizerItemSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_definitionMask;
}

#include "moc_qorganizeritemsaverequest.cpp"

QTM_END_NAMESPACE
