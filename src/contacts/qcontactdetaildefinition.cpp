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

#include "qcontactdetaildefinition.h"
#include "qcontactdetaildefinition_p.h"

/*!
  \class QContactDetailDefinition
  \brief The QContactDetailDefinition class supplies a definition of the
    semantics of the Details data representation and use.
    \ingroup contacts-main
 
  The QContactDetailDefinition class provides the specification for
  a detail that can be included in any particular QContact.
  The definition does not include any data, but defines
  the semantics of the representation and use of data
  details that are stored in a QContact.
  \sa QContact
 */

/*!
 * \fn QContactDetailDefinition::operator!=(const QContactDetailDefinition& other) const
 * Returns true if this detail definition has different allowable field types, access constraints or uniqueness to the \a other definition
 */

/*!
 * \enum QContactDetailDefinition::AccessConstraint
 *
 * This enum defines the access constraints which may be set on all details of this definition in the store for which the definition is valid.
 *
 * \value NoConstraint Details of definitions with this access constraint set have no special access semantics associated with them.  Users can read, write, and otherwise modify such details in any manner.
 * \value ReadOnly Details of definitions with this access constraint set are dynamically modified by the backend.  Users cannot write values to details of definitions with this access constraint set.
 * \value CreateOnly Details of definitions with this access constraint set are static once created.  Their value cannot be changed dynamically, nor can they be written or read by users.
 */

/*! Construct a new, invalid QContactDetailDefinition */
QContactDetailDefinition::QContactDetailDefinition()
    : d(new QContactDetailDefinitionData)
{
}

/*! Constructs a new QContactDetailDefinition from \a other */
QContactDetailDefinition::QContactDetailDefinition(const QContactDetailDefinition& other)
    : d(other.d)
{
}

/*! Assigns this QContactDetailDefinition to \a other */
QContactDetailDefinition& QContactDetailDefinition::operator=(const QContactDetailDefinition& other)
{
    d = other.d;
    return *this;
}

/*! Frees the memory used by this QContactDetailDefinition */
QContactDetailDefinition::~QContactDetailDefinition()
{
}

/*! Returns true if the definition has the same type, uniqueness, access constraint and allowable value datatypes as \a other */
bool QContactDetailDefinition::operator==(const QContactDetailDefinition& other) const
{
    if (d->m_name != other.d->m_name)
        return false;
    if (d->m_unique != other.d->m_unique)
        return false;
    if (d->m_constraint != other.d->m_constraint)
        return false;
    if (d->m_fields != other.d->m_fields)
        return false;
    return true;
}

/*! Returns true if the id and fields of this definition are empty */
bool QContactDetailDefinition::isEmpty() const
{
    if (!d->m_name.isEmpty())
        return false;
    if (!d->m_fields.isEmpty())
        return false;
    return true;
}

/*! Sets the unique identifier of this detail type to \a definitionName. */
void QContactDetailDefinition::setName(const QString& definitionName)
{
    d->m_name = definitionName;
}

/*!
 * Sets whether a contact can have more than one detail of this type.
 * If \a unique is true, only one detail of this type can be added.
 * Otherwise, any number can be added.
 */
void QContactDetailDefinition::setUnique(bool unique)
{
    d->m_unique = unique;
}

/*! Returns the identifier of this detail definition */
QString QContactDetailDefinition::name() const
{
    return d->m_name;
}

/*! Returns the per-contact uniqueness of this definition */
bool QContactDetailDefinition::isUnique() const
{
    return d->m_unique;
}

/*! Sets the fields which constitute the data of details of this this definition to \a fields */
void QContactDetailDefinition::setFields(const QMap<QString, QContactDetailDefinition::Field>& fields)
{
    d->m_fields = fields;
}

/*! Returns the map of keys to fields which are present in details of this definition */
QMap<QString, QContactDetailDefinition::Field> QContactDetailDefinition::fields() const
{
    return d->m_fields;
}

/*!
 * Returns a reference to the map of keys to fields which are present in details of this definition.
 *
 * You can make changes to the returned map.
 */
QMap<QString, QContactDetailDefinition::Field>& QContactDetailDefinition::fields()
{
    return d->m_fields;
}

/*! Returns the access constraint that is applied to details of this definition */
QContactDetailDefinition::AccessConstraint QContactDetailDefinition::accessConstraint() const
{
    return d->m_constraint;
}

/*!
 * \fn QContactDetailDefinition::setAccessConstraint(const AccessConstraint& constraint)
 * Sets the access constraint that is applied to details of this definition to \a constraint
 */
void QContactDetailDefinition::setAccessConstraint(const QContactDetailDefinition::AccessConstraint& constraint)
{
    d->m_constraint = constraint;
}

/*!
 * \fn QContactDetailDefinition::Field::operator!=(const QContactDetailDefinition::Field& other) const
 * Returns true if either the data type or range of allowable values of this field is different to that of \a other
 */

/*!
 * \fn QContactDetailDefinition::Field::Field()
 * Constructs a new detail definition field definition.
 */

/*!
 * Returns true if this detail definition field is equal to the \a other field; returns false if either the data type or the range of allowable values are not equal
 */
bool QContactDetailDefinition::Field::operator==(const QContactDetailDefinition::Field& other) const
{
    if (this->dataType == other.dataType && this->allowableValues == other.allowableValues)
        return true;
    return false;
}
