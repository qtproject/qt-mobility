/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#if defined(Q_OS_SYMBIAN)
// Maximized windows are resizing correctly on S60 w/ Qt 4.6.0
// Use tabs to reduce the minimal height required
#define USE_TABBED_LAYOUT
#endif

#include "messagesender.h"

#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QDebug>
#include <QScrollArea>

#ifdef USE_TABBED_LAYOUT
#include <QTabWidget>
#endif

MessageSender::MessageSender(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags),
      accountCombo(0),
      toEdit(0),
      subjectEdit(0),
      textEdit(0),
      sendButton(0),
      removeButton(0),
      addButton(0),
      attachmentsList(0)
{
    QVBoxLayout* vbl = new QVBoxLayout(this);
    vbl->setSpacing(0);
    vbl->setContentsMargins(0,0,0,0);

    QWidget* mainWidget = new QWidget(this);

    QScrollArea* sa = new QScrollArea(this);
    vbl->addWidget(sa);
    sa->setWidget(mainWidget);
    sa->setWidgetResizable(true);

    setWindowTitle(tr("Write Message"));

    accountCombo = new QComboBox;
    connect(accountCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(accountSelected(int)));

    toEdit = new QLineEdit;

    subjectEdit = new QLineEdit;

    QLabel *accountLabel = new QLabel(tr("Account"));
    accountLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QLabel *toLabel = new QLabel(tr("To"));
    toLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QLabel *subjectLabel = new QLabel(tr("Subject"));
    subjectLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QGridLayout *metaDataLayout = new QGridLayout;
    metaDataLayout->setContentsMargins(0, 0, 0, 0);
    metaDataLayout->addWidget(accountLabel, 0, 0);
    metaDataLayout->setAlignment(accountLabel, Qt::AlignRight);
    metaDataLayout->addWidget(toLabel, 1, 0);
    metaDataLayout->setAlignment(toLabel, Qt::AlignRight);
    metaDataLayout->addWidget(subjectLabel, 2, 0);
    metaDataLayout->setAlignment(subjectLabel, Qt::AlignRight);
    metaDataLayout->addWidget(accountCombo, 0, 1);
    metaDataLayout->addWidget(toEdit, 1, 1);
    metaDataLayout->addWidget(subjectEdit, 2, 1);

    removeButton = new QPushButton(tr("Remove"));
    removeButton->setEnabled(false);

    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeAttachment()));

    addButton = new QPushButton(tr("Add"));

    connect(addButton, SIGNAL(clicked()), this, SLOT(addAttachment()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);

    attachmentsList = new QListWidget;
    attachmentsList->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(attachmentsList, SIGNAL(currentRowChanged(int)), this, SLOT(attachmentSelected(int)));

    QVBoxLayout *attachmentsLayout = new QVBoxLayout;
    attachmentsLayout->setContentsMargins(0, 0, 0, 0);
    attachmentsLayout->addWidget(attachmentsList);
    attachmentsLayout->addLayout(buttonLayout);

    QGroupBox *attachmentsGroup = new QGroupBox(tr("Attachments"));
    attachmentsGroup->setLayout(attachmentsLayout);
#ifdef Q_WS_MAEMO_5
    // Maemo 5 style doesn't take group box titles into account.
    int spacingHack = QFontMetrics(QFont()).height();
    attachmentsLayout->setContentsMargins(0, spacingHack, 0, 0);
#endif

    textEdit = new QTextEdit;

    sendButton = new QPushButton(tr("Send"));

    connect(sendButton, SIGNAL(clicked()), this, SLOT(send()), Qt::QueuedConnection);

#ifdef USE_TABBED_LAYOUT
    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->addTab(textEdit, tr("Text"));
    tabWidget->addTab(attachmentsGroup, tr("Attachments"));
#endif

    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(metaDataLayout, 0);
#ifdef USE_TABBED_LAYOUT
    mainLayout->addWidget(tabWidget, 0);
#else
    mainLayout->addWidget(textEdit, 2);
    mainLayout->addWidget(attachmentsGroup, 1);
#endif
    mainLayout->addWidget(sendButton, 0);
    mainLayout->setAlignment(sendButton, Qt::AlignRight);

    connect(&service, SIGNAL(stateChanged(QMessageService::State)), this, SLOT(stateChanged(QMessageService::State)));

    QTimer::singleShot(0, this, SLOT(populateAccounts()));
}

MessageSender::~MessageSender()
{
}

void MessageSender::populateAccounts()
{
#ifndef _WIN32_WCE
    // How long can the accounts query take?
    setCursor(Qt::BusyCursor);
#endif

//! [accounts-listing]
    // Find the list of available accounts and add them to combo box
    foreach (const QMessageAccountId &id, manager.queryAccounts()) {
        QMessageAccount account(id);

        // What is the most capable message type supported by this account?
        QMessage::Type type(QMessage::NoType);
        if (account.messageTypes() & QMessage::Email) {
            type = QMessage::Email;
        } else if (account.messageTypes() & QMessage::Mms) {
            type = QMessage::Mms;
        } else if (account.messageTypes() & QMessage::Sms) {
            type = QMessage::Sms;
        }

        if (type != QMessage::NoType) {
            QString name(account.name());
            accountDetails.insert(name, qMakePair(type, account.id()));
            accountCombo->addItem(name);
        }
    }
//! [accounts-listing]

    if (accountDetails.isEmpty()) {
        QMessageBox::warning(0, tr("Cannot send"), tr("No accounts are available to send with!"));
        QCoreApplication::instance()->quit();
    } else {
        accountCombo->setCurrentIndex(0);
    }

#ifndef _WIN32_WCE
    setCursor(Qt::ArrowCursor);
#endif
}

void MessageSender::removeAttachment()
{
    delete attachmentsList->takeItem(attachmentsList->currentRow());
    if (attachmentsList->count() == 0) {
        removeButton->setEnabled(false);
    }
}

void MessageSender::addAttachment()
{
    QString path(QFileDialog::getOpenFileName());
    if (!path.isEmpty()) {
        attachmentsList->addItem(new QListWidgetItem(path));
    }
}

//! [account-selected]
void MessageSender::accountSelected(int index)
{
    QString name(accountCombo->itemText(index));
    if (!name.isEmpty()) {
        QPair<QMessage::Type, QMessageAccountId> details = accountDetails[name];

        // Enable subject only for email
        subjectEdit->setEnabled(details.first == QMessage::Email);

        // Disable attachments for SMS
        const bool smsOnly(details.first == QMessage::Sms);
        addButton->setEnabled(!smsOnly);
        removeButton->setEnabled(!smsOnly);
    }
}
//! [account-selected]

void MessageSender::attachmentSelected(int index)
{
    removeButton->setEnabled(index != -1);
}

void MessageSender::send()
{
//! [associate-account]
    QString accountName(accountCombo->currentText());
    if (accountName.isEmpty()) {
        QMessageBox::warning(0, tr("Missing information"), tr("No account is selected for transmission"));
        return;
    }

    QMessage message;

    QPair<QMessage::Type, QMessageAccountId> details = accountDetails[accountName];
    message.setType(details.first);
    message.setParentAccountId(details.second);
//! [associate-account]

//! [set-recipients]
    QString to(toEdit->text());
    if (to.isEmpty()) {
        QMessageBox::warning(0, tr("Missing information"), tr("Please enter a recipient address"));
        return;
    }

    // Find the address type to use for this message
    QMessageAddress::Type addrType(message.type() == QMessage::Email ? QMessageAddress::Email : QMessageAddress::Phone);

    QMessageAddressList toList;
    foreach (const QString &item, to.split(QRegExp("\\s"), QString::SkipEmptyParts)) {
        toList.append(QMessageAddress(addrType, item));
    }
    message.setTo(toList);
//! [set-recipients]

//! [set-properties]
    if (message.type() == QMessage::Email) {
        QString subject(subjectEdit->text());
        if (subject.isEmpty()) {
            QMessageBox::warning(0, tr("Missing information"), tr("Please enter a subject"));
            return;
        }
        message.setSubject(subject);
    }

    QString text(textEdit->toPlainText());
    if (text.isEmpty()) {
        QMessageBox::warning(0, tr("Missing information"), tr("Please enter a message"));
        return;
    }
    message.setBody(text);
//! [set-properties]

//! [add-attachments]
    if (message.type() != QMessage::Sms) {
        if (attachmentsList->count()) {
            QStringList paths;
            for (int i = 0; i < attachmentsList->count(); ++i) {
                paths.append(attachmentsList->item(i)->text());
            }

            message.appendAttachments(paths);
        }
    }
//! [add-attachments]

//! [send-message]
    sendButton->setEnabled(false);
    if (service.send(message)) {
        sendId = message.id();
    } else {
        sendButton->setEnabled(true);
        QMessageBox::warning(0, tr("Failed"), tr("Unable to send message"));
    }
//! [send-message]
}

//! [handle-response]
void MessageSender::stateChanged(QMessageService::State newState)
{
    if (newState == QMessageService::FinishedState) {
        if (service.error() == QMessageManager::NoError) {
            QMessageBox::information(0, tr("Success"), tr("Message sent successfully"));
        } else {
            QMessageBox::warning(0, tr("Failed"), tr("Unable to send message"));

            // Try to delete the failed message
            if (!manager.removeMessage(sendId)) {
                qWarning() << "Unable to remove failed message:" << sendId.toString();
            }
        }

        sendId = QMessageId();
        sendButton->setEnabled(true);
    }
}
//! [handle-response]

