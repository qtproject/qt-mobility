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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactrelationshipfilter.h"
#include "qcontactrelationshipfilter_p.h"
#include "qcontactmanager.h"

/*!
 * \class QContactRelationshipFilter
 * \brief The QContactRelationshipFilter class provides a filter based around relationship criteria
 *
 * It may be used to select contacts which are involved in relationships which are of a certain type, or which involve certain contacts.
 */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactRelationshipFilter)

/*!
 * \fn QContactRelationshipFilter::QContactRelationshipFilter(const QContactFilter& other)
 * Constructs a copy of \a other if possible, else constructs a new QContactRelationshipFilter.
 */

/*!
 * Constructs a new relationship filter
 */
QContactRelationshipFilter::QContactRelationshipFilter()
    : QContactFilter(new QContactRelationshipFilterPrivate)
{
}

/*!
 * Sets the role in the relationship that a contact must be in order to match this filter to \a roleInRelationship
 */
void QContactRelationshipFilter::setRole(QContactRelationshipFilter::Role roleInRelationship)
{
    Q_D(QContactRelationshipFilter);
    d->m_roleInRelationship = roleInRelationship;
}

/*!
 * Sets the type of relationship which a contact must have in order to match this filter to \a relationshipType
 */
void QContactRelationshipFilter::setType(const QString& relationshipType)
{
    Q_D(QContactRelationshipFilter);
    d->m_relationshipType;
}

/*!
 * Sets the manager URI and contact id of the other participant which must be present in the relationship with
 * the contact in order for the contact to match the filter to be \a managerUri and \a id respectively.
 * If the manager URI is the empty string, it denotes a contact in the manager in which the relationship is stored.
 */
void QContactRelationshipFilter::setOtherParticipant(const QString& managerUri, const QUniqueId& id)
{
    Q_D(QContactRelationshipFilter);
    d->m_otherParticipantId = id;
    d->m_otherParticipantManagerUri = managerUri;
}

/*!
 * Sets the contact id of the other participant which must be present in the relationship with the contact
 * in order for the contact to match the filter to be \a id, and sets the manager URI of the participant
 * to the empty string, which denotes the manager in which the relationship is stored.
 */
void QContactRelationshipFilter::setOtherParticipant(const QUniqueId& id)
{
    Q_D(QContactRelationshipFilter);
    d->m_otherParticipantId = id;
    d->m_otherParticipantManagerUri = QString();
}

/*!
 * Returns the role that a contact must have in a relationship in order to match the filter
 */
QContactRelationshipFilter::Role QContactRelationshipFilter::role() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_roleInRelationship;
}

/*!
 * Returns the type of relationship that a contact must have in order to match the filter
 */
QString QContactRelationshipFilter::type() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_relationshipType;
}

/*!
 * Returns the URI of the manager which must store the other participant for the contact to match the filter.
 * An empty URI denotes that the other participant must be stored in the manager that stores the relationship.
 */
QString QContactRelationshipFilter::otherParticipantManagerUri() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_otherParticipantManagerUri;
}

/*!
 * Returns the id of another contact with whom a contact must have a relationship in order to match the filter
 */
QUniqueId QContactRelationshipFilter::otherParticipantId() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_otherParticipantId;
}
