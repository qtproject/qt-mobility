/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qorganizeritemdetaildefinitionsaverequest.h"
#include "qorganizeritemrequests_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemDetailDefinitionSaveRequest
  \brief The QOrganizerItemDetailDefinitionSaveRequest class allows a client to
  asynchronously request that certain detail definitions be saved in an
  organizer item store manager.
  \inmodule QtOrganizer

  For a QOrganizerItemDetailDefinitionSaveRequest, the resultsAvailable() signal will be emitted when
  either the individual item errors (which may be retrieved by calling errorMap()), or the resultant
  detail definitions (which may be retrieved by calling definitions()), are updated, as well as if
  the overall operation error (which may be retrieved by calling error()) is updated.
  
  \ingroup organizer-requests
 */

/*! Constructs a new detail definition save request whose parent is the specified \a parent */
QOrganizerItemDetailDefinitionSaveRequest::QOrganizerItemDetailDefinitionSaveRequest(QObject* parent)
    : QOrganizerAbstractRequest(new QOrganizerItemDetailDefinitionSaveRequestPrivate, parent)
{
}

/*!
  Sets the definition to save to be the given \a definition.
  Equivalent to calling:
  \code
      setDefinitions(QList<QOrganizerItemDetailDefinition>() << definition);
  \endcode
 */
void QOrganizerItemDetailDefinitionSaveRequest::setDefinition(const QOrganizerItemDetailDefinition& definition)
{
    Q_D(QOrganizerItemDetailDefinitionSaveRequest);
    d->m_definitions.clear();
    d->m_definitions.append(definition);
}

/*! Sets the definitions to save to be \a definitions */
void QOrganizerItemDetailDefinitionSaveRequest::setDefinitions(const QList<QOrganizerItemDetailDefinition>& definitions)
{
    Q_D(QOrganizerItemDetailDefinitionSaveRequest);
    d->m_definitions = definitions;
}

/*! Returns the list of definitions that will be saved if called prior to calling \c start(),
    otherwise returns the list of detail definitions as they were saved in the organizer item store */
QList<QOrganizerItemDetailDefinition> QOrganizerItemDetailDefinitionSaveRequest::definitions() const
{
    Q_D(const QOrganizerItemDetailDefinitionSaveRequest);
    return d->m_definitions;
}

/*! Sets the type of organizer item for which detail definitions should be saved to \a organizeritemType */
void QOrganizerItemDetailDefinitionSaveRequest::setItemType(const QString& organizeritemType)
{
    Q_D(QOrganizerItemDetailDefinitionSaveRequest);
    d->m_organizeritemType = organizeritemType;
}

/*! Returns the type of organizer item for which detail definitions will be saved */
QString QOrganizerItemDetailDefinitionSaveRequest::itemType() const
{
    Q_D(const QOrganizerItemDetailDefinitionSaveRequest);
    return d->m_organizeritemType;
}

/*! Returns the map of input definition list indices to errors which occurred */
QMap<int, QOrganizerManager::Error> QOrganizerItemDetailDefinitionSaveRequest::errorMap() const
{
    Q_D(const QOrganizerItemDetailDefinitionSaveRequest);
    return d->m_errors;
}

#include "moc_qorganizeritemdetaildefinitionsaverequest.cpp"

QTM_END_NAMESPACE
