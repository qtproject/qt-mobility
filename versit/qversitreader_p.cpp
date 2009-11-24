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

#include "qversitreader_p.h"
#include "versitutils.h"

// Some big enough value for nested versit documents to prevent infite recursion
#define MAX_VERSIT_DOCUMENT_NESTING_DEPTH 20

/*! Construct a reader. */
QVersitReaderPrivate::QVersitReaderPrivate() : 
    mIoDevice(0), 
    mDocumentNestingLevel(0)
{
}
    
/*! Destroy a reader. */    
QVersitReaderPrivate::~QVersitReaderPrivate()
{
}

/*!
 * Checks whether the reader is ready for reading.
 */
bool QVersitReaderPrivate::isReady() const
{
    return (mIoDevice && mIoDevice->isOpen());
}

/*!
 * Inherited from QThread. Does the actual reading.
 */
bool QVersitReaderPrivate::read()
{
    mVersitDocuments.clear();
    if (isReady()) {
        QByteArray input = mIoDevice->readAll();
        VersitUtils::unfold(input);
        while (input.length() > 0) {
            QVersitDocument document;
            if (parseVersitDocument(input,document) &&
                document.properties().count() > 0)
                mVersitDocuments.append(document);
        }
    }

    return (mVersitDocuments.count() > 0);
}

/*!
 * Inherited from QThread, called by QThread when the thread has been started.
 */
void QVersitReaderPrivate::run()
{
    read();
}

/*!
 * Parses a versit document. Returns true if the parsing was successful.
 */
bool QVersitReaderPrivate::parseVersitDocument(
    QByteArray& text,
    QVersitDocument& document)
{
    if (mDocumentNestingLevel >= MAX_VERSIT_DOCUMENT_NESTING_DEPTH)
        return false; // To prevent infinite recursion
    bool parsingOk = true;
    mDocumentNestingLevel++;
    text = text.mid(VersitUtils::countLeadingWhiteSpaces(text));
    QVersitProperty property =
        parseNextVersitProperty(document.versitType(),text);
    if (property.name() == QString::fromAscii("BEGIN") && 
        property.value().trimmed().toUpper() == "VCARD") {
        while (property.name().length() > 0 &&
               property.name() != QString::fromAscii("END")) {
            property = parseNextVersitProperty(document.versitType(),text);
            if (property.name() == QString::fromAscii("BEGIN") &&
                property.value().trimmed().toUpper() == "VCARD") {
                parsingOk = false;
                // Parse through the embedded cards, but don't save them
                text.prepend("BEGIN:VCARD\r\n");
                QVersitDocument nestedDocument;
                if (!parseVersitDocument(text,nestedDocument))
                    break;
            }
            if (!setVersionFromProperty(document,property)) {
                parsingOk = false;
                break;
            }
            if (property.name() != QString::fromAscii("VERSION") && 
                property.name() != QString::fromAscii("END"))
                document.addProperty(property);

        }
    }
    mDocumentNestingLevel--;
    if (!parsingOk)
        document = QVersitDocument();
    return parsingOk;
}

/*!
 * Parses a versit document and returns whether parsing succeeded.
 */
QVersitProperty QVersitReaderPrivate::parseNextVersitProperty(
    QVersitDocument::VersitType versitType,
    QByteArray& text)
{
    QVersitProperty property;
    QPair<QStringList,QString> groupsAndName =
        VersitUtils::extractPropertyGroupsAndName(text);
    property.setGroups(groupsAndName.first);
    property.setName(groupsAndName.second);
    if (versitType == QVersitDocument::VCard21)
        parseVCard21Property(text,property);
    else if (versitType == QVersitDocument::VCard30)
        parseVCard30Property(text,property);
    else
        return QVersitProperty(); // type not supported
    return property;
}

/*!
 * Parses the property according to vCard 2.1 syntax.
 */
void QVersitReaderPrivate::parseVCard21Property(
    QByteArray& text,
    QVersitProperty& property)
{
    property.setParameters(VersitUtils::extractVCard21PropertyParams(text));
    text = VersitUtils::extractPropertyValue(text);
    if (property.name() == QString::fromAscii("AGENT")) {
        parseAgentProperty(text,property);
    } else {
        int crlfPos = -1;
        QString encoding(QString::fromAscii("ENCODING"));
        QString quotedPrintable(QString::fromAscii("QUOTED-PRINTABLE"));
        if (property.parameters().contains(encoding,quotedPrintable)) {
            crlfPos = VersitUtils::findHardLineBreakInQuotedPrintable(text);
            QByteArray value = text.left(crlfPos);
            VersitUtils::decodeQuotedPrintable(value);
            // Remove the encoding parameter as the value is now decoded
            property.removeParameter(encoding,quotedPrintable);
            property.setValue(value);
        } else {
            crlfPos = text.indexOf("\r\n");
            QByteArray value = text.left(crlfPos);
            QString base64(QString::fromAscii("BASE64"));
            if (property.parameters().contains(encoding,base64)) {
                // Remove the linear whitespaces left by vCard 2.1 unfolding
                value.replace(' ',"");
                value.replace('\t',"");
            }
            property.setValue(value);
        }
        text = text.mid(crlfPos+2); // +2 is for skipping the CRLF
    }
}

/*!
 * Parses the property according to vCard 3.0 syntax.
 */
void QVersitReaderPrivate::parseVCard30Property(
    QByteArray& text,
    QVersitProperty& property)
{
    property.setParameters(VersitUtils::extractVCard30PropertyParams(text));
    text = VersitUtils::extractPropertyValue(text);
    int crlfPos = text.indexOf("\r\n");
    QByteArray value = text.left(crlfPos);
    VersitUtils::removeBackSlashEscaping(value);
    if (property.name() == QString::fromAscii("AGENT")) {
        parseAgentProperty(value,property);
    } else {
        property.setValue(value);
    }
    text = text.mid(crlfPos+2); // +2 is for skipping the CRLF
}

/*!
 * Parses the value of AGENT \a property from \a text
 */
void QVersitReaderPrivate::parseAgentProperty(
    QByteArray& text,
    QVersitProperty& property)
{
    QVersitDocument agentDocument;
    if (!parseVersitDocument(text,agentDocument)) {
        property = QVersitProperty();
    } else {
        property.setEmbeddedDocument(agentDocument);
    }
}

/*!
 * Sets version to \a document if \a property contains a supported version.
 */
bool QVersitReaderPrivate::setVersionFromProperty(
    QVersitDocument& document,
    const QVersitProperty& property) const
{
    bool valid = true;
    if (property.name() == QString::fromAscii("VERSION")) {
        QByteArray value = property.value().trimmed();
        if (property.parameters().contains(
                QString::fromAscii("ENCODING"),QString::fromAscii("BASE64")))
            value = QByteArray::fromBase64(value);
        if (value == "2.1") {
            document.setVersitType(QVersitDocument::VCard21);
        } else if (value == "3.0") {
            document.setVersitType(QVersitDocument::VCard30);         
        } else {
            valid = false;
        }
    } 
    return valid;
}
