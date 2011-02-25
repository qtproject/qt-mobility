/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "versitutils_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qmobilityglobal.h"

#include <QMap>
#include <QTextCodec>
#include <QScopedPointer>

QTM_USE_NAMESPACE

QTextCodec* VersitUtils::m_previousCodec = 0;
QList<QByteArrayMatcher>* VersitUtils::m_newlineList = 0;
QByteArray VersitUtils::m_encodingMap[256];

/*!
 * Encode \a ch with \a codec, without adding an byte-order mark
 */
QByteArray VersitUtils::encode(char ch, QTextCodec* codec)
{
    if (codec != m_previousCodec) {
        changeCodec(codec);
    }
    return m_encodingMap[(int)ch];
}

/*!
 * Encode \a ba with \a codec, without adding an byte-order mark.  \a ba is interpreted as ASCII
 */
QByteArray VersitUtils::encode(const QByteArray& ba, QTextCodec* codec)
{
    QTextCodec::ConverterState state(QTextCodec::IgnoreHeader);
    return codec->fromUnicode(QString::fromAscii(ba.data()).data(), ba.length(), &state);
}

/*!
 * Returns the list of DOS, UNIX and Mac newline characters for \a codec.
 */
QList<QByteArrayMatcher>* VersitUtils::newlineList(QTextCodec* codec)
{
    if (m_newlineList != 0 && codec == m_previousCodec) {
        return m_newlineList;
    }
    changeCodec(codec);
    return m_newlineList;
}

/*!
 * Update the cached tables of pregenerated encoded text with \a codec.
 */
void VersitUtils::changeCodec(QTextCodec* codec) {
    // Build m_encodingMap
    QChar qch;
    QTextCodec::ConverterState state(QTextCodec::IgnoreHeader);
    for (int c = 0; c < 256; c++) {
        qch = QLatin1Char(c);
        m_encodingMap[c] = codec->fromUnicode(&qch, 1, &state);
    }

    // Build m_newlineList
    if (m_newlineList != 0)
        delete m_newlineList;
    m_newlineList = new QList<QByteArrayMatcher>;
    m_newlineList->append(QByteArrayMatcher(encode("\r\n", codec)));
    m_newlineList->append(QByteArrayMatcher(encode("\n", codec)));
    m_newlineList->append(QByteArrayMatcher(encode("\r", codec)));

    m_previousCodec = codec;
}

/*!
 * Finds a property in the \a document with the given \a propertyName, adds it to \a toBeRemoved,
 * and returns it.
 */
QVersitProperty VersitUtils::takeProperty(const QVersitDocument& document,
                                          const QString& propertyName,
                                          QList<QVersitProperty>* toBeRemoved) {
    foreach (const QVersitProperty& currentProperty, document.properties()) {
        if (currentProperty.name() == propertyName) {
            *toBeRemoved << currentProperty;
            return currentProperty;
        }
    }
    return QVersitProperty();
}

/*!
 * Returns true iff \a bytes is a valid UTF-8 sequence.
 */
bool VersitUtils::isValidUtf8(const QByteArray& bytes) {
    int sequenceLength = 1; // number of bytes in total for a sequence
    int continuation = 0;   // number of bytes left in a continuation
    quint32 codePoint = 0;
    for (int i = 0; i < bytes.size(); i++) {
        quint8 byte = bytes[i];
        if (continuation == 0) {
            if (byte & 0x80) { // 1xxxxxxx
                if (byte & 0x40) { // 11xxxxxx
                    if (byte == 0xc0 || byte == 0xc1) // 1100000x
                        return false; // overlong 2 byte sequence
                    if (byte & 0x20) { // 111xxxxx
                        if (byte & 0x10) { // 1111xxxx
                            if (byte & 0x08) { // 11111xxx
                                // Outside unicode range
                                return false;
                            } else { // 11110xxx
                                sequenceLength = 4;
                                continuation = 3; // three more bytes
                                codePoint = byte & 0x07; // take the last 3 bits
                            }
                        } else { // 1110xxxx
                            sequenceLength = 3;
                            continuation = 2; // two more bytes
                            codePoint = byte & 0x0f; // take last 4 bits
                        }
                    } else { // 110xxxxx
                        sequenceLength = 2;
                        continuation = 1; // one more byte
                        codePoint = byte & 0x1f; // take last 5 bits
                    }
                } else { // 10xxxxxx
                    // unexpected continuation
                    return false;
                }
            } else { // 0xxxxxxx
                sequenceLength = 1;
            }
        } else { // continuation > 0
            if ((byte & 0xc0) != 0x80) // 10xxxxxx
                return false; // expected continuation not found
            codePoint = (codePoint << 6) | (byte & 0x3f); // append last 6 bits
            continuation--;
        }

        if (continuation == 0) {
            // Finished decoding a character - it's not overlong and that it's in range
            switch (sequenceLength) {
                // 1-byte sequence can't be overlong
                // 2-byte sequence has already been checked for overlongness
                case 3:
                    if (codePoint < 0x800) // overlong
                        return false;

                    // Filter out codepoints outside the Unicode range
                    if ((codePoint >= 0xd800 && codePoint <= 0xdfff) // utf-16 surrogate halves
                            || (codePoint >= 0xfffe && codePoint <= 0xffff)) { // reversed utf-16 BOM
                        return false;
                    }
                    break;
                case 4:
                    if (codePoint < 0x10000      // overlong
                        || codePoint > 0x10ffff) // above Unicode range
                        return false;
                    break;
            }
            codePoint = 0;
        }
    }
    return continuation == 0;
}
