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
 * Parses a versit document and returns whether parsing succeeded.
 */
bool QVersitReader::parseVersitDocument(QByteArray& text)
{
    // TODO: Store the properties to a QVersitDocument
    text = text.mid(VersitUtils::countLeadingWhiteSpaces(text));
    QPair<QString,QByteArray> nameAndValue = parseNextVersitProperty(text);
    if (nameAndValue.first != "BEGIN" || 
        nameAndValue.second.trimmed() != "VCARD")
        return false;
    while (nameAndValue.first.length() > 0 && nameAndValue.first != "END") {
        nameAndValue = parseNextVersitProperty(text);
        if (nameAndValue.first == "VERSION" && nameAndValue.second != "2.1")
            return false;
    }
    return (nameAndValue.first == "END");
}

/*!
 * Parses a versit document and returns whether parsing succeeded.
 */
QPair<QString,QByteArray> QVersitReader::parseNextVersitProperty(
    QByteArray& text)
{
    QPair<QString,QByteArray> nameAndValue;
    nameAndValue.first = VersitUtils::extractPropertyName(text);
    QMultiMap<QString,QString> params = 
        VersitUtils::extractPropertyParams(text);
    text = VersitUtils::extractPropertyValue(text); 
    if (nameAndValue.first == "AGENT") {
        if (parseVersitDocument(text)) {
            // TODO: Store the property if parsing was successful
        }
    }
    else {
        int crlfPos = -1;
        if (params.contains("ENCODING","QUOTED-PRINTABLE")) {
            crlfPos = VersitUtils::findHardLineBreakInQuotedPrintable(text);
            nameAndValue.second = text.left(crlfPos);
            VersitUtils::decodeQuotedPrintable(nameAndValue.second);
            // TODO: Set the name and value to QVersitProperty
            // TODO: Remove the ENCODING=QUOTED-PRINTABLE parameter
        }
        else {
            crlfPos = text.indexOf("\r\n");
            nameAndValue.second = text.left(crlfPos);
            // TODO: Set the name and value to QVersitProperty
        }
        text = text.mid(crlfPos+2); // +2 is for skipping the CRLF
    }
    // TODO: Set the parameters to QVersitProperty
    return nameAndValue;
}
