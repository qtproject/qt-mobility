/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** OrganizerItem: Nokia Corporation (qt-info@nokia.com)
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

#include "qorganizeritemfetchhint.h"
#include "qorganizeritemfetchhint_p.h"

#include <QStringList>

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemFetchHint
  \brief The QOrganizerItemFetchHint class provides hints to the manager about which organizeritem
  information needs to be retrieved in an asynchronous fetch request or a synchronous
  function call.

  \inmodule QtOrganizer

  All of the hints may be ignored at the discretion of the manager, however if a manager
  is able to optimize retrieval of organizeritems due to hints, it may do so.  If a manager
  ignores a hint, it must retrieve the full set of data that the hint refers to.

  The fetch hint contains:
  \list
   \o a list of detail definition names which the client is interested
  in (empty if interested in all detail definitions)
   \o some optimization flags which allow the client to tell the backend if they are
  not interested in binary blobs (images etc).
  \endlist

  Important note: a client should not make changes to a organizeritem which has been retrieved
  using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the organizeritem back to the manager (as the "new" restricted organizeritem will
  replace the previously saved organizeritem in the backend).
 */

/*!
  \enum QOrganizerItemFetchHint::OptimizationHint

  This enum defines flags which may be set to inform the backend that the client does
  not require certain information.  The backend may safely ignore the hint, but then
  must return the full set of information relating to the optimization hint.

  \value AllRequired Tells the backend that all information is required
  \omitvalue NoActionPreferences Tells the backend that the client does not require retrieved organizeritems to include a cache of action preferences
  \value NoBinaryBlobs Tells the backend that the client does not require retrieved organizeritems to include binary blobs such as thumbnail images
 */

/*!
  Constructs a new organizeritem fetch hint which requests that the backend fetch all information
 */
QOrganizerItemFetchHint::QOrganizerItemFetchHint()
    : d(new QOrganizerItemFetchHintPrivate)
{
}

/*!
  Constructs a new organizeritem fetch hint as a copy of \a other
 */
QOrganizerItemFetchHint::QOrganizerItemFetchHint(const QOrganizerItemFetchHint &other)
    : d(other.d)
{
}

/*!
  Frees any memory in use by the fetch hint
 */
QOrganizerItemFetchHint::~QOrganizerItemFetchHint()
{
}

/*!
  Assigns this fetch hint to be equal to the \a other fetch hint
 */
QOrganizerItemFetchHint& QOrganizerItemFetchHint::operator=(const QOrganizerItemFetchHint& other)
{
    d = other.d;
    return *this;
}

/*!
  Returns the list of definition names that identify detail definitions of which details
  the manager should (at a minimum) retrieve when fetching organizeritems.
  This hint may be ignored by the backend, in which case it will return the full set of details for
  each organizeritem retrieved.

  \sa setDetailDefinitionsHint()
 */
QStringList QOrganizerItemFetchHint::detailDefinitionsHint() const
{
    return d->m_definitionsHint;
}

/*!
  Sets the list of definition names that identify detail definitions of which details
  the manager should (at a minimum) retrieve when fetching organizeritems to \a definitionNames.
  This hint may be ignored by the backend, in which case it will return the full set of details for
  each organizeritem retrieved.

  \sa detailDefinitionsHint()
 */
void QOrganizerItemFetchHint::setDetailDefinitionsHint(const QStringList& definitionNames)
{
    d->m_definitionsHint = definitionNames;
}

/*!
  Returns the optimization hint flags specified by the client.
  These hints may be ignored by the backend, in which case it will return
  the full set of information accessible in a organizeritem, including
  relationships and binary blobs.

  \sa setOptimizationHints()
 */
QOrganizerItemFetchHint::OptimizationHints QOrganizerItemFetchHint::optimizationHints() const
{
    return d->m_optimizationHints;
}

/*!
  Sets the optimization hint flags specified by the client to \a hints.
  These hints may be ignored by the backend, in which case it will return
  the full set of information accessible in a organizeritem, including
  relationships and binary blobs.

  \sa optimizationHints()
 */
void QOrganizerItemFetchHint::setOptimizationHints(OptimizationHints hints)
{
    d->m_optimizationHints = hints;
}

QTM_END_NAMESPACE
