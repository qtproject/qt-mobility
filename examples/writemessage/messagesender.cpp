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
      toEdit(0),
      subjectEdit(0),
      textEdit(0),
      sendButton(0),
      removeButton(0),
      addButton(0),
      attachmentsList(0)
{
    setWindowTitle(tr("Send Message"));

    toEdit = new QLineEdit;

    subjectEdit = new QLineEdit;

    QLabel *toLabel = new QLabel(tr("To"));
    toLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QLabel *subjectLabel = new QLabel(tr("Subject"));
    subjectLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QGridLayout *metaDataLayout = new QGridLayout;
    metaDataLayout->addWidget(toLabel, 0, 0);
    metaDataLayout->setAlignment(toLabel, Qt::AlignRight);
    metaDataLayout->addWidget(subjectLabel, 1, 0);
    metaDataLayout->setAlignment(subjectLabel, Qt::AlignRight);
    metaDataLayout->addWidget(toEdit, 0, 1);
    metaDataLayout->addWidget(subjectEdit, 1, 1);

    removeButton = new QPushButton(tr("Remove"));
    removeButton->setEnabled(false);

    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeAttachment()));

    addButton = new QPushButton(tr("Add"));

    connect(addButton, SIGNAL(clicked()), this, SLOT(addAttachment()));

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(removeButton);
    buttonLayout->addWidget(addButton);

    attachmentsList = new QListWidget;
    attachmentsList->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(attachmentsList, SIGNAL(currentRowChanged(int)), this, SLOT(attachmentSelected(int)));

    QHBoxLayout *attachmentsLayout = new QHBoxLayout;
    attachmentsLayout->addWidget(attachmentsList);
    attachmentsLayout->addLayout(buttonLayout);

    QGroupBox *attachmentsGroup = new QGroupBox(tr("Attachments"));
    attachmentsGroup->setLayout(attachmentsLayout);

    textEdit = new QTextEdit;

    sendButton = new QPushButton(tr("Send"));

    connect(sendButton, SIGNAL(clicked()), this, SLOT(send()), Qt::QueuedConnection);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(metaDataLayout, 0);
    mainLayout->addWidget(textEdit, 2);
    mainLayout->addWidget(attachmentsGroup, 1);
    mainLayout->addWidget(sendButton, 0);
    mainLayout->setAlignment(sendButton, Qt::AlignRight);

    connect(&service, SIGNAL(stateChanged(QMessageServiceAction::State)), this, SLOT(stateChanged(QMessageServiceAction::State)));
}

MessageSender::~MessageSender()
{
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

void MessageSender::attachmentSelected(int)
{
    removeButton->setEnabled(true);
}

void MessageSender::send()
{
    QString to(toEdit->text());
    if (to.isEmpty()) {
        QMessageBox::warning(0, "Missing information", "Please enter a recipient address");
        return;
    }

    QString subject(subjectEdit->text());
    if (subject.isEmpty()) {
        QMessageBox::warning(0, "Missing information", "Please enter a subject");
        return;
    }

    QString text(textEdit->toPlainText());
    if (text.isEmpty()) {
        QMessageBox::warning(0, "Missing information", "Please enter a message");
        return;
    }

    QMessage message;
    message.setType(QMessage::Email);
    message.setTo(QMessageAddress(to, QMessageAddress::Email));
    message.setSubject(subject);

    message.setBody(text);

    if (attachmentsList->count()) {
        QStringList paths;
        for (int i = 0; i < attachmentsList->count(); ++i) {
            paths.append(attachmentsList->item(i)->text());
        }

        message.appendAttachments(paths);
    }

    if (service.send(message)) {
        sendButton->setEnabled(false);
        sendId = message.id();
    } else {
        QMessageBox::warning(0, "Failed", "Unable to send message");
    }
}

void MessageSender::stateChanged(QMessageServiceAction::State s)
{
    if (s == QMessageServiceAction::Successful) {
        QMessageBox::information(0, "Success", "Message sent successfully");
        sendButton->setEnabled(true);
    } else if (s == QMessageServiceAction::Failed) {
        QMessageBox::warning(0, "Failed", "Unable to send message");

        if (!QMessageStore::instance()->removeMessage(sendId)) {
            qWarning() << "Unable to remove failed message:" << sendId.toString();
        }

        sendButton->setEnabled(true);
        sendId = QMessageId();
    }
}

