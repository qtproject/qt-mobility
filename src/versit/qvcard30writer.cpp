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

#include "qvcard30writer_p.h"
#include "versitutils_p.h"
#include "qversitdefs.h"
#include "qmobilityglobal.h"

QTM_BEGIN_NAMESPACE

/*! Constructs a writer. */
QVCard30Writer::QVCard30Writer()
    : QVersitWriterPrivate(QByteArray("VCARD"),QByteArray("3.0"))
{
    mPropertyNameMappings.insert(
        QString::fromAscii("X-NICKNAME"),QString::fromAscii("NICKNAME"));
    mPropertyNameMappings.insert(
        QString::fromAscii("X-IMPP"),QString::fromAscii("IMPP"));
}

/*! Destroys a writer. */
QVCard30Writer::~QVCard30Writer()
{
}

/*!
 * Encodes the \a property to text. 
 */
QByteArray QVCard30Writer::encodeVersitProperty(const QVersitProperty& property)
{
    QVersitProperty modifiedProperty(property);
    QString name = mPropertyNameMappings.value(property.name(),property.name());
    modifiedProperty.setName(name);
    QByteArray encodedProperty(encodeGroupsAndName(modifiedProperty));
    encodedProperty.append(encodeParameters(modifiedProperty.parameters()));
    encodedProperty.append(":");
    // TODO: do charset encoding
    QByteArray value(modifiedProperty.value().toAscii());
    if (modifiedProperty.name() == QString::fromAscii("AGENT")) {
        QVersitDocument embeddedDocument = modifiedProperty.embeddedDocument();
        value = encodeVersitDocument(embeddedDocument);
        QString escapedValue(QString::fromAscii(value));
        VersitUtils::backSlashEscape(escapedValue);
        value = escapedValue.toAscii();
    }    
    encodedProperty.append(value);
    encodedProperty.append("\r\n");

    return encodedProperty;
}

/*!
 * Encodes the \a parameters to text.
 */
QByteArray QVCard30Writer::encodeParameters(
    const QMultiHash<QString,QString>& parameters) const
{
    QByteArray encodedParameters;
    QList<QString> names = parameters.uniqueKeys();
    foreach (QString nameString, names) {
        encodedParameters.append(";");
        QStringList values = parameters.values(nameString);
        VersitUtils::backSlashEscape(nameString);
        QByteArray name(nameString.toAscii());
        encodedParameters.append(name);
        encodedParameters.append("=");
        for (int i=0; i<values.size(); i++) {
            if (i > 0)
                encodedParameters.append(",");
            QString value = values.at(i);
            // QVersitContactExporterPrivate implementation may have added
            // base64 encoding parameter according to vCard 2.1.
            // Convert it to vCard 3.0 compatible.
            if (name == "ENCODING" && value == QString::fromAscii("BASE64"))
                value = QString::fromAscii("B");

            VersitUtils::backSlashEscape(value);
            encodedParameters.append(value.toAscii());
        }
    }

    return encodedParameters;
}

QTM_END_NAMESPACE
