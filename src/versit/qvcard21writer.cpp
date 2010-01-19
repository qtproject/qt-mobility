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
 * Encodes the \a property to text. 
 */
QByteArray QVCard21Writer::encodeVersitProperty(const QVersitProperty& property, QTextCodec* codec)
{
    if (!codec)
        codec = QTextCodec::codecForName("UTF-8");

    QByteArray encodedProperty(encodeGroupsAndName(property));
    QMultiHash<QString,QString> parameters = property.parameters();
    QVariant variant(property.variantValue());

    QByteArray renderedValue;

    if (variant.type() == QVariant::String) {
        QString valueString = variant.toString();

        // Quoted-Printable encode the value and add Quoted-Printable parameter, if necessary
        if (!parameters.contains(QLatin1String("ENCODING"))) {
            if (VersitUtils::quotedPrintableEncode(valueString))
                parameters.insert(QLatin1String("ENCODING"), QLatin1String("QUOTED-PRINTABLE"));
        }

        // Encode with the correct charset and add the CHARSET parameter, if necessary
        // In vCard 2.1, ASCII is the default charset, unless the value can't be ASCII-encoded.
        QTextCodec* asciiCodec = QTextCodec::codecForName("ISO-8859-1");
        if (asciiCodec->canEncode(valueString)) {
            renderedValue = asciiCodec->fromUnicode(valueString);
        } else {
            renderedValue = codec->fromUnicode(valueString);
            parameters.insert(QLatin1String("CHARSET"), codec->name());
        }
    } else if (variant.type() == QVariant::ByteArray) {
        parameters.insert(QLatin1String("ENCODING"), QLatin1String("BASE64"));
        renderedValue = variant.toByteArray().toBase64();
    }

    // Encode parameters
    encodedProperty.append(encodeParameters(parameters));

    // Encode value
    encodedProperty.append(":");
    if (variant.canConvert<QVersitDocument>()) {
        encodedProperty.append("\r\n");
        QVersitDocument embeddedDocument = variant.value<QVersitDocument>();
        encodedProperty.append(encodeVersitDocument(embeddedDocument));
    } else if (variant.type() == QVariant::String) {
        encodedProperty += renderedValue;
    } else if (variant.type() == QVariant::ByteArray) {
        // One extra folding before the value and
        // one extra line break after the value are needed in vCard 2.1
        encodedProperty += "\r\n " + renderedValue + "\r\n";
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

