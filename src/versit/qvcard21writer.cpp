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

#include "qvcard21writer_p.h"
#include "versitutils_p.h"
#include "qmobilityglobal.h"

#include <QTextCodec>

QTM_USE_NAMESPACE

/*! Constructs a writer. */
QVCard21Writer::QVCard21Writer()
    : QVersitDocumentWriter(QByteArray("VCARD"),QByteArray("2.1"))
{
}

/*! Destroys a writer. */
QVCard21Writer::~QVCard21Writer()
{
}

/*!
 * Encodes the \a property and writes it to the device.
 */
void QVCard21Writer::encodeVersitProperty(const QVersitProperty& property)
{
    encodeGroupsAndName(property);
    QMultiHash<QString,QString> parameters = property.parameters();
    QVariant variant(property.variantValue());

    QString renderedValue;
    bool useUtf8 = false;

    if (variant.type() == QVariant::String) {
        QString valueString = variant.toString();

        // Quoted-Printable encode the value and add Quoted-Printable parameter, if necessary
        if (!parameters.contains(QLatin1String("ENCODING"))) {
            if (quotedPrintableEncode(valueString))
                parameters.insert(QLatin1String("ENCODING"), QLatin1String("QUOTED-PRINTABLE"));
        }

        // Add the CHARSET parameter, if necessary and encode in UTF-8 later
        if (!mCodec->canEncode(valueString)) {
            parameters.insert(QLatin1String("CHARSET"), QLatin1String("UTF-8"));
            useUtf8 = true;
        }
        renderedValue = valueString;
    } else if (variant.type() == QVariant::ByteArray) {
        parameters.insert(QLatin1String("ENCODING"), QLatin1String("BASE64"));
        renderedValue = QLatin1String(variant.toByteArray().toBase64().data());
    }

    // Encode parameters
    encodeParameters(parameters);

    // Encode value
    writeString(QLatin1String(":"));
    if (variant.canConvert<QVersitDocument>()) {
        writeCrlf();
        QVersitDocument embeddedDocument = variant.value<QVersitDocument>();
        encodeVersitDocument(embeddedDocument);
    } else if (variant.type() == QVariant::String) {
        writeString(renderedValue, useUtf8);
    } else if (variant.type() == QVariant::ByteArray) {
        // One extra folding before the value and
        // one extra line break after the value are needed in vCard 2.1
        writeCrlf();
        writeString(QLatin1String(" "));
        writeString(renderedValue, useUtf8);
        writeCrlf();
    }
    writeCrlf();
}

/*!
 * Encodes the \a parameters and writes it to the device.
 */
void QVCard21Writer::encodeParameters(const QMultiHash<QString,QString>& parameters)
{
    QList<QString> names = parameters.uniqueKeys();
    foreach (const QString& name, names) {
        QStringList values = parameters.values(name);
        foreach (const QString& value, values) {
            writeString(QLatin1String(";"));
            QString typeParameterName(QLatin1String("TYPE"));
            if (name.length() > 0 && name != typeParameterName) {
                writeString(name);
                writeString(QLatin1String("="));
            }
            writeString(value);
        }
    }
}



/*!
 * Encodes special characters in \a text
 * using Quoted-Printable encoding (RFC 1521).
 * Returns true if at least one character was encoded.
 */
bool QVCard21Writer::quotedPrintableEncode(QString& text) const
{
    bool encoded = false;
    for (int i=0; i<text.length(); i++) {
        QChar current = text.at(i);
        if (shouldBeQuotedPrintableEncoded(current)) {
            QString encodedStr(QString::fromAscii("=%1").
                               arg(current.unicode(), 2, 16, QLatin1Char('0')).toUpper());
            text.replace(i, 1, encodedStr);
            i += 2;
            encoded = true;
        }
    }
    return encoded;
}


/*!
 * Checks whether the \a chr should be Quoted-Printable encoded (RFC 1521).
 */
bool QVCard21Writer::shouldBeQuotedPrintableEncoded(QChar chr) const
{
    int c = chr.unicode();
    return (c < 32 ||
            c == '!' || c == '"' || c == '#' || c == '$' ||
            c == '=' || c == '@' || c == '[' || c == '\\' ||
            c == ']' || c == '^' || c == '`' ||
            (c > 122 && c < 256));
}
