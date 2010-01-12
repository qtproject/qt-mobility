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

QTM_BEGIN_NAMESPACE

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
 * Encodes the \a property to text. 
 */
QByteArray QVCard21Writer::encodeVersitProperty(const QVersitProperty& property)
{
    QByteArray encodedProperty(encodeGroupsAndName(property));

    const QString encoding(QString::fromAscii("ENCODING"));
    const QString quotedPrintable(QString::fromAscii("QUOTED-PRINTABLE"));
    QMultiHash<QString,QString> parameters = property.parameters();
    QVariant variant = property.value();

    QByteArray valueString;

    if (variant.canConvert(QVariant::String)) {
        // Quoted-Printable encode the value and add Quoted-Printable parameter, if necessary
        valueString = variant.toString().toAscii();
        bool valueQuotedPrintableEncoded = quotedPrintableEncode(property, valueString);
        if (valueQuotedPrintableEncoded &&
            !parameters.contains(encoding, quotedPrintable)) {
             // Add the encoding parameter to the copy, not to the actual property
             parameters.insert(encoding, quotedPrintable);
        }
    }

    // Encode parameters
    encodedProperty.append(encodeParameters(parameters));

    // Encode value
    encodedProperty.append(":");
    if (variant.canConvert<QVersitDocument>()) {
        encodedProperty.append("\r\n");
        QVersitDocument embeddedDocument = variant.value<QVersitDocument>();
        encodedProperty.append(encodeVersitDocument(embeddedDocument));
    } else if (parameters.contains(encoding, QString::fromAscii("BASE64"))) {
        // One extra folding before the value and
        // one extra line break after the value are needed in vCard 2.1
        encodedProperty += "\r\n " + valueString + "\r\n";
    } else {
        encodedProperty += valueString;
    }
    encodedProperty.append("\r\n");

    return encodedProperty;
}

/*!
 * Encodes the \a parameters to text.
 */
QByteArray QVCard21Writer::encodeParameters(
    const QMultiHash<QString,QString>& parameters) const
{
    QByteArray encodedParameters;
    QList<QString> names = parameters.uniqueKeys();
    foreach (QString name, names) {
        QStringList values = parameters.values(name);
        foreach (QString value, values) {
            encodedParameters.append(";");
            QString typeParameterName(QString::fromAscii("TYPE"));
            if (name.length() > 0 && name != typeParameterName) {
                encodedParameters.append(name.toAscii());
                encodedParameters.append("=");
            }
            encodedParameters.append(value.toAscii());
        }
    }
    return encodedParameters;
}

/*!
 * Encodes the \a value with Quoted-Printable encoding
 * if it needs to be encoded and the parameters
 * of the \a property do not yet indicate encoding.
 */
bool QVCard21Writer::quotedPrintableEncode(
    const QVersitProperty& property,
    QByteArray& value) const
{
    bool encoded = false;
    value = property.valueString().toAscii();
    if (!property.parameters().contains(QString::fromAscii("ENCODING"))) {
        encoded = VersitUtils::quotedPrintableEncode(value);
    }

    return encoded;
}

QTM_END_NAMESPACE
 
