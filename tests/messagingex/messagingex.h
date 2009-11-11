#ifndef MESSAGINGEX_H
#define MESSAGINGEX_H

#include <QtGui>

#include "ui_messagingex.h"
#include "ui_smsreceiveddialog.h"
#include "ui_accountdialog.h"
#include "ui_mmsreceiveddialog.h"

#include "QMessageServiceAction.h"
#include "QMessage.h"
#include "QMessageAccount.h"

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
	QMessageServiceAction m_serviceaction;
	QMessageStore* m_store;
	
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
