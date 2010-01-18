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

#include "qcontactdetaildefinitionremoverequest.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactDetailDefinitionRemoveRequest
  \brief The QContactDetailDefinitionRemoveRequest class allows a client to
  asynchronously request that certain detail definitions be removed from
  a contacts store.
  
  \ingroup contacts-requests
 */

/*!
 * \fn QContactDetailDefinitionRemoveRequest::progress(QContactDetailDefinitionRemoveRequest* self)
 * This signal is emitted when some progress has been made on the request, causing either a change of
 * status or an update of results, or both.  It identifies which request the signal originated from
 * by including a pointer to \a self.
 */

/*! Constructs a new detail definition remove request */
QContactDetailDefinitionRemoveRequest::QContactDetailDefinitionRemoveRequest()
    : QContactAbstractRequest(new QContactDetailDefinitionRemoveRequestPrivate)
{
}

/*! Cleans up the memory in use by this detail definition remove request */
QContactDetailDefinitionRemoveRequest::~QContactDetailDefinitionRemoveRequest()
{
}

/*!
 * \deprecated
 * Sets the names of the detail definitions to remove from the manager to be \a names
 */
void Q_DECL_DEPRECATED QContactDetailDefinitionRemoveRequest::setNames(const QStringList& names)
{
    Q_D(QContactDetailDefinitionRemoveRequest);
    qWarning("QContactDetailDefinitionRemoveRequest::setNames() This function was deprecated in week 1 and will be removed after the transition period has elapsed!  Use setDefinitionNames() instead!");
    d->m_names = names;
}

/*!
 * \deprecated
 * Returns the list of names of the detail definitions that will be removed from the manager
 */
QStringList Q_DECL_DEPRECATED QContactDetailDefinitionRemoveRequest::names() const
{
    Q_D(const QContactDetailDefinitionRemoveRequest);
    qWarning("QContactDetailDefinitionRemoveRequest::names() This function was deprecated in week 1 and will be removed after the transition period has elapsed!  Use definitionNames() instead!");
    return d->m_names;
}


/*!
 * Sets the type of contact for which detail definitions should be removed to \a contactType, and the names of the detail definitions to remove from the manager to \a names.
 * Managers may store different definitions which are valid for different contact types, and so attempting to remove definitions with certain names may fail if no such
 * definitions exist for contacts of the given contact type, or if \a contactType is empty.
 */
void QContactDetailDefinitionRemoveRequest::setDefinitionNames(const QString& contactType, const QStringList& names)
{
    Q_D(QContactDetailDefinitionRemoveRequest);
    d->m_contactType = contactType;
    d->m_names = names;
}

/*! Returns the list of names of the detail definitions that will be removed from the manager */
QStringList QContactDetailDefinitionRemoveRequest::definitionNames() const
{
    Q_D(const QContactDetailDefinitionRemoveRequest);
    return d->m_names;
}

/*! Returns the type of contact for which detail definitions will be removed */
QString QContactDetailDefinitionRemoveRequest::contactType() const
{
    Q_D(const QContactDetailDefinitionRemoveRequest);
    return d->m_contactType;
}

/*! Returns the map of input name list indices to errors which occurred */
QMap<int, QContactManager::Error> QContactDetailDefinitionRemoveRequest::errorMap() const
{
    Q_D(const QContactDetailDefinitionRemoveRequest);
    return d->m_errors;
}

#include "moc_qcontactdetaildefinitionremoverequest.cpp"

QTM_END_NAMESPACE
