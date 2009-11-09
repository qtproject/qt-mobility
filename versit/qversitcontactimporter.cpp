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

/*!
 * Constructor.
 */
QVersitContactImporter::QVersitContactImporter() 
    : d(new QVersitContactImporterPrivate)
{
}

/*!
 * Destructor.
 */
QVersitContactImporter::~QVersitContactImporter()
{
    delete d;
}

/*!
 * Sets the \a path where the contact photos will be saved.
 */
void QVersitContactImporter::setImagePath(const QString& path)
{
    d->mImagePath = path;
}

/*!
 * Returns the path where the contact photos will be saved.
 */
QString QVersitContactImporter::imagePath() const
{
    return d->mImagePath;
}

/*!
 * Sets the \a path where the contact related audio clips will be saved.
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
 * Generates a QContact from \a versitDocument.
 */
QContact QVersitContactImporter::importContact(
    const QVersitDocument& versitDocument)
{
    return d->importContact(versitDocument);
}

/*!
 * Returns the list of versit properties that were left unconverted
 * by the most recent call of importContact.
 */
QList<QVersitProperty> QVersitContactImporter::unconvertedVersitProperties()
{
    return d->mUnconvertedVersitProperties;
}
