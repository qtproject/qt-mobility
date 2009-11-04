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

// Some big enough value for nested versit documents to prevent infite recursion
#define MAX_VERSIT_DOCUMENT_NESTING_DEPTH 20

/*!
 * \class QVersitReader
 *
 * \brief The QVersitReader class provides an interface 
 * for parsing versit documents such as vCards from a stream. 
 *
 * \sa
 */


/*! Construct a reader. */
QVersitReader::QVersitReader() : d(new QVersitReaderPrivate)
{
    connect(d,SIGNAL(finished()),this,SIGNAL(readingDone()),Qt::DirectConnection);
}
    
/*! Destroy a reader. */    
QVersitReader::~QVersitReader()
{
    d->wait();
    delete d;
}

/*!
 * Sets the \a device used for parsing input. 
 */
void QVersitReader::setDevice(QIODevice* device)
{
    d->mIoDevice = device;
}

/*!
 * Returns the \a device used for parsing input. 
 */
QIODevice* QVersitReader::device() const
{
    return d->mIoDevice;
}

/*!
 * Starts reading the input asynchronously.
 * Signal readingDone is emitted when the reading has finished.
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
 * In most cases asynchronous startAsynchronousReading should be used instead.
 */
bool QVersitReader::readSynchronously()
{
    bool ok = startAsynchronousReading();
    if (ok)
        d->wait();
    return ok;
}

/*!
 * Returns the parsing result or an empty list 
 * if the parsing was not completed successfully. 
 */
QList<QVersitDocument> QVersitReader::result() const
{
    return d->mVersitDocuments;
}
