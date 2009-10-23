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
#include <QScrollArea>
#include <QMenuBar>
#include <QApplication>
#include <QStackedWidget>
#include <QMutex>

typedef QPointer<QMessageServiceAction> QMessageServiceActionPtr;

static const QSize WindowGeometry(400,300);
static const QString WindowTitle("Service-actions Example");

typedef QPair<QMessageId,QString> IdSubjectPair;
typedef QList<IdSubjectPair> MessageIdSubjectList;

class AccountsWidget : public QWidget
{
    Q_OBJECT

private:
    class Loader : public QThread
    {
    public:
        Loader(AccountsWidget* parent);
        void run();
    private:
        AccountsWidget* m_parent;
    };

public:
    AccountsWidget(QWidget* parent = 0);
    QMessageAccountId currentAccount() const;
    bool isEmpty() const;

protected:
    void showEvent(QShowEvent* e);
    void hideEvent(QHideEvent* e);

private slots:
    void load();
    void loadStarted();
    void loadFinished();

private:
    void setupUi();
    void setIds(const QMessageAccountIdList& ids);
    QMessageAccountIdList ids() const;

private:
    QStackedLayout* m_stackedLayout;
    QComboBox* m_accountsCombo;
    QLabel* m_busyLabel;

    Loader m_loader;
    mutable QMutex m_loadMutex;
    QMessageAccountIdList m_ids;
};

AccountsWidget::Loader::Loader(AccountsWidget* parent)
:
QThread(parent),
m_parent(parent)
{
}

void AccountsWidget::Loader::run()
{
    QMessageAccountIdList ids = QMessageStore::instance()->queryAccounts();
    m_parent->setIds(ids);
}

AccountsWidget::AccountsWidget(QWidget* parent)
:
QWidget(parent),
m_stackedLayout(0),
m_accountsCombo(0),
m_busyLabel(0),
m_loader(this)
{
    setupUi();

    connect(&m_loader,SIGNAL(started()),this,SLOT(loadStarted()));
    connect(&m_loader,SIGNAL(finished()),this,SLOT(loadFinished()));
}

QMessageAccountId AccountsWidget::currentAccount() const
{
    QMessageAccountId result;
    if(m_loader.isFinished() && m_accountsCombo->count())
    {
        int index = m_accountsCombo->currentIndex();
        return ids().at(index);
    }

    return result;
}

bool AccountsWidget::isEmpty() const
{
	return m_accountsCombo->count() == 0;
}

void AccountsWidget::showEvent(QShowEvent* e)
{
    load();
    QWidget::showEvent(e);
}

void AccountsWidget::hideEvent(QHideEvent* e)
{
    if(m_loader.isRunning())
        m_loader.exit();
    QWidget::hideEvent(e);
}

void AccountsWidget::load()
{
    static bool runonce = false;
    if(!runonce)
        m_loader.start();
    runonce = true;
}

void AccountsWidget::loadStarted()
{
#ifndef _WIN32_WCE
    setCursor(Qt::BusyCursor);
#endif
    m_stackedLayout->setCurrentWidget(m_busyLabel);
}

void AccountsWidget::loadFinished()
{
    m_accountsCombo->clear();

    QMessageAccountIdList accountIds = ids();

    if(!accountIds.isEmpty())
    {
        for(int i = 0; i < accountIds.count(); ++i)
            m_accountsCombo->addItem(QString("%1 - %2").arg(i+1).arg(QMessageAccount(accountIds[i]).name()));

        m_stackedLayout->setCurrentWidget(m_accountsCombo);
    }
    else
        m_busyLabel->setText("No accounts!");

#ifndef _WIN32_WCE
    setCursor(Qt::ArrowCursor);
#endif
}

void AccountsWidget::setupUi()
{
    m_stackedLayout = new QStackedLayout(this);

    m_accountsCombo = new QComboBox(this);
    m_stackedLayout->addWidget(m_accountsCombo);

    m_busyLabel = new QLabel("Loading..");
    m_stackedLayout->addWidget(m_busyLabel);
}

void AccountsWidget::setIds(const QMessageAccountIdList& ids)
{
    QMutexLocker mutex(&m_loadMutex);

    m_ids = ids;
}

QMessageAccountIdList AccountsWidget::ids() const
{
    QMutexLocker mutex(&m_loadMutex);
    return m_ids;
}

class RecentMessagesWidget : public QWidget
{
    Q_OBJECT

private:
    static const int MessageIdRole = Qt::UserRole + 1;

    class Loader : public QThread
    {
    public:
        Loader(RecentMessagesWidget* parent);
        void run();

    private:
        RecentMessagesWidget* m_parent;
    };

public:
    RecentMessagesWidget(QWidget* parent = 0);

    IdSubjectPair currentItem() const;

protected:
    void showEvent(QShowEvent* e);
    void hideEvent(QHideEvent* e);

private slots:
    void load();
    void loadStarted();
    void loadFinished();

private:
    void setupUi();
    void setIds(const MessageIdSubjectList& list);
    MessageIdSubjectList ids() const;

private:
    QListWidget* m_messageListWidget;
    QLabel* m_busyLabel;
    QStackedLayout* m_layout;
    Loader m_loader;
    mutable QMutex m_loadMutex;
    MessageIdSubjectList m_ids;
};

RecentMessagesWidget::Loader::Loader(RecentMessagesWidget* parent)
:
QThread(parent),
m_parent(parent)
{
}

void RecentMessagesWidget::Loader::run()
{
    QMessageIdList lastTenMessages = QMessageStore::instance()->queryMessages(QMessageFilter(),QMessageOrdering::byReceptionTimeStamp(Qt::DescendingOrder),10,0);

    MessageIdSubjectList ids;

    foreach(const QMessageId& id, lastTenMessages)
    {
        QMessage message(id);
        IdSubjectPair result(id,message.subject());
        ids.append(result);
    }

    m_parent->setIds(ids);
}

RecentMessagesWidget::RecentMessagesWidget(QWidget* parent)
:
QWidget(parent),
m_messageListWidget(0),
m_busyLabel(0),
m_layout(0),
m_loader(this)
{
    setupUi();
    connect(&m_loader,SIGNAL(started()),this,SLOT(loadStarted()));
    connect(&m_loader,SIGNAL(finished()),this,SLOT(loadFinished()));
}

IdSubjectPair RecentMessagesWidget::currentItem() const
{
    IdSubjectPair result;

    if(m_loader.isFinished() && !m_ids.isEmpty())
        result = ids().at(m_messageListWidget->currentRow());
    return result;
}

void RecentMessagesWidget::showEvent(QShowEvent* e)
{
    load();
    QWidget::showEvent(e);
}

void RecentMessagesWidget::hideEvent(QHideEvent* e)
{
    if(m_loader.isRunning())
        m_loader.exit();
    QWidget::hideEvent(e);
}

void RecentMessagesWidget::load()
{
    static bool runonce = false;
    if(!runonce)
        m_loader.start();
    runonce = true;
}

void RecentMessagesWidget::loadStarted()
{
#ifndef _WIN32_WCE
    setCursor(Qt::BusyCursor);
#endif
    m_layout->setCurrentWidget(m_busyLabel);
}

void RecentMessagesWidget::loadFinished()
{
    m_messageListWidget->clear();

    MessageIdSubjectList lastTenMessages = ids();

    if(lastTenMessages.isEmpty())
    {
        m_busyLabel->setText("No messages found!");
        return;
    }

    for(int index =  0 ; index <= lastTenMessages.count()-1; index++)
    {
        IdSubjectPair result(lastTenMessages[index]);
        QListWidgetItem* newItem = new QListWidgetItem(result.second);
        newItem->setData(MessageIdRole,result.first.toString());
        m_messageListWidget->addItem(newItem);
    }

    m_layout->setCurrentWidget(m_messageListWidget);
#ifndef _WIN32_WCE
    setCursor(Qt::ArrowCursor);
#endif
}

void RecentMessagesWidget::setupUi()
{
    m_layout = new QStackedLayout(this);

    m_messageListWidget = new QListWidget(this);
    m_layout->addWidget(m_messageListWidget);

    m_busyLabel = new QLabel("Loading...",this);
    m_busyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_busyLabel->setFrameStyle(QFrame::Box);
    m_layout->addWidget(m_busyLabel);

}

void RecentMessagesWidget::setIds(const MessageIdSubjectList& list)
{
    QMutexLocker mutex(&m_loadMutex);
    m_ids = list;
}

MessageIdSubjectList RecentMessagesWidget::ids() const
{
    QMutexLocker mutex(&m_loadMutex);
    return m_ids;
}

class ComposeSendWidget : public QWidget
{
    Q_OBJECT

public:
    ComposeSendWidget(QMessageServiceAction* service, QWidget* parent = 0);

private slots:
    void composeButtonClicked();
    void sendButtonClicked();
    void addAttachmentButtonClicked();

private:
    void setupUi();
    QMessage constructQMessage() const;

private:
    QStackedLayout* m_layoutStack;
    QWidget* m_composeWidget;
    QLabel* m_busyLabel;
    QMessageServiceAction* m_service;
    AccountsWidget* m_accountsWidget;
    QLineEdit* m_toEdit;
    QLineEdit* m_subjectEdit;
    QTextEdit* m_bodyEdit;
    AttachmentListWidget* m_attachmentList;
};

ComposeSendWidget::ComposeSendWidget(QMessageServiceAction* service, QWidget* parent)
:
QWidget(parent),
m_layoutStack(0),
m_composeWidget(0),
m_busyLabel(0),
m_service(service),
m_accountsWidget(0),
m_toEdit(0),
m_subjectEdit(0),
m_bodyEdit(0),
m_attachmentList(0)
{
    setupUi();
}

void ComposeSendWidget::composeButtonClicked()
{
    QMessage message(constructQMessage());
    m_service->compose(message);
}

void ComposeSendWidget::sendButtonClicked()
{
    QMessage message(constructQMessage());
    m_service->send(message);
}

void ComposeSendWidget::addAttachmentButtonClicked()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this,tr("Select attachments"));
    m_attachmentList->addAttachments(filenames);
}

void ComposeSendWidget::setupUi()
{
    QGridLayout* gl = new QGridLayout(this);

    QLabel* accountLabel = new QLabel("Account:",this);
    gl->addWidget(accountLabel,0,0);

    m_accountsWidget = new AccountsWidget(this);
    gl->addWidget(m_accountsWidget,0,1);

    QLabel* toLabel = new QLabel("To:",this);
    gl->addWidget(toLabel,1,0);

    m_toEdit = new QLineEdit(this);
    gl->addWidget(m_toEdit,1,1);

    QLabel* subjectLabel = new QLabel("Subject:",this);
    gl->addWidget(subjectLabel,2,0);

    m_subjectEdit = new QLineEdit(this);
    gl->addWidget(m_subjectEdit,2,1);

    m_bodyEdit = new QTextEdit(this);
    gl->addWidget(m_bodyEdit,3,0,1,2);

    m_attachmentList = new AttachmentListWidget(this);
    gl->addWidget(m_attachmentList,4,0,1,2);
    m_attachmentList->hide();

    QAction* composeAction = new QAction("Compose",this);
    connect(composeAction,SIGNAL(triggered()),this,SLOT(composeButtonClicked()));
    addAction(composeAction);
    QAction* sendAction = new QAction("Send",this);
    connect(sendAction,SIGNAL(triggered()),this,SLOT(sendButtonClicked()));
    addAction(sendAction);
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    addAction(separator);
    QAction* attachmentsAction = new QAction("Add attachment",this);
    connect(attachmentsAction,SIGNAL(triggered()),this,SLOT(addAttachmentButtonClicked()));
    addAction(attachmentsAction);
}

QMessage ComposeSendWidget::constructQMessage() const
{
    QMessage message;

    if(m_accountsWidget->isEmpty())
    {
        QMessageBox::critical(const_cast<ComposeSendWidget*>(this),"No Accounts","Cannot send a message without any available accounts");
        return message;
    }

    QMessageAccountId selectedAccountId = m_accountsWidget->currentAccount();

    QMessageAddressList toList;

    foreach(QString s, m_toEdit->text().split(QRegExp("\\s")))
        toList.append(QMessageAddress(s,QMessageAddress::Email));

    message.setTo(toList);

    message.setParentAccountId(selectedAccountId);
    message.setSubject(m_subjectEdit->text());
    message.setBody(m_bodyEdit->toPlainText());
    message.setType(QMessage::Email);
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

public:
    ShowWidget(QMessageServiceAction* service, QWidget* parent = 0);

private slots:
    void showButtonClicked();

private:
    void setupUi();

private:
    QMessageServiceAction* m_service;
    QStackedLayout* m_layoutStack;
    QScrollArea* m_showWidget;
    QLabel* m_busyLabel;
    RecentMessagesWidget* m_recentMessagesWidget;
};

ShowWidget::ShowWidget(QMessageServiceAction* service, QWidget* parent)
:
QWidget(parent),
m_service(service),
m_showWidget(0),
m_recentMessagesWidget(0)
{
    setupUi();
}

void ShowWidget::showButtonClicked()
{
    //get the selected account

    if(m_recentMessagesWidget->currentItem().first.isValid())
    {
        IdSubjectPair result = m_recentMessagesWidget->currentItem();
        QMessageId selectedId(result.first);
        m_service->show(selectedId);
    }
}

void ShowWidget::setupUi()
{
    QVBoxLayout* vbl = new QVBoxLayout(this);

    vbl->addWidget(new QLabel("Last 10 messages:",this));

    m_recentMessagesWidget = new RecentMessagesWidget(this);
    vbl->addWidget(m_recentMessagesWidget);

    QAction* showAction = new QAction("Show",this);
    connect(showAction,SIGNAL(triggered()),this,SLOT(showButtonClicked()));
    addAction(showAction);
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

    m_serviceAction = new QMessageServiceAction(this);

    connect(m_serviceAction,SIGNAL(stateChanged(QMessageServiceAction::State)),
            this,SLOT(serviceStateChanged(QMessageServiceAction::State)));

    //example widgets

    m_widgetStack = new QStackedWidget(this);
    setCentralWidget(m_widgetStack);
    m_widgetStack->addWidget(new ComposeSendWidget(m_serviceAction,this));
    m_widgetStack->addWidget(new ShowWidget(m_serviceAction,this));
    m_widgetStack->addWidget(new RetrieveWidget(m_serviceAction,this));
    m_widgetStack->addWidget(new QueryWidget(m_serviceAction,this));

    //main menu
#ifndef _WIN32_WCE
    QMenu* fileMenu = new QMenu("File",this);
#endif

    int index = 0;
    foreach(QAction* viewAction, QList<QAction*>() << new QAction("Compose\\Send",this)
                                                   << new QAction("Show",this)
                                                   << new QAction("Retrieve",this)
                                                   << new QAction("Query",this))
    {
        connect(viewAction,SIGNAL(triggered()),this,SLOT(viewSelected()));
#ifndef _WIN32_WCE
        fileMenu->addAction(viewAction);
#else
        menuBar()->addAction(viewAction);
#endif
        viewAction->setData(index);
        index++;
    }
#ifndef _WIN32_WCE
    fileMenu->addSeparator();
#else
    menuBar()->addSeparator();
#endif

    QAction* quitAction = new QAction("Quit",this);
    connect(quitAction,SIGNAL(triggered()),qApp,SLOT(quit()));
#ifndef _WIN32_WCE
    fileMenu->addAction(quitAction);
    menuBar()->addMenu(fileMenu);
#else
    menuBar()->addAction(quitAction);
#endif

    QTimer::singleShot(0,this,SLOT(viewSelected()));

    //window properties

    setWindowTitle(WindowTitle);
    resize(WindowGeometry);
}

void MainWindow::serviceStateChanged(QMessageServiceAction::State state)
{
    if(state == QMessageServiceAction::Failed)
        QMessageBox::critical(this,"Error","One or more service actions failed");
}

void MainWindow::viewSelected()
{
    static QMenu* actionMenu = 0;

    if(!actionMenu)
    {
        actionMenu = new QMenu("Action",this);
#ifndef _WIN32_WCE
        menuBar()->addMenu(actionMenu);
#endif
    }

    QAction* senderAction = static_cast<QAction*>(sender());
    if(senderAction)
        m_widgetStack->setCurrentIndex(senderAction->data().toInt());

    bool currentViewHasActions = m_widgetStack->currentWidget() && !m_widgetStack->currentWidget()->actions().isEmpty();
    actionMenu->clear();
    if(currentViewHasActions)
    {
        foreach(QAction* a, m_widgetStack->currentWidget()->actions())
            actionMenu->addAction(a);
    }
#ifdef _WIN32_WCE
    static QAction* leftSoftButton = new QAction("Action",this);
    leftSoftButton->setMenu(actionMenu);
    menuBar()->setDefaultAction(leftSoftButton);
#endif
}

#include <mainwindow.moc>

