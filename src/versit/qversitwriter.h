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

#ifndef QVERSITWRITER_H
#define QVERSITWRITER_H

#include "qversitdocument.h"
#include "qmobilityglobal.h"

#include <QObject>
#include <QIODevice>

QTM_BEGIN_NAMESPACE

class QVersitWriterPrivate;

// writes a QVersitDocument to i/o device
class Q_VERSIT_EXPORT QVersitWriter : public QObject
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

    QVersitWriter();
    ~QVersitWriter();

    // XXX it seems like most Qt classes have something like:
    // setDevice(output)
    // write(input data)
    // so we should perhaps remove these functions and move it to
    // writeAll/startWriting

    // input:
    void setInput(const QList<QVersitDocument>& input);
    QList<QVersitDocument> input() const;

    // output:
    void setDevice(QIODevice* device);
    QIODevice* device() const;

    // writing:
    bool startWriting(); // XXX again, perhaps add list param here, and maybe change names to be more intuitive
    bool writeAll();

    State state() const;
    Error error() const;

signals:
    void finished();
    // XXX Do we need incremental results?  I think we're more likely to just write it to the device, so no

private: // data
    QVersitWriterPrivate* d;
};

QTM_END_NAMESPACE

#endif // QVERSITWRITER_H
