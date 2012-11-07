/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
#include "qcontactdetailfielddefinition.h"

QTM_BEGIN_NAMESPACE

class QContactDetailDefinitionData;
class Q_CONTACTS_EXPORT QContactDetailDefinition
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

    /* can you have more than one detail of this definition? */
    void setUnique(bool unique);
    bool isUnique() const;

    /* Mapping of field key to fields allowed in details of this definition */
    void setFields(const QMap<QString, QContactDetailFieldDefinition>& fields);
    QMap<QString, QContactDetailFieldDefinition> fields() const;
    void insertField(const QString& key, const QContactDetailFieldDefinition& field);
    void removeField(const QString& key);

private:
    QSharedDataPointer<QContactDetailDefinitionData> d;
};

#ifndef QT_NO_DATASTREAM
Q_CONTACTS_EXPORT QDataStream& operator<<(QDataStream& out, const QContactDetailDefinition& definition);
Q_CONTACTS_EXPORT QDataStream& operator>>(QDataStream& in, QContactDetailDefinition& definition);
#endif

#ifndef QT_NO_DEBUG_STREAM
Q_CONTACTS_EXPORT QDebug operator<<(QDebug dbg, const QContactDetailDefinition& definition);
#endif

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QContactDetailDefinition), Q_MOVABLE_TYPE);

#endif
