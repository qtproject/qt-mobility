/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "qmessageserviceaction.h"
#include <QComboBox>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QPointer>
#include <QPushButton>
#include <QDebug>
#include <QLineEdit>
#include <QTextEdit>
#include "attachmentlistwidget.h"
#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>
#include <QThread>
#include <QStackedLayout>
#include <QPair>

typedef QPointer<QMessageServiceAction> QMessageServiceActionPtr;

static const QSize WindowGeometry(400,300);
static const QString WindowTitle("QMessageServiceAction Example");

class AccountIdsLoader : public QThread
{
public:
    void run();

    QMessageAccountIdList ids() const
    {
        if(isRunning())
            return QMessageAccountIdList();
        return m_accounts;
    }

private:
    QMessageAccountIdList m_accounts;
};

void AccountIdsLoader::run()
{
    m_accounts.clear();
    m_accounts = QMessageStore::instance()->queryAccounts();
}

typedef QPair<QMessageId,QString> IdSubjectPair;
typedef QList<IdSubjectPair> MessageIdSubjectList;

class MessageIdsLoader : public QThread
{
public:
    void run();

    MessageIdSubjectList ids() const
    {
        if(isRunning())
            return MessageIdSubjectList();
        return m_messages;
    }

private:
    MessageIdSubjectList m_messages;

};

void MessageIdsLoader::run()
{
    m_messages.clear();

    QMessageIdList lastTenMessages = QMessageStore::instance()->queryMessages(QMessageFilter(),QMessageOrdering::byReceptionTimeStamp(Qt::DescendingOrder),10,0);

    foreach(const QMessageId& id, lastTenMessages)
    {
        QMessage message(id);
        IdSubjectPair result(id,message.subject());
        m_messages.append(result);
    }
}

class ComposeSendWidget : public QWidget
{
    Q_OBJECT

public:
    ComposeSendWidget(QMessageServiceAction* service, QWidget* parent = 0);

protected:
    void showEvent(QShowEvent* e);
    void hideEvent(QHideEvent* e);

private slots:
    void composeButtonClicked();
    void sendButtonClicked();
    void addAttachmentButtonClicked();
    void updateAccounts();
    void updateAccountsStarted();
    void updateAccountsFinished();
    void updateAccountsAborted();

private:
    void setupUi();
    QMessage constructQMessage() const;

private:
    QStackedLayout* m_layoutStack;
    QWidget* m_composeWidget;
    QLabel* m_busyLabel;
    QMessageServiceAction* m_service;
    QComboBox* m_accountsCombo;
    QMessageAccountIdList m_availableAccounts;
    QLineEdit* m_toEdit;
    QLineEdit* m_subjectEdit;
    QTextEdit* m_bodyEdit;
    QPushButton* m_composeButton;
    QPushButton* m_sendButton;
    AttachmentListWidget* m_attachmentList;
    QPushButton* m_addAttachmentButton;
    AccountIdsLoader m_accountIdsLoader;
    bool m_loadedAccounts;
};

ComposeSendWidget::ComposeSendWidget(QMessageServiceAction* service, QWidget* parent)
:
QWidget(parent),
m_layoutStack(0),
m_composeWidget(0),
m_busyLabel(0),
m_service(service),
m_accountsCombo(0),
m_toEdit(0),
m_subjectEdit(0),
m_bodyEdit(0),
m_composeButton(0),
m_sendButton(0),
m_attachmentList(0),
m_addAttachmentButton(0),
m_loadedAccounts(false)
{
    setupUi();
}

void ComposeSendWidget::showEvent(QShowEvent* e)
{
    if(!m_loadedAccounts)
        updateAccounts();
    QWidget::showEvent(e);
}

void ComposeSendWidget::hideEvent(QHideEvent* e)
{
    if(m_accountIdsLoader.isRunning())
        m_accountIdsLoader.exit();

    QWidget::hideEvent(e);
}

void ComposeSendWidget::composeButtonClicked()
{
    m_service->compose(constructQMessage());
}

void ComposeSendWidget::sendButtonClicked()
{
    QMessage outgoing = constructQMessage();
    m_service->send(outgoing);
}

void ComposeSendWidget::addAttachmentButtonClicked()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this,tr("Select attachments"));
    m_attachmentList->addAttachments(filenames);
}

void ComposeSendWidget::updateAccounts()
{
    if(m_accountIdsLoader.isRunning())
        return;
    m_accountIdsLoader.start();
}

void ComposeSendWidget::updateAccountsStarted()
{
    m_accountsCombo->clear();
    m_layoutStack->setCurrentWidget(m_busyLabel);
    m_composeWidget->setEnabled(false);
    setCursor(Qt::BusyCursor);
}

void ComposeSendWidget::updateAccountsFinished()
{
    m_availableAccounts = m_accountIdsLoader.ids();

    for(int i = 0; i < m_availableAccounts.count(); ++i)
        m_accountsCombo->addItem(QString("%1 - %2").arg(i+1).arg(QMessageAccount(m_availableAccounts[i]).name()));

    m_composeWidget->setEnabled(true);
    m_layoutStack->setCurrentWidget(m_composeWidget);
    setCursor(Qt::ArrowCursor);
    m_loadedAccounts = true;
}

void ComposeSendWidget::updateAccountsAborted()
{
    m_availableAccounts.clear();
    m_loadedAccounts = false;
    m_accountsCombo->clear();

    m_layoutStack->setCurrentWidget(m_composeWidget);
    setCursor(Qt::ArrowCursor);
}

void ComposeSendWidget::setupUi()
{
    static bool runonce(false);
    if(runonce) return;

    m_layoutStack = new QStackedLayout(this);
    m_layoutStack->setStackingMode(QStackedLayout::StackAll);

    m_composeWidget = new QWidget(this);
    m_layoutStack->addWidget(m_composeWidget);

    QVBoxLayout* layout = new QVBoxLayout(m_composeWidget);

    QWidget* composeWidget = new QWidget(this);
    layout->addWidget(composeWidget);

    QGridLayout* gl = new QGridLayout(composeWidget);
    gl->setContentsMargins(0,0,0,0);

    QLabel* accountLabel = new QLabel("Account:",composeWidget);
    gl->addWidget(accountLabel,0,0);

    m_accountsCombo = new QComboBox(composeWidget);
    gl->addWidget(m_accountsCombo,0,1);

    QLabel* toLabel = new QLabel("To:",composeWidget);
    gl->addWidget(toLabel,1,0);

    m_toEdit = new QLineEdit(composeWidget);
    gl->addWidget(m_toEdit,1,1);

    QLabel* subjectLabel = new QLabel("Subject:",composeWidget);
    gl->addWidget(subjectLabel,2,0);

    m_subjectEdit = new QLineEdit(composeWidget);
    gl->addWidget(m_subjectEdit,2,1);

    m_bodyEdit = new QTextEdit(composeWidget);
    gl->addWidget(m_bodyEdit,3,0,1,2);

    m_attachmentList = new AttachmentListWidget(composeWidget);
    gl->addWidget(m_attachmentList,4,0,1,2);
    m_attachmentList->hide();

    m_addAttachmentButton = new QPushButton("Add attachment...",this);
    connect(m_addAttachmentButton,SIGNAL(clicked(bool)),this,SLOT(addAttachmentButtonClicked()));
    layout->addWidget(m_addAttachmentButton);

    m_sendButton = new QPushButton("Send",this);
    connect(m_sendButton,SIGNAL(clicked(bool)),this,SLOT(sendButtonClicked()));
    layout->addWidget(m_sendButton);

    m_composeButton = new QPushButton("Compose",this);
    connect(m_composeButton,SIGNAL(clicked(bool)),this,SLOT(composeButtonClicked()));
    layout->addWidget(m_composeButton);

    m_busyLabel = new QLabel("Loading accounts...",this);
    m_busyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_layoutStack->addWidget(m_busyLabel);

    connect(&m_accountIdsLoader,SIGNAL(started()),this,SLOT(updateAccountsStarted()));
    connect(&m_accountIdsLoader,SIGNAL(finished()),this,SLOT(updateAccountsFinished()));
    connect(&m_accountIdsLoader,SIGNAL(terminated()),this,SLOT(updateAccountsAborted()));

    runonce = true;
}

QMessage ComposeSendWidget::constructQMessage() const
{
    QMessage message;

    if(m_availableAccounts.isEmpty())
    {
        QMessageBox::critical(const_cast<ComposeSendWidget*>(this),"No Accounts","Cannot send a message without any available accounts");
        return message;
    }

    QMessageAccountId selectedAccountId = m_availableAccounts.at(m_accountsCombo->currentIndex());

    foreach(QString s, m_toEdit->text().split(QRegExp("\\s")))
    {
        QMessageAddressList toList;
        toList.append(QMessageAddress(s,QMessageAddress::Email));
        message.setTo(toList);
    }

    message.setParentAccountId(selectedAccountId);
    message.setSubject(m_subjectEdit->text());
    message.setBody(m_bodyEdit->toPlainText());
    message.appendAttachments(m_attachmentList->attachments());

    return message;
}

class RetrieveWidget : public QWidget
{
public:
    RetrieveWidget(QMessageServiceAction* service, QWidget* parent = 0);

private:
    QMessageServiceAction* m_service;
};

RetrieveWidget::RetrieveWidget(QMessageServiceAction* service, QWidget* parent)
:
QWidget(parent),
m_service(service)
{
}

class ShowWidget : public QWidget
{
    Q_OBJECT

    static const int MessageIdRole = Qt::UserRole + 1;

public:
    ShowWidget(QMessageServiceAction* service, QWidget* parent = 0);

protected:
    void showEvent(QShowEvent* e);
    void hideEvent(QHideEvent* e);

private slots:
    void showButtonClicked();
    void updateMessageList();
    void updateMessageListStarted();
    void updateMessageListFinished();

private:
    void setupUi();

private:
    QMessageServiceAction* m_service;
    QStackedLayout* m_layoutStack;
    QWidget* m_showWidget;
    QLabel* m_busyLabel;
    QListWidget* m_messageListWidget;
    QPushButton* m_showButton;
    MessageIdsLoader m_messageIdsLoader;
};

ShowWidget::ShowWidget(QMessageServiceAction* service, QWidget* parent)
:
QWidget(parent),
m_service(service),
m_layoutStack(0),
m_showWidget(0),
m_busyLabel(0),
m_messageListWidget(0),
m_showButton(0)
{
}

void ShowWidget::showEvent(QShowEvent* e)
{
    setupUi();
    QWidget::showEvent(e);
}

void ShowWidget::hideEvent(QHideEvent* e)
{
    if(m_messageIdsLoader.isRunning())
        m_messageIdsLoader.exit();
    QWidget::hideEvent(e);
}

void ShowWidget::showButtonClicked()
{
    //get the selected account

    QString idString = m_messageListWidget->currentItem()->data(MessageIdRole).toString();
    QMessageId selectedId(idString);
    m_service->show(selectedId);
}

void ShowWidget::updateMessageList()
{
    if(m_messageIdsLoader.isRunning())
        return;
    m_messageIdsLoader.start();
}

void ShowWidget::updateMessageListStarted()
{
    m_showWidget->setEnabled(false);
    m_layoutStack->setCurrentWidget(m_busyLabel);
    setCursor(Qt::BusyCursor);
}

void ShowWidget::updateMessageListFinished()
{
    m_messageListWidget->clear();

    MessageIdSubjectList lastTenMessages = m_messageIdsLoader.ids();

    for(int index = lastTenMessages.count()-1; index >= 0; index--)
    {
        IdSubjectPair result(lastTenMessages[index]);
        QListWidgetItem* newItem = new QListWidgetItem(result.second);
        newItem->setData(MessageIdRole,result.first.toString());
        m_messageListWidget->addItem(newItem);
    }

    m_showWidget->setEnabled(true);
    m_layoutStack->setCurrentWidget(m_showWidget);
    setCursor(Qt::ArrowCursor);
}

void ShowWidget::setupUi()
{
    static bool runonce(false);
    if(runonce) return;

    m_layoutStack = new QStackedLayout(this);
    m_layoutStack->setStackingMode(QStackedLayout::StackAll);

    m_showWidget = new QWidget(this);
    m_layoutStack->addWidget(m_showWidget);

    QVBoxLayout* vbl = new QVBoxLayout(m_showWidget);

    m_messageListWidget = new QListWidget(this);
    vbl->addWidget(m_messageListWidget);

    m_showButton = new QPushButton("Show",this);
    connect(m_showButton,SIGNAL(clicked(bool)),this,SLOT(showButtonClicked()));
    vbl->addWidget(m_showButton);

    m_busyLabel = new QLabel("Loading last ten messages...",this);
    m_busyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_layoutStack->addWidget(m_busyLabel);

    connect(&m_messageIdsLoader,SIGNAL(started()),this,SLOT(updateMessageListStarted()));
    connect(&m_messageIdsLoader,SIGNAL(finished()),this,SLOT(updateMessageListFinished()));

    QTimer::singleShot(1000,this,SLOT(updateMessageList()));

    runonce = true;
}

class QueryWidget : public QWidget
{
public:
    QueryWidget(QMessageServiceAction* service, QWidget* parent = 0);

private:
    QMessageServiceAction* m_service;
};

QueryWidget::QueryWidget(QMessageServiceAction* service, QWidget* parent)
:
QWidget(parent),
m_service(service)
{
}

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags f)
:
QMainWindow(parent,f),
m_tabWidget(0)
{
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    m_serviceAction = new QMessageServiceAction(this);

    connect(m_serviceAction,SIGNAL(stateChanged(QMessageServiceAction::State)),
            this,SLOT(serviceStateChanged(QMessageServiceAction::State)));

    m_tabWidget->addTab(new ComposeSendWidget(m_serviceAction,this),"Compose and Send");
    m_tabWidget->addTab(new ShowWidget(m_serviceAction,this),"Show");
    m_tabWidget->addTab(new RetrieveWidget(m_serviceAction,this),"Retrieve");
    m_tabWidget->addTab(new QueryWidget(m_serviceAction,this),"Query");

    m_tabWidget->setTabEnabled(2,false);
    m_tabWidget->setTabEnabled(3,false);

    setWindowTitle(WindowTitle);
    resize(WindowGeometry);
}

void MainWindow::serviceStateChanged(QMessageServiceAction::State state)
{
    if(state == QMessageServiceAction::Failed)
        QMessageBox::critical(this,"Error","One or more service actions failed");
}

#include <mainwindow.moc>
