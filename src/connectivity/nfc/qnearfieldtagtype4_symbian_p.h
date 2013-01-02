/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QNEARFIELDTAGTYPE4SYMBIAN_H
#define QNEARFIELDTAGTYPE4SYMBIAN_H

#include <qnearfieldtagtype4.h>
#include "symbian/nearfieldndeftarget_symbian.h"
#include "symbian/nearfieldtag_symbian.h"
#include "symbian/nearfieldtagimpl_symbian.h"
#include "symbian/debug.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE



class QNearFieldTagType4Symbian : public QNearFieldTagType4, private QNearFieldTagImpl<QNearFieldTagType4Symbian>
{
    Q_OBJECT

public:

    explicit QNearFieldTagType4Symbian(CNearFieldNdefTarget *tag, QObject *parent = 0);

    ~QNearFieldTagType4Symbian();

    virtual QByteArray uid() const;
    Type type() const { return NfcTagType4; }
    quint8 version();

    bool hasNdefMessage();
    RequestId readNdefMessages();
    RequestId writeNdefMessages(const QList<QNdefMessage> &messages);

    RequestId sendCommand(const QByteArray &command);
    RequestId sendCommands(const QList<QByteArray> &commands);

    RequestId select(const QByteArray &name);
    RequestId select(quint16 fileIdentifier);

    RequestId read(quint16 length = 0, quint16 startOffset = 0);
    RequestId write(const QByteArray &data, quint16 startOffset = 0);
    bool isProcessingCommand() const { return _isProcessingRequest(); }
    bool waitForRequestCompleted(const RequestId &id, int msecs = 5000);

    void setAccessMethods(const QNearFieldTarget::AccessMethods& accessMethods)
    {
        _setAccessMethods(accessMethods);
    }

    QNearFieldTarget::AccessMethods accessMethods() const
    {
        return _accessMethods();
    }

    void handleTagOperationResponse(const RequestId &id, const QByteArray &command, const QByteArray &response, bool emitRequestCompleted);
    QVariant decodeResponse(const QByteArray &command, const QByteArray &response);

private:
    RequestId selectNdefApplication();
    RequestId selectCC();

    friend class QNearFieldTagImpl<QNearFieldTagType4Symbian>;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QNEARFIELDTAGTYPE4SYMBIAN_H
