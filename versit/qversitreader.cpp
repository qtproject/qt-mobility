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


#include "qversitreader.h"
#include "qversitreader_p.h"
#include "versitutils.h"

/*!
 * \class QVersitReader
 *
 * \brief QVersitReader provides an interface
 * for parsing versit documents such as vCards from a stream.
 *
 * QVersitReader converts 0..n versit documents such as vCards
 * from a test stream into 0..n QVersitDocument instances.
 * QVersitReader supports reading from an abstract I/O device
 * which can be for example a file or a memory buffer.
 * The reading can be done either synchronously or asynchronously.
 *
 * \code
 * // An example of reading a simple vCard from a memory buffer:
 * QBuffer vCardBuffer;
 * vCardBuffer.open(QBuffer::ReadWrite);
 * QByteArray vCard =
 *     "BEGIN:VCARD\r\nVERSION:2.1\r\nN:Simpson;Homer;J;;\r\nEND:VCARD\r\n";
 * vCardBuffer.write(vCard);
 * vCardBuffer.seek(0);
 * QVersitReader reader;
 * reader.setDevice(&vCardBuffer);
 * reader.readSynchronously();
 * QList<QVersitDocument> versitDocuments = reader.result();
 * // Use the resulting document(s)...
 * \endcode
 *
 * \sa QVersitDocument
 */

/*!
 * \fn QVersitReader::readingDone()
 * The signal is emitted by the reader when the asynchronous reading has been completed.
 */

/*! Construct a reader. */
QVersitReader::QVersitReader() : d(new QVersitReaderPrivate)
{
    connect(d,SIGNAL(finished()),this,SIGNAL(readingDone()),Qt::DirectConnection);
}
    
/*! Destroys the reader. */
QVersitReader::~QVersitReader()
{
    d->wait();
    delete d;
}

/*!
 * Sets the device used for reading the input to \a device.
 */
void QVersitReader::setDevice(QIODevice* device)
{
    d->mIoDevice = device;
}

/*!
 * Returns the device used for reading input.
 */
QIODevice* QVersitReader::device() const
{
    return d->mIoDevice;
}

/*!
 * Starts reading the input asynchronously.
 * Returns false if the input device has not been set or opened
 * or if there is another asynchronous read operation already pending.
 * Signal \l readingDone is emitted when the reading has finished.
 */
bool QVersitReader::startAsynchronousReading()
{
    bool started = false;
    if (d->mIoDevice && d->mIoDevice->isOpen() && !d->isRunning()) {
        d->mVersitDocuments.clear();
        d->start();
        started = true;
    }
    return started;
}

/*!
 * Reads the input synchronously.
 * Using this function may block the user thread for an undefined period.
 * In most cases asynchronous \l startAsynchronousReading should be used instead.
 */
bool QVersitReader::readSynchronously()
{
    bool ok = startAsynchronousReading();
    if (ok)
        d->wait();
    return ok;
}

/*!
 * Returns the reading result or an empty list
 * if the reading was not completed successfully.
 */
QList<QVersitDocument> QVersitReader::result() const
{
    return d->mVersitDocuments;
}
