/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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

  Please see the class documentation of QContactAbstractRequest for more information about
  the usage of request classes and ownership semantics.


  \inmodule QtContacts
  \since 1.0

  \ingroup contacts-requests
 */

/*! Constructs a new detail definition save request whose parent is the specified \a parent */
QContactDetailDefinitionSaveRequest::QContactDetailDefinitionSaveRequest(QObject* parent)
    : QContactAbstractRequest(new QContactDetailDefinitionSaveRequestPrivate, parent)
{
}

/*! Frees any memory used by this request */
QContactDetailDefinitionSaveRequest::~QContactDetailDefinitionSaveRequest()
{
    QContactAbstractRequestPrivate::notifyEngine(this);
}

/*!
  Sets the definition to save to be the given \a definition.
  Equivalent to calling:
  \code
      setDefinitions(QList<QContactDetailDefinition>() << definition);
  \endcode
  \since 1.0
 */
void QContactDetailDefinitionSaveRequest::setDefinition(const QContactDetailDefinition& definition)
{
    Q_D(QContactDetailDefinitionSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_definitions.clear();
    d->m_definitions.append(definition);
}

/*! Sets the definitions to save to be \a definitions
    \since 1.0
*/
void QContactDetailDefinitionSaveRequest::setDefinitions(const QList<QContactDetailDefinition>& definitions)
{
    Q_D(QContactDetailDefinitionSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_definitions = definitions;
}

/*! Returns the list of definitions that will be saved if called prior to calling \c start(),
    otherwise returns the list of detail definitions as they were saved in the contacts store
    \since 1.0
*/
QList<QContactDetailDefinition> QContactDetailDefinitionSaveRequest::definitions() const
{
    Q_D(const QContactDetailDefinitionSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_definitions;
}

/*! Sets the type of contact for which detail definitions should be saved to \a contactType
    \since 1.0
*/
void QContactDetailDefinitionSaveRequest::setContactType(const QString& contactType)
{
    Q_D(QContactDetailDefinitionSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_contactType = contactType;
}

/*! Returns the type of contact for which detail definitions will be saved
    \since 1.0
*/
QString QContactDetailDefinitionSaveRequest::contactType() const
{
    Q_D(const QContactDetailDefinitionSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_contactType;
}

/*! Returns the map of input definition list indices to errors which occurred
    \since 1.0
*/
QMap<int, QContactManager::Error> QContactDetailDefinitionSaveRequest::errorMap() const
{
    Q_D(const QContactDetailDefinitionSaveRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_errors;
}

#include "moc_qcontactdetaildefinitionsaverequest.cpp"

QTM_END_NAMESPACE
