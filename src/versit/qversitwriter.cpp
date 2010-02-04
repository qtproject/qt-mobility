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
#include "qversitwriter_p.h"
#include "versitutils_p.h"
#include "qmobilityglobal.h"

#include <QStringList>
#include <QTextCodec>

QTM_USE_NAMESPACE

/*!
  \class QVersitWriter
  \preliminary
  \brief The QVersitWriter class provides an interface
  for writing a versit document such as a vCard to a text stream.

  \ingroup versit
 
  QVersitWriter converts a QVersitDocument into its textual representation.
  QVersitWriter supports writing to an abstract I/O device
  which can be for example a file or a memory buffer.
  The writing can be done asynchronously and the waitForFinished()
  function can be used to implement a blocking write.
 
  \code
  // An example of writing a simple vCard to a memory buffer:
  QBuffer vCardBuffer;
  vCardBuffer.open(QBuffer::ReadWrite);
  QVersitWriter writer;
  writer.setDevice(&vCardBuffer);
  QVersitDocument document;
  QVersitProperty property;
  property.setName("N");
  property.setValue("Citizen;John;Q;;");
  document.addProperty(property);
  writer.startWriting();
  if (writer.waitForFinished()) {
      // Use the vCardBuffer...
  }
  \endcode
 
  \sa QVersitDocument, QVersitProperty
 */

/*!
 * \fn QVersitWriter::stateChanged(QVersitWriter::State state)
 * The signal is emitted by the writer when its state has changed (eg. when it has finished
 * writing to the device).
 * \a state is the new state of the writer.
 */

/*! Constructs a new writer. */
QVersitWriter::QVersitWriter() : d(new QVersitWriterPrivate)
{
    connect(d, SIGNAL(stateChanged(QVersitWriter::State)),
            this, SIGNAL(stateChanged(QVersitWriter::State)), Qt::DirectConnection);
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
 * Sets the default codec for the writer to use for writing the entire output.
 *
 * If \a codec is NULL, the writer uses the codec according to the specification prescribed default.
 * (for vCard 2.1, ASCII; for vCard 3.0, UTF-8).
 */
void QVersitWriter::setDefaultCodec(QTextCodec *codec)
{
    d->mDefaultCodec = codec;
}

/*!
 * Returns the document's codec.
 */
QTextCodec* QVersitWriter::defaultCodec() const
{
    return d->mDefaultCodec;
}

/*!
 * Starts writing \a input to device() asynchronously.
 * Returns false if the output device has not been set or opened or
 * if there is another asynchronous write operation already pending.
 * Signal \l stateChanged() is emitted with parameter FinishedState
 * when the writing has finished.
 */
bool QVersitWriter::startWriting(const QList<QVersitDocument>& input)
{
    d->mInput = input;
    if (d->state() == ActiveState || d->isRunning()) {
        d->setError(QVersitWriter::NotReadyError);
        return false;
    } else if (!d->mIoDevice || !d->mIoDevice->isWritable()) {
        d->setError(QVersitWriter::IOError);
        return false;
    } else {
        d->setState(ActiveState);
        d->setError(NoError);
        d->start();
        return true;
    }
}

/*!
 * Attempts to asynchronously cancel the write request.
 */
void QVersitWriter::cancel()
{
    d->setCanceling(true);
}

/*!
 * If the state is ActiveState, blocks until the writer has finished writing or \a msec milliseconds
 * has elapsed, returning true if it successfully finishes or is cancelled by the user.
 * If the state is FinishedState, returns true immediately.
 * Otherwise, returns false immediately.
 */
bool QVersitWriter::waitForFinished(int msec)
{
    State state = d->state();
    if (state == ActiveState) {
        return d->wait(msec);
    } else if (state == FinishedState) {
        return true;
    } else {
        return false;
    }
}

/*!
 * Returns the state of the writer.
 */
QVersitWriter::State QVersitWriter::state() const
{
    return d->state();
}

/*!
 * Returns the error encountered by the last operation.
 */
QVersitWriter::Error QVersitWriter::error() const
{
    return d->error();
}


void Q_DECL_DEPRECATED QVersitWriter::setVersitDocument(const QVersitDocument& versitDocument)
{
    qWarning("QVersitWriter::setVersitDocument(): This function was deprecated in week 4 and will be removed after the transition period has elapsed!  The document should be passed directly into startWriting().");
    QList<QVersitDocument> documents;
    documents.append(versitDocument);
    d->mInput = documents;
}

QVersitDocument Q_DECL_DEPRECATED QVersitWriter::versitDocument() const
{
    qWarning("QVersitWriter::versitDocument(): This function was deprecated in week 4 and will be removed after the transition period has elapsed!");
    return QVersitDocument();
}

bool Q_DECL_DEPRECATED QVersitWriter::startWriting()
{
    qWarning("QVersitWriter::startWriting(): This function was deprecated in week 4 and will be removed after the transition period has elapsed!  The versit document should be specified as a parameter.");
    return startWriting(d->mInput);
}

bool Q_DECL_DEPRECATED QVersitWriter::writeAll()
{
    qWarning("QVersitWriter::writeAll(): This function was deprecated in week 4 and will be removed after the transition period has elapsed!  startWriting() and waitForFinished() should be used instead.");
    startWriting(d->mInput);
    return waitForFinished();
}

#include "moc_qversitwriter.cpp"
