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

#include "qcontactrelationshipremoverequest.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactRelationshipRemoveRequest

  \brief The QContactRelationshipRemoveRequest class allows a client
  to asynchronously request that certain relationships be removed from
  a contacts store.

  For a QContactRelationshipRemoveRequest, the resultsUpdated() signal will be emitted when
  the individual item errors (which may be retrieved by calling errorMap()) are updated, or if the overall
  operation error (which may be retrieved by calling error()) is updated.

  Please see the class documentation of QContactAbstractRequest for more information about
  the usage of request classes and ownership semantics.

  \ingroup contacts-requests
 */

/*! Constructs a new relationship remove request whose parent is the specified \a parent */
QContactRelationshipRemoveRequest::QContactRelationshipRemoveRequest(QObject* parent)
    : QContactAbstractRequest(new QContactRelationshipRemoveRequestPrivate, parent)
{
}

/*!
  Sets the relationship which will be removed to \a relationship.
  Equivalent to calling:
  \code
      setRelationships(QList<QContactRelationship>() << relationship);
  \endcode
 */
void QContactRelationshipRemoveRequest::setRelationship(const QContactRelationship& relationship)
{
    Q_D(QContactRelationshipRemoveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_relationships.clear();
    d->m_relationships.append(relationship);
}

/*! Sets the list of relationships which will be removed to \a relationships */
void QContactRelationshipRemoveRequest::setRelationships(const QList<QContactRelationship>& relationships)
{
    Q_D(QContactRelationshipRemoveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_relationships = relationships;
}

/*! Returns the list of relationships which will be removed */
QList<QContactRelationship> QContactRelationshipRemoveRequest::relationships() const
{
    Q_D(const QContactRelationshipRemoveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_relationships;
}

/*! Returns the map of input contact list indices to errors which occurred */
QMap<int, QContactManager::Error> QContactRelationshipRemoveRequest::errorMap() const
{
    Q_D(const QContactRelationshipRemoveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_errors;
}

#include "moc_qcontactrelationshipremoverequest.cpp"

QTM_END_NAMESPACE
