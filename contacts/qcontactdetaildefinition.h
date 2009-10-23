/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef QCONTACTDETAILDEFINITION_H
#define QCONTACTDETAILDEFINITION_H

#include <QStringList>
#include <QString>
#include <QSharedDataPointer>
#include <QVariant>

#include "qtcontactsglobal.h"

class QContactDetailDefinitionData;
class QTCONTACTS_EXPORT QContactDetailDefinition
{
    friend class QContactManager;

public:
    // default constructor: produces an invalid QContactFieldDefinition.
    QContactDetailDefinition();
    ~QContactDetailDefinition();

    /* copy ctor & assignment */
    QContactDetailDefinition(const QContactDetailDefinition& other);
    QContactDetailDefinition& operator=(const QContactDetailDefinition& other);

    /* Comparator (for use in lists) */
    bool operator==(const QContactDetailDefinition& other) const;
    bool operator!=(const QContactDetailDefinition& other) const {return !operator==(other);}

    /* Check emptiness */
    bool isEmpty() const;

    /* name (identifier) of the definition */
    void setName(const QString& definitionName);
    QString name() const;

    /* can you have more than one of these? */
    void setUnique(bool unique);
    bool isUnique() const;

    /* Field Structure - a detail consists of one or more fields. */
    struct QTCONTACTS_EXPORT Field {
        Field() : dataType(QVariant::Invalid) {}
        QVariant::Type dataType;
        QVariantList allowableValues;
        bool operator==(const QContactDetailDefinition::Field& other) const;
        bool operator!=(const QContactDetailDefinition::Field& other) const {return !operator==(other);}
    };

    /* Mapping of field key to fields allowed in details of this definition */
    void setFields(const QMap<QString, QContactDetailDefinition::Field>& fields);
    QMap<QString, QContactDetailDefinition::Field> fields() const;
    QMap<QString, QContactDetailDefinition::Field>& fields();

    enum AccessConstraint {
        NoConstraint = 0,
        ReadOnly,
        CreateOnly
    };

    /* Accessor and mutator for access constraints on details of this definition */
    AccessConstraint accessConstraint() const;
    void setAccessConstraint(const AccessConstraint& constraint);

private:
    QSharedDataPointer<QContactDetailDefinitionData> d;
};

#endif
