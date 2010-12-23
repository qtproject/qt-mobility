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
#include "nearfieldtagcommandsrequest_symbian.h"
#include "nearfieldutility_symbian.h"
#include "debug.h"

QTM_USE_NAMESPACE

NearFieldTagCommandsRequest::NearFieldTagCommandsRequest()
{
    iCurrentCommand = 0;
    iRequestCancelled = EFalse;
}

NearFieldTagCommandsRequest::~NearFieldTagCommandsRequest()
{
    BEGIN
    iRequestCancelled = ETrue;
    iOperator->DoCancelSendCommand();
    END
}

void NearFieldTagCommandsRequest::IssueRequest()
{
    BEGIN
    LOG("current command index = "<<iCurrentCommand);
    LOG("commands count = "<<iCommands.count());
    iRequestIssued = ETrue;
    if (iCurrentCommand < iCommands.count())
    {       
        iOperator->DoSendCommand(iCommands.at(iCurrentCommand), this);
        ++iCurrentCommand;
    }
    END
}

void NearFieldTagCommandsRequest::ProcessResponse(TInt aError)
{
    BEGIN
    LOG("error is "<<aError);
    QByteArray result;
    if (KErrNone == aError)
    {
        result = QNFCNdefUtility::FromTDesCToQByteArray(*iResponse);
        LOG("result is "<<result);
        LOG("clear the buffer");
        iResponse->Zero();
        iDecodedResponses.append(iOperator->decodeResponse(iCommands.at(iCurrentCommand - 1), result)); 
    }
    else
    {
        // error occurs
        LOG("error occurs, append QVariant() to rest list");
        for (--iCurrentCommand; iCurrentCommand < iCommands.count(); ++iCurrentCommand)
        {
            iDecodedResponses.append(QVariant());
        }
    }

    if (!iRequestCancelled && (iCurrentCommand < iCommands.count()))
    {
        LOG("issue another command in command list");
        IssueRequest();
    }
    else
    {
        // all commands finished
        LOG("all commands completed");
        MNearFieldTagAsyncRequest::ProcessResponse(aError);
    }
    END
}

void NearFieldTagCommandsRequest::HandleResponse(TInt /*aError*/)
{
    BEGIN
    iOperator->HandleResponse(iId, iDecodedResponses);
    END
}

void NearFieldTagCommandsRequest::CommandComplete(TInt aError)
{
    BEGIN
    ProcessResponse(aError);
    END
}

void NearFieldTagCommandsRequest::ProcessEmitSignal(TInt aError)
{
    BEGIN
    LOG(aError);
    if (aError != KErrNone)
    {
        iOperator->EmitError(aError, iId);
    }
    END
}
