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
#include "versitutils_p.h"
#include "qmobilityglobal.h"

#include <QTextCodec>
#include <QMutexLocker>

QTM_USE_NAMESPACE

/*!
  \class QVersitReader
 
  \brief The QVersitReader class provides an interface
  for reading versit documents such as vCards from a stream.

  \ingroup versit
 
  QVersitReader reads 0..n versit documents such as vCards
  from a text stream into 0..n QVersitDocument instances.
  QVersitReader supports reading from an abstract I/O device
  which can be for example a file or a memory buffer.
  The reading can be done asynchronously, and the
  waitForFinished() function can be used to make a blocking
  read.
 
  \code
  // An example of reading a simple vCard from a memory buffer:
  QBuffer vCardBuffer;
  vCardBuffer.open(QBuffer::ReadWrite);
  QByteArray vCard =
      "BEGIN:VCARD\r\nVERSION:2.1\r\nN:Citizen;John;Q;;\r\nEND:VCARD\r\n";
  vCardBuffer.write(vCard);
  vCardBuffer.seek(0);
  QVersitReader reader;
  reader.setDevice(&vCardBuffer);
  reader.startReading();
  reader.waitForFinished();
  QList<QVersitDocument> versitDocuments = reader.results();
  // Use the resulting document(s)...
  }
  \endcode
 
  \sa QVersitDocument
 */

/*!
 * \fn QVersitReader::finished()
 * The signal is emitted by the reader when the asynchronous reading has been completed.
 */

/*! Constructs a new reader. */
QVersitReader::QVersitReader() : d(new QVersitReaderPrivate)
{
    connect(d, SIGNAL(stateChanged(QVersitReader::State)),
            this, SIGNAL(stateChanged(QVersitReader::State)),Qt::DirectConnection);
    connect(d, SIGNAL(resultsAvailable(QList<QVersitDocument>&)),
            this, SIGNAL(resultsAvailable(QList<QVersitDocument>&)), Qt::DirectConnection);
}
    
/*! 
 * Frees the memory used by the reader. 
 * Waits until a pending asynchronous reading has been completed.
 */
QVersitReader::~QVersitReader()
{
    d->wait();
    delete d;
}

/*!
 * Sets the device used for reading the input to be the given device \a device.
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
 * Sets the codec for the reader to use when parsing the input stream.
 * This codec is not used when overridden with a CHARSET Versit parameter.
 */
void QVersitReader::setDefaultCodec(QTextCodec *codec)
{
    if (codec != NULL) {
        d->mDefaultCodec = codec;
    } else {
        d->mDefaultCodec = QTextCodec::codecForName("ISO 8859-1");
    }
}

/*!
 * Returns the codec the reader uses when parsing the input stream.
 */
QTextCodec* QVersitReader::defaultCodec()
{
    return d->mDefaultCodec;
}

/*!
 * Starts reading the input asynchronously.
 * Returns false if the input device has not been set or opened or
 * if there is another asynchronous read operation already pending.
 * Signal \l stateChanged() is emitted with parameter FinishedState
 * is emitted when the reading has finished.
 */
bool QVersitReader::startReading()
{    if (d->state() == ActiveState || d->isRunning()) {
        d->setError(QVersitReader::NotReadyError);
        return false;
    } else if (!d->mIoDevice || !d->mIoDevice->isReadable()) {
        d->setError(QVersitReader::IOError);
        return false;
    } else {
        d->setState(ActiveState);
        d->setError(NoError);
        d->setCanceling(false);
        d->start();
        return true;
    }
}

/*!
 * Attempts to asynchronously cancel the read request.
 */
void QVersitReader::cancel()
{
    d->setCanceling(true);
}

/*!
 * If the state is ActiveState, blocks until the reader has finished reading or \a msec milliseconds
 * has elapsed, returning true if it successfully finishes or is cancelled by the user.
 * If the state is FinishedState, returns true immediately.
 * Otherwise, returns false immediately.
 */
bool QVersitReader::waitForFinished(int msec)
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
 * Returns the reading result.  Even if there was an error, a partial list of results may be
 * returned.
 */
QList<QVersitDocument> QVersitReader::results() const
{
    QMutexLocker locker(&d->mMutex);
    return d->mVersitDocuments;
}

/*!
 * Returns the state of the reader.
 */
QVersitReader::State QVersitReader::state() const
{
    return d->state();
}

/*!
 * Returns the error encountered by the last operation.
 */
QVersitReader::Error QVersitReader::error() const
{
    return d->error();
}

#include "moc_qversitreader.cpp"
