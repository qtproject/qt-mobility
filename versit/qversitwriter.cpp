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

/*!
 * \class QVersitWriter
 *
 * \brief The QVersitWriter class provides an interface 
 * for encoding vCards from a versit document into a stream. 
 *
 * \sa
 */

/*!
 * \fn QVersitWriter::writingDone()
 * The signal is emitted by the writer when the asynchronous writing has been completed.
 */

/*! Constructs a writer. */
QVersitWriter::QVersitWriter() : d(new QVCard21Writer)
{
    connect(d,SIGNAL(finished()),this,SIGNAL(writingDone()),Qt::DirectConnection);
}

/*! Destroys the writer. */
QVersitWriter::~QVersitWriter()
{
    d->wait();
    delete d;
}

/*!
 * Set the versit document to be written and
 * selects the writer implementation based on the versit document type.
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
 * Starts writing the output asynchronously.
 * Signal writingDone is emitted when the writing has finished.
 */
bool QVersitWriter::startAsynchronousWriting()
{
    bool started = false;
    if (d->mIoDevice && d->mIoDevice->isOpen() && !d->isRunning()) {
        d->start();
        started = true;
    }
    return started;
}

/*!
 * Writes the output synchronously.
 * Using this function may block the user thread for an undefined period.
 * In most cases asynchronous startAsynchronousWriting should be used instead.
 */
bool QVersitWriter::writeSynchronously()
{
    bool ok = startAsynchronousWriting();
    if (ok)
        d->wait();
    return ok;
}
