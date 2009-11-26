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


#include "qversitcontactexporter.h"
#include "qversitcontactexporter_p.h"
#include "qmobilityglobal.h"

#include <qcontact.h>
#include <qcontactdetail.h>

QTM_BEGIN_NAMESPACE

/*!
 * \class QVersitContactExporter
 *
 * \brief QVersitContactExporter exports QContact(s) into QVersitDocument(s)
 *
 * If the exported QContact has some detail with an image as its value,
 * signal \l QVersitContactExporter::scale() is emitted and
 * the client can scale the image's data to the size it wishes.
 * The client may retrieve the list contact details
 * which were not exported using QVersitContactExporter::unknownContactDetails().
 *
 * \code
 *
 * // An example of exporting a QContact:
 *  QVersitContactExporter contactExporter;
 *  QContact contact;
 *
 *  // Create a name
 *  QContactName name;
 *  name.setFirst(QString::fromAscii("John"));
 *  contact.saveDetail(&name);
 *
 *  // Create an avatar type which is not supported by the exporter
 *  QContactAvatar contactAvatar;
 *  contactAvatar.setAvatar(QString::fromAscii("/my/image/avatar_path/texture.type"));
 *  contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
 *  contact.saveDetail(&contactAvatar);
 *
 *  // Create an organization detail with a title and a logo
 *  QContactOrganization organization;
 *  organization.setTitle(QString::fromAscii("Developer"));
 *  organization.setLogo(QString::fromAscii("/my/image/logo_path/logo.jpg"));
 *  contact.saveDetail(&organization);
 *
 *  QVersitDocument versitDocument = contactExporter.exportContact(contact);
 *  // Client will receive the signal "scale" with the logo image path
 *
 *  QList<QContactDetail> unknownDetails = contactExporter.unknownContactDetails();
 *
 *  // The returned unknownDetails can be processed by the client and
 *  // the client can append details directly into QVersitDocument if needed.
 *  // (In this example QContactAvatar::SubTypeTexturedMesh.
 *  //  Currently for QContactAvatar details,
 *  //  only exporting subtypes QContactAvatar::SubTypeImage and
 *  //  QContactAvatar::SubTypeAudioRingtone is supported.)
 *
 * \endcode
 *
 * \sa QVersitDocument, QVersitProperty
 */


/*!
 * \fn void QVersitContactExporter::scale(const QString& imageFileName, QByteArray& imageData)
 * This signal is emitted by \l QVersitContactExporter::exportContact(),
 * when a contact detail containing an image is found in a QContact.
 * The input for the client is the path of the image in \a imageFileName.
 * When the client has performed the scaling,
 * it should write the result to \a imageData.
 * Image scaling can be done for example by using class QImage.
 */

/*!
 * Constructs a new contact exporter
 */
QVersitContactExporter::QVersitContactExporter()
    : d(new QVersitContactExporterPrivate())
{
    connect(d, SIGNAL(scale(const QString&,QByteArray&)),
            this, SIGNAL(scale(const QString&,QByteArray&)));
}

/*!
 * Frees any memory in use by this contact exporter.
 */
QVersitContactExporter::~QVersitContactExporter()
{
}

/*!
 * Returns the versit document corresponding
 * to the \a contact and \a versitType.
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
 * Returns the list of contact details, which were not exported
 * by the most recent call of \l QVersitContactExporter::exportContact().
 */
QList<QContactDetail> QVersitContactExporter::unknownContactDetails()
{
    return d->mUnknownContactDetails;
}


#include "moc_qversitcontactexporter.cpp"

QTM_END_NAMESPACE
