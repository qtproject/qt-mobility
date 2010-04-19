/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "attachmentlistwidget.h"
#include "qmessageservice.h"
#include <qmessagemanager.h>
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
#include <QTextBrowser>
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
#include <QKeyEvent>

static const QSize WindowGeometry(400,300);
static const QString WindowTitle("Service-actions Example");
static unsigned int RecentMessagesCount = 50;

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
    QString currentAccountName() const;
    bool isEmpty() const;

signals:
    void accountChanged();

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
    QMessageManager manager;
    m_parent->setIds(manager.queryAccounts());
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

QString AccountsWidget::currentAccountName() const
{
    if(m_loader.isFinished() && m_accountsCombo->count())
        return m_accountsCombo->itemData(m_accountsCombo->currentIndex()).toString();
    return QString();
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
    //#define NOTHREAD
#ifdef NOTHREAD
    QMessageManager manager;
    if(!runonce)
     setIds(manager.queryAccounts());
    //        m_loader.start();

#else 
    if(!runonce) 
         m_loader.start();
#endif
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
        {
            QMessageAccount account(accountIds[i]);
            m_accountsCombo->addItem(QString("%1 - %2").arg(i+1).arg(account.name()),account.name());
        }

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
    connect(m_accountsCombo,SIGNAL(currentIndexChanged(int)),this,SIGNAL(accountChanged()));

    m_busyLabel = new QLabel("Loading...");
    m_stackedLayout->addWidget(m_busyLabel);

    setSizePolicy(m_accountsCombo->sizePolicy());

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

public:
    RecentMessagesWidget(QWidget* parent = 0, unsigned int maxRecent = 10);
    ~RecentMessagesWidget();
    QMessageId currentMessage() const;

signals:
    void selected(const QMessageId& messageId);

protected:
    void showEvent(QShowEvent* e);
    void hideEvent(QHideEvent* e);

private slots:
    void currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void messagesFound(const QMessageIdList& result);
    void stateChanged(QMessageService::State s);
    void messageUpdated(const QMessageId& id, const QMessageManager::NotificationFilterIdSet& filter);
    void messageRemoved(const QMessageId& id, const QMessageManager::NotificationFilterIdSet& filter);
    void processResults();

private:
    void setupUi();
    void updateState();
    void load();

private:
    enum State { Unloaded, Loading, LoadFinished, Processing, LoadFailed, Done };
    static const int MessageIdRole = Qt::UserRole + 1;

private:
    QListWidget* m_messageListWidget;
    QLabel* m_statusLabel;
    QStackedLayout* m_layout;
    QMessageIdList m_ids;
    QMap<QMessageId,QListWidgetItem*> m_indexMap;
    unsigned int m_maxRecent;
    QMessageService* m_service;
    State m_state;
    QMessageManager::NotificationFilterId m_storeFilterId;
    QMessageManager m_manager;
};

RecentMessagesWidget::RecentMessagesWidget(QWidget* parent, unsigned int maxRecent)
:
QWidget(parent),
m_messageListWidget(0),
m_statusLabel(0),
m_layout(0),
m_maxRecent(maxRecent),
m_service(new QMessageService(this)),
m_state(Unloaded)
{
    setupUi();
    connect(m_service,SIGNAL(messagesFound(const QMessageIdList&)),this,SLOT(messagesFound(const QMessageIdList&)));
    connect(m_service,SIGNAL(stateChanged(QMessageService::State)),this,SLOT(stateChanged(QMessageService::State)));

    //register for message update notifications

    connect(&m_manager, SIGNAL(messageUpdated(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)),
        this, SLOT(messageUpdated(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)));
    connect(&m_manager, SIGNAL(messageRemoved(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)),
        this, SLOT(messageRemoved(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)));

    m_storeFilterId = m_manager.registerNotificationFilter(QMessageFilter());
}

RecentMessagesWidget::~RecentMessagesWidget()
{
     m_manager.unregisterNotificationFilter(m_storeFilterId);
}

QMessageId RecentMessagesWidget::currentMessage() const
{
    QMessageId result;

    if(QListWidgetItem* currentItem = m_messageListWidget->currentItem())
        result = QMessageId(currentItem->data(MessageIdRole).toString());

    return result;
}

void RecentMessagesWidget::showEvent(QShowEvent* e)
{
    if(m_state == Unloaded)
        load();

    updateState();

    QWidget::showEvent(e);
}

void RecentMessagesWidget::hideEvent(QHideEvent* e)
{
    if(m_state == Loading || m_state == Processing)
    {
        m_service->cancel();
        m_state = Unloaded;
        m_ids.clear();
    }

    QWidget::hideEvent(e);
}

void RecentMessagesWidget::currentItemChanged(QListWidgetItem*, QListWidgetItem*)
{
    if(m_state != Processing || m_state != Loading)
        emit selected(currentMessage());
}

//! [process-results]
void RecentMessagesWidget::messagesFound(const QMessageIdList& ids)
{
    m_ids.append(ids);
}
//! [process-results]

void RecentMessagesWidget::stateChanged(QMessageService::State newState)
{
    //  qDebug() << "stateChanged state=" << m_state << " newState=" << newState << "error=" << m_service->error();
    if (newState == QMessageService::FinishedState) {
        if ((m_state != LoadFailed) && (m_service->error() == QMessageManager::NoError)) {
            m_state = LoadFinished;
        } else {
            m_state = LoadFailed;
        }
    }

    updateState();
}


void RecentMessagesWidget::messageUpdated(const QMessageId& id, const QMessageManager::NotificationFilterIdSet& filter)
{
    if(!filter.contains(m_storeFilterId) || m_state == Loading || !id.isValid() || !m_indexMap.contains(id))
        return;

    //update the pertinent entry to reflect completeness

    QListWidgetItem* item = m_indexMap.value(id);
    if(item)
    {

        QMessage message(id);
        bool partialMessage = !message.find(message.bodyId()).isContentAvailable();
        QFont itemFont = item->font();
        itemFont.setItalic(partialMessage);
        item->setFont(itemFont);
    }
}

void RecentMessagesWidget::messageRemoved(const QMessageId& id, const QMessageManager::NotificationFilterIdSet& filter)
{
    if(!filter.contains(m_storeFilterId) || m_state == Loading || !id.isValid() || !m_indexMap.contains(id))
        return;

    QListWidgetItem* item = m_indexMap.value(id);
    if(item)
    {
        int row = m_messageListWidget->row(item);
        QListWidgetItem* item = m_messageListWidget->takeItem(row);
        m_indexMap.remove(id);
        delete item;
    }
    m_ids.removeAll(id);
}

void RecentMessagesWidget::setupUi()
{
    m_layout = new QStackedLayout(this);

    m_messageListWidget = new QListWidget(this);
    m_layout->addWidget(m_messageListWidget);
    connect(m_messageListWidget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
        this,SLOT(currentItemChanged(QListWidgetItem*,QListWidgetItem*)));

    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_statusLabel->setFrameStyle(QFrame::Box);
    m_layout->addWidget(m_statusLabel);

}

void RecentMessagesWidget::updateState()
{
    switch(m_state)
    {
        case Unloaded:
        {
            m_statusLabel->setText(QString());
            m_layout->setCurrentWidget(m_statusLabel);
        }
        break;
        case Loading:
        {
            m_statusLabel->setText("Loading...");
            m_layout->setCurrentWidget(m_statusLabel);
        }
        break;
        case LoadFinished:
        {
            if(m_ids.isEmpty())
            {
                m_statusLabel->setText("Finished. No messages.");
                m_layout->setCurrentWidget(m_statusLabel);
            }
            else
            {
                m_state = Processing;
                updateState();
                processResults();
            }
        }
        break;
        case Processing:
            m_layout->setCurrentWidget(m_messageListWidget);
        break;
        case LoadFailed:
        {
            m_statusLabel->setText("Load failed!");
            m_layout->setCurrentWidget(m_statusLabel);
        }
        break;
    }

#ifndef _WIN32_WCE
    if(m_state == Loading || m_state == Processing)
        setCursor(Qt::BusyCursor);
    else
        setCursor(Qt::ArrowCursor);
#endif

}

//! [load-message]
void RecentMessagesWidget::load()
{
    m_ids.clear();
    m_state = Loading;
    bool b;

    b=m_service->queryMessages(QMessageFilter(),QMessageSortOrder::byReceptionTimeStamp(Qt::DescendingOrder),m_maxRecent);
    //    qDebug() << "RecentMessagesWidget::load" << b << m_state;
//! [load-message]
};

//! [process-results2]
void RecentMessagesWidget::processResults()
{
    if(!m_ids.isEmpty())
    {
        QMessageId id = m_ids.takeFirst();
        QMessage message(id);

        QListWidgetItem* newItem = new QListWidgetItem(message.subject());
        newItem->setData(MessageIdRole,id.toString());
        QFont itemFont = newItem->font();
        bool isPartialMessage = !message.find(message.bodyId()).isContentAvailable();
        itemFont.setItalic(isPartialMessage);
        newItem->setFont(itemFont);
        m_messageListWidget->addItem(newItem);
        m_indexMap.insert(id,newItem);
        m_messageListWidget->update();
        QTimer::singleShot(100,this,SLOT(processResults()));
    }
    else
    {
        m_state = Done;
        updateState();
    }
}
//! [process-results2]

class ComposeSendWidget : public QWidget
{
    Q_OBJECT

public:
    ComposeSendWidget(QMessageService* service, QWidget* parent = 0);

signals:
    void actionsChanged();

private slots:
    void composeButtonClicked();
    void sendButtonClicked();
    void addAttachmentButtonClicked();
    void accountChanged();

private:
    void setupUi();
    QMessage constructQMessage(bool asHtml = false) const;

private:
    QStackedLayout* m_layoutStack;
    QMessageService* m_service;
    AccountsWidget* m_accountsWidget;
    QLineEdit* m_toEdit;
    QLineEdit* m_ccEdit;
    QLabel* m_ccLabel;
    QLineEdit* m_bccEdit;
    QLabel* m_bccLabel;
    QLineEdit* m_subjectEdit;
    QLabel* m_subjectLabel;
    QTextEdit* m_bodyEdit;
    AttachmentListWidget* m_attachmentList;
    QAction* m_attachmentsAction;
    QAction* m_sendAsHTMLAction;
};

ComposeSendWidget::ComposeSendWidget(QMessageService* service, QWidget* parent)
:
QWidget(parent),
m_layoutStack(0),
m_service(service),
m_accountsWidget(0),
m_toEdit(0),
m_ccEdit(0),
m_ccLabel(0),
m_bccEdit(0),
m_bccLabel(0),
m_subjectEdit(0),
m_subjectLabel(0),
m_bodyEdit(0),
m_attachmentList(0),
m_attachmentsAction(0),
m_sendAsHTMLAction(0)
{
    setupUi();
}

static void notifyResult(bool result, const QString& description)
{
#ifndef _WIN32_WCE
    if(result) QMessageBox::information(0,description,"Succeeded!");
    else QMessageBox::critical(0,description,"Failed.");
#else
    Q_UNUSED(result);
    Q_UNUSED(description);
#endif
}

//! [send-compose-message]
void ComposeSendWidget::composeButtonClicked()
{
    QMessage message(constructQMessage());
    m_service->compose(message);
}

void ComposeSendWidget::sendButtonClicked()
{
    bool asHtml = (sender() == m_sendAsHTMLAction);
    QMessage message(constructQMessage(asHtml));
    notifyResult(m_service->send(message),"Send message");
}
//! [send-compose-message]

void ComposeSendWidget::addAttachmentButtonClicked()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this,tr("Select attachments"));
    m_attachmentList->addAttachments(filenames);
}

void ComposeSendWidget::accountChanged()
{
    QMessageAccount currentAccount(m_accountsWidget->currentAccount());

    bool isSmsAccount = (currentAccount.messageTypes() & QMessage::Sms) > 0;

    foreach(QWidget* emailSpecificWidget , QList<QWidget*>() << m_bccEdit << m_bccLabel <<
                                                                m_ccEdit <<  m_ccLabel <<
                                                                m_subjectEdit << m_subjectLabel) {
        emailSpecificWidget->setVisible(!isSmsAccount);
    }

    m_attachmentsAction->setEnabled(!isSmsAccount);
    m_sendAsHTMLAction->setEnabled(!isSmsAccount);
}

void ComposeSendWidget::setupUi()
{
    QGridLayout* gl = new QGridLayout(this);

    QLabel* accountLabel = new QLabel("Account:",this);
    gl->addWidget(accountLabel,0,0);

    m_accountsWidget = new AccountsWidget(this);
    gl->addWidget(m_accountsWidget,0,1);

    connect(m_accountsWidget,SIGNAL(accountChanged()),this,SLOT(accountChanged()));

    QLabel* toLabel = new QLabel("To:",this);
    gl->addWidget(toLabel,1,0);

    m_toEdit = new QLineEdit(this);
    gl->addWidget(m_toEdit,1,1);

    m_ccLabel = new QLabel("Cc:",this);
    gl->addWidget(m_ccLabel,2,0);

    m_ccEdit = new QLineEdit(this);
    gl->addWidget(m_ccEdit,2,1);

    m_bccLabel = new QLabel("Bcc",this);
    gl->addWidget(m_bccLabel,3,0);

    m_bccEdit = new QLineEdit(this);
    gl->addWidget(m_bccEdit,3,1);

    m_subjectLabel = new QLabel("Subject:",this);
    gl->addWidget(m_subjectLabel,4,0);

    m_subjectEdit = new QLineEdit(this);
    gl->addWidget(m_subjectEdit,4,1);

    m_bodyEdit = new QTextEdit(this);
    gl->addWidget(m_bodyEdit,5,0,1,2);

    m_attachmentList = new AttachmentListWidget(this);
    gl->addWidget(m_attachmentList,6,0,1,2);
    m_attachmentList->hide();

    QAction* composeAction = new QAction("Compose",this);
    connect(composeAction,SIGNAL(triggered()),this,SLOT(composeButtonClicked()));
    addAction(composeAction);

    QAction* sendAction = new QAction("Send",this);
    connect(sendAction,SIGNAL(triggered()),this,SLOT(sendButtonClicked()));
    addAction(sendAction);

    m_sendAsHTMLAction = new QAction("Send as HTML",this);
    connect(m_sendAsHTMLAction,SIGNAL(triggered()),this,SLOT(sendButtonClicked()));
    addAction(m_sendAsHTMLAction);

    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    addAction(separator);

    m_attachmentsAction = new QAction("Add attachment",this);
    connect(m_attachmentsAction,SIGNAL(triggered()),this,SLOT(addAttachmentButtonClicked()));
    addAction(m_attachmentsAction);
}


//! [construct-message]
QMessage ComposeSendWidget::constructQMessage(bool asHtml) const
{
    QMessage message;

    if(m_accountsWidget->isEmpty())
    {
        QMessageBox::critical(const_cast<ComposeSendWidget*>(this),"No Accounts","Cannot send a message without any available accounts");
        return message;
    }

    QMessageAccountId selectedAccountId = m_accountsWidget->currentAccount();
    QMessageAccount selectedAccount(selectedAccountId);

    bool composingSms = (selectedAccount.messageTypes() & QMessage::Sms) > 0;

    QMessageAddressList toList;
    QMessageAddressList ccList;
    QMessageAddressList bccList;

    QMessageAddress::Type addressType = QMessageAddress::Email;
    if(composingSms)
    {
        addressType = QMessageAddress::Phone;
        message.setType(QMessage::Sms);
    }

    foreach(QString s, m_toEdit->text().split(QRegExp("\\s"),QString::SkipEmptyParts))
        toList.append(QMessageAddress(addressType, s));
    message.setTo(toList);

    if(!composingSms)
    {
        foreach(QString s, m_ccEdit->text().split(QRegExp("\\s"),QString::SkipEmptyParts))
            ccList.append(QMessageAddress(QMessageAddress::Email, s));
        message.setCc(ccList);

        foreach(QString s, m_bccEdit->text().split(QRegExp("\\s"),QString::SkipEmptyParts))
            bccList.append(QMessageAddress(QMessageAddress::Email, s));
        message.setBcc(bccList);
        message.setSubject(m_subjectEdit->text());

        message.setType(QMessage::Email);

        message.appendAttachments(m_attachmentList->attachments());
    }

    message.setParentAccountId(selectedAccountId);

    if(!composingSms && asHtml) {
        //create html body
        QString htmlBody("<html><head><title></title></head><body><h2 align=center>%1</h2><hr>%2</body></html>");
        message.setBody(htmlBody.arg(message.subject()).arg(m_bodyEdit->toPlainText()),"text/html");
    }
    else
        message.setBody(m_bodyEdit->toPlainText());

    return message;
}
//! [construct-message]

class MessageViewWidget : public QWidget
{
    Q_OBJECT

    static const unsigned int LoadTimeLimit = 20; //seconds

    static QString downloadLinkURL()
    {
        static const QString url("MessageViewWidget://download");
        return url;
    };

public:
    MessageViewWidget(QWidget* parent = 0);
    ~MessageViewWidget();

    QMessageId viewing() const;

public slots:
    void view(const QMessageId& messageId);
    bool retrieveBody();

protected:
    void showEvent(QShowEvent* e);
    void hideEvent(QHideEvent* e);

private slots:
    void stateChanged(QMessageService::State s);
    void loadTimeout();
    void linkClicked(const QUrl&);
    void messageUpdated(const QMessageId&, const QMessageManager::NotificationFilterIdSet& filterSet);
    void messageRemoved(const QMessageId&, const QMessageManager::NotificationFilterIdSet& filterSet);


private:
    enum State { Unloaded , Loaded, Loading, LoadFailed };
    void setupUi();
    void updateState();
    void loadMessage();
    void resetService();

private:
    QStackedLayout* m_layoutStack;
    QLabel* m_statusLabel;
    QMessageService* m_service;
    QLineEdit* m_fromLabel;
    QLineEdit* m_subjectLabel;
    QTextBrowser* m_messageBrowser;
    QMessageId m_messageId;
    State m_state;
    QTimer m_loadTimer;
    QMessageManager::NotificationFilterId m_storeFilterId;
    QMessageManager m_manager;
};

MessageViewWidget::MessageViewWidget(QWidget* parent)
:
QWidget(parent),
m_layoutStack(0),
m_statusLabel(0),
m_service(new QMessageService(this)),
m_messageBrowser(0),
m_state(Unloaded)
{
    setupUi();
    resetService();
    connect(&m_loadTimer,SIGNAL(timeout()),this,SLOT(loadTimeout()));
    connect(&m_manager, SIGNAL(messageUpdated(const QMessageId&,const QMessageManager::NotificationFilterIdSet&)),
        this,SLOT(messageUpdated(const QMessageId&,const QMessageManager::NotificationFilterIdSet&)));
    connect(&m_manager, SIGNAL(messageRemoved(const QMessageId&,const QMessageManager::NotificationFilterIdSet&)),
        this,SLOT(messageRemoved(const QMessageId&,const QMessageManager::NotificationFilterIdSet&)));
    m_storeFilterId = m_manager.registerNotificationFilter(QMessageFilter());
}

MessageViewWidget::~MessageViewWidget()
{
    m_manager.unregisterNotificationFilter(m_storeFilterId);
}

void MessageViewWidget::view(const QMessageId& messageId)
{
    m_messageId = messageId;
    m_state = m_messageId.isValid() ? Loaded : Unloaded;

    updateState();
}

//! [retrieve-message-body]
bool MessageViewWidget::retrieveBody()
{
    if(m_state != Loading && !m_loadTimer.isActive())
    {
        m_loadTimer.setSingleShot(true);
        m_loadTimer.start(LoadTimeLimit * 1000);
        m_state = Unloaded;

        return m_service->retrieveBody(m_messageId);
    }

    return false;
}

//! [retrieve-message-body]

void MessageViewWidget::showEvent(QShowEvent* e)
{
    updateState();
    QWidget::showEvent(e);
}

void MessageViewWidget::hideEvent(QHideEvent* e)
{
    if(m_state == Loading)
    {
        m_service->cancel();
        m_state = Unloaded;
    }

    QWidget::hideEvent(e);
}

void MessageViewWidget::stateChanged(QMessageService::State newState)
{
    //  qDebug() << "stateChanged state=" << m_state << " newState=" << newState << "error=" << m_service->error();
    if (m_state == LoadFailed)
        return;

    if (newState == QMessageService::ActiveState) {
        m_state = Loading;
    } else if (newState == QMessageService::FinishedState) {
        m_state = (m_service->error() == QMessageManager::NoError ? Loaded : LoadFailed);
    }

    updateState();
}

void MessageViewWidget::loadTimeout()
{
    qWarning() << "Load timeout";
    m_service->cancel();
    m_state = LoadFailed;
    updateState();
}

void MessageViewWidget::linkClicked(const QUrl& url)
{
    bool downloadLinkClicked = url.toString() == downloadLinkURL();

    if(downloadLinkClicked)
        retrieveBody();
}

void MessageViewWidget::messageUpdated(const QMessageId& id, const QMessageManager::NotificationFilterIdSet& filterSet)
{
    if(!filterSet.contains(m_storeFilterId) || m_state == Loading || !id.isValid() || id != m_messageId)
        return;

    view(id);
}

void MessageViewWidget::messageRemoved(const QMessageId& id, const QMessageManager::NotificationFilterIdSet& filterSet)
{
    if(id == m_messageId)
    {
        m_state = Unloaded;
        m_loadTimer.stop();
        m_messageId = QMessageId();
        view(QMessageId());
    }
}

QMessageId MessageViewWidget::viewing() const
{
    return m_messageId;
}

void MessageViewWidget::setupUi()
{
    m_layoutStack = new QStackedLayout(this);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_layoutStack->addWidget(m_statusLabel);

    m_messageBrowser = new QTextBrowser(this);
    m_messageBrowser->setOpenLinks(false);
    connect(m_messageBrowser,SIGNAL(anchorClicked(const QUrl&)),this,SLOT(linkClicked(const QUrl&)));
    m_layoutStack->addWidget(m_messageBrowser);
}

void MessageViewWidget::updateState()
{
    switch(m_state)
    {
        case Unloaded:
        {
            m_messageBrowser->clear();
            m_layoutStack->setCurrentWidget(m_messageBrowser);
        } break;
        case Loading:
        {
            m_statusLabel->setText("Downloading...");
            m_layoutStack->setCurrentWidget(m_statusLabel);
        } break;
        case Loaded:
        {
            if(m_loadTimer.isActive())
            {
                m_loadTimer.stop();
                if(m_service->state() == QMessageService::ActiveState)
                    m_service->cancel();
            }

            loadMessage();
            m_layoutStack->setCurrentWidget(m_messageBrowser);
        } break;
        case LoadFailed:
        {
            m_statusLabel->setText("Download failed!");
            m_layoutStack->setCurrentWidget(m_statusLabel);
        } break;
    }
}

//! [partial-message-check]
void MessageViewWidget::loadMessage()
{
    m_messageBrowser->clear();

    static const QString htmlTemplate("\
    <html>\
    <head>\
    </head>\
    <body>\
    <table border=\"0\" cellspacing=\"0\">\
        <tr><td><b>From: </b></td><td>%1</td></tr>\
        <tr><td><b>Subject: </b></td><td>%2</td></tr>\
        <tr><td><b>Date: </b></td><td>%3</td></tr>\
    </table>\
    <hr>%4\
    <\body>\
    </html>\
    ");

    if(m_messageId.isValid())
    {
        QMessage message(m_messageId);

        QMessageContentContainer bodyPart = message.find(message.bodyId());

        QString bodyText;

        //for partial message display a download link instead

        bool bodyAvailable = bodyPart.isContentAvailable();

        if(bodyAvailable)
        {
            if(bodyPart.contentType() == "text")
                bodyText = bodyPart.textContent();
            else bodyText = "<Non-text content>";
        }
        else
            bodyText = QString("<p align=\"center\"><a href=\"%1\">Download</a></p>").arg(downloadLinkURL());
        m_messageBrowser->setHtml(htmlTemplate\
                                 .arg(message.from().addressee())\
                                 .arg(message.subject())\
                                 .arg(message.receivedDate().toString())\
                                 .arg(bodyText));
    }
}
//! [partial-message-check]

void MessageViewWidget::resetService()
{
    if(m_service)
        m_service->deleteLater();
    m_service = new QMessageService(this);
    connect(m_service,SIGNAL(stateChanged(QMessageService::State)),this,SLOT(stateChanged(QMessageService::State)));
}

class RetrieveWidget : public QWidget
{
    Q_OBJECT

public:
    RetrieveWidget(QWidget* parent = 0);

private slots:
    void messageSelected(const QMessageId& messageId);

private:
    void setupUi();

private:
    QMessageService* m_service;
    RecentMessagesWidget* m_recentMessagesWidget;
    MessageViewWidget* m_messageViewWidget;
    QAction* m_retrieveAction;
};

RetrieveWidget::RetrieveWidget(QWidget* parent)
:
QWidget(parent),
m_recentMessagesWidget(0),
m_messageViewWidget(0),
m_retrieveAction(0)
{
    setupUi();
}

void RetrieveWidget::messageSelected(const QMessageId& messageId)
{
    QMessage message(messageId);
    bool partialMessage = !message.find(message.bodyId()).isContentAvailable();

    m_retrieveAction->setEnabled(partialMessage && messageId.isValid());
}

void RetrieveWidget::setupUi()
{
    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(new QLabel(QString("Last %1 messages:").arg(RecentMessagesCount),this));

    m_recentMessagesWidget = new RecentMessagesWidget(this,RecentMessagesCount);
    l->addWidget(m_recentMessagesWidget);

    m_messageViewWidget = new MessageViewWidget(this);
    l->addWidget(m_messageViewWidget);

    m_retrieveAction = new QAction("Retrieve",this);
    connect(m_retrieveAction,SIGNAL(triggered(bool)),m_messageViewWidget,SLOT(retrieveBody()));
    addAction(m_retrieveAction);

    connect(m_recentMessagesWidget,SIGNAL(selected(const QMessageId&)),m_messageViewWidget,SLOT(view(const QMessageId&)));
    connect(m_recentMessagesWidget,SIGNAL(selected(const QMessageId&)),this,SLOT(messageSelected(const QMessageId&)));
}

class ShowWidget : public QWidget
{
    Q_OBJECT

public:
    ShowWidget(QMessageService* service, QWidget* parent = 0);

private slots:
    void showButtonClicked();

private:
    void setupUi();

private:
    QMessageService* m_service;
    RecentMessagesWidget* m_recentMessagesWidget;
};

ShowWidget::ShowWidget(QMessageService* service, QWidget* parent)
:
QWidget(parent),
m_service(service),
m_recentMessagesWidget(0)
{
    setupUi();
}

//! [show-message]
void ShowWidget::showButtonClicked()
{

    QMessageId id = m_recentMessagesWidget->currentMessage();

    if(id.isValid())
        m_service->show(id);
}
//! [show-message]

void ShowWidget::setupUi()
{
    QVBoxLayout* vbl = new QVBoxLayout(this);

    QString labelText("Last %1 messages:");
    vbl->addWidget(new QLabel(labelText.arg(RecentMessagesCount),this));

    m_recentMessagesWidget = new RecentMessagesWidget(this,RecentMessagesCount);
    vbl->addWidget(m_recentMessagesWidget);

    QAction* showAction = new QAction("Show",this);
    connect(showAction,SIGNAL(triggered()),this,SLOT(showButtonClicked()));
    addAction(showAction);
}

class StoreSignalsWidget : public QWidget
{
    Q_OBJECT

public:
    StoreSignalsWidget(QWidget* parent = 0);

private slots:
    void messageAdded(const QMessageId&, const QMessageManager::NotificationFilterIdSet&);
    void messageUpdated(const QMessageId&, const QMessageManager::NotificationFilterIdSet&);
    void messageRemoved(const QMessageId&, const QMessageManager::NotificationFilterIdSet&);

private:
    void setupUi();
    void appendString(const QString& message);

private:
    QListWidget* m_activityListWidget;
    QMessageManager::NotificationFilterId m_notificationFilterId;
    QMessageManager m_manager;
};

StoreSignalsWidget::StoreSignalsWidget(QWidget* parent)
:
QWidget(parent),
m_activityListWidget(0)
{
    setupUi();
}

//! [store-signals]
void StoreSignalsWidget::messageAdded(const QMessageId& id, const QMessageManager::NotificationFilterIdSet& filterSet)
{
    if(!filterSet.contains(m_notificationFilterId))
        return;

    QMessage message(id);

    QString msg = QString("Added: %1").arg(message.subject());
    m_activityListWidget->addItem(msg);
}

void StoreSignalsWidget::messageUpdated(const QMessageId& id, const QMessageManager::NotificationFilterIdSet& filterSet)
{
    if(!filterSet.contains(m_notificationFilterId))
        return;

    QMessage message(id);

    QString msg = QString("Updated: %1").arg(message.subject());
    m_activityListWidget->addItem(msg);
}

void StoreSignalsWidget::messageRemoved(const QMessageId& id, const QMessageManager::NotificationFilterIdSet& filterSet)
{
    if(!filterSet.contains(m_notificationFilterId))
        return;

    QString idString(id.toString());
    idString.truncate(10);

    QString msg = QString("Removed ID: %1 ...").arg(idString);
    m_activityListWidget->addItem(msg);
}
//! [store-signals]

void StoreSignalsWidget::setupUi()
{
    m_activityListWidget = new QListWidget(this);
    QVBoxLayout* l = new QVBoxLayout(this);
    l->setSpacing(0);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(m_activityListWidget);

    connect(&m_manager, 
            SIGNAL(messageAdded(const QMessageId&,const QMessageManager::NotificationFilterIdSet&)),
            this,
            SLOT(messageAdded(const QMessageId&,const QMessageManager::NotificationFilterIdSet&)));

    connect(&m_manager, 
            SIGNAL(messageRemoved(const QMessageId&,const QMessageManager::NotificationFilterIdSet&)),
            this,
            SLOT(messageRemoved(const QMessageId&,const QMessageManager::NotificationFilterIdSet&)));

    connect(&m_manager, 
            SIGNAL(messageUpdated(const QMessageId&,const QMessageManager::NotificationFilterIdSet&)),
            this,
            SLOT(messageUpdated(const QMessageId&,const QMessageManager::NotificationFilterIdSet&)));

    m_notificationFilterId = m_manager.registerNotificationFilter(QMessageFilter());

    QAction* clearAction = new QAction("Clear",this);
    connect(clearAction,SIGNAL(triggered(bool)),m_activityListWidget,SLOT(clear()));
    addAction(clearAction);
}

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags f)
:
QMainWindow(parent,f),
m_tabWidget(0)
{

    m_service = new QMessageService(this);

    connect(m_service,SIGNAL(stateChanged(QMessageService::State)),
            this,SLOT(serviceStateChanged(QMessageService::State)));

    //example widgets

    m_widgetStack = new QStackedWidget(this);
    setCentralWidget(m_widgetStack);

    foreach(QWidget* exampleWidget, QWidgetList() << new ComposeSendWidget(m_service,this)
                                                  << new ShowWidget(m_service,this)
                                                  << new RetrieveWidget(this)
                                                  << new StoreSignalsWidget(this)) {

        m_widgetStack->addWidget(exampleWidget);
#ifdef _WIN32_WCE
        exampleWidget->installEventFilter(this);
#endif
    }

    //main menu
#ifndef _WIN32_WCE
    QMenu* fileMenu = new QMenu("File",this);
#endif

    int index = 0;
    foreach(QAction* viewAction, QList<QAction*>() << new QAction("Compose\\Send",this)
                                                   << new QAction("Show",this)
                                                   << new QAction("Retrieve/Query",this)
                                                   << new QAction("Store Signals",this))
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

#ifdef _WIN32_WCE
bool MainWindow::eventFilter(QObject* source, QEvent* e)
{
    bool actionChanged = (m_widgetStack->currentWidget() == source) && e->type() == QEvent::ActionChanged;
    if(actionChanged)
        viewSelected(); //update the menu items
    return false;
}
#endif

void MainWindow::serviceStateChanged(QMessageService::State newState)
{
    //  qDebug() << "MainWindow::serviceStateChanged";
    if ((newState == QMessageService::FinishedState) && (m_service->error() != QMessageManager::NoError))
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
    QAction* senderAction = qobject_cast<QAction*>(sender());
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

