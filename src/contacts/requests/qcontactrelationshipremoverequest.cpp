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

#include "qcontactrelationshipremoverequest.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactRelationshipRemoveRequest

  \brief The QContactRelationshipRemoveRequest class allows a client
  to asynchronously request that certain relationships be removed from
  a contacts store.

  \ingroup contacts-requests
 */

/*!
 * \fn QContactRelationshipRemoveRequest::progress(QContactRelationshipRemoveRequest* self)
 * This signal is emitted when some progress has been made on the request, causing either a change of
 * status or an update of results, or both.  It identifies which request the signal originated from
 * by including a pointer to \a self.
 */

/*! Constructs a new relationship remove request */
QContactRelationshipRemoveRequest::QContactRelationshipRemoveRequest()
    : QContactAbstractRequest(new QContactRelationshipRemoveRequestPrivate)
{
}

/*! Cleans up the memory in use by this relationship remove request */
QContactRelationshipRemoveRequest::~QContactRelationshipRemoveRequest()
{
}

/*! Sets the first contact criterion of the remove request to \a firstId.
 *  If \a firstId is the default-constructed id, or the first contact is not set,
 *  the request will remove relationships involving any first contact. */
void QContactRelationshipRemoveRequest::setFirst(const QContactId& firstId)
{
    Q_D(QContactRelationshipRemoveRequest);
    d->m_first = firstId;
}

/*! Returns the first contact criterion of the remove request */
QContactId QContactRelationshipRemoveRequest::first() const
{
    Q_D(const QContactRelationshipRemoveRequest);
    return d->m_first;
}

/*! Sets the relationship type criterion of the remove request to \a relationshipType.
 *  If \a relationshipType is empty, or the relationship type is not set,
 *  the request will remove relationships of any type. */
void QContactRelationshipRemoveRequest::setRelationshipType(const QString& relationshipType)
{
    Q_D(QContactRelationshipRemoveRequest);
    d->m_relationshipType = relationshipType;
}

/*! Returns the relationship type criterion of the fetch request */
QString QContactRelationshipRemoveRequest::relationshipType() const
{
    Q_D(const QContactRelationshipRemoveRequest);
    return d->m_relationshipType;
}

/*! Sets the second contact criterion of the remove request to \a secondId.
 *  If \a secondId is the default-constructed id, or the second contact is not set,
 *  the request will remove relationships involving any second contact. */
void QContactRelationshipRemoveRequest::setSecond(const QContactId& secondId)
{
    Q_D(QContactRelationshipRemoveRequest);
    d->m_second = secondId;
}

/*! Returns the second contact criterion of the remove request */
QContactId QContactRelationshipRemoveRequest::second() const
{
    Q_D(const QContactRelationshipRemoveRequest);
    return d->m_second;
}

#include "moc_qcontactrelationshipremoverequest.cpp"

QTM_END_NAMESPACE
