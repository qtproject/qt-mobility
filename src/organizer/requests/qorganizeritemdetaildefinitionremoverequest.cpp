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

#include "qorganizeritemdetaildefinitionremoverequest.h"
#include "qorganizeritemrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemDetailDefinitionRemoveRequest
  \brief The QOrganizerItemDetailDefinitionRemoveRequest class allows a client to
  asynchronously request that certain detail definitions be removed from
  a organizeritems store.
  \inmodule QtOrganizer

  For a QOrganizerItemDetailDefinitionRemoveRequest, the resultsUpdated() signal will be emitted when
  the individual item errors (which may be retrieved by calling errorMap()) are updated, or if the overall
  operation error (which may be retrieved by calling error()) is updated.
  
  \ingroup organizer-requests
 */

/*! Constructs a new detail definition remove request whose parent is the specified \a parent */
QOrganizerItemDetailDefinitionRemoveRequest::QOrganizerItemDetailDefinitionRemoveRequest(QObject* parent)
    : QOrganizerItemAbstractRequest(new QOrganizerItemDetailDefinitionRemoveRequestPrivate, parent)
{
}

/*!
  Sets the name of the detail definition to remove from the manager to \a definitionName.
  Managers may store different definitions which are valid for different organizeritem types, and so attempting to remove definitions with certain names may fail if no such
  definitions exist for organizeritems of the given organizeritem type, specified via setItemType().
  Equivalent to calling:
  \code
      setDefinitionNames(QStringList(definitionName));
  \endcode

  \sa setItemType()
 */
void QOrganizerItemDetailDefinitionRemoveRequest::setDefinitionName(const QString& definitionName)
{
    Q_D(QOrganizerItemDetailDefinitionRemoveRequest);
    d->m_names.clear();
    d->m_names.append(definitionName);
}

/*!
  Sets the names of the detail definitions to remove from the manager to \a names.
  Managers may store different definitions which are valid for different organizeritem types, and so attempting to remove definitions with certain names may fail if no such
  definitions exist for organizeritems of the given organizeritem type, specified via setItemType().
  \sa setItemType()
 */
void QOrganizerItemDetailDefinitionRemoveRequest::setDefinitionNames(const QStringList& names)
{
    Q_D(QOrganizerItemDetailDefinitionRemoveRequest);
    d->m_names = names;
}

/*! Returns the list of names of the detail definitions that will be removed from the manager */
QStringList QOrganizerItemDetailDefinitionRemoveRequest::definitionNames() const
{
    Q_D(const QOrganizerItemDetailDefinitionRemoveRequest);
    return d->m_names;
}

/*!
  Sets the type of organizeritem for which detail definitions should be removed to \a organizeritemType
 */
void QOrganizerItemDetailDefinitionRemoveRequest::setItemType(const QString& organizeritemType)
{
    Q_D(QOrganizerItemDetailDefinitionRemoveRequest);
    d->m_organizeritemType = organizeritemType;
}

/*! Returns the type of organizeritem for which detail definitions will be removed */
QString QOrganizerItemDetailDefinitionRemoveRequest::itemType() const
{
    Q_D(const QOrganizerItemDetailDefinitionRemoveRequest);
    return d->m_organizeritemType;
}

/*! Returns the map of input name list indices to errors which occurred */
QMap<int, QOrganizerItemManager::Error> QOrganizerItemDetailDefinitionRemoveRequest::errorMap() const
{
    Q_D(const QOrganizerItemDetailDefinitionRemoveRequest);
    return d->m_errors;
}

#include "moc_qorganizeritemdetaildefinitionremoverequest.cpp"

QTM_END_NAMESPACE
