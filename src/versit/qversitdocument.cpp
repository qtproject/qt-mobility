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

#include "qversitdocument.h"
#include "qversitdocument_p.h"
#include "qmobilityglobal.h"

#include <QTextCodec>

QTM_BEGIN_NAMESPACE

/*!
  \class QVersitDocument
  \brief The QVersitDocument class is a container for a list of versit properties.
  \ingroup versit

  For example a vCard can be presented as a QVersitDocument that consists of a number of properties
  such as a name (N), a telephone number (TEL) and an email address (EMAIL) to name a few.
  Each of these properties is stored as an instance of a QVersitProperty in a QVersitDocument.

  QVersitDocument supports implicit sharing.

  \sa QVersitProperty
 */

/*!
  \enum QVersitDocument::VersitType
  This enum describes a versit document type and version.
  \value InvalidType No type specified or a document with an invalid type was parsed
  \value VCard21Type vCard version 2.1
  \value VCard30Type vCard version 3.0
 */

/*! Constructs a new empty document */
QVersitDocument::QVersitDocument() : d(new QVersitDocumentPrivate())
{
}

/*! Constructs a new empty document with the type set to \a type */
QVersitDocument::QVersitDocument(VersitType type) : d(new QVersitDocumentPrivate())
{
    d->mVersitType = type;
}


/*! Constructs a document that is a copy of \a other */
QVersitDocument::QVersitDocument(const QVersitDocument& other) : d(other.d)
{
}

/*! Frees the memory used by the document */
QVersitDocument::~QVersitDocument()
{
}

/*! Assigns this document to \a other */
QVersitDocument& QVersitDocument::operator=(const QVersitDocument& other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

/*! Returns true if this is equal to \a other; false otherwise. */
bool QVersitDocument::operator==(const QVersitDocument& other) const
{
    return d->mVersitType == other.d->mVersitType &&
            d->mProperties == other.d->mProperties;
}

/*! Returns true if this is not equal to \a other; false otherwise. */
bool QVersitDocument::operator!=(const QVersitDocument& other) const
{
    return !(*this == other);
}

/*! Returns the hash value for \a key. */
uint qHash(const QVersitDocument &key)
{
    int hash = QT_PREPEND_NAMESPACE(qHash)(key.type());
    foreach (const QVersitProperty& property, key.properties()) {
        hash += qHash(property);
    }
    return hash;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QVersitDocument& document)
{
    dbg.nospace() << "QVersitDocument(" << document.type() << ')';
    foreach (const QVersitProperty& property, document.properties()) {
        dbg.space() << '\n' << property;
    }
    return dbg.maybeSpace();
}
#endif

/*!
 * Sets the versit document type to \a type.  This determines the format in which the document is
 * to be serialized.
 */
void QVersitDocument::setType(VersitType type)
{
    d->mVersitType = type;
}

/*!
 * Gets the versit document type.
 */
QVersitDocument::VersitType QVersitDocument::type() const
{
    return d->mVersitType;
}

/*!
 * Sets the versit component type (eg. VCARD, VCALENDAR, VEVENT, etc.)
 */
void QVersitDocument::setComponentType(QString componentType)
{
    d->mComponentType = componentType;
}

/*!
 * Gets the versit component type
 */
QString QVersitDocument::componentType() const
{
    return d->mComponentType;
}

/*!
 * Add \a property to the list of contained versit properties.
 * The property is appended as the last property of the list.
 */
void QVersitDocument::addProperty(const QVersitProperty& property)
{
    d->mProperties.append(property);
}

/*!
 * Removes the property \a property from the versit document.
 */
void QVersitDocument::removeProperty(const QVersitProperty& property)
{
    d->mProperties.removeAll(property);
}

/*!
 * Removes all the properties with the given \a name from the versit document.
 */
void QVersitDocument::removeProperties(const QString& name)
{
    for (int i=d->mProperties.count()-1; i >=0; i--) {
        if (d->mProperties[i].name() == name) {
            d->mProperties.removeAt(i);
        }
    }
}

/*!
 * Clears the document, removing all properties and metadata
 * and resetting the codec to the default.
 */
void QVersitDocument::clear()
{
    d->mProperties.clear();
    d->mSubDocuments.clear();
    d->mVersitType = QVersitDocument::InvalidType;
}

/*!
 * Adds \a subdocument to the Versit document.
 */
void QVersitDocument::addSubDocument(const QVersitDocument& document)
{
    d->mSubDocuments.append(document);
}

/*!
 * Sets the list of subdocuments to \a documents.
 */
void QVersitDocument::setSubDocuments(const QList<QVersitDocument>& documents)
{
    d->mSubDocuments = documents;
}

/*!
 * Returns the list of subdocuments contained within this Versit document.
 */
QList<QVersitDocument> QVersitDocument::subDocuments() const
{
    return d->mSubDocuments;
}

/*!
 * Gets the list of the contained versit properties.
 * Note that the actual properties cannot be modified using the copy.
 */
QList<QVersitProperty> QVersitDocument::properties() const
{
    return d->mProperties;
}

/*!
 * Returns true if the document is empty.
 */
bool QVersitDocument::isEmpty() const
{
    return d->mProperties.isEmpty()
        && d->mSubDocuments.isEmpty()
        && d->mVersitType == QVersitDocument::InvalidType;
}

QTM_END_NAMESPACE
