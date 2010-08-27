/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
        NotReadyError
    };

    enum State {
        InactiveState = 0,
        ActiveState,
        CanceledState,
        FinishedState
    };

    QVersitWriter();
    QVersitWriter(QIODevice* outputDevice);
    QVersitWriter(QByteArray* outputBytes);
    ~QVersitWriter();

    // output device
    void setDevice(QIODevice* outputDevice);
    QIODevice* device() const;

    void setDefaultCodec(QTextCodec* codec);
    QTextCodec* defaultCodec() const;

    State state() const;
    Error error() const;

    // writing:
public Q_SLOTS:
    bool startWriting(const QList<QVersitDocument>& input);
    void cancel();
public:
    Q_INVOKABLE bool waitForFinished(int msec = -1);

Q_SIGNALS:
    void stateChanged(QVersitWriter::State state);

private: // data
    QVersitWriterPrivate* d;
};

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QVersitWriter::State))

#endif // QVERSITWRITER_H
