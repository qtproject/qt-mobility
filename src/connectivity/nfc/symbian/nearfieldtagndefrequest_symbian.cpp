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

#include "nearfieldtagndefrequest_symbian.h"
#include "nearfieldutility_symbian.h"
#include "debug.h"

NearFieldTagNdefRequest::NearFieldTagNdefRequest()
{
    iReadMessages = 0;
}

void NearFieldTagNdefRequest::IssueRequest()
{
    BEGIN
    if (iOperator)
    {
        LOG(iType);
        switch(iType)
        {
            case EReadRequest: 
            {
                iOperator->DoReadNdefMessages(this);
                break;
            }
            case EWriteRequest:
            {
                iOperator->DoSetNdefMessages(iMessages, this);
                break;
            }
        }
    }
    END
}

void NearFieldTagNdefRequest::ReadComplete(TInt aError, RPointerArray<CNdefMessage> * aMessage)
{
    BEGIN
    LOG(aError);
    iReadMessages = aMessage;
    ProcessResponse(aError);
    END
}

void NearFieldTagNdefRequest::WriteComplete(TInt aError)
{
    BEGIN
    ProcessResponse(aError);
    END
}

void NearFieldTagNdefRequest::ProcessEmitSignal(TInt aError)
{
    BEGIN
    LOG("error code is "<<aError<<" request type is "<<iType);
    if (aError != KErrNone)
    {
        iOperator->EmitError(aError);
    }
    else
    {
        if (EReadRequest == iType)
        {
            // since there is no error, iReadMessages can't be NULL.
            for(int i = 0; i < iReadMessages->Count(); ++i)
            {
                QNdefMessage message = QNFCNdefUtility::FromCNdefMsgToQndefMsgL(*(*iReadMessages)[i]);
                iOperator->EmitNdefMessageRead(message);
            }
        }
        else if (EWriteRequest == iType)
        {
            iOperator->EmitNdefMessagesWritten();
        }
    }
    END
}

void NearFieldTagNdefRequest::HandleResponse(TInt /*aError*/)
{
    BEGIN
    END
}
