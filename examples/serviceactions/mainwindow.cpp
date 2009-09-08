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
#include <QListView>
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

typedef QPointer<QMessageServiceAction> QMessageServiceActionPtr;

static const QSize WindowGeometry(400,300);
static const QString WindowTitle("QMessageServiceAction Example");

class ComposeSendWidget : public QWidget
{
    Q_OBJECT

public:
    ComposeSendWidget(QMessageServiceAction* service, QWidget* parent = 0);

private slots:
    void composeButtonClicked();
    void sendButtonClicked();
    void addAttachmentButtonClicked();
    void updateAccountsCombo();

private:
    void setupUi();
    QMessage constructQMessage() const;

private:
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
};

ComposeSendWidget::ComposeSendWidget(QMessageServiceAction* service, QWidget* parent)
:
QWidget(parent),
m_service(service),
m_accountsCombo(0),
m_toEdit(0),
m_subjectEdit(0),
m_bodyEdit(0),
m_composeButton(0),
m_sendButton(0),
m_attachmentList(0),
m_addAttachmentButton(0)
{
    setupUi();
    QTimer::singleShot(0,this,SLOT(updateAccountsCombo()));
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

void ComposeSendWidget::updateAccountsCombo()
{
    m_accountsCombo->clear();

    m_availableAccounts = QMessageStore::instance()->queryAccounts();
    for(int i = 0; i < m_availableAccounts.count(); ++i)
        m_accountsCombo->addItem(QString("%1 - %2").arg(i+1).arg(QMessageAccount(m_availableAccounts[i]).name()));
}

void ComposeSendWidget::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

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
}

QMessage ComposeSendWidget::constructQMessage() const
{
    QMessage message;

    if(m_availableAccounts.isEmpty())
    {
        QMessageBox::critical(0,"No Accounts","Cannot send a message without any avaialble accounts");
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
public:
    ShowWidget(QMessageServiceAction* service, QWidget* parent = 0);

private:
    QMessageServiceAction* m_service;
};

ShowWidget::ShowWidget(QMessageServiceAction* service, QWidget* parent)
:
QWidget(parent),
m_service(service)
{
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

    m_tabWidget->addTab(new ComposeSendWidget(m_serviceAction,this),"Compose and Send");
    m_tabWidget->addTab(new RetrieveWidget(m_serviceAction,this),"Retrieve");
    m_tabWidget->addTab(new ShowWidget(m_serviceAction,this),"Show");
    m_tabWidget->addTab(new QueryWidget(m_serviceAction,this),"Query");

    setWindowTitle(WindowTitle);
    resize(WindowGeometry);
}

#include <mainwindow.moc>
