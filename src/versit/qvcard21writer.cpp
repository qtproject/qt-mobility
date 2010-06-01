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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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

    /* Structured values need to have their components backslash-escaped (in vCard 2.1, semicolons
       must be escaped for compound values and commas must be escaped for list values). */
    if (variant.type() == QVariant::StringList) {
        QStringList values = property.variantValue().toStringList();
        QString separator;
        if (property.valueType() == QVersitProperty::CompoundType) {
            separator = QLatin1String(";");
        } else {
            if (property.valueType() != QVersitProperty::ListType) {
                qWarning("Variant value is a QStringList but the property's value type is neither "
                         "CompoundType or ListType");
            }
            // Assume it's a ListType
            separator = QLatin1String(",");
        }
        QString replacement = QLatin1Char('\\') + separator;
        QRegExp separatorRegex = QRegExp(separator);
        bool first = true;
        foreach (QString value, values) {
            if (!(value.isEmpty() && property.valueType() == QVersitProperty::ListType)) {
                useUtf8 |= encodeVersitValue(parameters, value);
                if (!first) {
                    renderedValue += separator;
                }
                renderedValue += value.replace(separatorRegex, replacement);
                first = false;
            }
        }
    } else if (variant.type() == QVariant::String) {
        renderedValue = variant.toString();
        useUtf8 = encodeVersitValue(parameters, renderedValue);
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
    } else if (variant.type() == QVariant::String || variant.type() == QVariant::StringList) {
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

/*! Performs Quoted-Printable encoding and charset encoding on \a value as per vCard 2.1 spec.
    Returns true if the value will need to be encoded with UTF-8, false if mCodec is sufficient. */
bool QVCard21Writer::encodeVersitValue(QMultiHash<QString,QString>& parameters, QString& value)
{
    // Quoted-Printable encode the value and add Quoted-Printable parameter, if necessary
    if (quotedPrintableEncode(value))
        parameters.insert(QLatin1String("ENCODING"), QLatin1String("QUOTED-PRINTABLE"));

    // Add the CHARSET parameter, if necessary and encode in UTF-8 later
    if (!mCodec->canEncode(value)) {
        parameters.insert(QLatin1String("CHARSET"), QLatin1String("UTF-8"));
        return true;
    }
    return false;
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
