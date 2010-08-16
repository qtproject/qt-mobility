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
#ifndef MESSAGINGEX_H
#define MESSAGINGEX_H

#include <QtGui>

#include "ui_messagingex.h"
#include "ui_smsreceiveddialog.h"
#include "ui_accountdialog.h"
#include "ui_mmsreceiveddialog.h"

#include "qmessageservice.h"
#include "qmessagemanager.h"
#include "qmessage.h"
#include "qmessageaccount.h"

QTM_USE_NAMESPACE

class MessagingEx : public QMainWindow, public Ui::MessagingExMainWindow
{
    Q_OBJECT

public:
    MessagingEx(QWidget* parent = 0);
    void createMenus();
    
private Q_SLOTS:
    void send();
    void on_sendSmsButton_clicked();
    void on_sendMmsButton_clicked();
    void on_sendEmailButton_clicked();
    void messageReceived(const QMessageId& aId);
    void messageRemoved(const QMessageId& aId);
    void messageUpdated(const QMessageId& aId);
    void accountSelected(int index);
    void addAttachment();
    void removeAttachment();
    void queryMessages();
    void composeSMS();
    void composeMMS();
    void composeEmail();
    void addMessage();
   
    void sortType();
    void sortSender();
    void sortRecipient();
    void sortSubject();
    void sortTimestamp();
    void sortReceptiontimestamp();
    void sortStatus();
    void sortParentAccountId();
    void sortStandardFolder();
    void findMessages();
    
    void messagesFound(const QMessageIdList &ids);

private:
    QMessageService m_service;
    QMessageManager m_manager;
        
    QAction* m_createEmail;
    QAction* m_createSms;
    QAction* m_createMms;
    QAction* m_sortMessages;
    QList<QAction*> m_accountActions;
    QStringList m_attachments;
    QString m_fileNames;
    
    QMenu *filterMenu;
    QMenu *composeMenu;
    QAction* m_composeSMS;
    QAction* m_composeMMS;
    QAction* m_composeEmail;
    QAction* m_sortId;
    QAction* m_sortType;
    QAction* m_sortSender;
    QAction* m_sortRecipient;
    QAction* m_sortSubject;
    QAction* m_sortTimestamp;
    QAction* m_sortReceptiontimestamp;
    QAction* m_sortStatus;
    QAction* m_sortParentAccountId;
    QAction* m_sortStandardFolder;
    QAction* m_result;
    
    QList<QMessageAccountId> m_accountList;
    QMessageAccount m_account;
};

class SMSReceivedDialog : public QDialog, public Ui::SMSReceivedDialog
{
    Q_OBJECT

public:
    SMSReceivedDialog(const QMessage& aMessage, QWidget* parent = 0);
    
};

class MMSReceivedDialog : public QDialog, public Ui::MMSReceivedDialog
{
    Q_OBJECT

public:
    MMSReceivedDialog(const QMessage& aMessage, QWidget* parent = 0);
    
};

class AccountDialog : public QDialog, public Ui::AccountDialog
{
    Q_OBJECT

public:
    AccountDialog(const QMessageAccount& account, QWidget* parent = 0);
};
 
#endif // MESSAGINGEX_H

// End of file
