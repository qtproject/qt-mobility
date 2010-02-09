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
#include "qversitdocument.h"
#include "versitutils_p.h"
#include "qmobilityglobal.h"
#include <QTextCodec>
#include <QDebug>
#include <QMutexLocker>
#include <QVariant>

QTM_USE_NAMESPACE

// Some big enough value for nested versit documents to prevent infite recursion
#define MAX_VERSIT_DOCUMENT_NESTING_DEPTH 20

/*! Construct a reader. */
QVersitReaderPrivate::QVersitReaderPrivate()
    : mIoDevice(0),
    mDocumentNestingLevel(0),
    mDefaultCodec(QTextCodec::codecForName("UTF-8")),
    mState(QVersitReader::InactiveState),
    mError(QVersitReader::NoError),
    mIsCanceling(false)
{
}
    
/*! Destroy a reader. */    
QVersitReaderPrivate::~QVersitReaderPrivate()
{
}

/*!
 * Does the actual reading and sets the error and state as appropriate.
 * If \a async, then stateChanged() signals are emitted as the reading happens.
 */
void QVersitReaderPrivate::read()
{
    mMutex.lock();
    mVersitDocuments.clear();
    mMutex.unlock();
    QByteArray input = mIoDevice->readAll();
    bool canceled = false;

    VersitCursor cursor(input);
    int oldPos = cursor.position;
    while(cursor.position < input.size()) {
        if (isCanceling()) {
            canceled = true;
            break;
        }
        QVersitDocument document;
        bool ok = parseVersitDocument(cursor, document);

        if (ok) {
            if (document.isEmpty())
                break;
            else {
                QMutexLocker locker(&mMutex);
                mVersitDocuments.append(document);
                emit resultsAvailable(mVersitDocuments);
            }
        } else {
            setError(QVersitReader::ParseError);
            if (cursor.position == oldPos)
                break;
        }

        oldPos = cursor.position;
    };
    if (canceled)
        setState(QVersitReader::CanceledState);
    else
        setState(QVersitReader::FinishedState);
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
bool QVersitReaderPrivate::parseVersitDocument(VersitCursor& cursor, QVersitDocument& document,
                                               bool foundBegin)
{
    if (mDocumentNestingLevel >= MAX_VERSIT_DOCUMENT_NESTING_DEPTH)
        return false; // To prevent infinite recursion

    bool parsingOk = true;
    mDocumentNestingLevel++;

    // TODO: Various readers should be made subclasses and eliminate assumptions like this.
    // We don't know what type it is: just assume it's a vCard 3.0
    document.setType(QVersitDocument::VCard30Type);

    // Skip some leading space
    if (!foundBegin)
        VersitUtils::skipLeadingWhiteSpaces(cursor, mDefaultCodec);

    QVersitProperty property;

    if (!foundBegin) {
        property = parseNextVersitProperty(document.type(), cursor);
        if (property.name() == QLatin1String("BEGIN")
            && property.value().trimmed().toUpper() == QLatin1String("VCARD")) {
            foundBegin = true;
        } else if (property.isEmpty()) {
            // A blank document (or end of file) was found.
            document = QVersitDocument();
        } else {
            // Some property other than BEGIN was found.
            parsingOk = false;
        }
    }
    
    if (foundBegin) {
        do {
            /* Grab it */
            property = parseNextVersitProperty(document.type(), cursor);

            /* Discard embedded vcard documents - not supported yet.  Discard the entire vCard */
            if (property.name() == QLatin1String("BEGIN") &&
                property.value().trimmed().toUpper() == QLatin1String("VCARD")) {
                parsingOk = false;
                QVersitDocument nestedDocument;
                if (!parseVersitDocument(cursor, nestedDocument, true))
                    break;
            }

            // See if this is a version property and continue parsing under that version
            if (!setVersionFromProperty(document, property)) {
                parsingOk = false;
                break;
            }

            /* Nope, something else.. just add it */
            if (property.name() != QLatin1String("VERSION") &&
                property.name() != QLatin1String("END"))
                document.addProperty(property);
        } while (property.name().length() > 0 && property.name() != QLatin1String("END"));
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
    if (versitType != QVersitDocument::VCard21Type && versitType != QVersitDocument::VCard30Type)
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

    if (versitType == QVersitDocument::VCard21Type)
        parseVCard21Property(cursor, property);
    else if (versitType == QVersitDocument::VCard30Type)
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

    if (property.name() == QLatin1String("AGENT")) {
        // Well, try to parse the agent thing
        cursor.setPosition(origPos); // XXX backtracks a little..
        parseAgentProperty(cursor, property);
    } else {
        QTextCodec* codec;
        QVariant valueVariant(decodeCharset(value, property, &codec));
        unencode(valueVariant, cursor, property, codec);
        if (valueVariant.type() == QVariant::ByteArray) {
            // hack: add the charset parameter back in (even if there wasn't one to start with and
            // the default codec was used).  This will help later on if someone calls valueString()
            // on the property.
            property.insertParameter(QLatin1String("CHARSET"), QLatin1String(codec->name()));
        }
        property.setValue(valueVariant);
    }
}

/*!
 * Parses the property according to vCard 3.0 syntax.
 */
void QVersitReaderPrivate::parseVCard30Property(VersitCursor& cursor, QVersitProperty& property)
{
    property.setParameters(VersitUtils::extractVCard30PropertyParams(cursor, mDefaultCodec));

    QByteArray value = VersitUtils::extractPropertyValue(cursor);

    QTextCodec* codec;
    QString valueString(decodeCharset(value, property, &codec));
    VersitUtils::removeBackSlashEscaping(valueString);

    if (property.name() == QLatin1String("AGENT")) {
        // this means 2.1 agent handling doesn't work (you only get the first line)
        VersitCursor agentCursor(valueString.toAscii());
        parseAgentProperty(agentCursor, property);
    } else {
        QVariant valueVariant(valueString);
        unencode(valueVariant, cursor, property, codec);
        if (valueVariant.type() == QVariant::ByteArray) {
            // hack: add the charset parameter back in (even if there wasn't one to start with and
            // the default codec was used).  This will help later on if someone calls valueString()
            // on the property.
            property.insertParameter(QLatin1String("CHARSET"), QLatin1String(codec->name()));
        }
        property.setValue(valueVariant);
    }
}

/*!
 * Parses the value of AGENT \a property from \a text
 */
void  QVersitReaderPrivate::parseAgentProperty(VersitCursor& cursor, QVersitProperty& property)
{
    QVersitDocument agentDocument;

    // Get rid of AGENT, find the meat..
    VersitUtils::getNextLine(cursor, mDefaultCodec);

    if (!parseVersitDocument(cursor, agentDocument)) {
        property = QVersitProperty();
    } else {
        property.setValue(QVariant::fromValue(agentDocument));
    }
}

/*!
 * Sets version to \a document if \a property contains a supported version.
 */
bool QVersitReaderPrivate::setVersionFromProperty(QVersitDocument& document, const QVersitProperty& property) const
{
    bool valid = true;
    if (property.name() == QLatin1String("VERSION")) {
        QString value = property.value().trimmed();
        if (property.parameters().contains(
                QLatin1String("ENCODING"),QLatin1String("BASE64")))
            value = QLatin1String(QByteArray::fromBase64(value.toAscii()));
        if (value == QLatin1String("2.1")) {
            document.setType(QVersitDocument::VCard21Type);
        } else if (value == QLatin1String("3.0")) {
            document.setType(QVersitDocument::VCard30Type);
        } else {
            valid = false;
        }
    } 
    return valid;
}

/*!
 * On entry, \a value should hold a QString.  On exit, it may be either a QString or a QByteArray.
 */
void QVersitReaderPrivate::unencode(QVariant& value, VersitCursor& cursor,
                                    QVersitProperty& property, QTextCodec* codec) const
{
    const QString encoding(QLatin1String("ENCODING"));
    const QString quotedPrintable(QLatin1String("QUOTED-PRINTABLE"));
    const QString base64(QLatin1String("BASE64"));
    const QString b(QLatin1String("B"));
    Q_ASSERT(value.type() == QVariant::String);

    QString valueString = value.toString();

    if (property.parameters().contains(encoding, quotedPrintable)) {
        // At this point, we need to accumulate bytes until we hit a real line break (no = before
        // it) value already contains everything up to the character before the newline
        while (valueString.endsWith(QLatin1Char('='))) {
            valueString.chop(1); // Get rid of '='
            // We add each line (minus the escaped = and newline chars)
            VersitUtils::getNextLine(cursor, codec);
            QString line = codec->toUnicode(
                    cursor.data.mid(cursor.position, cursor.selection-cursor.position));
            valueString.append(line);
            cursor.setPosition(cursor.selection);
        }
        VersitUtils::decodeQuotedPrintable(valueString);
        // Remove the encoding parameter as the value is now decoded
        property.removeParameters(encoding);
        value.setValue(valueString);
    } else if (property.parameters().contains(encoding, base64)
        || property.parameters().contains(encoding, b)) {
        // Remove any left-over linear whitespaces
        valueString.remove(QLatin1Char(' '));
        valueString.remove(QLatin1Char('\t'));
        // and do the conversion.
        value.setValue(QByteArray::fromBase64(valueString.toAscii()));
        // Remove the encoding parameter as the value is now decoded
        property.removeParameters(encoding);
    }
}

/*!
 * Decodes \a value, after working out what charset it is in using the context of \a property and
 * returns it.  The codec used to decode is returned in \a codec.
 */
QString QVersitReaderPrivate::decodeCharset(const QByteArray& value,
                                            QVersitProperty& property,
                                            QTextCodec** codec) const
{
    const QString charset(QLatin1String("CHARSET"));
    if (property.parameters().contains(charset)) {
        QString charsetValue = *property.parameters().find(charset);
        property.removeParameters(charset);
        *codec = QTextCodec::codecForName(charsetValue.toAscii());
        if (*codec != NULL) {
            return (*codec)->toUnicode(value);
        } else {
            *codec = mDefaultCodec;
            return mDefaultCodec->toUnicode(value);
        }
    }
    *codec = mDefaultCodec;
    return mDefaultCodec->toUnicode(value);
}

void QVersitReaderPrivate::setState(QVersitReader::State state)
{
    mMutex.lock();
    mState = state;
    mMutex.unlock();
    emit stateChanged(state);
}

QVersitReader::State QVersitReaderPrivate::state() const
{
    QMutexLocker locker(&mMutex);
    return mState;
}

void QVersitReaderPrivate::setError(QVersitReader::Error error)
{
    QMutexLocker locker(&mMutex);
    mError = error;
}

QVersitReader::Error QVersitReaderPrivate::error() const
{
    QMutexLocker locker(&mMutex);
    return mError;
}

void QVersitReaderPrivate::setCanceling(bool canceling)
{
    QMutexLocker locker(&mMutex);
    mIsCanceling = canceling;
}

bool QVersitReaderPrivate::isCanceling()
{
    QMutexLocker locker(&mMutex);
    return mIsCanceling;
}

#include "moc_qversitreader_p.cpp"
