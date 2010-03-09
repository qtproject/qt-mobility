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
#include "qmobilityglobal.h"
#include <QTextCodec>
#include <QBuffer>

QTM_USE_NAMESPACE

/*! Constructs a writer. */
QVCard30Writer::QVCard30Writer()
    : QVersitDocumentWriter(QByteArray("VCARD"),QByteArray("3.0"))
{
    mPropertyNameMappings.insert(
        QLatin1String("X-NICKNAME"),QLatin1String("NICKNAME"));
    mPropertyNameMappings.insert(
        QLatin1String("X-IMPP"),QLatin1String("IMPP"));
}

/*! Destroys a writer. */
QVCard30Writer::~QVCard30Writer()
{
}

/*!
 * Encodes the \a property and writes it to the device.
 */
void QVCard30Writer::encodeVersitProperty(const QVersitProperty& property)
{
    QVersitProperty modifiedProperty(property);
    QString name = mPropertyNameMappings.value(property.name(),property.name());
    modifiedProperty.setName(name);
    encodeGroupsAndName(modifiedProperty);

    QVariant variant(modifiedProperty.variantValue());
    if (variant.type() == QVariant::ByteArray) {
        modifiedProperty.insertParameter(QLatin1String("ENCODING"), QLatin1String("b"));
    }
    encodeParameters(modifiedProperty.parameters());
    writeString(QLatin1String(":"));

    QString value;
    if (variant.canConvert<QVersitDocument>()) {
        QVersitDocument embeddedDocument = variant.value<QVersitDocument>();
        QByteArray data;
        QBuffer buffer(&data);
        buffer.open(QIODevice::WriteOnly);
        QVCard30Writer subWriter;
        subWriter.setCodec(mCodec);
        subWriter.setDevice(&buffer);
        subWriter.encodeVersitDocument(embeddedDocument);
        QString documentString(mCodec->toUnicode(data));
        VersitUtils::backSlashEscape(documentString);
        value = documentString;
    } else if (variant.type() == QVariant::String) {
        value = variant.toString();
    } else if (variant.type() == QVariant::ByteArray) {
        value = QLatin1String(variant.toByteArray().toBase64().data());
    }
    writeString(value);
    writeCrlf();
}

/*!
 * Encodes the \a parameters and writes it to the device.
 */
void QVCard30Writer::encodeParameters(const QMultiHash<QString,QString>& parameters)
{
    QList<QString> names = parameters.uniqueKeys();
    foreach (QString nameString, names) {
        writeString(QLatin1String(";"));
        QStringList values = parameters.values(nameString);
        VersitUtils::backSlashEscape(nameString);
        writeString(nameString);
        writeString(QLatin1String("="));
        for (int i=0; i<values.size(); i++) {
            if (i > 0)
                writeString(QLatin1String(","));
            QString value = values.at(i);

            VersitUtils::backSlashEscape(value);
            writeString(value);
        }
    }
}
