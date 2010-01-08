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

#include "qcontactdetail.h"
#include "qcontactdetail_p.h"
#include "qcontactmanager.h"

QTM_BEGIN_NAMESPACE

/* Definitions of predefined string constants */
Q_DEFINE_LATIN1_LITERAL(QContactDetail::FieldContext, "Context");
Q_DEFINE_LATIN1_LITERAL(QContactDetail::ContextOther, "Other");
Q_DEFINE_LATIN1_LITERAL(QContactDetail::ContextHome, "Home");
Q_DEFINE_LATIN1_LITERAL(QContactDetail::ContextWork, "Work");

/*!
  \class QContactDetail
 
  \brief The QContactDetail class provides access to a single, complete detail about a contact.
  \ingroup contacts-main
 
  All of the information for a contact is stored in one or more QContactDetail objects.
 
  A detail is a group of logically related bits of data - for example, a street address is a single
  detail that has multiple fields (number, region, country etc).  Every QContactDetail has an
  associated QContactDetailDefinition id that describes the fields, their data type, any
  restrictions on their values, and any restrictions on creating or updating details of that
  definition.
 
  A field which is common to all details is the context field.  This field is intended to store the
  context that this detail is associated with.  Commonly this will be something like
  "Home" or "Work", although no limitations are placed on which values may be stored in this field.
 
  It is possible to inherit from QContactDetail to provide convenience or
  standardized access to values.  For example, \l QContactPhoneNumber provides
  a convenient API for manipulating a QContactDetail as a phone number, according
  to the schema.
 
  If you wish to create your own, customized contact detail, you should use
  the \l Q_DECLARE_CUSTOM_CONTACT_DETAIL macro in order to ensure proper
  operation.  See the predefined leaf classes (like \l QContactPhoneNumber,
  \l QContactAddress) for more information.
 
  QContactDetail objects act like values.  In general, you can assign them
  to and fro and have reasonable behaviour, like the following example.
 
  \code
 
  QContactPhoneNumber number;
  number.setNumber("555-1212");
  // number.value(QContactPhoneNumber::FieldNumber) == "555-1212";
  // number.definitionName() == QContactPhoneNumber::staticType()
 
  QContactDetail detail = number;
  // detail.value(QContactPhoneNumber::FieldNumber) == "555-1212";
  // detail.definitionName() == QContactPhoneNumber::staticType()
 
  QContactPhoneNumber otherNumber = detail;
  // otherNumber.number() == "555-1212";
  // otherNumber.definitionName() == QContactPhoneNumber::staticType()
 
  QContactAddress address = detail;
  // address is now a default constructed QContactAddress
  // address.error() == QContactDetail::IncompatibleAssignmentError
  // address.value(QContactPhoneNumber::FieldNumber) is empty
  // address.definitionName() == QContactAddress::staticType()
 
  QContactAddress otherAddress = number;
  // otherAddress is now a default constructed QContactAddress
  // otherAddress.error() == QContactDetail::IncompatibleAssignmentError
  // otherAddress.value(QContactPhoneNumber::FieldNumber) is empty
  // otherAddress.definitionName() == QContactAddress::staticType()
  \endcode
 
  \sa QContact, QContactDetailDefinition, QContactDetailFilter, QContactDetailRangeFilter, Q_DECLARE_CUSTOM_CONTACT_DETAIL
 */

/*!
 * \macro Q_DECLARE_CUSTOM_CONTACT_DETAIL
 * \relates QContactDetail
 *
 * Macro for simplifying declaring custom (leaf) detail classes.
 *
 * If you are creating a convenience class for a type of QContactDetail,
 * you should use this macro when declaring your class to ensure that
 * it interoperates with other contact functionality.
 *
 * Here is an example of a class (\l QContactPhoneNumber) using this macro.
 * Note that the class provides some predefined constants
 * and some convenience methods that return values associated with schema
 * fields.
 *
 * \snippet ../../src/contacts/details/qcontactphonenumber.h 0
 *
 */

/*!
 * \fn QContactDetail::operator!=(const QContactDetail& other) const
 * Returns true if the values or id of this detail is different to those of the \a other detail
 */

/*!
 * \fn T QContactDetail::value(const QString& key) const
 * \overload
 * Returns the value of the template type associated with the given \a key
 */

/*!
 * Constructs a new, empty detail
 */
QContactDetail::QContactDetail()
    : d(new QContactDetailPrivate)
{
}

/*! Constructs a new, empty detail of the definition identified by \a thisDefinitionId */
QContactDetail::QContactDetail(const QString& thisDefinitionId)
    : d(new QContactDetailPrivate)
{
    d->m_definitionName = thisDefinitionId;
}

/*! Constructs a detail that is a copy of \a other */
QContactDetail::QContactDetail(const QContactDetail& other)
    : d(other.d)
{
}

/*! Constructs a detail that is a copy of \a other if \a other is of the expected definition identified by \a expectedDefinitionId, else constructs a new, empty detail of the definition identified by the \a expectedDefinitionId */
QContactDetail::QContactDetail(const QContactDetail& other, const QString& expectedDefinitionId)
{
    if (other.definitionName() == expectedDefinitionId) {
        d = other.d;
    } else {
        d = new QContactDetailPrivate;
        d->m_definitionName = expectedDefinitionId;
    }
}

/*! Assigns this detail to \a other */
QContactDetail& QContactDetail::operator=(const QContactDetail& other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

/*! Assigns this detail to \a other if the definition of \a other is that identified by the given \a expectedDefinitionId, else assigns this detail to be a new, empty detail of the definition identified by the given \a expectedDefinitionId */
QContactDetail& QContactDetail::assign(const QContactDetail& other, const QString& expectedDefinitionId)
{
    if (this != &other) {
        if (other.definitionName() == expectedDefinitionId) {
            d = other.d;
        } else {
            d = new QContactDetailPrivate;
            d->m_definitionName = expectedDefinitionId;
        }
    }
    return *this;
}

/*! Frees the memory used by this detail */
QContactDetail::~QContactDetail()
{
}

/*! Returns the (unique) name of the definition which defines the semantics and structure of this detail */
QString QContactDetail::definitionName() const
{
    return d.constData()->m_definitionName;
}

/*! Compares this detail to \a other.  Returns true if the definition and values of \a other are equal to those of this detail */
bool QContactDetail::operator==(const QContactDetail& other) const
{
    if (d.constData()->m_definitionName != other.d.constData()->m_definitionName)
        return false;

    if (d.constData()->m_values != other.d.constData()->m_values)
        return false;

    return true;
}

/*! Sets the preferred actions for this detail to be the given list of \a preferredActions */
void QContactDetail::setPreferredActions(const QList<QContactActionDescriptor>& preferredActions)
{
    d->m_preferredActions = preferredActions;
}

/*! Returns the list of preferred actions for this detail */
QList<QContactActionDescriptor> QContactDetail::preferredActions() const
{
    return d->m_preferredActions;
}

/*! Returns true if no values are contained in this detail.  Note that context is stored as a value; hence, if a context is set, this function will return false. */
bool QContactDetail::isEmpty() const
{
    if (!d.constData()->m_values.isEmpty())
        return false;
    return true;
}

/*! Returns the value stored in this detail for the given \a key as a QString, or an empty QString if no value for the given \a key exists */
QString QContactDetail::value(const QString& key) const
{
    if (d.constData()->m_values.contains(key))
        return d.constData()->m_values.value(key).toString();
    return QString();
}

/*! Returns the value stored in this detail for the given \a key as a QVariant, or an invalid QVariant if no value for the given \a key exists */
QVariant QContactDetail::variantValue(const QString& key) const
{
    if (d.constData()->m_values.contains(key))
        return d.constData()->m_values.value(key);
    return QVariant(); // returns an invalid qvariant
}

/*!
 * Returns true if this detail has a field with the given \a key, or false otherwise.
 */
bool QContactDetail::hasValue(const QString& key) const
{
    if (d.constData()->m_values.contains(key))
        return true;
    return false;
}

/*! Inserts \a value into the detail for the given \a key if \a value is valid.  If \a value is invalid,
    removes the field with the given \a key from the detail.  Returns true if the given \a value was set
    for the \a key (if the \a value was valid), or if the given \a key was removed from detail (if the
    \a value was invalid), and returns false if the key was unable to be removed (and the \a value was invalid) */
bool QContactDetail::setValue(const QString& key, const QVariant& value)
{
    if (!value.isValid())
        return removeValue(key);

    d->m_values.insert(key, value);
    return true;
}

/*! Removes the value stored in this detail for the given \a key.  Returns true if a value was stored for the given \a key and the operation succeeded, and false otherwise */
bool QContactDetail::removeValue(const QString& key)
{
    if(d->m_values.remove(key))
        return true;
    return false;
}

/*!
 * \deprecated
 * Returns the values stored in this detail
 */
QVariantMap Q_DECL_DEPRECATED QContactDetail::values() const
{
    qWarning("This function was deprecated in week 1 and will be removed after the transition period has elapsed!  Use variantValues() instead!");
    return d.constData()->m_values;
}

/*!
 * Returns the values stored in this detail as a map from value key to value
 */
QVariantMap QContactDetail::variantValues() const
{
    return d.constData()->m_values;
}

/*!
 * \fn void QContactDetail::setContexts(const QStringList& contexts)
 *
 * This is a convenience function that sets the \c Context field of this detail to the given \a contexts.
 *
 * It is equivalent to the following code:
 * \code
 * setValue(QContactDetail::FieldContext, contexts);
 * \endcode
 *
 * \sa setValue()
 */

/*!
 * \fn void QContactDetail::setContexts(const QString& context)
 *
 * This is a convenience function that sets the \c Context field of this detail to the given \a context.
 * It is useful if the detail is only valid in a single context.
 *
 * It is equivalent to the following code:
 * \code
 * setValue(FieldContext, QStringList(context));
 * \endcode
 *
 * \sa setValue()
 */

/*!
 * \fn QStringList QContactDetail::contexts() const
 *
 * This is a convenience function to return the \c Context field of this detail.
 *
 * It is equivalent to the following code:
 * \code
 * value(QContactDetail::FieldContext);
 * \endcode
 *
 * \sa value()
 */

QTM_END_NAMESPACE
