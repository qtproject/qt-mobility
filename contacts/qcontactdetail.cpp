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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactdetail.h"
#include "qcontactdetail_p.h"
#include "qcontactmanager.h"

/* Definitions of predefined string constants */
Q_DEFINE_LATIN1_LITERAL(QContactDetail::AttributeContext, "Context");
Q_DEFINE_LATIN1_LITERAL(QContactDetail::AttributeSubType, "SubType");
Q_DEFINE_LATIN1_LITERAL(QContactDetail::AttributeContextOther, "Other");
Q_DEFINE_LATIN1_LITERAL(QContactDetail::AttributeContextHome, "Home");
Q_DEFINE_LATIN1_LITERAL(QContactDetail::AttributeContextWork, "Work");

/*!
 * \class QContactDetail
 *
 * \brief The QContactDetail class provides access to a single, complete detail about a contact.
 *
 * All of the information for a contact is stored in one or more QContactDetail objects.
 *
 * A detail is a group of logically related bits of data - for example, a street address is a single
 * detail that has multiple fields (number, region, country etc).  Every QContactDetail has an
 * associated QContactDetailDefinition id that describes the fields, their data type, any
 * restrictions on their values, and any restrictions on creating or updating details of that
 * definition.
 *
 * A detail might also has some metadata, stored in "attributes".  What these attributes
 * are used for varies a little depending on the detail, but two common attributes are
 * \c Context and \c SubType.
 *
 * \list
 * \o The \c Context attribute (with id \l AttributeContext) is intended to store the
 * context that this detail is associated with.  Commonly this will be something like
 * "Home" or "Work".
 *
 * \o The \c SubType attribute (with id \l AttributeSubType) is intended to store any
 * particular distinctions between details of this type.  For example, a phone number
 * might have a SubType of "Mobile" or "Fax".
 * \endlist
 *
 * In general, a given attribute has a single value.  In some cases, if you wish to
 * associate more than one value, you need to comma separate the values.
 *
 * It is possible to inherit from QContactDetail to provide convenience or
 * standardized access to values.  For example, \l QContactPhoneNumber provides
 * a convenient API for manipulating a QContactDetail as a phone number, according
 * to the schema.
 *
 * If you wish to create your own, customized contact detail, you should use
 * the \l Q_DECLARE_CUSTOM_CONTACT_DETAIL macro in order to ensure proper
 * operation.  See the predefined leaf classes (like \l QContactPhoneNumber,
 * \l QContactAddress) for more information.
 *
 * QContactDetail objects act like values.  In general, you can assign them
 * to and fro and have reasonable behaviour, like the following example.
 *
 * \code
 *
 * QContactPhoneNumber number;
 * number.setNumber("555-1212");
 * // number.value(QContactPhoneNumber::FieldNumber) == "555-1212";
 * // number.definitionName() == QContactPhoneNumber::staticType()
 *
 * QContactDetail detail = number;
 * // detail.value(QContactPhoneNumber::FieldNumber) == "555-1212";
 * // detail.definitionName() == QContactPhoneNumber::staticType()
 *
 * QContactPhoneNumber otherNumber = detail;
 * // otherNumber.number() == "555-1212";
 * // otherNumber.definitionName() == QContactPhoneNumber::staticType()
 *
 * QContactAddress address = detail;
 * // address is now a default constructed QContactAddress
 * // address.error() == QContactDetail::IncompatibleAssignmentError
 * // address.value(QContactPhoneNumber::FieldNumber) is empty
 * // address.definitionName() == QContactAddress::staticType()
 *
 * QContactAddress otherAddress = number;
 * // otherAddress is now a default constructed QContactAddress
 * // otherAddress.error() == QContactDetail::IncompatibleAssignmentError
 * // otherAddress.value(QContactPhoneNumber::FieldNumber) is empty
 * // otherAddress.definitionName() == QContactAddress::staticType()
 * \endcode
 *
 * \sa QContact, QContactDetailDefinition, Q_DECLARE_CUSTOM_CONTACT_DETAIL
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
 * Note that the class provides some predefined constants for attributes,
 * and some convenience methods that return values associated with schema
 * fields.
 *
 * \snippet contacts/qcontactphonenumber.h 0
 *
 */

/*!
 * \fn QContactDetail::operator!=(const QContactDetail& other) const
 * Returns true if the values, attributes or id of this detail is different to those of the \a other detail
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
    QContactDetailPrivate::setError(d, QContactDetail::NoError);
}

/*! Constructs a detail that is a copy of \a other if \a other is of the expected definition identified by \a expectedDefinitionId, else constructs a new, empty detail of the definition identified by the \a expectedDefinitionId */
QContactDetail::QContactDetail(const QContactDetail& other, const QString& expectedDefinitionId)
{
    if (other.definitionName() == expectedDefinitionId) {
        d = other.d;
        QContactDetailPrivate::setError(d, QContactDetail::NoError);
    } else {
        d = new QContactDetailPrivate;
        d->m_definitionName = expectedDefinitionId;
        d->m_error = QContactDetail::IncompatibleAssignmentError;
    }
}

/*! Assigns this detail to \a other */
QContactDetail& QContactDetail::operator=(const QContactDetail& other)
{
    if (this != &other) {
        d = other.d;
        QContactDetailPrivate::setError(d, QContactDetail::NoError);
    }
    return *this;
}

/*! Assigns this detail to \a other if the definition of \a other is that identified by the given \a expectedDefinitionId, else assigns this detail to be a new, empty detail of the definition identified by the given \a expectedDefinitionId */
QContactDetail& QContactDetail::assign(const QContactDetail& other, const QString& expectedDefinitionId)
{
    if (this != &other) {
        if (other.definitionName() == expectedDefinitionId) {
            d = other.d;
            QContactDetailPrivate::setError(d, QContactDetail::NoError);
        } else {
            d = new QContactDetailPrivate;
            d->m_definitionName = expectedDefinitionId;
            d->m_error = QContactDetail::IncompatibleAssignmentError;
        }
    }
    return *this;
}

/*! Frees the memory used by this detail */
QContactDetail::~QContactDetail()
{
}

/*!
 * \enum QContactDetail::Error
 *
 * This enum specifies an error that occurred during the most recent operation:
 *
 * \value NoError The most recent operation was successful
 * \value MissingValueError The most recent operation failed because the value the caller wished to retrieve was missing
 * \value IncompatibleAssignmentError The most recent operation failed because the detail is of a different definition than the one you are attempting to assign it to
 * \value OutOfMemoryError The most recent operation failed due to running out of memory
 */

/*! Returns the error code of the error resulting from the most recent state-modifying operation */
QContactDetail::Error QContactDetail::error() const
{
    return d.constData()->m_error;
}

/*! Returns the (unique) name of the definition which defines the semantics and structure of this detail */
QString QContactDetail::definitionName() const
{
    return d.constData()->m_definitionName;
}

/*! Compares this detail to \a other.  Returns true if the definition, attributes and values of \a other are equal to those of this detail */
bool QContactDetail::operator==(const QContactDetail& other) const
{
    if (d.constData()->m_definitionName != other.d.constData()->m_definitionName)
        return false;

    if (d.constData()->m_attributes != other.d.constData()->m_attributes)
        return false;

    if (d.constData()->m_values != other.d.constData()->m_values)
        return false;

    return true;
}

/*! Sets the attributes of this detail to \a attributes.  Returns true if the operation succeeded. */
bool QContactDetail::setAttributes(const QMap<QString, QString>& attributes)
{
    QContactDetailPrivate::setError(d, QContactDetail::NoError);
    d->m_attributes = attributes;
    return true;
}

/*! Returns the value of the attribute identified by the key \a attribute.  Detail attributes may include things like the context for which this detail is valid, detail type metadata, or user defined metadata */
QString QContactDetail::attribute(const QString& attribute) const
{
    return d.constData()->m_attributes.value(attribute);
}

/*! Sets the value of the attribute identified by the key \a attribute to be the given \a value.  The value associated with any key in the attribute map may be a comma-separated list of valid values, or user-defined values.  Returns true if the attribute was set successfully, otherwise returns false */
bool QContactDetail::setAttribute(const QString& attribute, const QString& value)
{
    QContactDetailPrivate::setError(d, QContactDetail::NoError);
    d->m_attributes.insert(attribute, value);
    return true;
}

/*! Removes the attribute identified by the key \a attribute from the detail's list of attributes.  Returns true if the attribute was removed successfully, otherwise returns false */
bool QContactDetail::removeAttribute(const QString& attribute)
{
    QContactDetailPrivate::setError(d, QContactDetail::NoError);
    d->m_attributes.remove(attribute);
    return true;
}

/*! Returns the attributes of this detail */
QMap<QString, QString> QContactDetail::attributes() const
{
    return d.constData()->m_attributes;
}

/*! Returns true if no values are contained in this detail.  Note: even if attributes have been set on this detail, this function will return true if no values are contained in this detail */
bool QContactDetail::isEmpty() const
{
    if (!d.constData()->m_values.isEmpty())
        return false;

    /* XXX should we check attributes? */
    return true;
}

/*! Returns the value stored in this detail for the given \a key as a QString, or an empty QString if no value for the given \a key exists */
QString QContactDetail::value(const QString& key) const
{
    if (d.constData()->m_values.contains(key)) {
        QContactDetailPrivate::setError(d, QContactDetail::NoError);
        return d.constData()->m_values.value(key).toString();
    } else {
        QContactDetailPrivate::setError(d, QContactDetail::MissingValueError);
        return QString();
    }
}

/*! Returns the value stored in this detail for the given \a key as a QVariant, or an invalid QVariant if no value for the given \a key exists */
QVariant QContactDetail::variantValue(const QString& key) const
{
    if (d.constData()->m_values.contains(key)) {
        QContactDetailPrivate::setError(d, QContactDetail::NoError);
        return d.constData()->m_values.value(key);
    } else {
        QContactDetailPrivate::setError(d, QContactDetail::MissingValueError);
        return QVariant();
    }
}

/*!
 * Returns true if this detail has a field with the given \a key, or false otherwise.
 */
bool QContactDetail::hasValue(const QString& key) const
{
    if (d.constData()->m_values.contains(key)) {
        QContactDetailPrivate::setError(d, QContactDetail::NoError);
        return true;
    } else {
        QContactDetailPrivate::setError(d, QContactDetail::MissingValueError);
        return false;
    }
}

/*! Inserts \a value into the detail for the given \a key.  Returns true if the operation succeeded */
bool QContactDetail::setValue(const QString& key, const QVariant& value)
{
    QContactDetailPrivate::setError(d, QContactDetail::NoError);
    d->m_values.insert(key, value);
    return true;
}

/*! Removes the value stored in this detail for the given \a key.  Returns true if a value was stored for the given \a key and the operation succeeded, and false otherwise */
bool QContactDetail::removeValue(const QString& key)
{
    if(d->m_values.remove(key)) {
        QContactDetailPrivate::setError(d, QContactDetail::NoError);
        return true;
    } else {
        QContactDetailPrivate::setError(d, QContactDetail::MissingValueError);
        return false;
    }
}

/*! Returns the values stored in this detail */
QVariantMap QContactDetail::values() const
{
    return d.constData()->m_values;
}

/*! Sets the values stored in this detail to \a values.  Returns true if the operation succeeded, otherwise false */
bool QContactDetail::setValues(const QVariantMap& values)
{
    QContactDetailPrivate::setError(d, QContactDetail::NoError);
    d->m_values = values;
    return true;
}

/*!
 * \fn void QContactDetail::setSubTypeAttribute(const QString& subType)
 *
 * This is a convenience function that sets the \c SubType attribute of this detail to \a subType.
 *
 * Not all details have meaningful values for the \c SubType attribute - refer to either the
 * schema or specific classes like \l QContactPhoneNumber for more information.
 *
 * It is equivalent to the following code:
 * \code
 * setAttribute(QContactDetail::AttributeSubType, subType);
 * \endcode
 */

/*!
 * \fn QString QContactDetail::subTypeAttribute() const
 *
 * This is a convenience function to return the \c SubType attribute of this detail.
 *
 * Not all details have meaningful values for the \c SubType attribute - refer to either the
 * schema or specific classes like \l QContactPhoneNumber for more information.
 *
 * It is equivalent to the following code:
 * \code
 * attribute(QContactDetail::AttributeSubType);
 * \endcode
 */

/*!
 * \fn void QContactDetail::setContextAttribute(const QString& context)
 *
 * This is a convenience function that sets the \c Context attribute of this detail to \a context.
 *
 * Not all details have meaningful values for the \c Context attribute - refer to either the
 * schema or specific classes like \l QContactPhoneNumber for more information.
 *
 * It is equivalent to the following code:
 * \code
 * setAttribute(QContactDetail::AttributeContext, context);
 * \endcode
 */

/*!
 * \fn QString QContactDetail::contextAttribute() const
 *
 * This is a convenience function to return the \c Context attribute of this detail.
 *
 * It is equivalent to the following code:
 * \code
 * attribute(QContactDetail::AttributeContext);
 * \endcode
 */
