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

#ifndef QVERSITREADER_H
#define QVERSITREADER_H

#include "qversitdocument.h"
#include "qmobilityglobal.h"

#include <QObject>

class QIODevice;
class QTextCodec;

QTM_BEGIN_NAMESPACE

class QVersitReaderPrivate;

// reads a QVersitDocument from i/o device
class Q_VERSIT_EXPORT QVersitReader : public QObject
{
    Q_OBJECT  
    
public:
    enum Error {
        NoError = 0,
        UnspecifiedError,
        IOError,
        OutOfMemoryError,
        NotReadyError,
        ParseError,
        InvalidCharsetError,
        BadDeviceError
    };

    enum State {
        InactiveState = 0,   // operation not yet started
        ActiveState,         // operation started, not yet finished
        CanceledState,       // operation is finished due to cancelation
        FinishedState        // operation successfully completed
    };

    QVersitReader();
    ~QVersitReader();

    // input:
    void setDevice(QIODevice* device);
    QIODevice* device() const;

    void setDefaultCodec(QTextCodec* codec);
    QTextCodec* defaultCodec();

    // XXX Other Qt classes have something like:
    // QVersitDocument read() or QList<QVersitDocument> readAll() etc..
    // we need to either pick an async signal driven API or an iterative while(state!=finished) read{}
    // (or something)

    // reading:
    bool startReading(); // XXX these two functions are still a little confusing/not intuitive
    QList<QVersitDocument> readAll();

    // output:
    QList<QVersitDocument> results() const;

    State state() const;
    Error error() const; // XXX If error are we in FinishedState ?

signals:
    void finished();
    // XXX No incremental progress!
    // void progress(QList<QVersitDocument>& documents) ?
    
private: // data
    QVersitReaderPrivate* d;   
};

QTM_END_NAMESPACE

#endif // QVERSITREADER_H
