/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qversitproperty.h"
#include "qversitproperty_p.h"
#include "qmobilityglobal.h"

#include <QStringList>
#include <QTextCodec>

QTM_BEGIN_NAMESPACE

/*!
  \class QVersitProperty
  \brief The QVersitProperty class stores the name, value, groups and parameters of a Versit property.
  \ingroup versit

  A vCard is represented in abstract form as a QVersitDocument that consists of a number of
  properties such as a name (N), a telephone number (TEL) and an email address (EMAIL), for
  instance.  Each of these properties is stored as an instance of a QVersitProperty in a
  QVersitDocument.

  A QVersitProperty consists of a list of groups, a name, a list of parameters (key/value pairs),
  and a value.

  The value of a QVersitProperty is stored as a QVariant and should always be one of four types:
  QString for textual values, QByteArray for binary data (eg. images), QStringList for structured
  textual data, or QVersitDocument for nested documents.  The \l QVersitReader will parse Versit
  properties and assign the correct type of object to the property value.  The \l QVersitWriter will
  serialize objects of these types correctly into the (text-based) Versit format.

  For example, a property might appear in a vCard as:
  \code
  ADR;TYPE=home,postal:;;123 Main Street;Any Town;CA;91921-1234
  \endcode
  This would be stored as a QVersitProperty with the name \tt{ADR} and two parameters (both named
  \tt{TYPE} and with values \tt{home} and \tt{postal} respectively.  The value of the
  QVersitProperty is a QStringList with six strings, and the valueType is CompoundType.

  QVersitProperty supports implicit sharing.  The property name and parameter names of a
  QVersitProperty are converted to upper-case when they are stored to a QVersitProperty.

  \sa QVersitDocument
 */

/*!
  \enum QVersitProperty::ValueType
  Describes the type of data held in the property's value.

  The vCard and iCalendar specifications allows a property value to hold a string, binary data, or a
  nested document.  String values can either be unstructured or structured.  Structured strings can
  be either of compound type or list type.  A compound value is one that is delimited by semicolons,
  allows empty components, and has a property-specific cardinality and ordering.  A list value is
  one that is delimited by commas, does not have empty components, and has no restrictions on
  cardinality or ordering.

  \value PlainType The property value holds an unstructured string and can be retrieved with
  QVersitProperty::value()
  \value CompoundType The property value holds a compound string and can be retrieved with
  QVersitProperty::value<QStringList>()
  \value ListType The property value holds a list of strings and can be retrieved with
  QVersitProperty::value<QStringList>()
  \value BinaryType The property value holds a binary value and can be retrieved with
  QVersitProperty::value<QByteArray>()
  \value VersitDocumentType The property value holds a nested Versit document and can be retrieved
  with QVersitProperty::value<QVersitDocument>()
 */

/*! Constructs a new empty property */
QVersitProperty::QVersitProperty() : d(new QVersitPropertyPrivate())
{
}

/*! Constructs a property that is a copy of \a other */
QVersitProperty::QVersitProperty(const QVersitProperty& other) : d(other.d)
{
}

/*! Frees the memory used by the property */
QVersitProperty::~QVersitProperty()
{
}

/*! Assigns this property to \a other */
QVersitProperty& QVersitProperty::operator=(const QVersitProperty& other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

/*! Returns true if this is equal to \a other; false otherwise. */
bool QVersitProperty::operator==(const QVersitProperty& other) const
{
    bool equal = d->mGroups == other.d->mGroups &&
            d->mName == other.d->mName &&
            d->mParameters == other.d->mParameters &&
            d->mValueType == other.d->mValueType;
    if (!equal)
        return false;

    // QVariant doesn't support == on QVersitDocuments - do it manually
    if (d->mValue.userType() == qMetaTypeId<QVersitDocument>())
        return  other.d->mValue.userType() == qMetaTypeId<QVersitDocument>()
            && d->mValue.value<QVersitDocument>() == other.d->mValue.value<QVersitDocument>();
    else
        return d->mValue == other.d->mValue;
}

/*! Returns true if this is not equal to \a other; false otherwise. */
bool QVersitProperty::operator!=(const QVersitProperty& other) const
{
    return !(*this == other);
}

/*! Returns the hash value for \a key. */
uint qHash(const QVersitProperty &key)
{
    uint hash = QT_PREPEND_NAMESPACE(qHash)(key.name()) + QT_PREPEND_NAMESPACE(qHash)(key.value());
    foreach (const QString& group, key.groups()) {
        hash += QT_PREPEND_NAMESPACE(qHash)(group);
    }
    QHash<QString,QString>::const_iterator it = key.parameters().constBegin();
    QHash<QString,QString>::const_iterator end = key.parameters().constEnd();
    while (it != end) {
        hash += QT_PREPEND_NAMESPACE(qHash)(it.key()) + QT_PREPEND_NAMESPACE(qHash)(it.value());
        ++it;
    }
    return hash;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QVersitProperty& property)
{
    QStringList groups = property.groups();
    QString name = property.name();
    QMultiHash<QString,QString> parameters = property.parameters();
    dbg.nospace() << "QVersitProperty(";
    foreach (const QString& group, groups) {
        dbg.nospace() << group << '.';
    }
    dbg.nospace() << name;
    QHash<QString,QString>::const_iterator it;
    for (it = parameters.constBegin(); it != parameters.constEnd(); ++it) {
        dbg.nospace() << ';' << it.key() << '=' << it.value();
    }
    dbg.nospace() << ':' << property.variantValue();
    dbg.nospace() << ')';
    return dbg.maybeSpace();
}
#endif

/*!
 * Sets the groups in the property to the given list of \a groups.
 */
void QVersitProperty::setGroups(const QStringList& groups)
{
    d->mGroups.clear();
    foreach (const QString& group, groups) {
        d->mGroups.append(group);
    }
}

/*!
 * Gets the groups of the property.
 */
QStringList QVersitProperty::groups() const
{
    return d->mGroups;
}

/*!
 * Sets the \a name of the property.
 * The \a name is converted to upper-case.
 */
void QVersitProperty::setName(const QString& name)
{
    d->mName = name.toUpper();
}

/*!
 * Gets the name of the property in upper-case.
 */
QString QVersitProperty::name() const
{
    return d->mName;
}

/*!
 * Replaces all the parameters with \a parameters.
 * The parameters are converted to upper-case.
 */
void QVersitProperty::setParameters(const QMultiHash<QString,QString>& parameters)
{
    d->mParameters.clear();
    // Traverse parameters in the reverse order, because they are added to
    // d->mParameters using insert in QVersitProperty::addParameter
    QList<QString> keys = parameters.uniqueKeys();
    for (int i=keys.count()-1; i >= 0; i--) {
        QString key = keys.at(i);
        QList<QString> values = parameters.values(key);
        for (int j=values.count()-1; j >= 0; j--) {
            // Convert all the parameter names and values to upper case
            insertParameter(key,values.at(j));
        }
    }
}

/*!
 * Adds a new parameter with \a name and \a value.
 * Both the name and the value are converted to upper-case.
 */
void QVersitProperty::insertParameter(const QString& name, const QString& value)
{
    d->mParameters.insert(name.toUpper(), value);
}

/*!
 * Removes a parameter with \a name and \a value.
 *
 * \sa removeParameters()
 */
void QVersitProperty::removeParameter(const QString& name, const QString& value)
{
    d->mParameters.remove(name.toUpper(), value);
}

/*!
 * Removes all parameters with the given \a name.
 *
 * \sa removeParameter()
 */
void QVersitProperty::removeParameters(const QString& name)
{
    d->mParameters.remove(name.toUpper());
}

/*!
 * Return a copy of the contained list of parameters.
 * Note that actual the parameters cannot be modified using the copy.
 */
QMultiHash<QString,QString> QVersitProperty::parameters() const
{
    return d->mParameters;
}

/*!
 * Sets the property value to \a value.
 */
void QVersitProperty::setValue(const QVariant& value)
{
    d->mValue = value;
}

/*!
 * Returns the value of the property.
 */
QVariant QVersitProperty::variantValue() const
{
    return d->mValue;
}

/*!
 * \fn T QVersitProperty::value() const
 * \overload
 * Returns the value of the property as a \tt T.
 */

/*!
 * Returns the value of the property as a string if possible, otherwise return an empty string.
 * If the property is stored as a QByteArray, it is decoded using the charset specified in the
 * property's parameters.
 * \sa QVariant::toString()
 */
QString QVersitProperty::value() const
{
    if (d->mValue.type() == QVariant::ByteArray) {
        if (d->mParameters.contains(QLatin1String("CHARSET"))) {
            QTextCodec* codec = QTextCodec::codecForName(
                    d->mParameters.value(QLatin1String("CHARSET")).toAscii());
            if (codec != NULL) {
                return codec->toUnicode(d->mValue.toByteArray());
            }
        }
        return QString();
    } else {
        return d->mValue.toString();
    }
}

/*!
 * Sets the type of value held in the property to \a type.
 */
void QVersitProperty::setValueType(QVersitProperty::ValueType type)
{
    d->mValueType = type;
}

/*!
 * Returns the type of value held in the property.
 */
QVersitProperty::ValueType QVersitProperty::valueType() const
{
    return d->mValueType;
}

/*!
 * Returns true if the property is empty.
 */
bool QVersitProperty::isEmpty() const
{
    return d->mGroups.isEmpty()
            && d->mName.isEmpty()
            && d->mParameters.isEmpty()
            && !d->mValue.isValid();
}

/*!
 * Clears the contents of this property.
 */
void QVersitProperty::clear()
{
    d->mGroups.clear();
    d->mName.clear();
    d->mValue.clear();
    d->mParameters.clear();
}

QTM_END_NAMESPACE
