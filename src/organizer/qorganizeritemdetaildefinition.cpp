/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** OrganizerItem: Nokia Corporation (qt-info@nokia.com)
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

#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemdetaildefinition_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemDetailDefinition

  The QOrganizerItemDetailDefinition class provides the specification for
  a detail that can be included in any particular QOrganizerItem.
  The definition does not include any data, but defines
  the semantics of the representation and use of data
  details that are stored in a QOrganizerItem.
 */

/*!
  \fn QOrganizerItemDetailDefinition::operator!=(const QOrganizerItemDetailDefinition& other) const
  Returns true if this detail definition has different allowable field types or uniqueness to the \a other definition
 */

/*! Construct a new, invalid QOrganizerItemDetailDefinition */
QOrganizerItemDetailDefinition::QOrganizerItemDetailDefinition()
    : d(new QOrganizerItemDetailDefinitionData)
{
}

/*! Constructs a new QOrganizerItemDetailDefinition from \a other */
QOrganizerItemDetailDefinition::QOrganizerItemDetailDefinition(const QOrganizerItemDetailDefinition& other)
    : d(other.d)
{
}

/*! Assigns this QOrganizerItemDetailDefinition to \a other */
QOrganizerItemDetailDefinition& QOrganizerItemDetailDefinition::operator=(const QOrganizerItemDetailDefinition& other)
{
    d = other.d;
    return *this;
}

/*! Frees the memory used by this QOrganizerItemDetailDefinition */
QOrganizerItemDetailDefinition::~QOrganizerItemDetailDefinition()
{
}

/*! Returns true if the definition has the same type, uniqueness and allowable value datatypes as \a other */
bool QOrganizerItemDetailDefinition::operator==(const QOrganizerItemDetailDefinition& other) const
{
    if (d->m_name != other.d->m_name)
        return false;
    if (d->m_unique != other.d->m_unique)
        return false;
    if (d->m_fields != other.d->m_fields)
        return false;
    return true;
}

/*! Returns true if the id and fields of this definition are empty */
bool QOrganizerItemDetailDefinition::isEmpty() const
{
    if (!d->m_name.isEmpty())
        return false;
    if (!d->m_fields.isEmpty())
        return false;
    return true;
}

/*! Sets the unique identifier of this detail type to \a definitionName. */
void QOrganizerItemDetailDefinition::setName(const QString& definitionName)
{
    d->m_name = definitionName;
}

/*!
  Sets whether a organizeritem can have more than one detail of this type.
  If \a unique is true, only one detail of this type can be added.
  Otherwise, any number can be added.
 */
void QOrganizerItemDetailDefinition::setUnique(bool unique)
{
    d->m_unique = unique;
}

/*! Returns the identifier of this detail definition */
QString QOrganizerItemDetailDefinition::name() const
{
    return d->m_name;
}

/*! Returns the per-organizeritem uniqueness of this definition */
bool QOrganizerItemDetailDefinition::isUnique() const
{
    return d->m_unique;
}

/*! Sets the fields which constitute the data of details of this this definition to \a fields */
void QOrganizerItemDetailDefinition::setFields(const QMap<QString, QOrganizerItemDetailFieldDefinition>& fields)
{
    d->m_fields = fields;
}

/*! Returns the map of keys to fields which are present in details of this definition */
QMap<QString, QOrganizerItemDetailFieldDefinition> QOrganizerItemDetailDefinition::fields() const
{
    return d->m_fields;
}

/*! Inserts the field \a field into the map of fields which constitute the data of details of this definition for the given field key \a key.
    If another field for that key already exists in the definition, it will be overwritten. */
void QOrganizerItemDetailDefinition::insertField(const QString& key, const QOrganizerItemDetailFieldDefinition& field)
{
    d->m_fields.insert(key, field);
}

/*! Removes the field associated with the given field key \a key from the map of fields which constitute the data of details of this definition. */
void QOrganizerItemDetailDefinition::removeField(const QString& key)
{
    d->m_fields.remove(key);
}

QTM_END_NAMESPACE
