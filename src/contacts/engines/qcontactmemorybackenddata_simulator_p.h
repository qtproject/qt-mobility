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

#ifndef QCONTACTMEMORYBACKENDDATA_SIMULATOR_P_H
#define QCONTACTMEMORYBACKENDDATA_SIMULATOR_P_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//


#include "qcontactmemorybackend_p.h"
#include <qcontactdetail_p.h>
#include <qcontact.h>
#include <qcontactrelationship.h>
#include <qcontactdetaildefinition.h>
#include <qtcontactsglobal.h>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QMetaType>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

void qt_registerContactsTypes();

struct QContactSimulatorData
{
    QContactLocalId m_selfContactId;               // the "MyCard" contact id
    QList<QContact> m_contacts;                    // list of contacts
    QList<QContactLocalId> m_contactIds;           // list of contact Id's
    QList<QContactRelationship> m_relationships;   // list of contact relationships
    QMap<QContactLocalId, QList<QContactRelationship> > m_orderedRelationships; // map of ordered lists of contact relationships
    QList<QString> m_definitionIds;                // list of definition types (id's)
    QMap<QString, QMap<QString, QContactDetailDefinition> > m_definitions; // map of contact type to map of definition name to definitions.
    QContactLocalId m_nextContactId;
    int m_lastDetailId;
};

namespace Simulator {

struct SaveContactReply
{
    QContact savedContact;
    QContactManager::Error error;
};

struct SaveRelationshipReply
{
    QContactRelationship savedRelationship;
    QContactManager::Error error;
};

} // namespace Simulator

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QtMobility::QContact)
Q_DECLARE_METATYPE(QtMobility::QContactRelationship)
Q_DECLARE_METATYPE(QtMobility::QContactSimulatorData)
Q_DECLARE_METATYPE(QtMobility::Simulator::SaveContactReply)
Q_DECLARE_METATYPE(QtMobility::Simulator::SaveRelationshipReply)

QT_END_HEADER

#endif // QCONTACTMEMORYBACKENDDATA_SIMULATOR_P_H
