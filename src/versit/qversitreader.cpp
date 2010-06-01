/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include "qversitreader.h"
#include "qversitreader_p.h"
#include "versitutils_p.h"
#include "qmobilityglobal.h"

#include <QTextCodec>
#include <QMutexLocker>
#include <QBuffer>

QTM_USE_NAMESPACE

/*!
  \class QVersitReader
  \brief The QVersitReader class reads Versit documents such as vCards from a device.
  \ingroup versit

  QVersitReader concatenation of Versit documents such as vCards
  from a text stream and returns a list of QVersitDocument instances.
  QVersitReader supports reading from an abstract I/O device
  which can be for example a file or a memory buffer.
  The reading can be done asynchronously, and the
  waitForFinished() function can be used to make a blocking
  read.

  \sa QVersitDocument
 */

/*!
 * \enum QVersitReader::Error
 * This enum specifies an error that occurred during the most recent operation:
 * \value NoError The most recent operation was successful
 * \value UnspecifiedError The most recent operation failed for an undocumented reason
 * \value IOError The most recent operation failed because of a problem with the device
 * \value OutOfMemoryError The most recent operation failed due to running out of memory
 * \value NotReadyError The most recent operation failed because there is an operation in progress
 * \value ParseError The most recent operation failed because the input was malformed
 */

/*!
 * \enum QVersitReader::State
 * Enumerates the various states that a reader may be in at any given time
 * \value InactiveState Read operation not yet started
 * \value ActiveState Read operation started, not yet finished
 * \value CanceledState Read operation is finished due to cancellation
 * \value FinishedState Read operation successfully completed
 */

/*!
 * \fn QVersitReader::stateChanged(QVersitReader::State state)
 * The signal is emitted by the reader when its state has changed (eg. when it has finished
 * reading from the device).
 * \a state is the new state of the reader.
 */

/*!
 * \fn QVersitReader::resultsAvailable()
 * The signal is emitted by the reader as it reads from the device when it has made more Versit
 * documents available.
 */

/*! Constructs a new reader. */
QVersitReader::QVersitReader() : d(new QVersitReaderPrivate)
{
    d->init(this);
}

/*! Constructs a new reader that reads from \a inputDevice. */
QVersitReader::QVersitReader(QIODevice *inputDevice) : d(new QVersitReaderPrivate)
{
    d->init(this);
    d->mIoDevice = inputDevice;
}

/*! Constructs a new reader that reads from \a inputData. */
QVersitReader::QVersitReader(const QByteArray &inputData) : d(new QVersitReaderPrivate)
{
    d->init(this);
    d->mInputBytes.reset(new QBuffer);
    d->mInputBytes->setData(inputData);
    d->mInputBytes->open(QIODevice::ReadOnly);
    d->mIoDevice = d->mInputBytes.data();
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
 * Sets the device used for reading the input to be the given \a device.
 * Does not take ownership of the device.  This overrides any byte array input source set with
 * setData().
 */
void QVersitReader::setDevice(QIODevice* device)
{
    d->mInputBytes.reset(0);
    d->mIoDevice = device;
}

/*!
 * Returns the device used for reading input, or 0 if no device has been set (or if the input source
 * was set with setData().
 */
QIODevice* QVersitReader::device() const
{
    if (d->mInputBytes.isNull())
        return d->mIoDevice;
    else
        return 0;
}

/*!
 * Sets the data to read from to the byte array input source, \a inputData.
 * This overrides any device set with setDevice().
 */
void QVersitReader::setData(const QByteArray &inputData)
{
    if (d->mInputBytes.isNull())
        d->mInputBytes.reset(new QBuffer);
    d->mInputBytes->setData(inputData);
    d->mIoDevice = d->mInputBytes.data();
}

/*!
 * Sets \a codec as the codec for the reader to use when parsing the input stream to.
 * This codec is not used for values where the CHARSET Versit parameter occurs.
 */
void QVersitReader::setDefaultCodec(QTextCodec *codec)
{
    if (codec != NULL) {
        d->mDefaultCodec = codec;
    } else {
        d->mDefaultCodec = QTextCodec::codecForName("UTF-8");
    }
}

/*!
 * Returns the codec the reader uses when parsing the input stream.
 */
QTextCodec* QVersitReader::defaultCodec() const
{
    return d->mDefaultCodec;
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

/*!
 * Starts reading the input asynchronously.
 * Returns false if the input device has not been set or opened or
 * if there is another asynchronous read operation already pending.
 * Signal \l stateChanged() is emitted with parameter FinishedState
 * when the reading has finished.
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
 * Returns the reading result.  Even if there was an error or reading has not completed, a partial
 * list of results may be returned.
 */
QList<QVersitDocument> QVersitReader::results() const
{
    QMutexLocker locker(&d->mMutex);
    return d->mVersitDocuments;
}

#include "moc_qversitreader.cpp"
