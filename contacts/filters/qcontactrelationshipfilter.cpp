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

void QContactRelationshipFilter::takeSide(QContactRelationshipFilter::Side side)
{
    // TODO
}

void QContactRelationshipFilter::setFixedUri(const QString& fixedUri)
{
    // TODO
}

void QContactRelationshipFilter::setFixedId(const QUniqueId& fixedId)
{
    // TODO
}

void QContactRelationshipFilter::setRelationshipType(const QString& relationshipType)
{
    // TODO
}

QContactRelationshipFilter::Side QContactRelationshipFilter::side() const
{
    // TODO
    return QContactRelationshipFilter::Left;
}

QString QContactRelationshipFilter::fixedUri() const
{
    // TODO
    return QString();
}

QUniqueId QContactRelationshipFilter::fixedId() const
{
    // TODO
    return QUniqueId();
}

QString QContactRelationshipFilter::relationshipType() const
{
    // TODO
    return QString();
}
