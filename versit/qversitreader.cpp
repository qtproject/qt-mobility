/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include "qversitreader.h"
#include "qversitreader_p.h"
#include "versitutils.h"

/*!
 * \class QVersitReader
 *
 * \brief The QVersitReader class provides an interface 
 * for parsing versit documents such as vCards from a stream. 
 *
 * \sa
 */


/*! Construct a reader. */
QVersitReader::QVersitReader() : d(new QVersitReaderPrivate)
{
}
    
/*! Destroy a reader. */    
QVersitReader::~QVersitReader()
{
    delete d;
}

/*!
 * Sets the \a device used for parsing input. 
 */
void QVersitReader::setDevice(QIODevice* device)
{
    d->m_iodevice = device;
}

/*!
 * Returns the \a device used for parsing input. 
 */
QIODevice* QVersitReader::device() const
{
    return d->m_iodevice;
}

/*!
 * Starts parsing the input. 
 */
bool QVersitReader::start()
{
    return false;
}

/*!
 * Returns the parsing result or an empty list 
 * if the parsing was not completed successfully. 
 */
QList<QVersitDocument> QVersitReader::result() const
{
    return d->m_versitDocuments;
}

/*!
 * Parses a versit document and returns the resulting document object
 */
QVersitDocument QVersitReader::parseVersitDocument(QByteArray& text)
{
    QVersitDocument document;
    text = text.mid(VersitUtils::countLeadingWhiteSpaces(text));
    QVersitProperty property = parseNextVersitProperty(text);
    if (property.name() == "BEGIN" && property.value().trimmed() == "VCARD") {
        while (property.name().length() > 0 && property.name() != "END") {
            property = parseNextVersitProperty(text);   
            if (property.name() == "VERSION" && 
                property.value().trimmed() != "2.1")
                return QVersitDocument(); // return an empty document
            if (property.name() != "VERSION" && 
                property.name() != "END")
                document.addProperty(property);
        }
    }
    return document;
}

/*!
 * Parses a versit document and returns whether parsing succeeded.
 */
QVersitProperty QVersitReader::parseNextVersitProperty(QByteArray& text)
{
    QVersitProperty property;
    property.setName(VersitUtils::extractPropertyName(text));
    property.setParameters(VersitUtils::extractPropertyParams(text));
    text = VersitUtils::extractPropertyValue(text); 
    if (property.name() == "AGENT") {
        property.setEmbeddedDocument(parseVersitDocument(text));
    }
    else {
        int crlfPos = -1;
        if (property.parameters().contains("ENCODING","QUOTED-PRINTABLE")) {
            crlfPos = VersitUtils::findHardLineBreakInQuotedPrintable(text);
            QByteArray value = text.left(crlfPos);
            VersitUtils::decodeQuotedPrintable(value);
            // Remove the encoding parameter as the value is now decoded
            property.removeParameter("ENCODING","QUOTED-PRINTABLE");
            property.setValue(value);
        }
        else {
            crlfPos = text.indexOf("\r\n");
            property.setValue(text.left(crlfPos));
        }
        text = text.mid(crlfPos+2); // +2 is for skipping the CRLF
    }
    return property;
}
