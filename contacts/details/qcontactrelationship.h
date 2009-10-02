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


#ifndef QCONTACTRELATIONSHIP_H
#define QCONTACTRELATIONSHIP_H

#include <QString>
#include <QVariant>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"

/* Leaf class */
class QTCONTACTS_EXPORT QContactRelationship : public QContactDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldRelationshipType;
    const char* FieldRelatedContactLabel;
    const char* FieldRelatedContactId;
    const char* FieldRelatedContactManagerUri;
    const char* RelationshipTypeHasMember;
    const char* RelationshipTypeIsMemberOf;
    const char* RelationshipTypeAggregates;
    const char* RelationshipTypeIsAggregatedBy;
    const char* RelationshipTypeIs;
    const char* RelationshipTypeAssistant;
    const char* RelationshipTypeManager;
    const char* RelationshipTypeSpouse;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactRelationship, "Relationship")
    Q_DECLARE_LATIN1_LITERAL(FieldRelationshipType, "RelationshipType");
    Q_DECLARE_LATIN1_LITERAL(FieldRelatedContactLabel, "RelatedContactLabel");
    Q_DECLARE_LATIN1_LITERAL(FieldRelatedContactId, "RelatedContactId");
    Q_DECLARE_LATIN1_LITERAL(FieldRelatedContactManagerUri, "RelatedContactManagerUri");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeHasMember, "HasMember");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeIsMemberOf, "IsMemberOf");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeAggregates, "Aggregates");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeIsAggregatedBy, "IsAggregatedBy");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeIs, "Is");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeAssistant, "Assistant");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeManager, "Manager");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeSpouse, "Spouse");
#endif

    void setRelationshipType(const QString& relationshipType) {setValue(FieldRelationshipType, relationshipType);}
    QString relationshipType() const {return value(FieldRelationshipType);}
    void setRelatedContactLabel(const QString& label) {setValue(FieldRelatedContactLabel, label);}
    QString relatedContactLabel() const {return value(FieldRelatedContactLabel);}
    void setRelatedContactId(const QUniqueId& id) {setValue(FieldRelatedContactId, id);}
    QUniqueId relatedContactId() const {return QUniqueId(value(FieldRelatedContactId).toUInt());}
    void setRelatedContactManagerUri(const QString& managerUri) {setValue(FieldRelatedContactManagerUri, managerUri);}
    QString relatedContactManagerUri() const {return value(FieldRelatedContactManagerUri);}
};

#endif

