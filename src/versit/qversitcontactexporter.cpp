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


#include "qversitcontactexporter.h"
#include "qversitcontactexporter_p.h"
#include "qmobilityglobal.h"

#include <qcontact.h>
#include <qcontactdetail.h>

QTM_USE_NAMESPACE

/*!
  \deprecated
  \class QVersitContactExporterDetailHandler
  \brief The QVersitContactExporterDetailHandler class is an interface for clients wishing to
  implement custom export behaviour for certain contact details.

  This interface is replaced by QVersitContactExporterDetailHandlerV2.
  \ingroup versit

  \sa QVersitContactExporter
 */

/*!
  \fn QVersitContactExporterDetailHandler::~QVersitContactExporterDetailHandler()
  Frees any memory in use by this handler.
 */

/*!
  \fn bool QVersitContactExporterDetailHandler::preProcessDetail(const QContact& contact, const QContactDetail& detail, QVersitDocument* document)
  Process \a detail and update \a document with the corresponding QVersitProperty(s).
  \a contact provides the context within which the detail was found.

  Returns true if the detail has been handled and requires no further processing, false otherwise.

  This function is called on every QContactDetail encountered during an export.  Supply this
  function and return true to implement custom export behaviour.
 */

/*!
  \fn bool QVersitContactExporterDetailHandler::postProcessDetail(const QContact& contact, const QContactDetail& detail, bool alreadyProcessed, QVersitDocument* document)
  Process \a detail and update \a document with the corresponding QVersitProperty(s).
  \a contact provides the context within which the detail was found.
  \a alreadyProcessed is true if the detail has already been processed either by
  \l preProcessDetail() or by QVersitContactExporter itself.

  Returns true if the detail has been handled, false otherwise.

  This function is called on every \l QContactDetail encountered during an export.  This can be
  used to implement support for QContactDetails not supported by QVersitContactExporter.
 */

/*!
  \class QVersitContactExporterDetailHandlerV2
  \brief The QVersitContactExporterDetailHandlerV2 class is an interface for clients wishing to
  implement custom export behaviour for certain contact details.

  This interface supercedes QVersitContactImporterPropertyHandler.

  \ingroup versit

  \sa QVersitContactExporter
 */

/*!
  \fn QVersitContactExporterDetailHandlerV2::~QVersitContactExporterDetailHandlerV2()
  Frees any memory in use by this handler.
 */

/*!
  \fn void QVersitContactExporterDetailHandlerV2::detailProcessed(const QContact& contact, const QContactDetail& detail, const QSet<QString>& processedFields, const QVersitDocument& document, QList<QVersitProperty>* toBeRemoved, QList<QVersitProperty>* toBeAdded)

  Process \a detail and provide a list of updated \l{QVersitProperty}{QVersitProperties} by
  modifying the \a toBeRemoved and \a toBeAdded lists.  \a contact provides the context within which
  the detail was found.

  This function is called on every QContactDetail encountered during an export, after the detail has
  been processed by the QVersitContactExporter.  This can be used to implement support for
  QContactDetails not supported by QVersitContactExporter.

  \a processedFields contains a list of fields in the \a detail that were considered by the
  QVersitContactExporter in processing the detail.  \a document holds the state of the document
  before the detail was processed by the exporter.
  
  \a toBeRemoved and \a toBeAdded are initially filled with a list of properties that the exporter
  will remove from and add to the document.  These lists can be modified (by removing, modifying or
  adding properties) by the handler to control the changes that will actually be made to the
  document.  If a property is to be modified in the document, the old version will appear in the
  \a toBeRemoved list and the new version will appear in the \a toBeAdded list.

  After the handler returns control back to the exporter, the properties in the \a toBeRemoved
  list will be removed and the properties in the \a toBeAdded list will be appended to the document.
 */

/*!
  \fn void QVersitContactExporterDetailHandlerV2::contactProcessed(const QContact& contact, QVersitDocument* document)
  Perform any finalizing processing on the \a document generated by the \a contact.

  This function is called after all QContactDetails have been handled by the
  QVersitContactExporter.
*/

/*!
  \fn int QVersitContactExporterDetailHandlerV2::version() const
  Returns the version of the handler.  Currently, always returns 2.
*/

/*!
  \class QVersitContactExporter
  \brief The QVersitContactExporter class converts \l {QContact}{QContacts} into
  \l {QVersitDocument}{QVersitDocuments}.
  \ingroup versit

  A \l QVersitResourceHandler is associated with the exporter to supply the behaviour for loading
  files from persistent storage.  By default, this is set to a \l QVersitDefaultResourceHandler,
  which supports basic resource loading from the file system.  An alternative resource handler
  can be specified with setResourceHandler().

  By associating a \l QVersitContactExporterDetailHandlerV2 with the exporter using
  setDetailHandler(), the client can pass in a handler to override the processing of details and/or
  handle details that QVersitContactExporter doesn't support.  By default, this is set to a
  QVersitContactExporterDefaultDetailHandler, which encodes details that the exporter doesn't
  recognise.  The details are encoded in a format that can be decoded by a
  QVersitContactImporterDefaultPropertyHandler.  This means that if a contact is exported and then
  imported again, the resulting contact will be virtually identical to the original one, even if
  there are non-standard details.


  An example usage of QVersitContactExporter
  \snippet ../../doc/src/snippets/qtversitdocsample/qtversitdocsample.cpp Export example

  \section1 Exporting group relationships
  The exporter does not handle QContactRelationships at all.

  Some managers use the \l{QContactRelationship::HasMember}{HasMember} QContactRelationship along
  with contacts of type \l{QContactType::TypeGroup}{TypeGroup} to indicate categorization of
  contacts.  In vCard, categorization is represented by the CATEGORIES property, which has
  semantics most similar to the QContactTag detail.  For contact manager backends that supports
  groups but not QContactTag, if the categorization information needs to be retained through
  CATEGORIES vCard properties, extra work can be done to convert from group relationships to
  QContactTag before passing the contact list to the exporter.  Below is some example code that
  does this translation.

  \snippet ../../doc/src/snippets/qtversitdocsample/qtversitdocsample.cpp Export relationship example

  \sa QVersitDocument, QVersitProperty, QVersitContactExporterDetailHandlerV2, QVersitResourceHandler
  \sa QVersitContactExporterDefaultDetailHandler
 */

/*!
  \enum QVersitContactExporter::Error
  This enum specifies an error that occurred during the most recent call to exportContacts()
  \value NoError The most recent operation was successful
  \value EmptyContactError One of the contacts was empty
  \value NoNameError One of the contacts has no QContactName field
  */

/*!
 * Constructs a new contact exporter
 */
QVersitContactExporter::QVersitContactExporter()
    : d(new QVersitContactExporterPrivate())
{
}

/*!
 * Frees any memory in use by this contact exporter.
 */
QVersitContactExporter::~QVersitContactExporter()
{
    delete d;
}

/*!
 * Converts \a contacts into a list of corresponding QVersitDocuments, using the format given by
 * \a versitType.
 * Returns true on success.  If any of the contacts could not be exported, false is returned and
 * errors() will return a list describing the errors that occurred.  The successfully exported
 * documents will still be available via documents().
 */
bool QVersitContactExporter::exportContacts(
    const QList<QContact>& contacts,
    QVersitDocument::VersitType versitType)
{
    int contactIndex = 0;
    d->mDocuments.clear();
    d->mErrors.clear();
    bool ok = true;
    foreach (const QContact& contact, contacts) {
        QVersitDocument versitDocument;
        versitDocument.setType(versitType);
        QVersitContactExporter::Error error;
        if (d->exportContact(contact, versitDocument, &error)) {
            d->mDocuments.append(versitDocument);
        } else {
            d->mErrors.insert(contactIndex, error);
            ok = false;
        }
        contactIndex++;
    }

    return ok;
}

/*!
 * Returns the documents exported in the most recent call to exportContacts().
 *
 * \sa exportContacts()
 */
QList<QVersitDocument> QVersitContactExporter::documents() const
{
    return d->mDocuments;
}

/*!
 * Returns the map of errors encountered in the most recent call to exportContacts().  The key is
 * the index into the input list of contacts and the value is the error that occurred on that
 * contact.
 *
 * \sa exportContacts()
 */
QMap<int, QVersitContactExporter::Error> QVersitContactExporter::errors() const
{
    return d->mErrors;
}

/*!
 * \deprecated
 * Sets \a handler to be the handler for processing QContactDetails, or 0 to have no handler.
 *
 * Does not take ownership of the handler.  The client should ensure the handler remains valid for
 * the lifetime of the exporter.  This function is used for version 1 handlers.
 *
 * Only one detail handler can be set.  If a version 2 handler was previously set, it will no
 * longer be associated with the importer.
 */
void QVersitContactExporter::setDetailHandler(QVersitContactExporterDetailHandler* handler)
{
    d->mDetailHandlerVersion = 1;
    d->mDetailHandler = handler;
    d->mDetailHandler2 = 0;
}

/*!
 * Sets \a handler to be the handler for processing QContactDetails, or 0 to have no handler.
 *
 * Does not take ownership of the handler.  The client should ensure the handler remains valid for
 * the lifetime of the exporter.  This function is used for version 2 and above handlers.
 *
 * Only one detail handler can be set.  If a version 1 handler was previously set, it will no
 * longer be associated with the importer.
 */
void QVersitContactExporter::setDetailHandler(QVersitContactExporterDetailHandlerV2* handler)
{
    if (handler)
        d->mDetailHandlerVersion = handler->version();
    d->mDetailHandler = 0;
    d->mDetailHandler2 = handler;
}

/*!
 * \deprecated
 * Gets the handler for processing QContactDetails.
 */
QVersitContactExporterDetailHandler* QVersitContactExporter::detailHandler() const
{
    return d->mDetailHandler;
}

/*!
 * Sets \a handler to be the handler to load files with, or 0 to have no handler.
 *
 * Does not take ownership of the handler.  The client should ensure the handler remains valid for
 * the lifetime of the exporter.
 */
void QVersitContactExporter::setResourceHandler(QVersitResourceHandler* handler)
{
    d->mResourceHandler = handler;
}

/*!
 * Returns the associated resource handler.
 */
QVersitResourceHandler* QVersitContactExporter::resourceHandler() const
{
    return d->mResourceHandler;
}

// Deprecated:

/*!
  \deprecated

  Use the version of the function returning a bool, instead.
*/
QList<QVersitDocument> QVersitContactExporter::exportContacts(const QList<QContact>& contacts)
{
    exportContacts(contacts, QVersitDocument::VCard30Type);
    return documents();
}
