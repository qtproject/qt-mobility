/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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

MainWindow::MainWindow() {
    setWindowTitle("Contacts Partial Save Tester");

    QWidget* centralWidget = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout;

    mainLayout->addWidget(new QLabel(QString("Manager: %1").arg(m_manager.managerName())));

    QPushButton* reloadButton = new QPushButton("Reload");
    connect(reloadButton, SIGNAL(clicked()), this, SLOT(reloadClicked()));
    mainLayout->addWidget(reloadButton);

    QHBoxLayout* nameLayout = new QHBoxLayout;
    QLabel* nameLabel = new QLabel("Name");
    m_nameEdit = new QLineEdit;
    QPushButton* nameButton = new QPushButton("Save");
    connect(nameButton, SIGNAL(clicked()), this, SLOT(nameSaveClicked()));
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(m_nameEdit);
    nameLayout->addWidget(nameButton);
    mainLayout->addLayout(nameLayout);

    QHBoxLayout* phoneLayout = new QHBoxLayout;
    QLabel* phoneLabel = new QLabel("Phone");
    m_phoneEdit = new QLineEdit;
    QPushButton* phoneButton = new QPushButton("Save");
    connect(phoneButton, SIGNAL(clicked()), this, SLOT(phoneSaveClicked()));
    phoneLayout->addWidget(phoneLabel);
    phoneLayout->addWidget(m_phoneEdit);
    phoneLayout->addWidget(phoneButton);
    mainLayout->addLayout(phoneLayout);

    QHBoxLayout* emailLayout = new QHBoxLayout;
    QLabel* emailLabel = new QLabel("Email");
    m_emailEdit = new QLineEdit;
    QPushButton* emailButton = new QPushButton("Save");
    connect(emailButton, SIGNAL(clicked()), this, SLOT(emailSaveClicked()));
    emailLayout->addWidget(emailLabel);
    emailLayout->addWidget(m_emailEdit);
    emailLayout->addWidget(emailButton);
    mainLayout->addLayout(emailLayout);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

#ifdef Q_OS_SYMBIAN
    showMaximized();
#else
    show();
#endif
}

void MainWindow::reloadClicked()
{
    QList<QContact> contacts = m_manager.contacts();
    if (!contacts.isEmpty()) {
        m_contact = contacts.first();
        m_nameEdit->setText(m_contact.detail<QContactName>().customLabel());
        m_emailEdit->setText(m_contact.detail<QContactEmailAddress>().emailAddress());
        m_phoneEdit->setText(m_contact.detail<QContactPhoneNumber>().number());
    }
}

void MainWindow::nameSaveClicked()
{
    updateContact();

    QList<QContact> contacts;
    contacts.append(m_contact);
    m_manager.saveContacts(&contacts, QStringList() << QContactName::DefinitionName);
}

void MainWindow::phoneSaveClicked()
{
    updateContact();

    QList<QContact> contacts;
    contacts.append(m_contact);
    m_manager.saveContacts(&contacts, QStringList() << QContactPhoneNumber::DefinitionName);
}

void MainWindow::emailSaveClicked()
{
    updateContact();

    QList<QContact> contacts;
    contacts.append(m_contact);
    m_manager.saveContacts(&contacts, QStringList() << QContactEmailAddress::DefinitionName);
}

void MainWindow::updateContact()
{
    const QString& name = m_nameEdit->text();
    const QString& phone = m_phoneEdit->text();
    const QString& email = m_emailEdit->text();

    if (!name.isEmpty()) {
        QContactName detail = m_contact.detail<QContactName>();
        detail.setFirstName(name);
        m_contact.saveDetail(&detail);
    }

    if (!phone.isEmpty()) {
        QContactPhoneNumber detail = m_contact.detail<QContactPhoneNumber>();
        detail.setNumber(phone);
        m_contact.saveDetail(&detail);
    }

    if (!email.isEmpty()) {
        QContactEmailAddress detail = m_contact.detail<QContactEmailAddress>();
        detail.setEmailAddress(email);
        m_contact.saveDetail(&detail);
    }
}
