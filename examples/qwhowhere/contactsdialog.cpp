/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include "contactsdialog.h"
#include <QHBoxLayout>
#include <QApplication>
#include <QAction>

ContactsDialog::ContactsDialog(QWidget *parent) :
        QDialog(parent)
{
    QHBoxLayout* l = new QHBoxLayout;
    m_listWidget = new QListWidget(this);
    QObject::connect(m_listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
    l->addWidget(m_listWidget);
    this->setLayout(l);

    // Create QContactManager and search contacts
    createContactManager();
    searchContact();

    // Remove context menu from the all widgets
#ifdef Q_OS_SYMBIAN
    QWidgetList widgets = QApplication::allWidgets();
    QWidget* w = 0;
    foreach(w, widgets) {
        w->setContextMenuPolicy(Qt::NoContextMenu);
    }
#endif

    // Enable back softkey for the dialog
#ifdef Q_OS_SYMBIAN
    QAction* back = new QAction("Back", this);
    back->setSoftKeyRole(QAction::NegativeSoftKey);
    QObject::connect(back, SIGNAL(triggered()), this, SLOT(close()));
    addAction(back);
#endif

    showMaximized();
}

ContactsDialog::~ContactsDialog()
{
    delete m_contactManager;
}

void ContactsDialog::createContactManager()
{
    // Get list of different contact backends
    QStringList availableManagers = QContactManager::availableManagers();

    // Try to find contacts from some backend
    while (!availableManagers.isEmpty()) {
        m_contactManager = new QContactManager(availableManagers.first());
        availableManagers.removeFirst();
        if (!m_contactManager->contacts().isEmpty())
            availableManagers.clear();
    }

    // Use default
    if (!m_contactManager) {
        m_contactManager = new QContactManager();
    }
}

void ContactsDialog::searchContact()
{
    m_listWidget->clear();
    QContact currContact;

    // Sort contacts by lastname
    QContactSortOrder sort;
    sort.setDirection(Qt::AscendingOrder);
    sort.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLast);

    // Build QListWidget from the contact list
    QList<QContactLocalId> contactIds = m_contactManager->contacts(sort);
    foreach(const QContactLocalId& id, contactIds) {
        QListWidgetItem* listItem = new QListWidgetItem;
        currContact = m_contactManager->contact(id);
        listItem->setData(Qt::DisplayRole, currContact.displayLabel());
        listItem->setData(Qt::UserRole, currContact.localId()); // also store the id of the contact
#ifdef Q_OS_SYMBIAN
        listItem->setSizeHint(QSize(width(), 50));
#endif
        m_listWidget->addItem(listItem);
    }
}

void ContactsDialog::itemClicked(QListWidgetItem *item)
{
    QVariant data = item->data(Qt::UserRole);
    QContactLocalId id = data.toInt();

    QContact contact = m_contactManager->contact(id);
    QContactPhoneNumber cpn = contact.detail<QContactPhoneNumber> ();

    // Emit contact phonenumber
    if (!cpn.isEmpty()) {
        emit contactSelected(cpn.number());
        // Close dialog
        close();
    }
}

