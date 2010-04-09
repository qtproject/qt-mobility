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

#include "qcontactdetaildefinitionsaverequest.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactDetailDefinitionSaveRequest
  \brief The QContactDetailDefinitionSaveRequest class allows a client to
  asynchronously request that certain detail definitions be saved in a
  contacts store.

  For a QContactDetailDefinitionSaveRequest, the resultsAvailable() signal will be emitted when
  either the individual item errors (which may be retrieved by calling errorMap()), or the resultant
  detail definitions (which may be retrieved by calling definitions()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.
  
  \ingroup contacts-requests
 */

/*! Constructs a new detail definition save request */
QContactDetailDefinitionSaveRequest::QContactDetailDefinitionSaveRequest()
    : QContactAbstractRequest(new QContactDetailDefinitionSaveRequestPrivate)
{
}

/*! Sets the definitions to save to be \a definitions */
void QContactDetailDefinitionSaveRequest::setDefinitions(const QList<QContactDetailDefinition>& definitions)
{
    Q_D(QContactDetailDefinitionSaveRequest);
    d->m_definitions = definitions;
}

/*! Returns the list of definitions that will be saved if called prior to calling \c start(),
    otherwise returns the list of detail definitions as they were saved in the contacts store */
QList<QContactDetailDefinition> QContactDetailDefinitionSaveRequest::definitions() const
{
    Q_D(const QContactDetailDefinitionSaveRequest);
    return d->m_definitions;
}

/*! Sets the type of contact for which detail definitions should be saved to \a contactType */
void QContactDetailDefinitionSaveRequest::setContactType(const QString& contactType)
{
    Q_D(QContactDetailDefinitionSaveRequest);
    d->m_contactType = contactType;
}

/*! Returns the type of contact for which detail definitions will be saved */
QString QContactDetailDefinitionSaveRequest::contactType() const
{
    Q_D(const QContactDetailDefinitionSaveRequest);
    return d->m_contactType;
}

/*! Returns the map of input definition list indices to errors which occurred */
QMap<int, QContactManager::Error> QContactDetailDefinitionSaveRequest::errorMap() const
{
    Q_D(const QContactDetailDefinitionSaveRequest);
    return d->m_errors;
}

#include "moc_qcontactdetaildefinitionsaverequest.cpp"

QTM_END_NAMESPACE
