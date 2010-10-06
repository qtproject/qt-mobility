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

#ifndef QFSNMASYNCOPERATIONS_SYMBIAN_H
#define QFSNMASYNCOPERATIONS_SYMBIAN_H

#include <QObject>
#include <e32base.h>
#include <qmessageglobal.h>
#include <qmessage.h>
#include "qmessageservice_symbian_p.h"

#include <nmapimessage.h>
#include <nmapimessagemanager.h>
#include <nmapiemailservice.h>


using namespace EmailClientApi;

QTM_BEGIN_NAMESPACE

class CFSAsynchronousOperation : public QObject
{
    Q_OBJECT
    
protected:
    CFSAsynchronousOperation(); 
    CFSAsynchronousOperation(QMessageServicePrivate &privateService); 

public:
    virtual ~CFSAsynchronousOperation();
    
    void createFSMessage(QMessage &message, NmApiMessage &fsMessage);

protected:
    QMessageServicePrivate *m_privateService;

};



class CFSAsynchronousSendOperation : public CFSAsynchronousOperation
{
    Q_OBJECT
    
public:
    CFSAsynchronousSendOperation(QMessageServicePrivate &privateService); 
    ~CFSAsynchronousSendOperation();

    void sendMessage(QMessage &message);
    
public slots:
    void createDraftMessageCompleted(int success, QVariant message);
    void saveCompleted(int success);
    void sendCompleted(int success);
    
signals:    
    void messageSend(int success, CFSAsynchronousSendOperation *operation);
    
private:
    void saveMessage();
    void createDraftMessage();
    void sendMessage();
    
private:
    NmApiMessage m_fsMessage;
    QMessage m_qMessage;
    NmApiMessageManager* m_manager;

};

class CFSAsynchronousAddOperation : public CFSAsynchronousOperation
{
    Q_OBJECT
    
public:
    CFSAsynchronousAddOperation(); 
    ~CFSAsynchronousAddOperation();

    void addMessage(QMessage &message);
    
public slots:
    void createDraftMessageCompleted(int success, QVariant message);
    void saveCompleted(int success);
    
signals:    
    void messageAdded(int success, CFSAsynchronousAddOperation *operation);
    
private:
    void saveMessage();
    void createDraftMessage();
    
private:
    NmApiMessage m_fsMessage;
    QMessage m_qMessage;
    NmApiMessageManager* m_manager;

};

class CFSAsynchronousRetrieveBodyOperation : public CFSAsynchronousOperation

{
    Q_OBJECT
  
public:
    CFSAsynchronousRetrieveBodyOperation(QMessageServicePrivate &privateService);
    ~CFSAsynchronousRetrieveBodyOperation();
    
    void retrieveBody(QMessageId &messageId);
    
private:

};

class CFSAsynchronousRetrieveAttachmentOperation : public CFSAsynchronousOperation

{
    Q_OBJECT
  
public:
    CFSAsynchronousRetrieveAttachmentOperation(QMessageServicePrivate &privateService);
    ~CFSAsynchronousRetrieveAttachmentOperation();
    
    void retrieveAttachment(QMessageId &messageId);
    
private:

};

class CFSAsynchronousSynchronizeOperation : public CFSAsynchronousOperation
{
    Q_OBJECT
    
public:
    CFSAsynchronousSynchronizeOperation(NmApiEmailService *emailService, QMessageServicePrivate &privateService); 
    virtual ~CFSAsynchronousSynchronizeOperation();

    void syncronizeMailbox(const QMessageAccountId &id);
    
public slots:
    void syncronizeCompleted(int success);
    
signals:    
    void syncronized(int success, CFSAsynchronousSynchronizeOperation *operation);
    
private:
    NmApiEmailService *m_emailService;
};

QTM_END_NAMESPACE

#endif // QFSENGINE_SYMBIAN_H
