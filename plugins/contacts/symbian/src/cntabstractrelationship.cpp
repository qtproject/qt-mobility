/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "cntabstractrelationship.h"

/*!
 * Constructor
 * 
 * \a contactDatabase CContactDatabase with established connection to the database
 * \a managerUri current manager uri
 * \a relationshipType name of the relationship type supported by the inherited class
 */
CntAbstractRelationship::CntAbstractRelationship(CContactDatabase *contactDatabase, const QString &managerUri, const QString &relationshipType) : 
    m_contactDatabase(contactDatabase),
    m_managerUri(managerUri),
    m_relationshipType(relationshipType) 
{}

/*!
 * Destructor
 */
CntAbstractRelationship::~CntAbstractRelationship()
{}

/*!
 * \return the name of the supported relationship type
 */
QString CntAbstractRelationship::relationshipType() const 
{
    return m_relationshipType;
}

/*!
 * \return the database instance
 */
CContactDatabase *CntAbstractRelationship::database()
{
    return m_contactDatabase;
}

/*!
 * \return the manager uri
 */
QString CntAbstractRelationship::managerUri()
{
    return m_managerUri;
}
