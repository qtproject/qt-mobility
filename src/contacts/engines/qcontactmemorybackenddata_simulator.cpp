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

#include "qcontactmemorybackenddata_simulator_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QAtomicInt>
#include <qcontact_p.h>
#include <qcontactrelationship_p.h>
#include <qcontactdetail_p.h>
#include <qcontactid_p.h>
#include <qcontactdetaildefinition_p.h>
#include <qcontactdetailfielddefinition_p.h>

Q_DECLARE_METATYPE(QtMobility::QContactData)
Q_DECLARE_METATYPE(QtMobility::QContactId)
Q_DECLARE_METATYPE(QtMobility::QContactIdPrivate)
Q_DECLARE_METATYPE(QtMobility::QContactDetail)
Q_DECLARE_METATYPE(QtMobility::QContactDetailPrivate)
Q_DECLARE_METATYPE(QtMobility::QContactDetailDefinition)
Q_DECLARE_METATYPE(QtMobility::QContactDetailDefinitionData)
Q_DECLARE_METATYPE(QtMobility::QContactDetailFieldDefinition)
Q_DECLARE_METATYPE(QtMobility::QContactDetailFieldDefinitionPrivate)
Q_DECLARE_METATYPE(QtMobility::QContactRelationshipPrivate)

QTM_BEGIN_NAMESPACE

#ifdef SIMULATOR_APPLICATION
// Workaround for non-exported symbol that is used by this file.
// It won't matter if this wrong lastDetailKey is used here, , since m_id is always
// set again when a QContactDetail is sent through the sockets.
QAtomicInt QContactDetailPrivate::lastDetailKey(1);

// Should work too, since they are only used for performance.
QHash<QString, char*> QContactStringHolder::s_allocated;
QHash<const char *, QString> QContactStringHolder::s_qstrings;

uint qHash(const QContactStringHolder &h)
{ return qHash(h.toQString()); }
#endif

void qt_registerContactsTypes()
{
    qRegisterMetaTypeStreamOperators<QContact>("QtMobility::QContact");
    qRegisterMetaTypeStreamOperators<QContactData>("QtMobility::QContactData");
    qRegisterMetaTypeStreamOperators<QContactId>("QtMobility::QContactId");
    qRegisterMetaTypeStreamOperators<QContactIdPrivate>("QtMobility::QContactIdPrivate");
    qRegisterMetaTypeStreamOperators<QContactDetail>("QtMobility::QContactDetail");
    qRegisterMetaTypeStreamOperators<QContactDetailPrivate>("QtMobility::QContactDetailPrivate");
    qRegisterMetaTypeStreamOperators<QContactDetailDefinition>("QtMobility::QContactDetailDefinition");
    qRegisterMetaTypeStreamOperators<QContactDetailDefinitionData>("QtMobility::QContactDetailDefinitionData");
    qRegisterMetaTypeStreamOperators<QContactDetailFieldDefinition>("QtMobility::QContactDetailFieldDefinition");
    qRegisterMetaTypeStreamOperators<QContactDetailFieldDefinitionPrivate>("QtMobility::QContactDetailFieldDefinitionPrivate");
    qRegisterMetaTypeStreamOperators<QContactRelationship>("QtMobility::QContactRelationship");
    qRegisterMetaTypeStreamOperators<QContactRelationshipPrivate>("QtMobility::QContactRelationshipPrivate");
    qRegisterMetaTypeStreamOperators<QContactSimulatorData>("QtMobility::QContactSimulatorData");
    qRegisterMetaTypeStreamOperators<QContactStringHolder>("QtMobility::QContactStringHolder");
    qRegisterMetaTypeStreamOperators<Simulator::SaveContactReply>("QtMobility::Simulator::SaveContactReply");
    qRegisterMetaTypeStreamOperators<Simulator::SaveRelationshipReply>("QtMobility::Simulator::SaveRelationshipReply");
}

QDataStream &operator<<(QDataStream &out, const QContactIdPrivate &s)
{
    out << s.m_localId << s.m_managerUri;
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactIdPrivate &s)
{
    in >> s.m_localId >> s.m_managerUri;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QContactId &s)
{
    out << *s.d;
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactId &s)
{
    in >> *s.d;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QContactStringHolder &s)
{
    out << s.toQString();
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactStringHolder &s)
{
    QString data;
    in >> data;
    s = QContactStringHolder(data);
    return in;
}

QDataStream &operator<<(QDataStream &out, const QContactDetailPrivate &s)
{
    out << s.m_id << s.m_definitionName;
    out << s.m_values;
    out << static_cast<int>(s.m_access);
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactDetailPrivate &s)
{
    in >> s.m_id >> s.m_definitionName;
    in >> s.m_values;
    int access;
    in >> access;
    s.m_access = static_cast<QContactDetail::AccessConstraints>(access);
    return in;
}

QDataStream &operator<<(QDataStream &out, const QContactDetail &s)
{
    out << *s.d;
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactDetail &s)
{
    in >> *s.d;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QContactDetailFieldDefinitionPrivate &s)
{
    out << s.m_allowableValues << s.m_dataType;
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactDetailFieldDefinitionPrivate &s)
{
    in >> s.m_allowableValues >> s.m_dataType;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QContactDetailFieldDefinition &s)
{
    out << *s.d;
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactDetailFieldDefinition &s)
{
    in >> *s.d;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QContactDetailDefinitionData &s)
{
    out << s.m_fields << s.m_name << s.m_unique;
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactDetailDefinitionData &s)
{
    in >> s.m_fields >> s.m_name >> s.m_unique;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QContactDetailDefinition &s)
{
    out << *s.d;
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactDetailDefinition &s)
{
    in >> *s.d;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QContactRelationshipPrivate &s)
{
    out << s.m_first << s.m_second;
    out << s.m_relationshipType;
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactRelationshipPrivate &s)
{
    in >> s.m_first >> s.m_second;
    in >> s.m_relationshipType;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QContactRelationship &s)
{
    out << *s.d;
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactRelationship &s)
{
    in >> *s.d;
    return in;
}


QDataStream &operator<<(QDataStream &out, const QContactData &s)
{
    out << s.m_id << s.m_details;
    out << s.m_relationshipsCache;
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactData &s)
{
    in >> s.m_id >> s.m_details;
    in >> s.m_relationshipsCache;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QContact &s)
{
    out << *s.d;
    return out;
}
QDataStream &operator>>(QDataStream &in, QContact &s)
{
    in >> *s.d;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QContactSimulatorData &s)
{
    out << s.m_selfContactId << s.m_contacts << s.m_contactIds;
    out << s.m_relationships << s.m_orderedRelationships;
    out << s.m_definitionIds << s.m_definitions;
    out << s.m_nextContactId << s.m_lastDetailId;
    return out;
}
QDataStream &operator>>(QDataStream &in, QContactSimulatorData &s)
{
    in >> s.m_selfContactId >> s.m_contacts >> s.m_contactIds;
    in >> s.m_relationships >> s.m_orderedRelationships;
    in >> s.m_definitionIds >> s.m_definitions;
    in >> s.m_nextContactId >> s.m_lastDetailId;
    return in;
}


namespace Simulator {

QDataStream &operator<<(QDataStream &out, const SaveContactReply &s)
{
    out << s.savedContact;
    qint32 error = s.error;
    out << error;
    return out;
}
QDataStream &operator>>(QDataStream &in, SaveContactReply &s)
{
    in >> s.savedContact;
    qint32 error;
    in >> error;
    s.error = static_cast<QContactManager::Error>(error);
    return in;
}

QDataStream &operator<<(QDataStream &out, const SaveRelationshipReply &s)
{
    out << s.savedRelationship;
    qint32 error = s.error;
    out << error;
    return out;
}
QDataStream &operator>>(QDataStream &in, SaveRelationshipReply &s)
{
    in >> s.savedRelationship;
    qint32 error;
    in >> error;
    s.error = static_cast<QContactManager::Error>(error);
    return in;
}

} // namespace Simulator

QTM_END_NAMESPACE
