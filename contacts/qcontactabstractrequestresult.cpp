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

#include "qcontactabstractrequestresult.h"
#include "qcontactabstractrequestresult_p.h"

/*!
 * \class QContactAbstractRequestResult
 * \brief The result of an asynchronous request
 * This class should be extended for each particular request type implemented.
 */

/*! Constructs a new request result */
QContactAbstractRequestResult::QContactAbstractRequestResult()
    : d(new QContactAbstractRequestResultData)
{
}

/*! Constructs a copy of the \a other request result */
QContactAbstractRequestResult::QContactAbstractRequestResult(const QContactAbstractRequestResult& other)
    : d(other.d)
{
}

/*! Clears the memory used by this result */
QContactAbstractRequestResult::~QContactAbstractRequestResult()
{
}

/*! Sets the list of errors associated with the result of the request to \a errors */
void QContactAbstractRequestResult::setErrors(const QList<QContactManager::Error>& errors)
{
    d->m_errors = errors;
}

/*! Sets the error associated with the operation of the request itself to \a error */
void QContactAbstractRequestResult::setError(const QContactManager::Error& error)
{
    d->m_error = error;
}

/*! Returns the list of errors associated with the result of the request */
QList<QContactManager::Error> QContactAbstractRequestResult::errors() const
{
    return d->m_errors;
}

/*! Returns the error associated with the operation of the request */
QContactManager::Error QContactAbstractRequestResult::error() const
{
    return d->m_error;
}

/*! Sets the status of the order of the results to \a appendOnly.  If \a appendOnly is true, the order of the results
 *  since the last update remains unchanged (i.e., new results have merely been appended). */
void QContactAbstractRequestResult::setAppendOnly(bool appendOnly)
{
    d->m_appendOnly = appendOnly;
}

/*! Retrieve the status of the order of the results.  Returns true if the order of the results has remained unchanged
 *  since the last udpate (i.e., new results have merely been appended to the old set). */
bool QContactAbstractRequestResult::appendOnly() const
{
    return d->m_appendOnly;
}

