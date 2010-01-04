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

#include "contactlistpage.h"

#include <QtGui>

ContactListPage::ContactListPage(QWidget *parent)
        : QWidget(parent)
{
    m_manager = 0;
    m_currentFilter = QContactFilter();

    m_backendsCombo = new QComboBox(this);
    QStringList availableManagers = QContactManager::availableManagers();
    m_backendsCombo->addItems(availableManagers);
    connect(m_backendsCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(backendSelected()));
    m_filterActiveLabel = new QLabel("Inactive");
    m_filterActiveLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    m_contactsList = new QListWidget(this);

    m_addContactBtn = new QPushButton("Add", this);
    connect(m_addContactBtn, SIGNAL(clicked()), this, SLOT(addContactClicked()));
    m_editBtn = new QPushButton("Edit", this);
    connect(m_editBtn, SIGNAL(clicked()), this, SLOT(editClicked()));
    m_deleteBtn = new QPushButton("Delete", this);
    connect(m_deleteBtn, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    m_filterBtn = new QPushButton("Filter", this);
    connect(m_filterBtn, SIGNAL(clicked()), this, SLOT(filterClicked()));

    QFormLayout *backendLayout = new QFormLayout;
    backendLayout->addRow("Store:", m_backendsCombo);
    backendLayout->addRow("Filter:", m_filterActiveLabel);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_addContactBtn);
    btnLayout->addWidget(m_editBtn);
    btnLayout->addWidget(m_deleteBtn);
    btnLayout->addWidget(m_filterBtn);

    QVBoxLayout *bookLayout = new QVBoxLayout;
    bookLayout->addLayout(backendLayout);
    bookLayout->addWidget(m_contactsList);
    bookLayout->addLayout(btnLayout);

    setLayout(bookLayout);

    // force update to backend.
    QTimer::singleShot(0, this, SLOT(backendSelected()));
}

ContactListPage::~ContactListPage()
{
    QList<QContactManager*> initialisedManagers = m_initialisedManagers.values();
    while (!initialisedManagers.isEmpty()) {
        QContactManager *deleteMe = initialisedManagers.takeFirst();
        delete deleteMe;
    }
}

void ContactListPage::backendSelected()
{
    QString backend = m_backendsCombo->currentText();

    // first, check to see if they reselected the same backend.
    if (m_manager && m_manager->managerName() == backend)
        return;

    // the change is real.  update.
    if (m_initialisedManagers.contains(backend)) {
        m_manager = m_initialisedManagers.value(backend);
    } else {
        m_manager = new QContactManager(backend);
        m_initialisedManagers.insert(backend, m_manager);
    }

    // signal that the manager has changed.
    emit managerChanged(m_manager);

    // and... rebuild the list.
    rebuildList(m_currentFilter);
}

void ContactListPage::rebuildList(const QContactFilter& filter)
{
    // first, check to see whether the filter does anything
    if (filter == QContactFilter())
        m_filterActiveLabel->setText("Inactive");
    else
        m_filterActiveLabel->setText("Active");

    QContact currContact;
    m_currentFilter = filter;
    m_contactsList->clear();
    m_idToListIndex.clear();
    QList<QContactLocalId> contactIds = m_manager->contacts(m_currentFilter);
    foreach (const QContactLocalId& id, contactIds) {
        QListWidgetItem *currItem = new QListWidgetItem;
        currContact = m_manager->contact(id);
        currItem->setData(Qt::DisplayRole, currContact.displayLabel());
        currItem->setData(Qt::UserRole, currContact.localId()); // also store the id of the contact.
        m_idToListIndex.insert(currContact.localId(), m_contactsList->count());
        m_contactsList->addItem(currItem);
    }
}

void ContactListPage::addContactClicked()
{
    emit showEditorPage(QContactLocalId(0));
}

void ContactListPage::editClicked()
{
    if (m_contactsList->currentItem())
        emit showEditorPage(QContactLocalId(m_contactsList->currentItem()->data(Qt::UserRole).toUInt()));
    // else, nothing selected; ignore.
}

void ContactListPage::filterClicked()
{
    emit showFilterPage(m_currentFilter);
}

void ContactListPage::deleteClicked()
{
    if (!m_manager) {
        qWarning() << "No manager selected; cannot delete.";
        return;
    }

    if (!m_contactsList->currentItem()) {
        qWarning() << "Nothing to delete.";
        return;
    }
        
    QContactLocalId contactId = QContactLocalId(m_contactsList->currentItem()->data(Qt::UserRole).toUInt());
    bool success = m_manager->removeContact(contactId);
    if (success) {
        delete m_contactsList->takeItem(m_contactsList->currentRow());
        QMessageBox::information(this, "Success!", "Contact deleted successfully!");
    }
    else
        QMessageBox::information(this, "Failed!", "Failed to delete contact!");
}
