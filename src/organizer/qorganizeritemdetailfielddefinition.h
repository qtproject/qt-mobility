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

#ifndef QORGANIZERITEMDETAILFIELDDEFINITION_H
#define QORGANIZERITEMDETAILFIELDDEFINITION_H

#include <QList>
#include <QVariant>
#include <QSharedDataPointer>

#include "qtorganizerglobal.h"

QTM_BEGIN_NAMESPACE

class QOrganizerItemDetailFieldDefinitionPrivate;
class Q_ORGANIZER_EXPORT QOrganizerItemDetailFieldDefinition
{
public:
    QOrganizerItemDetailFieldDefinition();
    ~QOrganizerItemDetailFieldDefinition();

    QOrganizerItemDetailFieldDefinition(const QOrganizerItemDetailFieldDefinition& other);
    QOrganizerItemDetailFieldDefinition& operator=(const QOrganizerItemDetailFieldDefinition& other);

    QVariant::Type dataType() const;
    void setDataType(QVariant::Type type);

    QVariantList allowableValues() const;
    void setAllowableValues(const QVariantList values);

    bool operator==(const QOrganizerItemDetailFieldDefinition& other) const;
    bool operator!=(const QOrganizerItemDetailFieldDefinition& other) const;

private:
    QSharedDataPointer<QOrganizerItemDetailFieldDefinitionPrivate> d;
};

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItemDetailFieldDefinition), Q_MOVABLE_TYPE);

#endif
