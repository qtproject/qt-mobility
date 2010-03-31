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
#ifdef BUILD_VERSIT
#include "qversitreader.h"
#include "qversitcontactimporter.h"
#include "qversitwriter.h"
#include "qversitcontactexporter.h"
#endif

#include <QtGui>

ContactListPage::ContactListPage(QMainWindow *mainWindow, QWidget *parent)
        : QWidget(parent), m_mainWindow(mainWindow)
{
    m_manager = 0;
    m_currentFilter = QContactFilter();

    m_backendsCombo = new QComboBox(this);
    QStringList availableManagers = QContactManager::availableManagers();
    availableManagers.removeAll("invalid");
    foreach(QString managerName, availableManagers) {

        QMap<QString, QString> params;
        QString managerUri = QContactManager::buildUri(managerName, params);

        // Add some parameters to SIM backend so that we can use
        // all the stores.
        if (managerName == "symbiansim") {
            availableManagers.removeAll("symbiansim");

            availableManagers.append("symbiansim:adn");
            params.insert("store", "ADN");
            managerUri = QContactManager::buildUri(managerName, params);
            m_availableManagers.insert(availableManagers.last(), managerUri);

            availableManagers.append("symbiansim:fdn");
            params.clear();
            params.insert("store", "FDN");
            managerUri = QContactManager::buildUri(managerName, params);
            m_availableManagers.insert(availableManagers.last(), managerUri);

            availableManagers.append("symbiansim:sdn");
            params.clear();
            params.insert("store", "SDN");
            managerUri = QContactManager::buildUri(managerName, params);
            m_availableManagers.insert(availableManagers.last(), managerUri);
        }
        else {
            m_availableManagers.insert(managerName, managerUri);
        }
    }
    m_backendsCombo->addItems(availableManagers);
    connect(m_backendsCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(backendSelected()));
    m_filterActiveLabel = new QLabel(tr("Inactive"));
    m_filterActiveLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));



    QVBoxLayout *bookLayout = new QVBoxLayout;
    QFormLayout *backendLayout = new QFormLayout;
    backendLayout->addRow(tr("Store:"), m_backendsCombo);
    backendLayout->addRow(tr("Filter:"), m_filterActiveLabel);
    bookLayout->addLayout(backendLayout);

    m_contactsList = new QListWidget(this);
    bookLayout->addWidget(m_contactsList);

    // Action buttons at the bottom
    QHBoxLayout *btnLayout1 = new QHBoxLayout;

    QPushButton* addBtn = new QPushButton(tr("&Add"), this);
    connect(addBtn, SIGNAL(clicked()), this, SLOT(addClicked()));
    btnLayout1->addWidget(addBtn);

    QPushButton* editBtn = new QPushButton(tr("&Edit"), this);
    connect(editBtn, SIGNAL(clicked()), this, SLOT(editClicked()));
    btnLayout1->addWidget(editBtn);

    QPushButton* deleteBtn = new QPushButton(tr("&Delete"), this);
    connect(deleteBtn, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    btnLayout1->addWidget(deleteBtn);

    bookLayout->addLayout(btnLayout1);

    setLayout(bookLayout);

    // Add items to the menu
    if (m_mainWindow) {
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
        QMenuBar *optionsMenu = m_mainWindow->menuBar();
#else
        QMenu *optionsMenu = new QMenu(tr("&Contacts"), this);
        m_mainWindow->menuBar()->addMenu(optionsMenu);
#endif
        QAction* addAction = new QAction(tr("&Add Contact..."), this);
        connect(addAction, SIGNAL(triggered()), this, SLOT(addClicked()));
        optionsMenu->addAction(addAction);
        QAction* editAction = new QAction(tr("&Edit Contact..."), this);
        connect(editAction, SIGNAL(triggered()), this, SLOT(editClicked()));
        optionsMenu->addAction(editAction);
        QAction* deleteAction = new QAction(tr("&Delete Contact"), this);
        connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteClicked()));
        optionsMenu->addAction(deleteAction);
        optionsMenu->addSeparator();
        QAction* filterAction = new QAction(tr("Apply &Filter..."), this);
        connect(filterAction, SIGNAL(triggered()), this, SLOT(filterClicked()));
        optionsMenu->addAction(filterAction);
        QAction* clearFilterAction = new QAction(tr("&Clear Filter"), this);
        connect(clearFilterAction, SIGNAL(triggered()), this, SLOT(clearFilterClicked()));
        optionsMenu->addAction(clearFilterAction);
        optionsMenu->addSeparator();

#ifdef BUILD_VERSIT
        QAction* importAction = new QAction(tr("&Import contacts..."), this);
        connect(importAction, SIGNAL(triggered()), this, SLOT(importClicked()));
        optionsMenu->addAction(importAction);
        QAction* exportAction = new QAction(tr("Ex&port contacts..."), this);
        connect(exportAction, SIGNAL(triggered()), this, SLOT(exportClicked()));
        optionsMenu->addAction(exportAction);
        optionsMenu->addSeparator();
#endif
        QAction* exitAction = new QAction(tr("E&xit"), this);
        connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
        optionsMenu->addAction(exitAction);
    }

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
    QString managerUri = m_availableManagers.value(m_backendsCombo->currentText());

    // first, check to see if they reselected the same backend.
    if (m_manager && m_manager->managerUri() == managerUri)
        return;

    // the change is real.  update.
    if (m_initialisedManagers.contains(managerUri)) {
        m_manager = m_initialisedManagers.value(managerUri);
    } else {
        m_manager = QContactManager::fromUri(managerUri);
        if (m_manager->error()) {
            QMessageBox::information(this, tr("Failed!"), QString("Failed to open store!\n(error code %1)").arg(m_manager->error()));
            delete m_manager;
            m_manager = 0;
            return;
        }
        m_initialisedManagers.insert(managerUri, m_manager);
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
        m_filterActiveLabel->setText(tr("Inactive"));
    else
        m_filterActiveLabel->setText(tr("Active"));

    QContact currContact;
    m_currentFilter = filter;
    m_contactsList->clear();
    m_idToListIndex.clear();
    QList<QContactLocalId> contactIds = m_manager->contactIds(m_currentFilter);
    foreach (const QContactLocalId& id, contactIds) {
        QListWidgetItem *currItem = new QListWidgetItem;
        currContact = m_manager->contact(id);
        currItem->setData(Qt::DisplayRole, currContact.displayLabel());
        currItem->setData(Qt::UserRole, currContact.localId()); // also store the id of the contact.
        m_idToListIndex.insert(currContact.localId(), m_contactsList->count());
        m_contactsList->addItem(currItem);
    }
}

void ContactListPage::addClicked()
{
    if (m_manager)
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
    if (m_manager)
        emit showFilterPage(m_currentFilter);
}

void ContactListPage::clearFilterClicked()
{
    if (m_manager)
        rebuildList(QContactFilter());

    emit clearFilter();

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
    }
    else
        QMessageBox::information(this, "Failed!", "Failed to delete contact!");
}

void ContactListPage::importClicked()
{
#ifdef BUILD_VERSIT
    if (!m_manager) {
        qWarning() << "No manager selected; cannot import";
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(this,
       tr("Select vCard file"), ".", tr("vCard files (*.vcf)"));
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    if (file.isReadable()) {
        QVersitReader reader;
        reader.setDevice(&file);
        if (reader.startReading() && reader.waitForFinished()) {
            QVersitContactImporter importer;
            if (importer.importDocuments(reader.results())) {
                QList<QContact> contacts = importer.contacts();
                QMap<int, QContactManager::Error> errorMap;
                m_manager->saveContacts(&contacts, &errorMap);
                rebuildList(m_currentFilter);
            }
        }
    }
#endif
}

void ContactListPage::exportClicked()
{
#ifdef BUILD_VERSIT
    if (!m_manager) {
        qWarning() << "No manager selected; cannot import";
        return;
    }
    QList<QContact> contacts = m_manager->contacts(QList<QContactSortOrder>(), QContactFetchHint());
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save vCard"),
                                                    "./contacts.vcf",
                                                    tr("vCards (*.vcf)"));
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    if (file.isWritable()) {
        QVersitContactExporter exporter;
        if(exporter.exportContacts(contacts, QVersitDocument::VCard30Type)) {
            QList<QVersitDocument> documents = exporter.documents();
            QVersitWriter writer;
            writer.setDevice(&file);
            writer.startWriting(documents);
            writer.waitForFinished();
        }
    }
#endif
}
