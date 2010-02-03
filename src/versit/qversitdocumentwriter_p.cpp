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

#include "qversitdocumentwriter_p.h"
#include "versitutils_p.h"
#include <QTextCodec>

QTM_USE_NAMESPACE

#define MAX_CHARS_FOR_LINE 76

/*!
 * \class QVersitDocumentWriter
 * \internal
 * \brief The QVersitDocumentWriter class provides an interface for writing a
 * single versit document into a vCard text string.
 */

/*! Constructs a writer.
 * \a documentType is the type of Versit document, as printed on the BEGIN line of output
 * eg. "VCARD"
 * \a version is the version of the Versit format, as printed on the VERSION line of output.
 * eg. "2.1"
 */
QVersitDocumentWriter::QVersitDocumentWriter(
    const QByteArray& documentType,
    const QByteArray& version)
    : mDocumentType(documentType),
    mVersion(version)
{
}


/*!
* Encodes the \a document to text using the given \a codec.
*/
QByteArray QVersitDocumentWriter::encodeVersitDocument(const QVersitDocument& document,
                                                       QTextCodec* codec)
{
    QList<QVersitProperty> properties = document.properties();
    QByteArray encodedDocument;

    encodedDocument += codec->fromUnicode(QLatin1String("BEGIN:" + mDocumentType + "\r\n"));
    encodedDocument += codec->fromUnicode(QLatin1String("VERSION:" + mVersion + "\r\n"));
    foreach (QVersitProperty property, properties) {
        encodedDocument.append(encodeVersitProperty(property, codec));
    }
    encodedDocument += codec->fromUnicode(QLatin1String("END:" + mDocumentType + "\r\n"));

    VersitUtils::fold(encodedDocument, MAX_CHARS_FOR_LINE);
    return encodedDocument;
}

/*!
 * Encodes the groups and name in the \a property to text using the given \a codec.
 */
QByteArray QVersitDocumentWriter::encodeGroupsAndName(const QVersitProperty& property,
                                                      QTextCodec* codec) const
{
    QByteArray encodedGroupAndName;
    QStringList groups = property.groups();
    if (!groups.isEmpty()) {
        QString groupAsString = groups.join(QLatin1String("."));
        encodedGroupAndName.append(codec->fromUnicode(groupAsString));
        encodedGroupAndName.append(codec->fromUnicode(QLatin1String(".")));
    }
    encodedGroupAndName.append(codec->fromUnicode(property.name()));
    return encodedGroupAndName;
}
