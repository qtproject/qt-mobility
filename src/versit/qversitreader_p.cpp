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
#include "versitutils_p.h"
#include "qmobilityglobal.h"
#include <QTextCodec>
#include <QDebug>

QTM_BEGIN_NAMESPACE

// Some big enough value for nested versit documents to prevent infite recursion
#define MAX_VERSIT_DOCUMENT_NESTING_DEPTH 20

/*! Construct a reader. */
QVersitReaderPrivate::QVersitReaderPrivate()
    : mIoDevice(0),
    mDocumentNestingLevel(0),
    mDefaultCodec(QTextCodec::codecForName("ISO 8859-1"))
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

        VersitCursor cursor(input);
        int oldPos = cursor.position;
        do {
            QVersitDocument document;
            bool ret = parseVersitDocument(cursor, document);

            if (ret && document.properties().count() > 0)
                mVersitDocuments.append(document);

            if (!ret && cursor.position == oldPos)
                break;

            oldPos = cursor.position;
        } while(cursor.position < input.size());
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
bool QVersitReaderPrivate::parseVersitDocument(VersitCursor& cursor, QVersitDocument& document, bool foundBegin)
{
    if (mDocumentNestingLevel >= MAX_VERSIT_DOCUMENT_NESTING_DEPTH)
        return false; // To prevent infinite recursion

    bool parsingOk = false;
    mDocumentNestingLevel++;

    // Skip some leading space
    if (!foundBegin)
        cursor.setPosition(cursor.position + VersitUtils::countLeadingWhiteSpaces(cursor.data, cursor.position));

    QVersitProperty property;

    if (!foundBegin)
        property = parseNextVersitProperty(document.versitType(), cursor);
    
    if (foundBegin ||
        (property.name() == QString::fromAscii("BEGIN")
        && property.value().trimmed().toUpper() == QString::fromAscii("VCARD"))) {
        
        parsingOk = true;
        do {
            /* Grab it */
            property = parseNextVersitProperty(document.versitType(), cursor);

            /* Discard embedded vcard documents (XXX why?) */
            if (property.name() == QString::fromAscii("BEGIN") &&
                property.value().trimmed().toUpper() == QString::fromAscii("VCARD")) {
                parsingOk = false;
                QVersitDocument nestedDocument;
                if (!parseVersitDocument(cursor, nestedDocument, true))
                    break;
            }

            /* See if this is a version property - we might need to restart parsing as 3.0 */
            if (!setVersionFromProperty(document, property)) {
                parsingOk = false;
                break;
            }

            /* Nope, something else.. just add it */
            if (property.name() != QString::fromAscii("VERSION") && 
                property.name() != QString::fromAscii("END"))
                document.addProperty(property);
        } while (property.name().length() > 0 && property.name() != QString::fromAscii("END"));
    }
    mDocumentNestingLevel--;
    if (!parsingOk)
        document = QVersitDocument();

    return parsingOk;
}

/*!
 * Parses a versit document and returns whether parsing succeeded.
 */
QVersitProperty QVersitReaderPrivate::parseNextVersitProperty(QVersitDocument::VersitType versitType, VersitCursor& cursor)
{
    if (versitType != QVersitDocument::VCard21 && versitType != QVersitDocument::VCard30)
        return QVersitProperty();

    // Make sure we have a line
    if (!VersitUtils::getNextLine(cursor, mDefaultCodec))
        return QVersitProperty();

//    qDebug() << "Current line is" << cursor.position << cursor.selection << cursor.data.mid(cursor.position, cursor.selection - cursor.position);

    // Otherwise, do stuff.
    QPair<QStringList,QString> groupsAndName =
            VersitUtils::extractPropertyGroupsAndName(cursor, mDefaultCodec);

    QVersitProperty property;
    property.setGroups(groupsAndName.first);
    property.setName(groupsAndName.second);

    if (versitType == QVersitDocument::VCard21)
        parseVCard21Property(cursor, property);
    else if (versitType == QVersitDocument::VCard30)
        parseVCard30Property(cursor, property);

    return property;
}

/*!
 * Parses the property according to vCard 2.1 syntax.
 */
void QVersitReaderPrivate::parseVCard21Property(VersitCursor& cursor, QVersitProperty& property)
{
    property.setParameters(VersitUtils::extractVCard21PropertyParams(cursor, mDefaultCodec));

    int origPos = cursor.position;
    QByteArray value = VersitUtils::extractPropertyValue(cursor);

    if (property.name() == QString::fromAscii("AGENT")) {
        // Well, try to parse the agent thing
        cursor.setPosition(origPos); // XXX backtracks a little..
        parseAgentProperty(cursor, property);
    } else {
        unencode(value, cursor, property);
        property.setValue(decodeCharset(value, property));
    }
}

/*!
 * Parses the property according to vCard 3.0 syntax.
 */
void QVersitReaderPrivate::parseVCard30Property(VersitCursor& cursor, QVersitProperty& property)
{
    property.setParameters(VersitUtils::extractVCard30PropertyParams(cursor, mDefaultCodec));

    QByteArray value = VersitUtils::extractPropertyValue(cursor);

    QString valueString(QString::fromAscii(value));
    VersitUtils::removeBackSlashEscaping(valueString);

    if (property.name() == QString::fromAscii("AGENT")) {
        // XXX this means 2.1 agent handling doesn't work (you only get the first line)
        VersitCursor agentCursor(valueString.toAscii());
        parseAgentProperty(agentCursor, property);
    } else {
        property.setValue(valueString);
    }
}

/*!
 * Parses the value of AGENT \a property from \a text
 */
void QVersitReaderPrivate::parseAgentProperty(VersitCursor& cursor, QVersitProperty& property)
{
    QVersitDocument agentDocument;

    // Get rid of AGENT, find the meat..
    VersitUtils::getNextLine(cursor, mDefaultCodec);

    if (!parseVersitDocument(cursor, agentDocument)) {
        property = QVersitProperty();
    } else {
        property.setEmbeddedDocument(agentDocument);
    }
}

/*!
 * Sets version to \a document if \a property contains a supported version.
 */
bool QVersitReaderPrivate::setVersionFromProperty(QVersitDocument& document, const QVersitProperty& property) const
{
    bool valid = true;
    if (property.name() == QString::fromAscii("VERSION")) {
        QString value = property.value().trimmed();
        if (property.parameters().contains(
                QString::fromAscii("ENCODING"),QString::fromAscii("BASE64")))
            value = QString::fromAscii(QByteArray::fromBase64(value.toAscii()));
        if (value == QString::fromAscii("2.1")) {
            document.setVersitType(QVersitDocument::VCard21);
        } else if (value == QString::fromAscii("3.0")) {
            document.setVersitType(QVersitDocument::VCard30);         
        } else {
            valid = false;
        }
    } 
    return valid;
}

void QVersitReaderPrivate::unencode(QByteArray& value, VersitCursor& cursor, QVersitProperty& property) const
{
    const QString encoding(QString::fromAscii("ENCODING"));
    const QString quotedPrintable(QString::fromAscii("QUOTED-PRINTABLE"));
    const QString base64(QString::fromAscii("BASE64"));

    if (property.parameters().contains(encoding, quotedPrintable)) {
        // At this point, we need to accumulate bytes until we hit a real line break (no = before it)
        // value already contains everything up to the character before the newline
        if (value.at(value.length() - 1) == '=') {
            value.chop(1); // Get rid of '='
            // We add each line (minus the escaped = and newline chars)
            while(VersitUtils::getNextLine(cursor, mDefaultCodec)) {
                if (cursor.data.at(cursor.selection - 1) == '=') {
                    value.append(cursor.data.mid(cursor.position, cursor.selection - cursor.position - 1));
                    cursor.setPosition(cursor.selection);
                    // another escaped newline.. loop again
                } else {
                    value.append(cursor.data.mid(cursor.position, cursor.selection - cursor.position));
                    cursor.setPosition(cursor.selection);
                    // We've reached the end
                    break;
                }
            }
        }
        VersitUtils::decodeQuotedPrintable(value);
        // Remove the encoding parameter as the value is now decoded
        property.removeParameter(encoding, quotedPrintable);
    } else if (property.parameters().contains(encoding, base64)) {
        // Remove the linear whitespaces left by vCard 2.1 unfolding
        // XXX why is this necessary?
        value.replace(' ',"");
        value.replace('\t',"");
    }
}

QString QVersitReaderPrivate::decodeCharset(const QByteArray& value, QVersitProperty& property) const
{
    const QString charset(QString::fromAscii("CHARSET"));
    if (property.parameters().contains(charset)) {
        QString charsetValue = *property.parameters().find(charset);
        property.removeParameter(charset, charsetValue);
        QTextCodec *codec = QTextCodec::codecForName(charsetValue.toAscii());
        if (codec != NULL) {
            return codec->toUnicode(value);
        } else {
            return QString::fromAscii(value);
        }
    }
    return QString::fromAscii(value);
}

#include "moc_qversitreader_p.cpp"

QTM_END_NAMESPACE
