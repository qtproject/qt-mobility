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

#include "qorganizeritemdetail.h"
#include "qorganizeritemdetail_p.h"
#include "qorganizeritemmanager.h"
#include <QDebug>

QTM_BEGIN_NAMESPACE

/* Initialise our static private data member */
QAtomicInt QOrganizerItemDetailPrivate::lastDetailKey(1);

/* Storage */
QHash<QString, char*> QOrganizerItemStringHolder::s_allocated;
QHash<const char *, QString> QOrganizerItemStringHolder::s_qstrings;

/* Dtor function */
static int qClearAllocatedStringHash()
{
    QHash<QString, char*>::const_iterator it = QOrganizerItemStringHolder::s_allocated.constBegin();
    while (it != QOrganizerItemStringHolder::s_allocated.constEnd()) {
        delete[] it.value();
        it++;
    }
    QOrganizerItemStringHolder::s_allocated.clear();
    QOrganizerItemStringHolder::s_qstrings.clear();
    return 1;
}
Q_DESTRUCTOR_FUNCTION(qClearAllocatedStringHash);

/*!
  \class QOrganizerItemDetail

  \brief The QOrganizerItemDetail class represents a single, complete detail about a organizeritem.
  \inmodule QtOrganizer
  \ingroup organizer-main

  All of the information for a organizeritem is stored in one or more QOrganizerItemDetail objects.

  A detail is a group of logically related bits of data - for example, a street address is a single
  detail that has multiple fields (number, region, country etc).  Every QOrganizerItemDetail has the name of an
  associated QOrganizerItemDetailDefinition that describes the fields, their data type, and any
  restrictions on their values.  Different organizeritem managers might have different detail definitions
  for the same name, depending on their capabilities.  For example, for the QOrganizerItemGeoLocation definition name,
  one manager might not support the altitude field, while a different manager may add an extra field for
  specific extra information not present in the default schema.

  Both the names of all the fields, and the name of the associated QOrganizerItemDetailDefinition are stored
  as 8-bit strings encoded in Latin 1 for memory conservation.  Note, however, that the values stored
  in each field are not constrained in this way, and full unicode QStrings or QVariant data can be stored.

  When a QOrganizerItemDetail has been retrieved in a QOrganizerItem from a QOrganizerItemManager, it may have certain
  access constraints provided with it, like \l ReadOnly or \l Irremovable.  This might mean that the
  supplied detail is calculated or otherwise not modifiable by the user.
  Also, some details may be marked \l Irremovable.  These are typically things that
  a organizeritem has to have - like a QOrganizerItemType.

  It is possible to inherit from QOrganizerItemDetail to provide convenience or
  standardized access to values.  For example, \l QOrganizerEventTimeRange provides
  a convenient API for manipulating a QOrganizerItemDetail to describe the start and end time
  of an event, according to the schema.

  In general, QOrganizerItemDetail and the built in subclasses (like \l QOrganizerEventTimeRange) provide
  constants for the names of fields (like \l QOrganizerEventTimeRange::FieldStartDateTime).
  Typically the constants for field names start with \c Field, and the constants for predefined values
    of a field start with the name of that field
  (e.g. \c TypeEvent is a predefined constant for \c FieldType).

  If you wish to create your own, customized organizeritem detail, you should use
  the \l Q_DECLARE_CUSTOM_ORGANIZER_DETAIL macro in order to ensure proper
  operation, and declare your own field constants with \l Q_DECLARE_LATIN1_CONSTANT.
  See the predefined detail subclasses (like \l QOrganizerEventTimeRange,
  \l QOrganizerItemType) for more information.

  QOrganizerItemDetail objects act like type checked values.  In general, you can assign them
  to and fro and have reasonable behaviour, like the following example.

  \code

  QOrganizerItemDescription description;
  description.setDescription("Some descriptive text");
  // description.value(QOrganizerItemDescription::FieldDescription) == "Some descriptive text";
  // description.definitionName() == QOrganizerItemDescription::DefinitionName

  QOrganizerItemDetail detail = description;
  // detail.value(QOrganizerItemDescription::FieldDescription) == "Some descriptive text";
  // detail.definitionName() == QOrganizerItemDescription::DefinitionName

  QOrganizerItemDescription otherDescription = detail;
  // otherDescription.description() == "Some descriptive text";
  // otherDescription.definitionName() == QOrganizerItemDescription::DefinitionName

  QOrganizerItemDisplayLabel label = detail;
  // label is now a default constructed QOrganizerItemDisplayLabel
  // label.value(QOrganizerItemDescription::FieldDescription) is empty
  // label.definitionName() == QOrganizerItemDisplayLabel::DefinitionName

  QOrganizerItemDisplayLabel otherLabel = description;
  // otherLabel is now a default constructed QOrganizerItemDisplayLabel
  // otherLabel.value(QOrganizerItemDescription::FieldDescription) is empty
  // otherLabel.definitionName() == QOrganizerItemDisplayLabel::DefinitionName
  \endcode

  \sa QOrganizerItem, QOrganizerItemDetailDefinition, QOrganizerItemDetailFilter, QOrganizerItemDetailRangeFilter, Q_DECLARE_CUSTOM_ORGANIZER_DETAIL
 */

/*!
  \macro Q_DECLARE_CUSTOM_ORGANIZER_DETAIL
  \relates QOrganizerItemDetail

  Macro for simplifying declaring custom (leaf) detail classes.

  The first argument is the name of the class, and the second argument
  is a Latin-1 string literal naming the detail type.

  If you are creating a convenience class for a type of QOrganizerItemDetail,
  you should use this macro when declaring your class to ensure that
  it interoperates with other organizeritem functionality.

  Here is an example of a class (\l QOrganizerItemDescription) using this macro.
  Note that the class provides some predefined constants
  and some convenience methods that return values associated with schema
  fields.
 */


/*!
  \fn QOrganizerItemDetail::operator!=(const QOrganizerItemDetail& other) const
  Returns true if the values or id of this detail is different to those of the \a other detail
 */

/*!
  Constructs a new, empty detail
 */
QOrganizerItemDetail::QOrganizerItemDetail()
    : d(new QOrganizerItemDetailPrivate)
{
}

/*!
    Constructs a new, empty detail of the definition identified by \a thisDefinitionId.
    The definitionId must be restricted to the Latin 1 character set.
 */
QOrganizerItemDetail::QOrganizerItemDetail(const QString& thisDefinitionId)
    : d(new QOrganizerItemDetailPrivate)
{
    d->m_definitionName = thisDefinitionId;
}

/*!
    Constructs a new, empty detail of the definition identified by \a thisDefinitionId

    The supplied pointer must be valid for the lifetime of the program.  In general
    this means it should be a constant, and not allocated on the stack.  If you cannot
    meet this requirement, use the alternative constructor that takes a QString instead.
*/
QOrganizerItemDetail::QOrganizerItemDetail(const char* thisDefinitionId)
    : d(new QOrganizerItemDetailPrivate)
{
    d->m_definitionName = thisDefinitionId;
}

/*! Constructs a detail that is a copy of \a other */
QOrganizerItemDetail::QOrganizerItemDetail(const QOrganizerItemDetail& other)
    : d(other.d)
{
}

/*!
    \internal

    Constructs a detail that is a copy of \a other if \a other is of the expected definition
    identified by \a expectedDefinitionId, else constructs a new, empty detail of the
    definition identified by the \a expectedDefinitionId.

    The \a expectedDefinitionId pointer must be valid for the lifetime of the program.
*/
QOrganizerItemDetail::QOrganizerItemDetail(const QOrganizerItemDetail& other, const char* expectedDefinitionId)
{
    if (other.d->m_definitionName == expectedDefinitionId) {
        d = other.d;
    } else {
        d = new QOrganizerItemDetailPrivate;
        d->m_definitionName = expectedDefinitionId;
    }
}

/*!
    \internal

    Constructs a detail that is a copy of \a other if \a other is of the expected definition
    identified by \a expectedDefinitionId, else constructs a new, empty detail of the
    definition identified by the \a expectedDefinitionId
*/
QOrganizerItemDetail::QOrganizerItemDetail(const QOrganizerItemDetail& other, const QString& expectedDefinitionId)
{
    if (other.d->m_definitionName == expectedDefinitionId) {
        d = other.d;
    } else {
        d = new QOrganizerItemDetailPrivate;
        d->m_definitionName = expectedDefinitionId;
    }
}

/*! Assigns this detail to \a other */
QOrganizerItemDetail& QOrganizerItemDetail::operator=(const QOrganizerItemDetail& other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

/*!
    \internal

    Assigns this detail to \a other if the definition of \a other is that identified
    by the given \a expectedDefinitionId, else assigns this detail to be a new, empty
    detail of the definition identified by the given \a expectedDefinitionId
*/
QOrganizerItemDetail& QOrganizerItemDetail::assign(const QOrganizerItemDetail& other, const char* expectedDefinitionId)
{
    if (this != &other) {
        if (other.d->m_definitionName == expectedDefinitionId) {
            d = other.d;
        } else {
            d = new QOrganizerItemDetailPrivate;
            d->m_definitionName = expectedDefinitionId;
        }
    }
    return *this;
}

/*!
    \internal

    Assigns this detail to \a other if the definition of \a other is that identified
    by the given \a expectedDefinitionId, else assigns this detail to be a new, empty
    detail of the definition identified by the given \a expectedDefinitionId
*/
QOrganizerItemDetail& QOrganizerItemDetail::assign(const QOrganizerItemDetail& other, const QString& expectedDefinitionId)
{
    if (this != &other) {
        if (other.d->m_definitionName == expectedDefinitionId) {
            d = other.d;
        } else {
            d = new QOrganizerItemDetailPrivate;
            d->m_definitionName = expectedDefinitionId;
        }
    }
    return *this;
}

/*! Frees the memory used by this detail */
QOrganizerItemDetail::~QOrganizerItemDetail()
{
}

/*!
    Returns the (unique) name of the definition which defines the semantics and structure of this detail.
    The actual QOrganizerItemDetailDefinition should be retrieved from the relevant QOrganizerItemManager using this name.
 */
QString QOrganizerItemDetail::definitionName() const
{
    return d.constData()->m_definitionName.toQString();
}

/*!
    Compares this detail to \a other.  Returns true if the definition, access constraints and values of \a other are equal to those of this detail.
    The keys of each detail are not considered during the comparison, in order to allow details from different organizeritems to
    be compared according to their values.
 */
bool QOrganizerItemDetail::operator==(const QOrganizerItemDetail& other) const
{
    if (! (d.constData()->m_definitionName == other.d.constData()->m_definitionName))
        return false;

    if (d.constData()->m_access != other.d.constData()->m_access)
        return false;

    if (d.constData()->m_values != other.d.constData()->m_values)
        return false;

    return true;
}

uint qHash(const QOrganizerItemStringHolder& key)
{
    if (!key.m_str)
        return 0;
    uint h = 0;
    uint g;
    const register uchar*p = (const uchar*)key.m_str;

    while (*p) {
        h = (h << 4) + *p++;
        if ((g = (h & 0xf0000000)) != 0)
            h ^= g >> 23;
        h &= ~g;
    }
    return h;
}

/*! Returns the hash value for \a key. */
uint qHash(const QOrganizerItemDetail &key)
{
    const QOrganizerItemDetailPrivate* dptr= QOrganizerItemDetailPrivate::detailPrivate(key);
    uint hash = qHash(dptr->m_definitionName)
                + QT_PREPEND_NAMESPACE(qHash)(dptr->m_access);
    QHash<QOrganizerItemStringHolder, QVariant>::const_iterator it = dptr->m_values.constBegin();
    while(it != dptr->m_values.constEnd()) {
        hash += qHash(it.key())
                + QT_PREPEND_NAMESPACE(qHash)(it.value().toString());
        ++it;
    }
    return hash;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QOrganizerItemDetail& detail)
{
    dbg.nospace() << "QOrganizerItemDetail(name=" << detail.definitionName() << ", key=" << detail.key();
    QVariantMap fields = detail.variantValues();
    QVariantMap::const_iterator it;
    for (it = fields.constBegin(); it != fields.constEnd(); ++it) {
        dbg.nospace() << ", " << it.key() << '=' << it.value();
    }
    dbg.nospace() << ')';
    return dbg.maybeSpace();
}
#endif

/*!
    Returns true if no values are contained in this detail.  Note that context is stored as a value; hence, if a context is set, this function will return false.
 */
bool QOrganizerItemDetail::isEmpty() const
{
    if (!d.constData()->m_values.isEmpty())
        return false;
    return true;
}

/*! Returns the key of this detail. */
int QOrganizerItemDetail::key() const
{
    return d->m_id;
}

/*! Causes the implicitly-shared detail to be detached from any other copies, and generates a new key for it.
    This ensures that calling QOrganizerItem::saveDetail() will result in a new detail being saved, rather than
    another detail being updated. */
void QOrganizerItemDetail::resetKey()
{
    d->m_id = QOrganizerItemDetailPrivate::lastDetailKey.fetchAndAddOrdered(1);
}

/*! \overload
  Returns the value stored in this detail for the given \a key as a QString, or an empty QString if
  no value for the given \a key exists */
QString QOrganizerItemDetail::value(const QString& key) const
{
    return d.constData()->m_values.value(QOrganizerItemStringHolder(key)).toString();
}

/*!
    \internal
    \overload
    Returns the value stored in this detail for the given \a key as a QString, or an empty QString if
    no value for the given \a key exists
*/
QString QOrganizerItemDetail::value(const char* key) const
{
    return d.constData()->m_values.value(key).toString();
}

/*!
    \fn T QOrganizerItemDetail::value(const char* key) const
    \internal
    \overload
*/

/*! Returns the value stored in this detail for the given \a key as a QVariant, or an invalid QVariant if no value for the given \a key exists */
QVariant QOrganizerItemDetail::variantValue(const QString& key) const
{
    return d.constData()->m_values.value(QOrganizerItemStringHolder(key));
}

/*!
    \internal
    Returns the value stored in this detail for the given \a key as a QVariant, or an invalid QVariant if no value for the given \a key exists
 */
QVariant QOrganizerItemDetail::variantValue(const char* key) const
{
    return d.constData()->m_values.value(key);
}

/*!
  Returns true if this detail has a field with the given \a key, or false otherwise.
 */
bool QOrganizerItemDetail::hasValue(const QString& key) const
{
    return d.constData()->m_values.contains(QOrganizerItemStringHolder(key));
}

/*!
  \internal
  Returns true if this detail has a field with the given \a key, or false otherwise.
 */
bool QOrganizerItemDetail::hasValue(const char * key) const
{
    return d.constData()->m_values.contains(key);
}

/*! Inserts \a value into the detail for the given \a key if \a value is valid.  If \a value is invalid,
    removes the field with the given \a key from the detail.  Returns true if the given \a value was set
    for the \a key (if the \a value was valid), or if the given \a key was removed from detail (if the
    \a value was invalid), and returns false if the key was unable to be removed (and the \a value was invalid) */
bool QOrganizerItemDetail::setValue(const QString& key, const QVariant& value)
{
    if (!value.isValid())
        return removeValue(key);

    d->m_values.insert(QOrganizerItemStringHolder(key), value);
    return true;
}

/*!
    \internal

    Inserts \a value into the detail for the given \a key if \a value is valid.  If \a value is invalid,
    removes the field with the given \a key from the detail.  Returns true if the given \a value was set
    for the \a key (if the \a value was valid), or if the given \a key was removed from detail (if the
    \a value was invalid), and returns false if the key was unable to be removed (and the \a value was invalid)
*/
bool QOrganizerItemDetail::setValue(const char* key, const QVariant& value)
{
    if (!value.isValid())
        return removeValue(key);

    d->m_values.insert(key, value);
    return true;
}

/*!
    Removes the value stored in this detail for the given \a key.  Returns true if a value was stored
    for the given \a key and the operation succeeded, and false otherwise.
*/
bool QOrganizerItemDetail::removeValue(const QString& key)
{
    if(d->m_values.remove(QOrganizerItemStringHolder(key)))
        return true;
    return false;
}

/*!
    \internal
    Removes the value stored in this detail for the given \a key.  Returns true if a value was stored
    for the given \a key and the operation succeeded, and false otherwise.
*/
bool QOrganizerItemDetail::removeValue(const char * key)
{
    if(d->m_values.remove(key))
        return true;
    return false;
}

/*!
  Returns the values stored in this detail as a map from value key to value
 */
QVariantMap QOrganizerItemDetail::variantValues() const
{
    QVariantMap ret;
    QHash<QOrganizerItemStringHolder, QVariant>::const_iterator it = d.constData()->m_values.constBegin();
    while(it != d.constData()->m_values.constEnd()) {
        ret.insert(it.key().toQString(), it.value());
        ++it;
    }

    return ret;
}


/*!
    \fn bool QOrganizerItemDetail::setValue(const QLatin1Constant& key, const QVariant& value)

    Inserts \a value into the detail for the given \a key if \a value is valid.  If \a value is invalid,
    removes the field with the given \a key from the detail.  Returns true if the given \a value was set
    for the \a key (if the \a value was valid), or if the given \a key was removed from detail (if the
    \a value was invalid), and returns false if the key was unable to be removed (and the \a value was invalid)
*/
/*!
    \fn bool QOrganizerItemDetail::removeValue(const QLatin1Constant& key)

    Removes the value stored in this detail for the given \a key.  Returns true if a value was stored
    for the given \a key and the operation succeeded, and false otherwise.
*/

/*!
    \fn bool QOrganizerItemDetail::hasValue(const QLatin1Constant& key) const
    Returns true if this detail has a field with the given \a key, or false otherwise.
 */

/*!
    \fn QVariant QOrganizerItemDetail::variantValue(const QLatin1Constant& key) const
    Returns the value stored in this detail for the given \a key as a QVariant, or an invalid QVariant if no value for the given \a key exists
 */

/*!
    \fn T QOrganizerItemDetail::value(const QLatin1Constant& key) const
    \overload
    Returns the value of the template type associated with the given \a key
 */
/*!
    \fn QString QOrganizerItemDetail::value(const QLatin1Constant& key) const
    Returns the value stored in this detail for the given \a key as a QString, or an empty QString if
    no value for the given \a key exists
*/
/*!
    \fn T QOrganizerItemDetail::value(const QString& key) const
    Returns the value of the template type associated with the given \a key
 */

/*!
  \enum QOrganizerItemDetail::AccessConstraint

  This enum defines the access constraints for a detail.  This information is typically provided by
  the manager when a organizeritem is retrieved.

  \value NoConstraint Users can read, write, and otherwise modify this detail in any manner.
  \value ReadOnly Users cannot write or modify values in this detail.
  \value Irremovable Users cannot remove this detail from a organizeritem.
 */


/*!
  Returns the access constraints associated with the detail.

  Some details may not be written to, while other details may
  not be removed from a organizeritem.

  \sa QOrganizerItemDetail::AccessConstraints
 */
QOrganizerItemDetail::AccessConstraints QOrganizerItemDetail::accessConstraints() const
{
    return d.constData()->m_access;
}


QTM_END_NAMESPACE
