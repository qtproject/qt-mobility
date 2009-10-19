/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qversitwriter.h"
#include "qversitwriter_p.h"
#include "versitutils.h"

#include <QStringList>

/*!
 * \class QVersitWriter
 *
 * \brief The QVersitWriter class provides an interface 
 * for encoding vCards from a versit document into a stream. 
 *
 * \sa
 */

/*! Constructs a writer. */
QVersitWriter::QVersitWriter() 
{
    d = new QVersitWriterPrivate;
}

/*! Destroys a writer. */
QVersitWriter::~QVersitWriter()
{
    delete d;
}

// input:
void QVersitWriter::setVersitDocument(const QVersitDocument& versitDocument)
{
    d->mVersitDocument = versitDocument;
}

QVersitDocument QVersitWriter::versitDocument() const
{
    return d->mVersitDocument;
}

/*!
 * Sets the \a device used for encoding.
 */
void QVersitWriter::setDevice(QIODevice* device)
{
    d->mIoDevice = device;
}

/*!
 * Returns the device used for encoding. 
 */
QIODevice* QVersitWriter::device() const
{
    return d->mIoDevice;
}

/*!
 * Starts encoding.
 */
bool QVersitWriter::start()
{
    if (d->mIoDevice
        && !d->mVersitDocument.properties().empty()) {
        QByteArray output = encodeVersitDocument(d->mVersitDocument);
        if (d->mIoDevice->write(output) > 0) {
            return true;
        }
        return false;
    }
    return false;
}

/*!
 * Encodes the \a document to text. 
 */
QByteArray QVersitWriter::encodeVersitDocument(const QVersitDocument& document)
{
    QList<QVersitProperty> properties = document.properties();
    QByteArray encodedDocument;
    
    encodedDocument.append("BEGIN:VCARD\r\n");
    encodedDocument.append("VERSION:2.1\r\n");
    foreach (QVersitProperty property, properties) {
        encodedDocument.append(encodeVersitProperty(property));
    }
    encodedDocument.append("END:VCARD\r\n");
    
    return encodedDocument;
}

/*!
 * Encodes the \a property to text. 
 */
QByteArray QVersitWriter::encodeVersitProperty(const QVersitProperty& property)
{
    QByteArray encodedProperty;
    QString name = property.name();    
    encodedProperty.append(name.toAscii());

    QByteArray value(property.value());
    bool valueQuotedPrintableEncoded = quotedPrintableEncode(property,value);
    QByteArray encodedParameters = 
        encodeParameters(property.parameters(),valueQuotedPrintableEncoded);
    encodedProperty.append(encodedParameters);

    encodedProperty.append(":");
    if (name == QString::fromAscii("AGENT")) {
        encodedProperty.append("\r\n");
        QVersitDocument embeddedDocument = property.embeddedDocument();
        encodedProperty.append(encodeVersitDocument(embeddedDocument));
    } else {
        encodedProperty.append(value);
    }
    encodedProperty.append("\r\n");

    return encodedProperty;
}

/*!
 * Encodes the \a parameters to text. 
 */
QByteArray QVersitWriter::encodeParameters(
    const QMultiHash<QString,QString>& parameters,
    bool addQuotedPrintable)
{
    QByteArray encodedParameters;
    QString typeParameterName(QString::fromAscii("TYPE"));
    QList<QString> names = parameters.uniqueKeys();
    foreach (QString name, names) {
        QStringList values = parameters.values(name);
        foreach (QString value, values) {
            encodedParameters.append(";");
            if (name != typeParameterName) {
                encodedParameters.append(name.toAscii());
                encodedParameters.append("=");
            }
            encodedParameters.append(value.toAscii());
        } 
    }
    QString encodingParameterName(QString::fromAscii("ENCODING"));
    QString quotedPrintableValue(QString::fromAscii("QUOTED-PRINTABLE"));
    if (addQuotedPrintable && 
        !parameters.contains(encodingParameterName,quotedPrintableValue))
        encodedParameters.append(";ENCODING=QUOTED-PRINTABLE");
    return encodedParameters;
}

/*!
 * Encodes the \a value with Quoted-Printable encoding
 * if it needs to be encoded and the parameters 
 * of the \a property do not yet indicate encoding.  
 */
bool QVersitWriter::quotedPrintableEncode(
    const QVersitProperty& property,
    QByteArray& value) const
{
    bool encoded = false;
    value = property.value();
    if (!property.parameters().contains(QString::fromAscii("ENCODING"))) {
        encoded = VersitUtils::quotedPrintableEncode(value);
    }
    return encoded;
}
