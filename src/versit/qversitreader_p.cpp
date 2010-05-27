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

#include "qversitreader_p.h"
#include "qversitdocument.h"
#include "versitutils_p.h"
#include "qmobilityglobal.h"
#include <QTextCodec>
#include <QMutexLocker>
#include <QVariant>
#include <QBuffer>

QTM_USE_NAMESPACE

// Some big enough value for nested versit documents to prevent infinite recursion
#define MAX_VERSIT_DOCUMENT_NESTING_DEPTH 20

/*!
  \class LineReader
  \brief The LineReader class is a wrapper around a QIODevice that allows line-by-line reading.
  \internal

  This class keeps an internal buffer which it uses to temporarily store data which it has read from
  the device but not returned to the user.
 */

/*!
  Constructs a LineReader that reads from the given \a device using the given \a codec.
  \a chunkSize is the number of bytes to read at a time (it is useful for testing but shouldn't
  otherwise be set).
  */
LineReader::LineReader(QIODevice* device, QTextCodec *codec, int chunkSize)
    : mDevice(device),
    mCodec(codec),
    mChunkSize(chunkSize),
    mCrlfList(*VersitUtils::newlineList(mCodec)),
    mBuffer(LByteArray(QByteArray())),
    mOdometer(0)
{
}

/*!
  Attempts to read a line and returns an LByteArray containing the line.
  */
LByteArray LineReader::readLine()
{
    if (!mFirstLine.isEmpty()) {
        LByteArray retval(mFirstLine);
        mFirstLine.clear();
        return retval;
    }
    mBuffer.mStart = mBuffer.mEnd;
    mSearchFrom = mBuffer.mStart;

    // First, look for a newline in the already-existing buffer.  If found, return the line.
    if (tryReadLine(mBuffer, false)) {
        mBuffer.dropOldData();
        mOdometer += mBuffer.size();
        return mBuffer;
    }

    // Otherwise, keep reading more data until either a CRLF is found, or there's no more to read.
    while (!mDevice->atEnd()) {
        QByteArray temp = mDevice->read(mChunkSize);
        if (!temp.isEmpty()) {
            mBuffer.mData.append(temp);
            if (tryReadLine(mBuffer, false)) {
                mBuffer.dropOldData();
                mOdometer += mBuffer.size();
                return mBuffer;
            }
        } else {
            mDevice->waitForReadyRead(500);
        }
    }

    // We've reached the end of the stream.  Find a newline from the buffer (or return what's left).
    tryReadLine(mBuffer, true);
    mBuffer.dropOldData();
    mOdometer += mBuffer.size();
    return mBuffer;
}

/*!
  Push a line onto the front of the line reader so it will be returned on the next call to readLine().
  */
void LineReader::pushLine(const QByteArray& line)
{
    mFirstLine = line;
}

/*!
  How many bytes have been returned in the LByteArray in the lifetime of the LineReader.
 */
int LineReader::odometer()
{
    return mOdometer;
}

/*!
  Returns true if there are no more lines left for readLine() to return.  It is possible for atEnd()
  to return false and for there to be no more data left (eg. if there are trailing newlines at the
  end of the input.  In this case, readLine() will return an empty line.
 */
bool LineReader::atEnd()
{
    return mFirstLine.isEmpty() && mDevice->atEnd() && mBuffer.mEnd == mBuffer.mData.size();
}

/*!
  Returns the codec that the LineReader reads with.
 */
QTextCodec* LineReader::codec()
{
    return mCodec;
}

/*!
 * Get the next line of input from \a device to parse.  Also performs unfolding by removing
 * sequences of newline-space from the retrieved line.  Skips over any newlines at the start of the
 * input.
 *
 * Returns an LByteArray containing the line.
 */
bool LineReader::tryReadLine(LByteArray &cursor, bool atEnd)
{
    int crlfPos = -1;

    QByteArray space = VersitUtils::encode(' ', mCodec);
    QByteArray tab = VersitUtils::encode('\t', mCodec);
    int spaceLength = space.length();

    forever {
        foreach(const QByteArrayMatcher& crlf, mCrlfList) {
            int crlfLength = crlf.pattern().length();
            crlfPos = crlf.indexIn(cursor.mData, mSearchFrom);
            if (crlfPos == cursor.mStart) {
                // Newline at start of line.  Set mStart to directly after it.
                cursor.mStart += crlfLength;
                mSearchFrom = cursor.mStart;
                break;
            } else if (crlfPos > cursor.mStart) {
                // Found the CRLF.
                if (QVersitReaderPrivate::containsAt(cursor.mData, space, crlfPos + crlfLength)
                    || QVersitReaderPrivate::containsAt(cursor.mData, tab, crlfPos + crlfLength)) {
                    // If it's followed by whitespace, collapse it.
                    cursor.mData.remove(crlfPos, crlfLength + spaceLength);
                    mSearchFrom = crlfPos;
                    break;
                } else if (!atEnd && crlfPos + crlfLength + spaceLength >= cursor.mData.size()) {
                    // If our CRLF is at the end of the current buffer but there's more to read,
                    // it's possible that a space could be hiding on the next read from the device.
                    // Just pretend we didn't see the CRLF and pick it up the next time round.
                    mSearchFrom = crlfPos;
                    return false;
                } else {
                    // Found the CRLF.
                    cursor.mEnd = crlfPos;
                    return true;
                }
            }
        }
        if (crlfPos == -1) {
            // No CRLF found.
            cursor.mEnd = cursor.mData.size();
            return false;
        }
    }
}

/*! Links the signals from this to the signals of \a reader. */
void QVersitReaderPrivate::init(QVersitReader* reader)
{
    qRegisterMetaType<QVersitReader::State>("QVersitReader::State");
    connect(this, SIGNAL(stateChanged(QVersitReader::State)),
            reader, SIGNAL(stateChanged(QVersitReader::State)),Qt::DirectConnection);
    connect(this, SIGNAL(resultsAvailable()),
            reader, SIGNAL(resultsAvailable()), Qt::DirectConnection);
}

/*! Construct a reader. */
QVersitReaderPrivate::QVersitReaderPrivate()
    : mIoDevice(0),
    mDocumentNestingLevel(0),
    mDefaultCodec(QTextCodec::codecForName("UTF-8")),
    mState(QVersitReader::InactiveState),
    mError(QVersitReader::NoError),
    mIsCanceling(false)
{
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard21Type, QString::fromAscii("AGENT")),
                         QVersitProperty::VersitDocumentType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard30Type, QString::fromAscii("AGENT")),
                         QVersitProperty::VersitDocumentType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard21Type, QString::fromAscii("N")),
                         QVersitProperty::CompoundType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard30Type, QString::fromAscii("N")),
                         QVersitProperty::CompoundType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard21Type, QString::fromAscii("ADR")),
                         QVersitProperty::CompoundType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard30Type, QString::fromAscii("ADR")),
                         QVersitProperty::CompoundType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard21Type, QString::fromAscii("GEO")),
                         QVersitProperty::CompoundType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard30Type, QString::fromAscii("GEO")),
                         QVersitProperty::CompoundType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard21Type, QString::fromAscii("ORG")),
                         QVersitProperty::CompoundType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard30Type, QString::fromAscii("ORG")),
                         QVersitProperty::CompoundType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard21Type, QString::fromAscii("NICKNAME")),
                         QVersitProperty::ListType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard30Type, QString::fromAscii("NICKNAME")),
                         QVersitProperty::ListType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard21Type, QString::fromAscii("CATEGORIES")),
                         QVersitProperty::ListType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard30Type, QString::fromAscii("CATEGORIES")),
                         QVersitProperty::ListType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard21Type, QString::fromAscii("X-CHILDREN")),
                         QVersitProperty::ListType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard30Type, QString::fromAscii("X-CHILDREN")),
                         QVersitProperty::ListType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard21Type, QString::fromAscii("X-NICKNAME")),
                         QVersitProperty::ListType);
    mValueTypeMap.insert(qMakePair(QVersitDocument::VCard30Type, QString::fromAscii("X-NICKNAME")),
                         QVersitProperty::ListType);
}

/*! Destroy a reader. */
QVersitReaderPrivate::~QVersitReaderPrivate()
{
}

/*!
 * Inherited from QThread, called by QThread when the thread has been started.
 */
void QVersitReaderPrivate::run()
{
    read();
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
    bool canceled = false;

    LineReader lineReader(mIoDevice, mDefaultCodec);
    while(!lineReader.atEnd()) {
        if (isCanceling()) {
            canceled = true;
            break;
        }
        QVersitDocument document;
        int oldPos = lineReader.odometer();
        bool ok = parseVersitDocument(lineReader, document);

        if (ok) {
            if (document.isEmpty())
                break;
            else {
                QMutexLocker locker(&mMutex);
                mVersitDocuments.append(document);
                emit resultsAvailable();
            }
        } else {
            setError(QVersitReader::ParseError);
            if (lineReader.odometer() == oldPos)
                break;
        }
    };
    if (canceled)
        setState(QVersitReader::CanceledState);
    else
        setState(QVersitReader::FinishedState);
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

/*!
 * Parses a versit document. Returns true if the parsing was successful.
 */
bool QVersitReaderPrivate::parseVersitDocument(LineReader& lineReader, QVersitDocument& document)
{
    if (mDocumentNestingLevel >= MAX_VERSIT_DOCUMENT_NESTING_DEPTH)
        return false; // To prevent infinite recursion

    // If we don't know what type it is, just assume it's a vCard 3.0
    if (document.type() == QVersitDocument::InvalidType)
        document.setType(QVersitDocument::VCard30Type);

    QVersitProperty property;

    property = parseNextVersitProperty(document.type(), lineReader);
    QString propertyValue = property.value().trimmed().toUpper();
    if (property.isEmpty()) {
        // A blank document (or end of file) was found.
        document = QVersitDocument();
        return true;
    } else if (property.name() == QLatin1String("BEGIN")) {
        if (propertyValue == QLatin1String("VCARD")) {
            document.setComponentType(propertyValue);
        } else if (propertyValue == QLatin1String("VCALENDAR")) {
            document.setType(QVersitDocument::ICalendar20Type);
            document.setComponentType(propertyValue);
        } else {
            // Unknown document type
            document = QVersitDocument();
            return false;
        }
    } else {
        // Some property other than BEGIN was found.
        document = QVersitDocument();
        return false;
    }

    return parseVersitDocumentBody(lineReader, document);
}

bool QVersitReaderPrivate::parseVersitDocumentBody(LineReader& lineReader, QVersitDocument& document)
{
    mDocumentNestingLevel++;
    bool parsingOk = true;
    while (true) {
        /* Grab it */
        QVersitProperty property = parseNextVersitProperty(document.type(), lineReader);

        if (property.name() == QLatin1String("BEGIN")) {
            // Nested Versit document
            QVersitDocument subDocument;
            subDocument.setType(document.type());
            subDocument.setComponentType(property.value().trimmed().toUpper());
            if (!parseVersitDocumentBody(lineReader, subDocument))
                break;
            document.addSubDocument(subDocument);
        } else if (property.name() == QLatin1String("VERSION")) {
            // A version property
            if (!setVersionFromProperty(document, property)) {
                parsingOk = false;
                break;
            }
        } else if (property.name() == QLatin1String("END")) {
            // End of document
            break;
        } else if (property.name().isEmpty()) {
            // End of input or some other error
            parsingOk = false;
            break;
        } else {
            // A normal property - just add it.
            document.addProperty(property);
        }
    }
    if (!parsingOk)
        document = QVersitDocument();
    mDocumentNestingLevel--;

    return parsingOk;
}

/*!
 * Parses a versit document and returns whether parsing succeeded.
 */
QVersitProperty QVersitReaderPrivate::parseNextVersitProperty(
        QVersitDocument::VersitType versitType,
        LineReader& lineReader)
{
    LByteArray cursor = lineReader.readLine();
    if (cursor.isEmpty())
        return QVersitProperty();

    // Otherwise, do stuff.
    QPair<QStringList,QString> groupsAndName =
            extractPropertyGroupsAndName(cursor, lineReader.codec());

    QVersitProperty property;
    property.setGroups(groupsAndName.first);
    property.setName(groupsAndName.second);
    // set the propertyValueType
    QPair<QVersitDocument::VersitType, QString> key =
        qMakePair(versitType, property.name());
    if (mValueTypeMap.contains(key))
        property.setValueType(mValueTypeMap.value(key));

    if (versitType == QVersitDocument::VCard21Type)
        parseVCard21Property(cursor, property, lineReader);
    else if (versitType == QVersitDocument::VCard30Type
            || versitType == QVersitDocument::ICalendar20Type)
        parseVCard30Property(versitType, cursor, property, lineReader);

    return property;
}

/*!
 * Parses the property according to vCard 2.1 syntax.
 */
void QVersitReaderPrivate::parseVCard21Property(LByteArray& cursor, QVersitProperty& property,
                                                LineReader& lineReader)
{
    property.setParameters(extractVCard21PropertyParams(cursor, lineReader.codec()));

    QByteArray value = cursor.toByteArray();
    if (property.valueType() == QVersitProperty::VersitDocumentType) {
        // Hack to handle cases where start of document is on the same or next line as "AGENT:"
        if (value == "BEGIN:VCARD") {
            lineReader.pushLine(value);
        } else if (value.isEmpty()) {
        } else {
            property = QVersitProperty();
            return;
        }
        QVersitDocument subDocument(QVersitDocument::VCard21Type);
        if (!parseVersitDocument(lineReader, subDocument)) {
            property = QVersitProperty();
        } else {
            property.setValue(QVariant::fromValue(subDocument));
        }
    } else {
        QTextCodec* codec;
        bool isBinary = unencode(value, property, lineReader);
        if (isBinary) {
            property.setValue(value);
            property.setValueType(QVersitProperty::BinaryType);
        }
        else {
            property.setValue(decodeCharset(value, property, lineReader.codec(), &codec));
            splitStructuredValue(property, false);
        }
    }
}

/*!
 * Parses the property according to vCard 3.0 syntax.  This function is called for both vCard 3.0
 * and iCalendar properties.
 */
void QVersitReaderPrivate::parseVCard30Property(QVersitDocument::VersitType versitType,
                                                LByteArray& cursor, QVersitProperty& property,
                                                LineReader& lineReader)
{
    property.setParameters(extractVCard30PropertyParams(cursor, lineReader.codec()));

    QByteArray value = cursor.toByteArray();

    QTextCodec* codec;

    if (property.valueType() == QVersitProperty::VersitDocumentType) {
        QString valueString(decodeCharset(value, property, lineReader.codec(), &codec));
        removeBackSlashEscaping(valueString);
        // Make a line reader from the value of the property.
        QByteArray subDocumentValue(codec->fromUnicode(valueString));
        QBuffer subDocumentData(&subDocumentValue);
        subDocumentData.open(QIODevice::ReadOnly);
        subDocumentData.seek(0);
        LineReader subDocumentLineReader(&subDocumentData, codec);

        QVersitDocument subDocument(versitType);
        if (!parseVersitDocument(subDocumentLineReader, subDocument)) {
            property = QVersitProperty();
        } else {
            property.setValue(QVariant::fromValue(subDocument));
        }
    } else {
        bool isBinary = unencode(value, property, lineReader);
        if (isBinary) {
            property.setValue(value);
            property.setValueType(QVersitProperty::BinaryType);
        } else {
            property.setValue(decodeCharset(value, property, lineReader.codec(), &codec));
            bool isList = splitStructuredValue(property, true);
            // Do backslash unescaping
            if (isList) {
                QStringList list = property.value<QStringList>();
                for (int i = 0; i < list.length(); i++) {
                    removeBackSlashEscaping(list[i]);
                }
                property.setValue(list);
            } else {
                QString value = property.value();
                removeBackSlashEscaping(value);
                property.setValue(value);
            }
        }
    }
}

/*!
 * Sets version to \a document if \a property contains a supported version.
 */
bool QVersitReaderPrivate::setVersionFromProperty(QVersitDocument& document, const QVersitProperty& property) const
{
    QString value = property.value().trimmed();
    if (document.componentType() == QLatin1String("VCARD")
            && value == QLatin1String("2.1")) {
        document.setType(QVersitDocument::VCard21Type);
    } else if (document.componentType() == QLatin1String("VCARD")
            && value == QLatin1String("3.0")) {
        document.setType(QVersitDocument::VCard30Type);
    } else if ((document.componentType() == QLatin1String("VCALENDAR")
                || document.type() == QVersitDocument::ICalendar20Type) // covers VEVENT, etc. when nested inside a VCALENDAR
            && value == QLatin1String("2.0")) {
        document.setType(QVersitDocument::ICalendar20Type);
    } else {
        return false;
    }
    return true;
}

/*!
 * On entry, \a value should be the byte array to unencode.  It is modified to be the unencoded
 * version.  Returns true if and only if the value was base-64 encoded.
 * \a lineReader is supplied in case more lines need to be read (for quoted-printable).  The
 * \a property is supplied so we know what kind of encoding was used.
 */
bool QVersitReaderPrivate::unencode(QByteArray& value,
                                    QVersitProperty& property,
                                    LineReader& lineReader) const
{
    QStringList encodingParameters = property.parameters().values(QLatin1String("ENCODING"));
    QStringList typeParameters = property.parameters().values(QLatin1String("TYPE"));
    if (encodingParameters.contains(QLatin1String("QUOTED-PRINTABLE"), Qt::CaseInsensitive)) {
        // At this point, we need to accumulate bytes until we hit a real line break (no = before
        // it) value already contains everything up to the character before the newline
        while (value.endsWith('=')) {
            value.chop(1); // Get rid of '='
            // We add each line (minus the escaped = and newline chars)
            value.append(lineReader.readLine().toByteArray());
        }
        decodeQuotedPrintable(value);
        // Remove the encoding parameter as the value is now decoded
        property.removeParameters(QLatin1String("ENCODING"));
        return false;
    } else if (encodingParameters.contains(QLatin1String("BASE64"), Qt::CaseInsensitive)
        || encodingParameters.contains(QLatin1String("B"), Qt::CaseInsensitive)
        || typeParameters.contains(QLatin1String("BASE64"), Qt::CaseInsensitive)
        || typeParameters.contains(QLatin1String("B"), Qt::CaseInsensitive)) {
        value = QByteArray::fromBase64(value);
        // Remove the encoding parameter as the value is now decoded
        property.removeParameters(QLatin1String("ENCODING"));
        return true;
    }
    return false;
}

/*!
 * Decodes \a value, after working out what charset it is in using the context of \a property and
 * returns it.  The codec used to decode is returned in \a codec.
 */
QString QVersitReaderPrivate::decodeCharset(const QByteArray& value,
                                            QVersitProperty& property,
                                            QTextCodec* defaultCodec,
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
            *codec = defaultCodec;
            return defaultCodec->toUnicode(value);
        }
    }
    *codec = defaultCodec;
    return defaultCodec->toUnicode(value);
}

/*!
 * Decodes Quoted-Printable encoded (RFC 1521) characters in /a text.
 */
void QVersitReaderPrivate::decodeQuotedPrintable(QByteArray& text) const
{
    for (int i=0; i < text.length(); i++) {
        char current = text.at(i);
        if (current == '=' && i+2 < text.length()) {
            char next = text.at(i+1);
            char nextAfterNext = text.at(i+2);
            if (((next >= 'a' && next <= 'f') ||
                 (next >= 'A' && next <= 'F') ||
                 (next >= '0' && next <= '9')) &&
                ((nextAfterNext >= 'a' && nextAfterNext <= 'f') ||
                 (nextAfterNext >= 'A' && nextAfterNext <= 'F') ||
                 (nextAfterNext >= '0' && nextAfterNext <= '9'))) {
                bool ok;
                char decodedChar(text.mid(i+1, 2).toInt(&ok,16));
                if (ok) {
                    text[i] = decodedChar;
                    text.remove(i+1, 2);
                }
            } else if (next == '\r' && nextAfterNext == '\n') {
                // Newlines can still be found here if they are encoded in a non-default charset.
                text.remove(i, 3);
            }
        }
    }
}

/*!
 * Extracts the groups and the name of the property using \a codec to determine the delimiters
 *
 * On entry, \a line should contain a whole line
 * On exit, \a line will be updated to remove the groups and name
 */
QPair<QStringList,QString>QVersitReaderPrivate::extractPropertyGroupsAndName(
        LByteArray& line, QTextCodec *codec) const
{
    const QByteArray semicolon = VersitUtils::encode(';', codec);
    const QByteArray colon = VersitUtils::encode(':', codec);
    const QByteArray backslash = VersitUtils::encode('\\', codec);
    QPair<QStringList,QString> groupsAndName;
    int length = 0;

    int separatorLength = semicolon.length();
    for (int i = 0; i < line.size() - separatorLength + 1; i++) {
        if ((containsAt(line, semicolon, i) && !containsAt(line, backslash, i-separatorLength))
            || containsAt(line, colon, i)) {
            length = i;
            break;
        }
    }
    if (length > 0) {
        QString trimmedGroupsAndName = codec->toUnicode(line.left(length)).trimmed();
        QStringList parts = trimmedGroupsAndName.split(QLatin1Char('.'));
        if (parts.count() > 1) {
            groupsAndName.second = parts.takeLast();
            groupsAndName.first = parts;
        } else {
            groupsAndName.second = trimmedGroupsAndName;
        }
        line.chopLeft(length);
    }

    return groupsAndName;
}

/*!
 * Extracts the property parameters as a QMultiHash using \a codec to determine the delimiters.
 * The parameters without names are added as "TYPE" parameters.
 *
 * On entry \a line should contain the line sans the group and name
 * On exit, line will be updated to have the parameters removed.
 */
QMultiHash<QString,QString> QVersitReaderPrivate::extractVCard21PropertyParams(
        LByteArray& line, QTextCodec *codec) const
{
    QMultiHash<QString,QString> result;
    QList<QByteArray> paramList = extractParams(line, codec);
    while (!paramList.isEmpty()) {
        QByteArray param = paramList.takeLast();
        QString name = paramName(param, codec);
        QString value = paramValue(param, codec);
        result.insert(name,value);
    }

    return result;
}

/*!
 * Extracts the property parameters as a QMultiHash using \a codec to determine the delimiters.
 * The parameters without names are added as "TYPE" parameters.
 *
 * On entry \a line should contain the line sans the group and name
 * On exit, line will be updated to have the parameters removed.
 */
QMultiHash<QString,QString> QVersitReaderPrivate::extractVCard30PropertyParams(
        LByteArray& line, QTextCodec *codec) const
{
    QMultiHash<QString,QString> result;
    QList<QByteArray> paramList = extractParams(line, codec);
    while (!paramList.isEmpty()) {
        QByteArray param = paramList.takeLast();
        QString name(paramName(param, codec));
        removeBackSlashEscaping(name);
        QString values = paramValue(param, codec);
        QStringList valueList = splitValue(values, QLatin1Char(','), QString::SkipEmptyParts, true);
        foreach (QString value, valueList) {
            removeBackSlashEscaping(value);
            result.insert(name, value);
        }
    }
    return result;
}


/*!
 * Extracts the parameters as delimited by semicolons using \a codec to determine the delimiters.
 *
 * On entry \a line should contain the content line sans the group and name
 * On exit, \a line will be updated to only have the value remain
 */
QList<QByteArray> QVersitReaderPrivate::extractParams(LByteArray& line, QTextCodec *codec) const
{
    const QByteArray colon = VersitUtils::encode(':', codec);
    QList<QByteArray> params;

    /* find the end of the name&params */
    int colonIndex = line.indexOf(colon);
    if (colonIndex > 0) {
        QByteArray nameAndParamsString = line.left(colonIndex);
        params = extractParts(nameAndParamsString, VersitUtils::encode(';', codec), codec);

        /* Update line */
        line.chopLeft(colonIndex + colon.length());
    } else if (colonIndex == 0) {
        // No parameters.. advance past it
        line.chopLeft(colon.length());
    }

    return params;
}

/*!
 * Extracts the parts separated by separator discarding the separators escaped with a backslash
 * encoded with \a codec
 */
QList<QByteArray> QVersitReaderPrivate::extractParts(
        const QByteArray& text, const QByteArray& separator, QTextCodec* codec) const
{
    QList<QByteArray> parts;
    int partStartIndex = 0;
    int textLength = text.length();
    int separatorLength = separator.length();
    QByteArray backslash = VersitUtils::encode('\\', codec);
    int backslashLength = backslash.length();

    for (int i=0; i < textLength-separatorLength+1; i++) {
        if (containsAt(text, separator, i)
            && (i < backslashLength
                || !containsAt(text, backslash, i-backslashLength))) {
            int length = i-partStartIndex;
            QByteArray part = extractPart(text,partStartIndex,length);
            if (part.length() > 0)
                parts.append(part);
            partStartIndex = i+separatorLength;
        }
    }

    // Add the last or only part
    QByteArray part = extractPart(text,partStartIndex);
    if (part.length() > 0)
        parts.append(part);
    return parts;
}

/*!
 * Extracts a substring limited by /a startPosition and /a length.
 */
QByteArray QVersitReaderPrivate::extractPart(
        const QByteArray& text, int startPosition, int length) const
{
    QByteArray part;
    if (startPosition >= 0)
        part = text.mid(startPosition,length).trimmed();
    return part;
}

/*!
 * Extracts the name of the parameter using \a codec to determine the delimiters.
 * No name is interpreted as an implicit "TYPE".
 */
QString QVersitReaderPrivate::paramName(const QByteArray& parameter, QTextCodec* codec) const
{
     if (parameter.trimmed().length() == 0)
         return QString();
     QByteArray equals = VersitUtils::encode('=', codec);
     int equalsIndex = parameter.indexOf(equals);
     if (equalsIndex > 0) {
         return codec->toUnicode(parameter.left(equalsIndex)).trimmed();
     }

     return QLatin1String("TYPE");
}

/*!
 * Extracts the value of the parameter using \a codec to determine the delimiters
 */
QString QVersitReaderPrivate::paramValue(const QByteArray& parameter, QTextCodec* codec) const
{
    QByteArray value(parameter);
    QByteArray equals = VersitUtils::encode('=', codec);
    int equalsIndex = parameter.indexOf(equals);
    if (equalsIndex > 0) {
        int valueLength = parameter.length() - (equalsIndex + equals.length());
        value = parameter.right(valueLength).trimmed();
    }

    return codec->toUnicode(value);
}

/*!
 * Returns true if and only if \a text contains \a ba at \a index
 *
 * On entry, index must be >= 0
 *
 * T is either a QByteArray or LByteArray
 */
template <class T> bool QVersitReaderPrivate::containsAt(const T& text, const QByteArray& match, int index)
{
    int n = match.length();
    if (text.size() - index < n)
        return false;
    const char* textData = text.constData();
    const char* matchData = match.constData();
    return memcmp(textData+index, matchData, n) == 0;
}

/*!
 * If the \a type and the \a property's name is known to contain a structured value, \a property's
 * value is split according to the type of structuring (compound vs. list) it is known to have.
 * Returns true if and only if such a split happened (ie. the property value holds a QStringList on
 * exit).
 */
bool QVersitReaderPrivate::splitStructuredValue(
        QVersitProperty& property,
        bool hasEscapedBackslashes) const
{
    QVariant variant = property.variantValue();
    if (property.valueType() == QVersitProperty::CompoundType) {
        variant.setValue(splitValue(variant.toString(), QLatin1Char(';'),
                                    QString::KeepEmptyParts, hasEscapedBackslashes));
        property.setValue(variant);
        return true;
    } else if (property.valueType() == QVersitProperty::ListType) {
        variant.setValue(splitValue(variant.toString(), QLatin1Char(','),
                                    QString::SkipEmptyParts, hasEscapedBackslashes));
        property.setValue(variant);
        return true;
    }
    return false;
}

/*!
 * Splits the \a string into substrings wherever \a sep occurs.
 * If \a hasEscapedBackslashes is false, then a \a sep preceded by a backslash is not considered
 * a split point (but the backslash is removed).
 * If \a hasEscapedBackslashes is true, then a \a sep preceded by an odd number of backslashes is
 * not considered a split point (but one backslash is removed).
 */
QStringList QVersitReaderPrivate::splitValue(const QString& string,
                                             const QChar& sep,
                                             QString::SplitBehavior behaviour,
                                             bool hasEscapedBackslashes)
{
    QStringList list;
    bool isEscaped = false; // is the current character escaped
    int segmentStartIndex = 0;
    QString segment;
    for (int i = 0; i < string.length(); i++) {
        if (string.at(i) == QLatin1Char('\\')) {
            if (hasEscapedBackslashes)
                isEscaped = !isEscaped; // two consecutive backslashes make isEscaped false
            else
                isEscaped = true;
        } else if (string.at(i) == sep) {
            if (isEscaped) {
                // we see an escaped separator - remove the backslash
                segment += string.midRef(segmentStartIndex, i-segmentStartIndex-1);
                segment += sep;
            } else {
                // we see a separator
                segment += string.midRef(segmentStartIndex, i - segmentStartIndex);
                if (behaviour == QString::KeepEmptyParts || !segment.isEmpty())
                    list.append(segment);
                segment.clear();
            }
            segmentStartIndex = i+1;
            isEscaped = false;
        } else { // normal character - keep going
            isEscaped = false;
        }
    }
    // The rest of the string after the last sep.
    segment += string.midRef(segmentStartIndex);
    if (behaviour == QString::KeepEmptyParts || !segment.isEmpty())
        list.append(segment);
    return list;
}

/*!
 * Removes backslash escaping for line breaks (CRLFs), colons, semicolons, backslashes and commas
 * according to RFC 2426.  This is called on parameter names and values and property values.
 * Colons ARE unescaped because the text of RFC2426 suggests that they should be.
 */
void QVersitReaderPrivate::removeBackSlashEscaping(QString& text)
{
    if (!(text.startsWith(QLatin1Char('"')) && text.endsWith(QLatin1Char('"')))) {
        /* replaces \; with ;
                    \, with ,
                    \: with :
                    \\ with \
         */
        text.replace(QRegExp(QLatin1String("\\\\([;,:\\\\])")), QLatin1String("\\1"));
        // replaces \n with a CRLF
        text.replace(QLatin1String("\\n"), QLatin1String("\r\n"), Qt::CaseInsensitive);
    }
}


#include "moc_qversitreader_p.cpp"
