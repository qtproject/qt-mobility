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

#include "qversitdocumentwriter_p.h"
#include "versitutils_p.h"
#include <QTextCodec>

QTM_USE_NAMESPACE

#define MAX_LINE_LENGTH 76

/*!
  \class QVersitDocumentWriter
  \internal
  \brief The QVersitDocumentWriter class provides an interface for writing a
  single versit document into a vCard text string.
 */

/*! Constructs a writer.
 * \a documentType is the type of Versit document, as printed on the BEGIN line of output
 * eg. "VCARD"
 * \a version is the version of the Versit format, as printed on the VERSION line of output.
 * eg. "2.1"
 */
QVersitDocumentWriter::QVersitDocumentWriter()
    : mCodec(0),
    mEncoder(0),
    mUtf8Encoder(QTextCodec::codecForName("UTF-8")->makeEncoder()),
    mSuccessful(true),
    mCurrentLineLength(0)
{
    // Hack so the encoder doesn't output a byte order mark for UTF-8.
    mUtf8Encoder->fromUnicode(QString());
}

QVersitDocumentWriter::~QVersitDocumentWriter()
{
    if (mEncoder)
        delete mEncoder;
    delete mUtf8Encoder;
}

/*!
  Sets the codec to write with.
  */
void QVersitDocumentWriter::setCodec(QTextCodec *codec)
{
    if (mEncoder)
        delete mEncoder;
    mCodec = codec;
    mEncoder = codec->makeEncoder();

    // Hack so the encoder doesn't output a byte order mark for UTF-8.
    if (mCodec->name() == "UTF-8")
        mEncoder->fromUnicode(QString());
}

/*!
  Sets the device to write to.
  */
void QVersitDocumentWriter::setDevice(QIODevice *device)
{
    mDevice = device;
}

/*!
* Encodes the \a document and writes it to the device
*/
void QVersitDocumentWriter::encodeVersitDocument(const QVersitDocument& document)
{
    mSuccessful = true;
    QList<QVersitProperty> properties = document.properties();

    writeString(QLatin1String("BEGIN:") + document.componentType());
    writeCrlf();
    switch (document.type()) {
    case QVersitDocument::VCard21Type:
        writeString(QLatin1String("VERSION:2.1"));
        writeCrlf();
        break;
    case QVersitDocument::VCard30Type:
        writeString(QLatin1String("VERSION:3.0"));
        writeCrlf();
        break;
    case QVersitDocument::ICalendar20Type:
        writeString(QLatin1String("VERSION:2.0"));
        writeCrlf();
        break;
    default:
        ; // don't print version
    }

    foreach (const QVersitProperty& property, properties) {
        encodeVersitProperty(property);
    }
    writeString(QLatin1String("END:") + document.componentType());
    writeCrlf();
}

/*!
 * Encodes the groups and name in the \a property and writes it to the device
 */
void QVersitDocumentWriter::encodeGroupsAndName(const QVersitProperty& property)
{
    QStringList groups = property.groups();
    if (!groups.isEmpty()) {
        writeString(groups.join(QLatin1String(".")));
        writeString(QLatin1String("."));
    }
    writeString(property.name());
}

/*!
  Writes \a string to the device.
  If \a useUtf8 is true, uses the UTF-8 codec instead of the one set in setCodec().

  This function tracks how many characters have been written to the line and folds (wraps) the line
  according to RFC2425.
  */
void QVersitDocumentWriter::writeString(const QString &string, bool useUtf8)
{
    QString value(string); // nonconst copy
    QTextEncoder* encoder = useUtf8 ? mUtf8Encoder : mEncoder;
    int spaceRemaining = MAX_LINE_LENGTH - mCurrentLineLength;
    while (spaceRemaining < value.length()) {
        // Write the first "spaceRemaining" characters
        QString line(value.left(spaceRemaining));
        value.remove(0, spaceRemaining);
        if (mDevice->write(encoder->fromUnicode(line + QLatin1String("\r\n "))) < 0)
            mSuccessful = false;
        spaceRemaining = MAX_LINE_LENGTH - 1; // minus 1 for the space at the front.
        mCurrentLineLength = 1;
    }

    if (mDevice->write(encoder->fromUnicode(value)) < 0)
        mSuccessful = false;
    mCurrentLineLength += value.length();
}

/*!
  Writes a CRLF to the device.  By using this function, rather than writeString("\\r\\n"), you will
  allow the writer to know where a line starts, for folding purposes.
  */
void QVersitDocumentWriter::writeCrlf()
{
    writeString(QLatin1String("\r\n"));
    mCurrentLineLength = 0;
}
