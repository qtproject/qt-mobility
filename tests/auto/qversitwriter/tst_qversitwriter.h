/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef tst_QVERSITWRITER_H
#define tst_QVERSITWRITER_H

#include <QObject>
#include <QBuffer>
#include <qmobilityglobal.h>
#include "qversitwriter.h"

QTM_BEGIN_NAMESPACE

class QVersitWriterPrivate;

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

// Poor man's QSignalSpy because I couldn't get QSignalSpy to work with the user type QVR::State.
class SignalCatcher : public QObject
{
Q_OBJECT
public slots:
    void stateChanged(QVersitWriter::State state) {
        mReceived.append(state);
    }

public:
    QList<QVersitWriter::State> mReceived;
};

class tst_QVersitWriter : public QObject
{
     Q_OBJECT

private slots: // Tests

    void init();
    void cleanup();

    void testDevice();
    void testDefaultCodec();
    void testWritingVersions();
    void testWriting21();
    void testWriting30();
    void testByteArrayOutput();
    void testWritingDocument();
    void testWritingDocument_data();

private: // Data
    QVersitWriter* mWriter;
    QBuffer* mOutputDevice;
    SignalCatcher* mSignalCatcher;
};

#endif // tst_QVERSITWRITER_H
