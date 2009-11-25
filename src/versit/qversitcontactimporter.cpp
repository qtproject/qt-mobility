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

#include "qversitcontactimporter.h"
#include "qversitcontactimporter_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"

QTM_BEGIN_NAMESPACE

/*!
 * \class QVersitContactImporter
 *
 * \brief QVersitContactImporter creates QContacts from QVersitDocuments.
 *
 * The versit properties (\l QVersitProperty) that were not imported by
 * \l QVersitContactImporter::importContact() can be fetched after importing
 * by calling \l QVersitContactImporter::unknownVersitProperties().
 * For the returned properties,
 * the client can perform the conversions from versit properties 
 * to contact details and add the converted details to the QContact.
 *
 * \code
 *
 * QVersitDocument document;
 * QVersitProperty property;
 *
 * property.setName(QString::fromAscii("N"));
 * property.setValue("Simpson;Homer;J;;");
 * document.addProperty(property);
 *
 * property.setName(QString::fromAscii("X-UNKNOWN-PROPERTY"));
 * property.setValue("some value");
 * document.addProperty(property);
 *
 * QVersitContactImporter importer;
 * importer.setImagePath(QString::fromAscii("/my/image/path"));
 * importer.setAudioClipPath(QString::fromAscii("my/audio_clip/path"));
 *
 * QContact contact = importer.importContact(document);
 * // contact now contains the "N" property as a QContactName
 * QList<QVersitProperty> unknownProperties = importer.unknownVersitProperties();
 * // unknownProperties contains "X-UNKNOWN-PROPERTY"
 * // that can be handled by the client itself
 *
 * \endcode
 *
 * \sa QVersitDocument, QVersitReader
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
 * Sets the \a path where the contact photos will be saved.
 * This function should be called before calling \l importContact().
 * If the image path has not been set,
 * the images in the versit document will not be added to the contact.
 * There is no default path for them.
 */
void QVersitContactImporter::setImagePath(const QString& path)
{
    d->mImagePath = path;
}

/*!
 * Returns the path where the contact photos are saved.
 */
QString QVersitContactImporter::imagePath() const
{
    return d->mImagePath;
}

/*!
 * Sets the \a path where the contact related audio clips will be saved.
 * This function should be called before calling \l importContact().
 * If the audio clip path has not been set,
 * the audio clips in the versit document will not be added to the contact.
 * There is no default path for them.
 */
void QVersitContactImporter::setAudioClipPath(const QString& path)
{
    d->mAudioClipPath = path;
}

/*!
 * Returns the path where the contact related audio clips will be saved.
 */
QString QVersitContactImporter::audioClipPath() const
{
    return d->mAudioClipPath;
}

/*!
 * Creates a QContact from \a versitDocument.
 */
QContact QVersitContactImporter::importContact(const QVersitDocument& versitDocument)
{
    return d->importContact(versitDocument);
}

/*!
 * Returns the list of versit properties that were not imported
 * by the most recent call of \l importContact().
 */
QList<QVersitProperty> QVersitContactImporter::unknownVersitProperties()
{
    return d->mUnknownVersitProperties;
}

QTM_END_NAMESPACE
