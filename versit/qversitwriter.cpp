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

#include "qversitwriter.h"
#include "qvcard21writer.h"
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
    d = new QVCard21Writer;
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
        QByteArray output = d->encodeVersitDocument(d->mVersitDocument);
        if (d->mIoDevice->write(output) > 0) {
            return true;
        }
        return false;
    }
    return false;
}
