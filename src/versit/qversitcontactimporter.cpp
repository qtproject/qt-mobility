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

#include "qcontactmanagerengine.h"
#include "qversitcontactimporter.h"
#include "qversitcontactimporter_p.h"
#include "qvcardbackuphandlers_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qmobilityglobal.h"

QTM_USE_NAMESPACE

/*!
  \deprecated
  \class QVersitContactImporterPropertyHandler
  \brief The QVersitContactImporterPropertyHandler class is an interface for clients wishing to
  implement custom import behaviour for vCard properties

  This interface is replaced by QVersitContactImporterPropertyHandlerV2.

  \sa QVersitContactImporter
 */

/*!
  \fn QVersitContactImporterPropertyHandler::~QVersitContactImporterPropertyHandler()
  Frees any memory in use by this handler.
 */

/*!
  \fn bool QVersitContactImporterPropertyHandler::preProcessProperty(const QVersitDocument& document, const QVersitProperty& property, int contactIndex, QContact* contact)
  Process \a property and update \a contact with the corresponding QContactDetail(s).
  \a document provides the context within which the property was found.
  \a contactIndex specifies the position that \a contact will take in the list returned by
  \l QVersitContactImporter::importDocuments().

  Returns true if the property has been handled and requires no further processing, false
  otherwise.

  This function is called on every QVersitProperty encountered during an import.  Supply this
  function and return true to implement custom import behaviour.
 */

/*!
  \fn bool QVersitContactImporterPropertyHandler::postProcessProperty(const QVersitDocument& document, const QVersitProperty& property, bool alreadyProcessed, int contactIndex, QContact* contact)
  Process \a property and update \a contact with the corresponding QContactDetail(s).
  \a document provides the context within which the property was found.
  \a contactIndex specifies the position that \a contact will take in the list returned by
  \l QVersitContactImporter::importDocuments().
  \a alreadyProcessed is true if the detail has already been processed either by
  \l preProcessProperty() or by QVersitContactImporter itself.

  Returns true if the property has been handled, false otherwise.

  This function is called on every QVersitProperty encountered during an import.  This can be
  used to implement support for vCard properties not supported by QVersitContactImporter.
 */

/*!
  \class QVersitContactImporterPropertyHandlerV2
  \brief The QVersitContactImporterPropertyHandlerV2 class is an interface for clients wishing to
  implement custom import behaviour for vCard properties.
  \ingroup versit-extension

  This interface supercedes QVersitContactImporterPropertyHandler.

  \sa QVersitContactImporter
 */

/*!
  \fn QVersitContactImporterPropertyHandlerV2::~QVersitContactImporterPropertyHandlerV2()
  Frees any memory in use by this handler.
 */

/*!
  \fn void QVersitContactImporterPropertyHandlerV2::propertyProcessed(const QVersitDocument& document, const QVersitProperty& property, bool alreadyProcessed, const QContact& contact, QList<QContactDetail>* updatedDetails)
  Process \a property and provide a list of updated details by adding them to \a updatedDetails.

  This function is called on every QVersitProperty encountered during an import, after the property
  has been processed by the QVersitContactImporter.  An implementation of this function can be made
  to provide support for vCard properties not supported by QVersitContactImporter.

  The supplied \a document is the container for the \a property.  \a alreadyProcessed is true if the
  QVersitContactImporter was successful in processing the property.  \a contact holds the state of
  the contact before the property was processed by the importer.  \a updatedDetails is initially
  filled with a list of details that the importer will update, and can be modified (by removing,
  modifying or adding details to the list)
 */

/*!
  \fn void QVersitContactImporterPropertyHandlerV2::documentProcessed(const QVersitDocument& document, QContact* contact)
  Perform any final processing on the \a contact generated by the \a document.  This can be
  implemented by the handler to clear any internal state before moving onto the next document.

  This function is called after all QVersitProperties have been handled by the
  QVersitContactImporter.
*/

/*!
  \class QVersitContactImporter
  \brief The QVersitContactImporter class converts \l{QVersitDocument}{QVersitDocuments} to
  \l{QContact}{QContacts}.

  \ingroup versit

  This class is used to convert lists of \l{QVersitDocument}{QVersitDocuments} (which may be
  produced by a QVersitReader) to lists of \l{QContact}{QContacts} (which may be saved into a
  QContactManager.  Unless there is an error, there is a one-to-one mapping between Versit
  documents and QContacts.  The importer can be extended by clients by associating resource
  and property handlers.

  A \l QVersitResourceHandler is associated with the importer to supply the behaviour for saving
  files to persistent storage.  By default, this is set to a \l QVersitDefaultResourceHandler,
  which does not save files to persistent storage.  Note that although photos found in vCards
  are not saved to disk by default, the importer does add a QContactThumbnail detail to the
  image.  If a full-sized image needs to be loaded from a URL and persisted on disk, a custom
  QVersitResourceHandler should be supplied which implements this.

  By associating a QVersitContactImporterPropertyHandlerV2 with the importer using
  setPropertyHandler(), the client can pass in a handler to override the processing of properties
  and/or handle properties that QVersitContactImporter doesn't support.  A "backup" handler is
  provided by QVersitContactImporterPropertyHandlerV2::createBackupHandler() which imports
  properties generated by the corresponding backup handler for the QVersitContactExporter.

  An example usage of QVersitContactImporter:
  \snippet ../../doc/src/snippets/qtversitdocsample/qtversitdocsample.cpp Import example

  \section1 Importing categories
  The importer imports the vCard CATEGORIES property by converting each category to a QContactTag.
  Some managers may not have support for QContactTag, but instead support categorization using the
  \l{QContactRelationship::HasMember}{HasMember} QContactRelationship along with contacts of type
  \l{QContactType::TypeGroup}{TypeGroup}.  For these backends, if the categorization information
  needs to be retained through group relationships, extra work needs to be done to do the
  conversion.  Below is some example code that does this translation.

  \snippet ../../doc/src/snippets/qtversitdocsample/qtversitdocsample.cpp Import relationship example

  \sa QVersitDocument, QVersitProperty, QVersitResourceHandler, QVersitContactImporterPropertyHandlerV2
 */

/*!
  \enum QVersitContactImporter::Error
  This enum specifies an error that occurred during the most recent call to importDocuments()
  \value NoError The most recent operation was successful
  \value InvalidDocumentError One of the documents is not a vCard
  \value EmptyDocumentError One of the documents is empty
  */


/*!
  Constructs and returns a property handler that restores data from a vCard generated using the
  exporter backup detail handler (see QVersitContactExporterDetailHandlerV2::createBackupHandler()).
  The caller is responsible for deleting the object.

  Clients wishing to implement their own property handler and also benefit from the functionality of
  the default handler can use this function to construct one, and wrap a custom
  QVersitContactImporterPropertyHandlerV2 around it.  In the implementation of propertyProcessed and
  documentProcessed, the respective functions in the backup handler should be called as the last
  step (ensuring the arguments are correctly updated and passed through).
 */
QVersitContactImporterPropertyHandlerV2* QVersitContactImporterPropertyHandlerV2::createBackupHandler() {
    return new QVCardImporterBackupHandler;
}

/*! Constructs a new importer */
QVersitContactImporter::QVersitContactImporter()
    : d(new QVersitContactImporterPrivate)
{
}

/*!
 * Constructs a new importer for the given \a profile.  The profile strings should be one of those
 * defined by QVersitContactHandlerFactory, or a value otherwise agreed to by a Versit plugin.
 */
QVersitContactImporter::QVersitContactImporter(QSet<QString> profiles)
    : d(new QVersitContactImporterPrivate(profiles))
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
 * \deprecated
 * Sets \a handler to be the handler for processing QVersitProperties, or 0 to have no handler.
 *
 * Does not take ownership of the handler.  The client should ensure the handler remains valid for
 * the lifetime of the exporter.  This function is used for version 1 handlers.
 *
 * Only one property handler can be set.  If another property handler (of any version) was
 * previously set, it will no longer be associated with the importer.
 */
void QVersitContactImporter::setPropertyHandler(QVersitContactImporterPropertyHandler* handler)
{
    d->mPropertyHandlerVersion = 1;
    d->mPropertyHandler = handler;
    d->mPropertyHandler2 = NULL;
}

/*!
 * Sets \a handler to be the handler for processing QVersitProperties, or 0 to have no handler.
 *
 * Does not take ownership of the handler.  The client should ensure the handler remains valid for
 * the lifetime of the exporter.  This function is used for version 2 and higher handlers.
 *
 * Only one property handler can be set.  If another property handler (of any version) was
 * previously set, it will no longer be associated with the importer.
 */
void QVersitContactImporter::setPropertyHandler(QVersitContactImporterPropertyHandlerV2* handler)
{
    d->mPropertyHandlerVersion = 2;
    d->mPropertyHandler = 0;
    d->mPropertyHandler2 = handler;
}

/*!
 * \internal
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
