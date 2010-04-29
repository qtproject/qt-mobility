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

#include "qcontactmanagerengine.h"
#include "qversitcontactimporter.h"
#include "qversitcontactimporter_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qmobilityglobal.h"

QTM_USE_NAMESPACE

/*!
  \class QVersitContactImporterPropertyHandler
  \brief The QVersitContactImporterPropertyHandler class is an interface for clients wishing to
  implement custom import behaviour for versit properties

  \ingroup versit

  \sa QVersitContactImporter
 */

/*!
 * \fn QVersitContactImporterPropertyHandler::~QVersitContactImporterPropertyHandler()
 * Frees any memory in use by this handler.
 */

/*!
 * \fn virtual bool QVersitContactImporterPropertyHandler::preProcessProperty(const QVersitDocument& document, const QVersitProperty& property, int contactIndex, QContact* contact) = 0;
 * Process \a property and update \a contact with the corresponding QContactDetail(s).
 * \a document provides the context within which the property was found.
 * \a contactIndex specifies the position that \a contact will take in the list returned by
 * \l QVersitContactImporter::importDocuments().
 *
 * Returns true if the property has been handled and requires no further processing, false
 * otherwise.
 *
 * This function is called on every QVersitProperty encountered during an import.  Supply this
 * function and return true to implement custom import behaviour.
 */

/*!
 * \fn virtual bool QVersitContactImporterPropertyHandler::postProcessProperty(const QVersitDocument& document, const QVersitProperty& property, bool alreadyProcessed, int contactIndex, QContact* contact) = 0;
 * Process \a property and update \a contact with the corresponding QContactDetail(s).
 * \a document provides the context within which the property was found.
 * \a contactIndex specifies the position that \a contact will take in the list returned by
 * \l QVersitContactImporter::importDocuments().
 * \a alreadyProcessed is true if the detail has already been processed either by
 * \l preProcessProperty() or by QVersitContactImporter itself.
 *
 * Returns true if the property has been handled, false otherwise.
 *
 * This function is called on every QVersitProperty encountered during an import.  This can be
 * used to implement support for QVersitProperties not supported by QVersitContactImporter.
 */

/*!
 * \class QVersitContactImporter
 * \brief The QVersitContactImporter class creates QContacts from QVersitDocuments.
 *
 * \ingroup versit
 *
 * A \l QVersitResourceHandler is associated with the importer to supply the behaviour for saving
 * files to persistent storage.  By default, this is set to a \l QVersitDefaultResourceHandler,
 * which does not save files to persistent storage.  Note that although avatars found in vCards
 * are not saved to disk by default, the importer does set the pixmap of the contact detail to the
 * image.  If a full-sized avatar image needs to be persisted, a custom QVersitResourceHandler
 * should be supplied which implements this.
 *
 * By associating a QVersitContactImporterPropertyHandler with the importer using
 * setPropertyHandler(), the client can pass in a handler to override the processing of properties
 * and/or handle properties that QVersitContactImporter doesn't support.
 *
 * An example property handler that logs unknown properties:
 * \snippet ../../doc/src/snippets/qtversitdocsample/qtversitdocsample.cpp Property handler
 *
 * An example usage of QVersitContactImporter
 * \snippet ../../doc/src/snippets/qtversitdocsample/qtversitdocsample.cpp Import example
 *
 * \section1 Importing categories
 * The importer imports the vCard CATEGORIES property by converting each category to a QContactTag.
 * Some managers may not have support for QContactTag, but instead support categorization using the
 * \l{QContactRelationship::HasMember}{HasMember} QContactRelationship along with contacts of type
 * \l{QContactType::TypeGroup}{TypeGroup}.  For these backends, if the categorization information
 * needs to be retained through group relationships, extra work needs to be done to do the
 * conversion.  Below is some example code that does this translation.
 *
 * \snippet ../../doc/src/snippets/qtversitdocsample/qtversitdocsample.cpp Import relationship example
 *
 * \sa QVersitDocument, QVersitReader, QVersitContactImporterPropertyHandler
 */

/*!
  \enum QVersitContactImporter::Error
  This enum specifies an error that occurred during the most recent call to importDocuments()
  \value NoError The most recent operation was successful
  \value InvalidDocumentError One of the documents is not a vCard
  \value EmptyDocumentError One of the documents is empty
  */

/*! Constructs a new importer */
QVersitContactImporter::QVersitContactImporter()
    : d(new QVersitContactImporterPrivate)
{
}

/*! Frees the memory used by the importer */
QVersitContactImporter::~QVersitContactImporter()
{
    delete d;
}

/*!
 * Converts \a documents into a corresponding list of QContacts.  After calling this, the converted
 * contacts can be retrieved by calling contacts().
 * Returns true on success.  If any of the documents cannot be imported as contacts (eg. they aren't
 * vCards), false is returned and errors() will return a list describing the errors that occurred.
 * The successfully imported documents will still be available via contacts().
 *
 * \sa contacts(), errors()
 */
bool QVersitContactImporter::importDocuments(const QList<QVersitDocument>& documents)
{
    int documentIndex = 0;
    int contactIndex = 0;
    d->mContacts.clear();
    d->mErrors.clear();
    bool ok = true;
    foreach (const QVersitDocument& document, documents) {
        QContact contact;
        QVersitContactImporter::Error error;
        if (d->importContact(document, contactIndex, &contact, &error)) {
            d->mContacts.append(contact);
            contactIndex++;
        } else {
            d->mErrors.insert(documentIndex, error);
            ok = false;
        }
        documentIndex++;
    }

    return ok;
}

/*!
 * Returns the contacts imported in the most recent call to importDocuments().
 *
 * \sa importDocuments()
 */
QList<QContact> QVersitContactImporter::contacts() const
{
    return d->mContacts;
}

/*!
 * Returns the map of errors encountered in the most recent call to importDocuments().  The key is
 * the index into the input list of documents and the value is the error that occurred on that
 * document.
 *
 * \sa importDocuments()
 */
QMap<int, QVersitContactImporter::Error> QVersitContactImporter::errors() const
{
    return d->mErrors;
}

/*!
 * Sets \a handler to be the handler for processing QVersitProperties, or 0 to have no handler.
 *
 * Does not take ownership of the handler.  The client should ensure the handler remains valid for
 * the lifetime of the exporter.
 */
void QVersitContactImporter::setPropertyHandler(QVersitContactImporterPropertyHandler* handler)
{
    d->mPropertyHandler = handler;
}

/*!
 * Gets the handler for processing QVersitProperties.
 */
QVersitContactImporterPropertyHandler* QVersitContactImporter::propertyHandler() const
{
    return d->mPropertyHandler;
}

/*!
 * Sets \a handler to be the handler to save files with, or 0 to have no handler.
 *
 * Does not take ownership of the handler.  The client should ensure the handler remains valid for
 * the lifetime of the exporter.
 */
void QVersitContactImporter::setResourceHandler(QVersitResourceHandler* handler)
{
    d->mResourceHandler = handler;
}

/*!
 * Returns the associated resource handler.
 */
QVersitResourceHandler* QVersitContactImporter::resourceHandler() const
{
    return d->mResourceHandler;
}
