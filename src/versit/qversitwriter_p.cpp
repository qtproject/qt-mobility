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

#include "qversitwriter_p.h"
#include "versitutils_p.h"
#include "qmobilityglobal.h"

#include <QStringList>

QTM_BEGIN_NAMESPACE

#define MAX_CHARS_FOR_LINE 76

/*! Constructs a writer. */
QVersitWriterPrivate::QVersitWriterPrivate()
    : mIoDevice(0)
{
}

/*! Constructs a writer. */
QVersitWriterPrivate::QVersitWriterPrivate(
    const QByteArray& documentType,
    const QByteArray& version)
    : mIoDevice(0), mDocumentType(documentType), mVersion(version)
{
}

/*! Destroys a writer. */
QVersitWriterPrivate::~QVersitWriterPrivate()
{
}

/*!
 * Checks whether the writer is ready for writing.
 */
bool QVersitWriterPrivate::isReady() const
{
    return (mIoDevice && mIoDevice->isOpen() && !mVersitDocument.properties().empty());
}

/*!
 * Do the actual writing.
 */
bool QVersitWriterPrivate::write()
{
    bool ok = false;
    if (isReady()) {
        QByteArray output = encodeVersitDocument(mVersitDocument);
        ok = (mIoDevice->write(output) > 0);
    }

    return ok;
}

/*!
 * Inherited from QThread, called by QThread when the thread has been started.
 */
void QVersitWriterPrivate::run()
{
    write();
}

/*!
* Encodes the \a document to text.
*/
QByteArray QVersitWriterPrivate::encodeVersitDocument(const QVersitDocument& document)
{
    QList<QVersitProperty> properties = document.properties();
    QByteArray encodedDocument;

    encodedDocument += "BEGIN:" + mDocumentType + "\r\n";
    encodedDocument += "VERSION:" + mVersion + "\r\n";
    foreach (QVersitProperty property, properties) {
        encodedDocument.append(encodeVersitProperty(property));
    }
    encodedDocument += "END:" + mDocumentType + "\r\n";

    VersitUtils::fold(encodedDocument,MAX_CHARS_FOR_LINE);
    return encodedDocument;
}

/*!
 * Encodes the groups and name in the \a property to text.
 */
QByteArray QVersitWriterPrivate::encodeGroupsAndName(
    const QVersitProperty& property) const
{
    QByteArray encodedGroupAndName;
    QStringList groups = property.groups();
    if (!groups.isEmpty()) {
        QString groupAsString = groups.join(QString::fromAscii("."));
        encodedGroupAndName.append(groupAsString.toAscii());
        encodedGroupAndName.append(".");
    }
    encodedGroupAndName.append(property.name().toAscii());
    return encodedGroupAndName;
}

#include "moc_qversitwriter_p.cpp"

QTM_END_NAMESPACE
