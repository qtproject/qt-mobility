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

#include "qcontactdetaildefinitionfield.h"
#include "qcontactdetaildefinitionfield_p.h"

/*!
 * \class QContactDetailDefinitionField
 * \brief A field in a QContactDetail.
 *
 * Encapsulates information about a particular datum which may be part of a QContactDetail, including the type,
 * allowable values and access constraints.
 */

/*!
 * \enum QContactDetailDefinitionField::AccessConstraint
 *
 * This enum defines the access constraints which may be set on fields of a detail definition in the store for which the definition is valid.
 * The constraint which applies to the definition takes precedence over a constraint which applies to a field of that definition.
 * For example, if a field has the \c QContactDetailDefinitionField::NoConstraint constraint, but the detail definition from which the field came has
 * either the \c QContactDetailDefinition::ReadOnly or \c QContactDetailDefinition::CreateOnly constraint, then the field will be a read-only field.
 *
 * \value NoConstraint Fields with this access constraint set have no special access semantics associated with them.  Users can read, write, and otherwise modify such fields in any manner.
 * \value ReadOnly Fields with this access constraint set are dynamically modified by the backend.  Users cannot write values to fields of details of definitions with this access constraint set.
 */

/*!
 * Constructs a new field with no constraints and an invalid data type.
 */
QContactDetailDefinitionField::QContactDetailDefinitionField()
        : d(new QContactDetailDefinitionFieldPrivate)
{
}

/*!
 * Cleans up the memory in use by the field
 */
QContactDetailDefinitionField::~QContactDetailDefinitionField()
{
}

/*!
 * Constructs a copy of the \a other field
 */
QContactDetailDefinitionField::QContactDetailDefinitionField(const QContactDetailDefinitionField& other)
        : d(other.d)
{
}

/*!
 * Assigns the field to be equal to the \a other field
 */
QContactDetailDefinitionField& QContactDetailDefinitionField::operator=(const QContactDetailDefinitionField& other)
{
    d = other.d;
    return *this;
}

/*!
 * Returns the data type of the field
 */
QVariant::Type QContactDetailDefinitionField::dataType() const
{
    return d->m_dataType;
}

/*!
 * Sets the data type of the field to \a type
 */
void QContactDetailDefinitionField::setDataType(QVariant::Type type)
{
    d->m_dataType = type;
}

/*!
 * Returns the list of allowable values which this field may store
 */
QVariantList QContactDetailDefinitionField::allowableValues() const
{
    return d->m_allowableValues;
}

/*!
 * Sets the list of allowable values which this field may store to \a values
 */
void QContactDetailDefinitionField::setAllowableValues(const QVariantList values)
{
    d->m_allowableValues = values;
}

/*!
 * Returns the access constraints which apply to this field
 */
QContactDetailDefinitionField::AccessConstraint QContactDetailDefinitionField::accessConstraint() const
{
    return d->m_accessConstraint;
}

/*!
 * Sets the access constraints which apply to this field to \a constraint
 */
void QContactDetailDefinitionField::setAccessConstract(QContactDetailDefinitionField::AccessConstraint constraint)
{
    d->m_accessConstraint = constraint;
}

/*!
 * Returns true if the access constraint, allowable values and data type of the \a other field are equal to those of this field
 */
bool QContactDetailDefinitionField::operator==(const QContactDetailDefinitionField& other) const
{
    if (d->m_accessConstraint != other.d->m_accessConstraint)
        return false;
    if (d->m_allowableValues != other.d->m_allowableValues)
        return false;
    if (d->m_dataType != other.d->m_dataType)
        return false;
    return true;
}

/*!
 * Returns true if the access constraint, allowable values or data type of the \a other field differ from those of this field
 */
bool QContactDetailDefinitionField::operator!=(const QContactDetailDefinitionField& other) const
{
    return !(*this == other);
}
