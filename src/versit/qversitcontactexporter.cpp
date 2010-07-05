/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "qversitcontactexporter.h"
#include "qversitcontactexporter_p.h"
#include "qmobilityglobal.h"

#include <qcontact.h>
#include <qcontactdetail.h>

QTM_USE_NAMESPACE

/*!
  \class QVersitContactExporterDetailHandler
  \brief The QVersitContactExporterDetailHandler class is an interface for clients wishing to
  implement custom export behaviour for certain contact details.
  \ingroup versit

  \sa QVersitContactExporter
 */

/*!
 * \fn virtual QVersitContactExporterDetailHandler::~QVersitContactExporterDetailHandler()
 * Frees any memory in use by this handler.
 */

/*!
 * \fn virtual bool QVersitContactExporterDetailHandler::preProcessDetail(const QContact& contact, const QContactDetail& detail, QVersitDocument* document) = 0;
 * Process \a detail and update \a document with the corresponding QVersitProperty(s).
 * \a contact provides the context within which the detail was found.
 *
 * Returns true if the detail has been handled and requires no further processing, false otherwise.
 *
 * This function is called on every QContactDetail encountered during an export.  Supply this
 * function and return true to implement custom export behaviour.
 */

/*!
 * \fn virtual bool QVersitContactExporterDetailHandler::postProcessDetail(const QContact& contact, const QContactDetail& detail, bool alreadyProcessed, QVersitDocument* document) = 0;
 * Process \a detail and update \a document with the corresponding QVersitProperty(s).
 * \a contact provides the context within which the detail was found.
 * \a alreadyProcessed is true if the detail has already been processed either by
 * \l preProcessDetail() or by QVersitContactExporter itself.
 *
 * Returns true if the detail has been handled, false otherwise.
 *
 * This function is called on every \l QContactDetail encountered during an export.  This can be
 * used to implement support for QContactDetails not supported by QVersitContactExporter.
 */

/*!
 * \class QVersitContactExporter
 * \brief The QVersitContactExporter class converts \l {QContact}{QContacts} into
 * \l {QVersitDocument}{QVersitDocuments}.
 * \ingroup versit
 *
 * A \l QVersitResourceHandler is associated with the exporter to supply the behaviour for loading
 * files from persistent storage.  By default, this is set to a \l QVersitDefaultResourceHandler,
 * which supports basic resource loading from the file system.  An alternative resource handler
 * can be specified with setResourceHandler().
 *
 * By associating a \l QVersitContactExporterDetailHandler with the exporter using
 * setDetailHandler(), the client can pass in a handler to override the processing of details and/or
 * handle details that QVersitContactExporter doesn't support.
 *
 * An example detail handler that encodes all unknown details as nonstandard vCard propreties:
 * \snippet ../../doc/src/snippets/qtversitdocsample/qtversitdocsample.cpp Detail handler
 *
 * An example usage of QVersitContactExporter
 * \snippet ../../doc/src/snippets/qtversitdocsample/qtversitdocsample.cpp Export example
 *
 * \section1 Exporting group relationships
 * The exporter does not handle QContactRelationships at all.
 *
 * Some managers use the \l{QContactRelationship::HasMember}{HasMember} QContactRelationship along
 * with contacts of type \l{QContactType::TypeGroup}{TypeGroup} to indicate categorization of
 * contacts.  In vCard, categorization is represented by the CATEGORIES property, which has
 * semantics most similar to the QContactTag detail.  For contact manager backends that supports
 * groups but not QContactTag, if the categorization information needs to be retained through
 * CATEGORIES vCard properties, extra work can be done to convert from group relationships to
 * QContactTag before passing the contact list to the exporter.  Below is some example code that
 * does this translation.
 *
 * \snippet ../../doc/src/snippets/qtversitdocsample/qtversitdocsample.cpp Export relationship example
 *
 * \sa QVersitDocument, QVersitProperty, QVersitContactExporterDetailHandler, QVersitResourceHandler
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
 * Sets \a handler to be the handler for processing QContactDetails, or 0 to have no handler.
 *
 * Does not take ownership of the handler.  The client should ensure the handler remains valid for
 * the lifetime of the exporter.
 */
void QVersitContactExporter::setDetailHandler(QVersitContactExporterDetailHandler* handler)
{
    d->mDetailHandler = handler;
}

/*!
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
