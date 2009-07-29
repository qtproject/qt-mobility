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


#include "qcontactdetaildefinitionrequestresult.h"
#include "qcontactdetaildefinitionrequestresult_p.h"
#include "qcontactdetaildefinitionrequest.h"
#include "qcontactdetaildefinitionrequest_p.h"

/*!
 * \class QContactDetailDefinitionRequestResult
 * \brief The current result of an asynchronous request for contact detail definitions
 * This class allows the result of an asynchronous request for definitions to be returned
 * to the request.  It provides an interface to manager engines which allows them
 * to set the result and status of a particular request.
 */

/*! Cosntructs a new contact request result */
QContactDetailDefinitionRequestResult::QContactDetailDefinitionRequestResult()
    : d(new QContactDetailDefinitionRequestResultData)
{
}

/*! Clears any memory in use by this contact request result */
QContactDetailDefinitionRequestResult::~QContactDetailDefinitionRequestResult()
{
}

/*! Sets the result definition names to \a names */
void QContactDetailDefinitionRequestResult::setDefinitionNames(const QStringList& names)
{
    d->m_definitionNames = names;
}

/*! Sets the result definitions to \a definitions */
void QContactDetailDefinitionRequestResult::setDefinitions(const QList<QContactDetailDefinition>& definitions)
{
    d->m_definitions = definitions;
}

/*! Returns the result list of definition names */
QStringList QContactDetailDefinitionRequestResult::definitionNames() const
{
    return d->m_definitionNames;
}

/*! Returns the result list of definitions */
QList<QContactDetailDefinition> QContactDetailDefinitionRequestResult::definitions() const
{
    return d->m_definitions;
}
