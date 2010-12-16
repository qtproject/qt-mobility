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

#ifndef NEARFIELDTARGETOPERATION_H
#define NEARFIELDTARGETOPERATION_H

#include <qndefmessage.h>
#include <QList>

QTM_USE_NAMESPACE

class MNearFieldNdefOperationCallback;
class MNearFieldTagOperationCallback;

class MNearFieldTargetOperation
    {
public:
    virtual void DoReadNdefMessages(MNearFieldNdefOperationCallback * const aCallback) = 0;
    virtual void DoSetNdefMessages(const QList<QNdefMessage> &messages, MNearFieldNdefOperationCallback * const aCallback) = 0;
    virtual bool DoHasNdefMessages() = 0;
    virtual void DoSendCommand(const QByteArray& command, MNearFieldTagOperationCallback * const aCallback) = 0;
    
    virtual void DoCancelSendCommand() = 0;
    virtual void DoCancelNdefAccess() = 0;

    // Return true if next pending request is issued. 
    virtual bool IssueNextRequest() = 0;
    virtual void RemoveRequestFromQueue(QNearFieldTarget::RequestId aId) = 0;
    virtual QNearFieldTarget::RequestId AllocateRequestId() = 0;
    virtual bool HandleResponse(const QNearFieldTarget::RequestId &id, const QByteArray &command, const QByteArray &response) = 0;

    virtual void EmitNdefMessageRead(const QNdefMessage &message) = 0;
    virtual void EmitNdefMessagesWritten() = 0;
    virtual void EmitRequestCompleted(const QNearFieldTarget::RequestId &id) = 0;
    virtual void EmitError(int error) = 0;
    };

#endif

