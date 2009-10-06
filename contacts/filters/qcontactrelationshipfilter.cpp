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
 * Sets the list of left-contact identifiers to \a possibleIds.
 * If \a possibleIds is empty, all contact ids will match.
 */
void QContactRelationshipFilter::setLeftIds(const QList<QUniqueId>& possibleIds)
{
    Q_D(QContactRelationshipFilter);
    d->m_leftIds = possibleIds;
}

/*!
 * Sets the list of right-contact identifiers to \a possibleIds.
 * If \a possibleIds is empty, all contact ids will match.
 */
void QContactRelationshipFilter::setRightIds(const QList<QUniqueId>& possibleIds)
{
    Q_D(QContactRelationshipFilter);
    d->m_rightIds = possibleIds;
}

/*!
 * Sets the list of left-contact manager URIs to \a possibleManagerUris.
 * If \a possibleManagerUris is empty, all manager URIs will match.
 */
void QContactRelationshipFilter::setLeftManagerUris(const QStringList& possibleManagerUris)
{
    Q_D(QContactRelationshipFilter);
    d->m_leftManagerUris = possibleManagerUris;
}

/*!
 * Sets the list of right-contact manager URIs to \a possibleManagerUris.
 * If \a possibleManagerUris is empty, all manager URIs will match.
 */
void QContactRelationshipFilter::setRightManagerUris(const QStringList& possibleManagerUris)
{
    Q_D(QContactRelationshipFilter);
    d->m_rightManagerUris = possibleManagerUris;
}

/*!
 * Sets the list of relationship types which will be matched to \a relationshipTypes.
 * If \a relationshipTypes is empty, all relationship types will match.
 */
void QContactRelationshipFilter::setRelationshipTypes(const QStringList& relationshipTypes)
{
    Q_D(QContactRelationshipFilter);
    d->m_relationshipTypes = relationshipTypes;
}

/*!
 * Sets the upper limit for priority which will be used when filtering to \a min.
 * If \a min is negative, the lower limit is taken to be infinitely small.
 */
void QContactRelationshipFilter::setMinimumPriority(int min)
{
    Q_D(QContactRelationshipFilter);
    d->m_minimumPriority = min;
}

/*!
 * Sets the upper limit for priority which will be used when filtering to \a max.
 * If \a max is negative, the upper limit is taken to be infinity.
 */
void QContactRelationshipFilter::setMaximumPriority(int max)
{
    Q_D(QContactRelationshipFilter);
    d->m_maximumPriority = max;
}

/*!
 * Returns the list of left-contact ids which will be matched by this filter.
 * An empty list signifies that any left-contact id will match.
 */
QList<QUniqueId> QContactRelationshipFilter::leftIds() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_leftIds;
}

/*!
 * Returns the list of right-contact ids which will be matched by this filter.
 * An empty list signifies that any right-contact id will match.
 */
QList<QUniqueId> QContactRelationshipFilter::rightIds() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_rightIds;
}

/*!
 * Returns all left-contact manager URIs which will be matched by this filter.
 * An empty list signifies that any manager URI will match.
 */
QStringList QContactRelationshipFilter::leftManagerUris() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_leftManagerUris;
}

/*!
 * Returns all right-contact manager URIs which will be matched by this filter.
 * An empty list signifies that any manager URI will match.
 */
QStringList QContactRelationshipFilter::rightManagerUris() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_rightManagerUris;
}

/*!
 * Returns the types of relationships which will be matched by this filter.
 * An empty list signifies that any relationship type will match.
 */
QStringList QContactRelationshipFilter::relationshipTypes() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_relationshipTypes;
}

/*!
 * Returns the lower limit for priority which will be used when filtering contacts via relationship.
 * A negative minimum priority denotes an infinitely small minimum priority.
 */
int QContactRelationshipFilter::minimumPriority() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_minimumPriority;
}

/*!
 * Returns the upper limit for priority which will be used when filtering contacts via relationship.
 * A negative maximum priority denotes an infinite maximum priority.
 */
int QContactRelationshipFilter::maximumPriority() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_maximumPriority;
}
