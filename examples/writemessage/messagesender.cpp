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

    textEdit = new QTextEdit;

    sendButton = new QPushButton(tr("Send"));

    connect(sendButton, SIGNAL(clicked()), this, SLOT(send()), Qt::QueuedConnection);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(metaDataLayout, 0);
    mainLayout->addWidget(textEdit, 2);
    mainLayout->addWidget(attachmentsGroup, 1);
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

    // Find the list of available accounts and add them to combo box
    foreach (const QMessageAccountId &id, manager.queryAccounts()) {
        QMessageAccount account(id);

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

void MessageSender::accountSelected(int index)
{
    QString name(accountCombo->itemText(index));
    if (!name.isEmpty()) {
        QPair<QMessage::Type, QMessageAccountId> details = accountDetails[name];

        // Disable subject and attachments for SMS
        const bool smsOnly(details.first == QMessage::Sms);
        subjectEdit->setEnabled(!smsOnly);
        addButton->setEnabled(!smsOnly);
        removeButton->setEnabled(!smsOnly);
    }
}

void MessageSender::attachmentSelected(int index)
{
    removeButton->setEnabled(index != -1);
}

void MessageSender::send()
{
    QMessage message;

    QString name(accountCombo->currentText());
    if (!name.isEmpty()) {
        QPair<QMessage::Type, QMessageAccountId> details = accountDetails[name];
        message.setType(details.first);
        message.setParentAccountId(details.second);
    }

    QString to(toEdit->text());
    if (to.isEmpty()) {
        QMessageBox::warning(0, tr("Missing information"), tr("Please enter a recipient address"));
        return;
    }

    QMessageAddressList toList;
    foreach (const QString &item, to.split(QRegExp("\\s"), QString::SkipEmptyParts)) {
        toList.append(QMessageAddress(item, message.type() == QMessage::Email ? QMessageAddress::Email : QMessageAddress::Phone));
    }
    message.setTo(toList);

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

    if (message.type() != QMessage::Sms) {
        if (attachmentsList->count()) {
            QStringList paths;
            for (int i = 0; i < attachmentsList->count(); ++i) {
                paths.append(attachmentsList->item(i)->text());
            }

            message.appendAttachments(paths);
        }
    }

    if (service.send(message)) {
        sendButton->setEnabled(false);
        sendId = message.id();
    } else {
        QMessageBox::warning(0, tr("Failed"), tr("Unable to send message"));
    }
}

void MessageSender::stateChanged(QMessageService::State s)
{
    if (s == QMessageService::Successful) {
        QMessageBox::information(0, tr("Success"), tr("Message sent successfully"));
        sendButton->setEnabled(true);
    } else if (s == QMessageService::Failed) {
        QMessageBox::warning(0, tr("Failed"), tr("Unable to send message"));

        if (!manager.removeMessage(sendId)) {
            qWarning() << "Unable to remove failed message:" << sendId.toString();
        }

        sendButton->setEnabled(true);
        sendId = QMessageId();
    }
}

