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

#include "qcontactdetaildefinitionfetchrequest.h"
#include "qcontactrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactDetailDefinitionFetchRequest
  \brief The QContactDetailDefinitionFetchRequest class allows a client to
    asynchronously request detail definitions from a contacts store manager.

  For a QContactDetailDefinitionFetchRequest, the resultsAvailable() signal will be emitted when
  either the individual item errors (which may be retrieved by calling errorMap()), or the resultant
  detail definitions (which may be retrieved by calling definitions()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.

  \ingroup contacts-requests
 */

/*! Constructs a new detail definition fetch request whose parent is the specified \a parent */
QContactDetailDefinitionFetchRequest::QContactDetailDefinitionFetchRequest(QObject* parent)
    : QContactAbstractRequest(new QContactDetailDefinitionFetchRequestPrivate, parent)
{
}

/*! Sets the name of the detail definition to retrieve to \a definitionName.
    Equivalent to calling
    \code
        setDefinitionNames(QList<QContactDetailDefinition>() << definitionName);
    \endcode
 */
void QContactDetailDefinitionFetchRequest::setDefinitionName(const QString& definitionName)
{
    Q_D(QContactDetailDefinitionFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_names.clear();
    d->m_names.append(definitionName);
}

/*! Sets the names of the detail definitions to retrieve to \a names */
void QContactDetailDefinitionFetchRequest::setDefinitionNames(const QStringList& names)
{
    Q_D(QContactDetailDefinitionFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_names = names;
}

/*! Returns the list of names of the detail definitions that will be retrieved */
QStringList QContactDetailDefinitionFetchRequest::definitionNames() const
{
    Q_D(const QContactDetailDefinitionFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_names;
}

/*! Sets the type of contact for which detail definitions should be retrieved to \a contactType */
void QContactDetailDefinitionFetchRequest::setContactType(const QString& contactType)
{
    Q_D(QContactDetailDefinitionFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    d->m_contactType = contactType;
}

/*! Returns the type of contact for which detail definitions will be retrieved */
QString QContactDetailDefinitionFetchRequest::contactType() const
{
    Q_D(const QContactDetailDefinitionFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_contactType;
}

/*!
  Returns the map of detail definition names to detail definitions that was the result of the request
 */
QMap<QString, QContactDetailDefinition> QContactDetailDefinitionFetchRequest::definitions() const
{
    Q_D(const QContactDetailDefinitionFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_definitions;
}

/*! Returns the map of input name list indices to errors which occurred */
QMap<int, QContactManager::Error> QContactDetailDefinitionFetchRequest::errorMap() const
{
    Q_D(const QContactDetailDefinitionFetchRequest);
    QMutexLocker ml(&d->m_mutex);
    return d->m_errors;
}

#include "moc_qcontactdetaildefinitionfetchrequest.cpp"

QTM_END_NAMESPACE
