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

/*!
 * \class QVersitContactExporter
 *
 * \brief The QVersitContactExporter class exports the QTContact into QVersitDocument.
 *
 * A QVersitContactExporter exportContact(), is used for processing QTcontact and
 * generating corresponding QVersitDocument. If the QTContact has some detail with
 * image value it will emit scale(const QString& imageFileName, QByteArray& imageData)
 * signal which can be used by client for image scalling. Client may reterive
 * list of contact details wihch are not encoded using unknownContactDetails().
 *
 * \code
 *
 * // An example of exporting the QTContact:
 *  QVersitContactExporter contactExporter;
 *  QContact contact;
 *  QContactName contactName;
 *  QContactPresence presence;
 *  QContactAvatar contactAvatar;
 *
 *  // Encode Name
 *  name.setFirst(QString::fromAscii("Simpson"));
 *  contact.saveDetail(&name);
 *
 *  // Encode presence
 *  presence.setAccountUri(QString::fromAscii("a@abc.com"));
 *  contact.saveDetail(&presence);
 *
 *  // Encode Avatar, ensure to provide valid avatar
 *  contactAvatar.setAvatar("/my/image/path");
 *  contactAvatar.setSubType(QContactAvatar::SubTypeImage);
 *  contact.saveDetail(&contactAvatar);
 *  QVersitDocument versitDocument = contactExporter->exportContact(contact);
 *
 *  // Client will receive the signal scale with image data and path client may
 *  // scale the image
 *
 *  QList<QContactDetail> unknownDetails = contactExporter->unknownContactDetails();
 *
 *  // unknownDetails can be processed by the client to append details directly
 *  // into versitDocument if needed, in this example presence is not encoded into the
 *  // versitDocument
 *
 *  // use versitDocument ...
 *
 * \endcode
 *
 * \sa QContact, QVersitDocument, QVersitProperty
 */


/*!
 * \fn void scale(const QString& imageFileName, QByteArray& imageData)
 * Scale signal is emitted by the exportContact, when image data is found in QTContact.
 * It provides clients \a imageFileName and \a imageData which client can use for image
 * scalling. Image scalling can be done using utilties functions for example QImage scale.
 *
 * \sa QImage
 */

#include "qversitcontactexporter.h"
#include "qversitcontactexporter_p.h"
#include <qcontact.h>
#include <qcontactdetail.h>


/*!
 * Constructs a new contact converter
 */
QVersitContactExporter::QVersitContactExporter()
    : d(new QVersitContactExporterPrivate())
{
    connect(d, SIGNAL(scale(const QString&,QByteArray&)),
            this, SIGNAL(scale(const QString&,QByteArray&)));
}

/*!
 * Frees any memory in use by this contact converter
 */
QVersitContactExporter::~QVersitContactExporter()
{
}

/*!
 * Returns the versit document corresponding to the \a contact and
 * \a versitType.
 */
QVersitDocument QVersitContactExporter::exportContact(
    const QContact& contact,
    QVersitDocument::VersitType versitType)
{
    QVersitDocument versitDocument;
    versitDocument.setVersitType(versitType);
    d->exportContact(versitDocument,contact);
    return versitDocument;
}

/*!
 * Returns the list of contact detils, which are not encoded by
 * the most recent call of exportContact.
 */
QList<QContactDetail> QVersitContactExporter::unknownContactDetails()
{
    return d->mUnknownContactDetails;
}
