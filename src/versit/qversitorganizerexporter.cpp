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


#include "qversitorganizerexporter.h"
#include "qversitorganizerexporter_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qmobilityglobal.h"

QTM_USE_NAMESPACE

/*!
  \class QVersitOrganizerExporter
  \brief The QVersitOrganizerExporter class converts \l {QOrganizerItem}{QOrganizerItems} into
  \l {QVersitDocument}{QVersitDocuments}.
  \ingroup versit

  This class is used to convert a list of \l {QOrganizerItem}{QOrganizerItems} (which may be stored
  in a QOrganizerItemManager) into a QVersitDocument (which may be written to an I/O device using
  QVersitReader.  While multiple items are provided as input, a single QVersitDocument is produced
  as output.  Unless there is an error, there is a one-to-one mapping between organizer items
  and sub-documents of the result.
 */

QVersitOrganizerExporterDetailHandler* QVersitOrganizerExporterDetailHandler::createBackupHandler()
{
    // TODO
    return NULL;
}

/*! Constructs a new importer */
QVersitOrganizerExporter::QVersitOrganizerExporter()
    : d(new QVersitOrganizerExporterPrivate)
{
}

/*! Frees the memory used by the importer */
QVersitOrganizerExporter::~QVersitOrganizerExporter()
{
    delete d;
}

/*!
 * Converts \a items into a QVersitDocument, using the format given by \a versitType.
 * Returns true on success.  If any of the items could not be exported, false is returned and
 * errors() will return a list describing the errors that occurred.  The successfully exported
 * components will still be available via document().
 */
bool QVersitOrganizerExporter::exportItems(
    const QList<QOrganizerItem>& items,
    QVersitDocument::VersitType versitType)
{
    int itemIndex = 0;
    d->mErrors.clear();
    d->mResult.clear();
    d->mResult.setType(versitType);
    d->mResult.setComponentType(QLatin1String("VCALENDAR"));
    bool ok = true;
    QList<QVersitDocument> results;
    foreach (const QOrganizerItem& item, items) {
        QVersitDocument document;
        document.setType(versitType);
        QVersitOrganizerExporter::Error error;
        if (d->exportItem(item, &document, &error)) {
            results.append(document);
        } else {
            d->mErrors.insert(itemIndex, error);
            ok = false;
        }
        itemIndex++;
    }
    d->mResult.setSubDocuments(results);

    return ok;
}

/*!
 * Returns the document exported in the most recent call to exportItems().
 *
 * \sa exportItems()
 */
QVersitDocument QVersitOrganizerExporter::document() const
{
    return d->mResult;
}

/*!
 * Returns the map of errors encountered in the most recent call to exportItems().  The key is
 * the index into the input list of organizer items and the value is the error that occurred on that
 * item.
 *
 * \sa exportItems()
 */
QMap<int, QVersitOrganizerExporter::Error> QVersitOrganizerExporter::errors() const
{
    return d->mErrors;
}

/*!
 * \preliminary
 * Sets \a handler to be the handler for processing QOrganizerItemDetails, or 0 to have no handler.
 *
 * Does not take ownership of the handler.  The client should ensure the handler remains valid for
 * the lifetime of the exporter.
 *
 * Only one detail handler can be set.  If another detail handler (of any version) was
 * previously set, it will no longer be associated with the exporter.
 *
 * NOTE: Detail handlers for organizer items have not been implemented yet.
 */
void QVersitOrganizerExporter::setDetailHandler(QVersitOrganizerExporterDetailHandler* handler)
{
    d->mDetailHandler = handler;
}

/*!
 * \preliminary
 * Sets \a handler to be the handler to load files with, or 0 to have no handler.
 *
 * Does not take ownership of the handler.  The client should ensure the handler remains valid for
 * the lifetime of the exporter.
 *
 * NOTE: Resource handlers for organizer items have not been implemented yet.
 */
void QVersitOrganizerExporter::setResourceHandler(QVersitResourceHandler* handler)
{
    d->mResourceHandler = handler;
}

/*!
 * \preliminary
 * Returns the associated resource handler.
 *
 * NOTE: Resource handlers for organizer items have not been implemented yet.
 */
QVersitResourceHandler* QVersitOrganizerExporter::resourceHandler() const
{
    return d->mResourceHandler;
}
