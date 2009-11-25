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
#include "qvcard30writer.h"
#include "versitutils.h"

#include <QStringList>

QTM_BEGIN_NAMESPACE

/*!
 * \class QVersitWriter
 *
 * \brief QVersitWriter provides an interface
 * for writing a versit document such as a vCard to a text stream.
 *
 * QVersitWriter converts a QVersitDocument into its textual representation.
 * QVersitWriter supports writing to an abstract I/O device
 * which can be for example a file or a memory buffer.
 * The writing can be done synchronously or asynchronously.
 *
 * \code
 * // An example of writing a simple vCard to a memory buffer:
 * QBuffer vCardBuffer;
 * vCardBuffer.open(QBuffer::ReadWrite);
 * QVersitWriter writer;
 * writer.setDevice(&vCardBuffer);
 * QVersitDocument document;
 * QVersitProperty property;
 * property.setName("N");
 * property.setValue("Simpson;Homer;J;;");
 * document.addProperty(property);
 * writer.setVersitDocument(document);
 * if (writer.writeAll()) {
 *     // Use the vCardBuffer...
 * }
 * \endcode
 *
 * \sa QVersitDocument, QVersitProperty
 */

/*!
 * \fn QVersitWriter::writingDone()
 * The signal is emitted by the writer when the asynchronous writing has been completed.
 */

/*! Constructs a new writer. */
QVersitWriter::QVersitWriter() : d(new QVCard21Writer)
{
    connect(d,SIGNAL(finished()),this,SIGNAL(writingDone()),Qt::DirectConnection);
}

/*! 
 * Frees the memory used by the writer. 
 * Waits until a pending asynchronous writing has been completed.
 */
QVersitWriter::~QVersitWriter()
{
    d->wait();
    delete d;
}

/*!
 * Set the versit document to be written to \a versitDocument and
 * selects the actual writer implementation based on the versit document type.
 */
void QVersitWriter::setVersitDocument(const QVersitDocument& versitDocument)
{
    QVersitWriterPrivate* updatedWriter = 0;
    switch (versitDocument.versitType()) {
        case QVersitDocument::VCard21:
            updatedWriter = new QVCard21Writer;
            break;
        case QVersitDocument::VCard30:
            updatedWriter = new QVCard30Writer;
            break;
        default:
            break;
    }
    if (updatedWriter) {
        updatedWriter->mIoDevice = d->mIoDevice;
        delete d;
        d = updatedWriter;
        connect(d,SIGNAL(finished()),this,SIGNAL(writingDone()),Qt::DirectConnection);
    }
    d->mVersitDocument = versitDocument;
}

/*!
 * Returns the current versit document.
 */
QVersitDocument QVersitWriter::versitDocument() const
{
    return d->mVersitDocument;
}

/*!
 * Sets the device used for writing to \a device.
 */
void QVersitWriter::setDevice(QIODevice* device)
{
    d->mIoDevice = device;
}

/*!
 * Returns the device used for writing.
 */
QIODevice* QVersitWriter::device() const
{
    return d->mIoDevice;
}

/*!
 * Starts writing the output asynchronously.
 * Returns false if the output device has not been set or opened or
 * if there is another asynchronous write operation already pending.
 * Signal \l writingDone() is emitted when the writing has finished.
 */
bool QVersitWriter::startWriting()
{
    bool started = false;
    if (d->isReady() && !d->isRunning()) {
        d->start();
        started = true;
    }

    return started;
}

/*!
 * Writes the output synchronously.
 * Returns false if the output device has not been set or opened or
 * if there is an asynchronous write operation pending.
 * Using this function may block the user thread for an undefined period.
 * In most cases asynchronous \l startWriting() should be used instead.
 */
bool QVersitWriter::writeAll()
{
    bool ok = false;
    if (!d->isRunning())
        ok = d->write();
    return ok;
}

QTM_END_NAMESPACE
