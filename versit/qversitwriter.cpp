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
 * Returns the \a device used for encoding. 
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

QByteArray QVersitWriter::encodeVersitDocument(const QVersitDocument& versitDocument)
{
    QList<QVersitProperty> propertyList = versitDocument.properties();
    QByteArray docArray;
    
    docArray.append("BEGIN:VCARD\r\n");
    docArray.append("VERSION:2.1\r\n");
    foreach (QVersitProperty property, propertyList) {
        docArray.append(encodeVersitProperty(property));
    }
    docArray.append("END:VCARD\r\n");
    
    return docArray;
}


QByteArray QVersitWriter::encodeVersitProperty(const QVersitProperty& versitProperty)
{
    QByteArray propertyArray;
    
    // 1. Encode the property name
    QString name = versitProperty.name();    
    propertyArray.append(name.toAscii());
    if (name == QString("AGENT")) {
        // TODO: QVersitDocument embDoc = versitProperty.embeddedDocument();  
    } else {
        // 2. Encode the property parameters and values (if they exist)
        QMultiMap<QString,QString> paramsMap = versitProperty.parameters();
        QList<QString> keys = paramsMap.uniqueKeys();
        foreach (QString param, keys) {
            QStringList values = paramsMap.values(param);
            foreach (QString value, values) {
                propertyArray.append(";");
                if (!(param.contains("TYPE",Qt::CaseInsensitive))) {
                    propertyArray.append(param.toAscii());
                    propertyArray.append("=");
                }
                propertyArray.append(value.toAscii());
            }
        }
        
        // 3. Encode the property value
        propertyArray.append(":");
        QByteArray value = versitProperty.value();
        propertyArray.append(value);
        propertyArray.append("\r\n");
    }
    return propertyArray;
}
