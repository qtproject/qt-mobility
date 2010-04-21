/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactdetailfielddefinition.h"
#include "qcontactdetailfielddefinition_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactDetailFieldDefinition
  \brief The QContactDetailFieldDefinition class provides a field in a QContactDetail.
 
  Encapsulates information about a particular datum which may be part of a
  QContactDetail, including the type, allowable values and access constraints.
 */

/*!
 * Constructs a new field with no constraints and an invalid data type.
 */
QContactDetailFieldDefinition::QContactDetailFieldDefinition()
        : d(new QContactDetailFieldDefinitionPrivate)
{
}

/*!
 * Cleans up the memory in use by the field
 */
QContactDetailFieldDefinition::~QContactDetailFieldDefinition()
{
}

/*!
 * Constructs a copy of the \a other field
 */
QContactDetailFieldDefinition::QContactDetailFieldDefinition(const QContactDetailFieldDefinition& other)
        : d(other.d)
{
}

/*!
 * Assigns the field to be equal to the \a other field
 */
QContactDetailFieldDefinition& QContactDetailFieldDefinition::operator=(const QContactDetailFieldDefinition& other)
{
    d = other.d;
    return *this;
}

/*!
 * Returns the data type of the field
 */
QVariant::Type QContactDetailFieldDefinition::dataType() const
{
    return d->m_dataType;
}

/*!
 * Sets the data type of the field to \a type
 */
void QContactDetailFieldDefinition::setDataType(QVariant::Type type)
{
    d->m_dataType = type;
}

/*!
 * Returns the list of allowable values which this field may store
 */
QVariantList QContactDetailFieldDefinition::allowableValues() const
{
    return d->m_allowableValues;
}

/*!
 * Sets the list of allowable values which this field may store to \a values
 */
void QContactDetailFieldDefinition::setAllowableValues(const QVariantList values)
{
    d->m_allowableValues = values;
}

/*!
 * Returns true if the allowable values and data type of the \a other field are equal to those of this field
 */
bool QContactDetailFieldDefinition::operator==(const QContactDetailFieldDefinition& other) const
{
    if (d->m_allowableValues != other.d->m_allowableValues)
        return false;
    if (d->m_dataType != other.d->m_dataType)
        return false;
    return true;
}

/*!
 * Returns true if the allowable values or data type of the \a other field differ from those of this field
 */
bool QContactDetailFieldDefinition::operator!=(const QContactDetailFieldDefinition& other) const
{
    return !(*this == other);
}

QTM_END_NAMESPACE
