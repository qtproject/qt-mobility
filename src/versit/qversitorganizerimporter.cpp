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

#include "qorganizeritemmanagerengine.h"
#include "qversitorganizerimporter.h"
#include "qversitorganizerimporter_p.h"
#include "qvcardbackuphandlers_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qmobilityglobal.h"

QTM_USE_NAMESPACE

QVersitOrganizerImporterPropertyHandler* QVersitOrganizerImporterPropertyHandler::createBackupHandler()
{
    // TODO
    return NULL;
}

/*! Constructs a new importer */
QVersitOrganizerImporter::QVersitOrganizerImporter()
    : d(new QVersitOrganizerImporterPrivate)
{
}

/*! Frees the memory used by the importer */
QVersitOrganizerImporter::~QVersitOrganizerImporter()
{
    delete d;
}

/*!
 * Converts \a document into a corresponding list of QOrganizerItems.  After calling this, the
 * converted organizer items can be retrieved by calling items().
 *
 * Returns true on success.  The document should contain at least one subdocument.  In the
 * importing process, each subdocument roughly corresponds to a QOrganizerItem.  If any of the
 * subdocuments cannot be imported as organizer items (eg. they don't conform to the iCalendar
 * format), false is returned and errors() will return a list describing the errors that occurred.
 * The successfully imported items will still be available via items().
 *
 * \sa items(), errors()
 */
bool QVersitOrganizerImporter::importDocument(const QVersitDocument& document)
{
    d->mItems.clear();
    d->mErrors.clear();
    bool ok = true;
    if (document.type() != QVersitDocument::ICalendar20Type
        || document.componentType() != QLatin1String("VCALENDAR")) {
        d->mErrors.insert(-1, QVersitOrganizerImporter::InvalidDocumentError);
        return false;
    }
    const QList<QVersitDocument> subDocuments = document.subDocuments();
    if (subDocuments.isEmpty()) {
        d->mErrors.insert(-1, QVersitOrganizerImporter::EmptyDocumentError);
        return false;
    }

    int documentIndex = 0;
    foreach (const QVersitDocument& subDocument, subDocuments) {
        QOrganizerItem item;
        QVersitOrganizerImporter::Error error;
        if (d->importDocument(subDocument, &item, &error)) {
            d->mItems.append(item);
        } else {
            d->mErrors.insert(documentIndex, error);
            ok = false;
        }
        documentIndex++;
    }

    return ok;
}

/*!
 * Returns the organizer items imported in the most recent call to importDocuments().
 *
 * \sa importDocuments()
 */
QList<QOrganizerItem> QVersitOrganizerImporter::items() const
{
    return d->mItems;
}

/*!
 * Returns the map of errors encountered in the most recent call to importDocuments().  The key is
 * the index into the input list of documents and the value is the error that occurred on that
 * document.
 *
 * \sa importDocuments()
 */
QMap<int, QVersitOrganizerImporter::Error> QVersitOrganizerImporter::errors() const
{
    return d->mErrors;
}

/*!
 * Sets \a handler to be the handler for processing QVersitProperties, or 0 to have no handler.
 *
 * Does not take ownership of the handler.  The client should ensure the handler remains valid for
 * the lifetime of the exporter.
 */
void QVersitOrganizerImporter::setPropertyHandler(QVersitOrganizerImporterPropertyHandler* handler)
{
    d->mPropertyHandler = handler;
}

/*!
 * Sets \a handler to be the handler to save files with, or 0 to have no handler.
 *
 * Does not take ownership of the handler.  The client should ensure the handler remains valid for
 * the lifetime of the exporter.
 */
void QVersitOrganizerImporter::setResourceHandler(QVersitResourceHandler* handler)
{
    d->mResourceHandler = handler;
}

/*!
 * Returns the associated resource handler.
 */
QVersitResourceHandler* QVersitOrganizerImporter::resourceHandler() const
{
    return d->mResourceHandler;
}
