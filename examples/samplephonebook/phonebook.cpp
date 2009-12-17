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

#include "phonebook.h"
#include "contactlistpage.h"
#include "contacteditor.h"
#include "filterpage.h"

#include <QtGui>

PhoneBook::PhoneBook(QWidget *parent)
        : QWidget(parent)
{
    m_editorPage = new ContactEditor(this);
    connect(m_editorPage, SIGNAL(showListPage()), this, SLOT(activateList()));

    m_filterPage = new FilterPage(this);
    connect(m_filterPage, SIGNAL(showListPage(QContactFilter)), this, SLOT(activateList(QContactFilter)));

    m_listPage = new ContactListPage(this);
    connect(m_listPage, SIGNAL(showEditorPage(QContactLocalId)), this, SLOT(activateEditor(QContactLocalId)));
    connect(m_listPage, SIGNAL(showFilterPage(QContactFilter)), this, SLOT(activateFind()));
    connect(m_listPage, SIGNAL(managerChanged(QContactManager*)), this, SLOT(managerChanged(QContactManager*)));

    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->addWidget(m_listPage);
    m_stackedWidget->addWidget(m_editorPage);
    m_stackedWidget->addWidget(m_filterPage);
    m_stackedWidget->setCurrentIndex(0);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_stackedWidget);

    setLayout(centralLayout);
}

PhoneBook::~PhoneBook()
{
    foreach (const QContactManager* manager, managers.values())
        delete manager;
}

void PhoneBook::backendChanged(const QList<QContactLocalId>& changes)
{
    // load all contacts from the updated backend
    QContactDetailFilter contactFilter;
    contactFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    contactFilter.setValue(QString(QLatin1String(QContactType::TypeContact)));
    QList<QContactLocalId> contactIds = cm->contacts(contactFilter);
    contacts.clear();
    foreach (const QContactLocalId cid, contactIds)
        contacts.append(cm->contact(cid));
qDebug() << "backend changed, and now have" << contactIds.size() << "contacts which are TypeContact!";

    // if there are no contacts in the backend any more, we add a new, unsaved contact
    // otherwise, display the current one.  Either way, need to repopulate the list.
    populateList(cm->contact(changes.value(0))); // this may fail if the change was a removal.
    if (!contacts.isEmpty()){
        displayContact();
    } else {
        nameLine->setText(QString());
        emailLine->setText(QString());
        homePhoneLine->setText(QString());
        workPhoneLine->setText(QString());
        mobilePhoneLine->setText(QString());
        addressText->setPlainText(QString());
        avatarPixmapLabel->clear();
        updateButtons();
    }
}

void PhoneBook::backendSelected(const QString& backend)
{
    currentIndex = -1;

    // first, disconnect any signals from the current manager
    if (cm)
        cm->disconnect();

    // then, load the selected manager (or create it if it doesn't exist)
    if (managers.value(backend, 0) != 0) {
        cm = managers.value(backend);
    } else {
        cm = new QContactManager(backend);
        managers.insert(backend, cm);
    }

    // and connect the selected manager's signals to our slots
    connect(cm, SIGNAL(contactsAdded(const QList<QContactLocalId>&)), this, SLOT(backendChanged(const QList<QContactLocalId>&)));
    connect(cm, SIGNAL(contactsChanged(const QList<QContactLocalId>&)), this, SLOT(backendChanged(const QList<QContactLocalId>&)));
    connect(cm, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), this, SLOT(backendChanged(const QList<QContactLocalId>&)));

    // and trigger an update.
    backendChanged(QList<QContactLocalId>());
}

bool PhoneBook::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == avatarButton && event->type() == QEvent::Resize) {
        QResizeEvent *re = (QResizeEvent*)event;
        QSize trim(20,20);
        QSize newSize = re->size();
        newSize -= trim;
        avatarButton->setIconSize(newSize);
        return true;
    }

    return false;
}

void PhoneBook::contactSelected(int row)
{
    if (row < 0)
        return;

    while (currentIndex > row) {
        previous();
    }

    while (currentIndex < row) {
        next();
    }
}

void PhoneBook::populateList(const QContact& currentContact)
{
    // first, we sort the contacts according to display name
    // then we populate the list widget.
    QList<QContact> sorted;
    if (contacts.size() > 0) {
        sorted.append(contacts.at(0));
        contacts.removeFirst();
    }

    // sort the list.
    foreach (const QContact& contact, contacts) {
        bool inserted = false;
        for (int i = 0; i < sorted.size(); i++) {
            // first, retrieve the display labels from each contact
            QContact sortedContact = sorted.at(i);
            QContactDisplayLabel cdl = contact.detail(QContactDisplayLabel::DefinitionName);
            QContactDisplayLabel sdl = sortedContact.detail(QContactDisplayLabel::DefinitionName);

            // compare the display labels, and insert into list.
            if (cdl.label().toLower() < sdl.label().toLower()) {
                sorted.insert(i, contact);
                inserted = true;
                break;
            }
        }

        if (!inserted) {
            sorted.append(contact);
        }
    }

    // replace our member list.
    contacts = sorted;

    // and repopulate the list widget.
    contactsList->clear();
    foreach (const QContact& contact, contacts) {
        new QListWidgetItem(contact.displayLabel(), contactsList);
    }

    // now find out what our new current index is
    currentIndex = 0;
    for (int i = 0; i < contacts.size(); i++) {
        if (contacts.at(i) == currentContact) {
            currentIndex = i;
            break;
        }
    }

    // and set the selection in the list widget.
    contactsList->setCurrentRow(currentIndex);
}

QContact PhoneBook::buildContact() const
{
    // builds the contact from the current index / current UI.
    QContact c;
    QContactName contactName = buildName(nameLine->text());
    c.saveDetail(&contactName);

    QContactEmailAddress emailAddress;
    emailAddress.setEmailAddress(emailLine->text());
    c.saveDetail(&emailAddress);

    QContactPhoneNumber homePhone;
    homePhone.setNumber(homePhoneLine->text());
    homePhone.setContexts(QStringList(QContactDetail::ContextHome));
    homePhone.setSubTypes(QStringList() << QContactPhoneNumber::SubTypeLandline << QContactPhoneNumber::SubTypeVoice);
    c.saveDetail(&homePhone);

    QContactPhoneNumber workPhone;
    workPhone.setNumber(workPhoneLine->text());
    workPhone.setContexts(QStringList(QContactDetail::ContextWork));
    workPhone.setSubTypes(QStringList() << QContactPhoneNumber::SubTypeLandline << QContactPhoneNumber::SubTypeVoice);
    c.saveDetail(&workPhone);

    QContactPhoneNumber mobilePhone;
    mobilePhone.setNumber(mobilePhoneLine->text());
    mobilePhone.setSubTypes(QStringList(QContactPhoneNumber::SubTypeMobile));
    c.saveDetail(&mobilePhone);

    // note that we abuse the "street" field in this example.
    QContactAddress address;
    address.setStreet(addressText->toPlainText());
    if (!address.street().isEmpty())
        c.saveDetail(&address);

    return c;
}

/*!
 * Build QContactName with one of the following set:
 * 1. custom label, 2. first name, 3. last name
 * If none is supported the returned QContactName is left empty.
 */
QContactName PhoneBook::buildName(const QString &name) const
{
    QContactName contactName;
    QContactDetailDefinition nameDef = cm->detailDefinition(QContactName::DefinitionName, QContactType::TypeContact);
    if(nameDef.fields().contains(QContactName::FieldCustomLabel)) {
        contactName.setCustomLabel(name);
    } else if(nameDef.fields().contains(QContactName::FieldFirst)) {
        contactName.setFirst(name);
    } else if(nameDef.fields().contains(QContactName::FieldLast)) {
        contactName.setLast(name);
    }
    return contactName;
}

void PhoneBook::displayContact()
{
    QContact c = contacts.value(currentIndex);
    c = cm->contact(c.id().localId()); // this removes any unsaved information.

    QContactId contactUri = c.id();
    QList<QContactRelationship> relationships = cm->relationships(QContactRelationship::HasMember, contactUri);
    QList<QContactLocalId> currentGroups;
    foreach (const QContactRelationship& currRel, relationships) {
        if (currRel.second() == contactUri) {
            currentGroups.append(currRel.first().localId());
        }
    }
    contactGroups = currentGroups;

    // display the name
    nameLine->setText(c.displayLabel());

    // display the email address
    emailLine->setText(c.detail(QContactEmailAddress::DefinitionName).value(QContactEmailAddress::FieldEmailAddress));

    // display the phone numbers
    QList<QContactDetail> phns = c.details(QContactPhoneNumber::DefinitionName);
    bool foundHomePhone = false;
    bool foundWorkPhone = false;
    bool foundMobilePhone = false;
    for (int i = 0; i < phns.size(); i++) {
        QContactPhoneNumber current = phns.at(i);
        if (current.subTypes().contains(QContactPhoneNumber::SubTypeMobile)) {
            mobilePhoneLine->setText(current.number());
            foundMobilePhone = true;
        } else if (current.contexts().contains(QContactDetail::ContextWork)) {
            workPhoneLine->setText(current.number());
            foundWorkPhone = true;
        } else {
            homePhoneLine->setText(current.number());
            foundHomePhone = true;
        }
    }

    if (!foundHomePhone)
        homePhoneLine->setText("");
    if (!foundWorkPhone)
        workPhoneLine->setText("");
    if (!foundMobilePhone)
        mobilePhoneLine->setText("");

    // display the address - again, we abuse the street() field.
    addressText->setPlainText((QContactAddress(c.detail(QContactAddress::DefinitionName))).street());

    QContactAvatar av = c.detail<QContactAvatar>();
    QPixmap avatarPix = av.pixmap();

    avatarPixmapLabel->clear();
    if (!avatarPix.isNull() || avatarPix.load(av.avatar()){
    	avatarPixmapLabel->setPixmap(avatarPix.scaled(avatarPixmapLabel->size()));
    }   
    updateButtons();

}

void PhoneBook::selectAvatar()
{
    //determine the list of file formats to pass to QFileDialog::getOpenFileName()
    QString supportedImageFormats;
    for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
	supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
    }

	QString selected = QFileDialog::getOpenFileName(detailsForm, "Select avatar image file", ".", tr("Images (%1)").arg(supportedImageFormats));
    if (!selected.isNull()) {
        QContact curr = contacts.at(currentIndex);
        QContactAvatar av = curr.detail(QContactAvatar::DefinitionName);
        av.setAvatar(selected);
	    QPixmap avatarPix;

	    if (avatarPix.load(selected)){
	        avatarPixmapLabel->setPixmap(avatarPix.scaled(avatarPixmapLabel->size()));
            av.setPixmap(avatarPix);
	    }else{
	        qWarning() << "Unable to load avatar" << selected;
	        qWarning() << "Supported image formats are " << supportedImageFormats << " ;"  <<  QImageReader::supportedImageFormats();
	        avatarPixmapLabel->clear();
	    }

        curr.saveDetail(&av);
        contacts.replace(currentIndex, curr);
    }
}

void PhoneBook::addContact()
{
    if (addingContact)
        saveContact();

    QMessageBox msgBox(QMessageBox::Question, tr("Add Contact or Group"), tr("Add Contact or Group"), QMessageBox::NoButton, this);
    QAbstractButton *addContactButton = msgBox.addButton(tr("Add Contact"), QMessageBox::YesRole);
    (void*)msgBox.addButton(tr("Add Group"), QMessageBox::NoRole);
    (void)msgBox.exec();
    if(msgBox.clickedButton() == addContactButton){
        addingContact = true;
        lastIndex = currentIndex;
        currentIndex = contacts.size();
        contacts.append(QContact());
        displayContact();
        nameLine->setFocus();
        if (smallScreenSize){
	    contactDetailsForm->showMaximized();
	    if (contactDetailsForm->exec() == QDialog::Accepted)
		saveContact();
        }
    }else{
	// Groups will be modified in QContactManager by dialog
	GroupEditDialog grpDialog(this, cm);
	if (smallScreenSize)
	     grpDialog.showMaximized();
	(void)grpDialog.exec();
    }
}

void PhoneBook::saveContact()
{
    if (smallScreenSize && !(addingContact || editingContact))
	return;

    addingContact = false;
    editingContact = false;
    if (smallScreenSize){
	contactDetailsForm->accept();
	showMaximized();
    }

    QContact c = buildContact();
    c.setId(contacts.at(currentIndex).id());
    QContactAvatar av = contacts.at(currentIndex).detail(QContactAvatar::DefinitionName);
    c.saveDetail(&av);

    if (!cm->saveContact(&c)) {
        QString errorCode = "Unable to save the contact in the database; error code:" + QString::number(cm->error());
        QMessageBox::information(this, "Save Failed", errorCode);
        return;
    }
}

void PhoneBook::activateEditor(QContactLocalId contactId)
{
    m_editorPage->setCurrentContact(m_manager, contactId);
    m_stackedWidget->setCurrentIndex(1); // list = 0, editor = 1, find = 2.
}

void PhoneBook::activateList(const QContactFilter& filter)
{  
    m_currentFilter = filter;
    activateList(); // call base now.
}

void PhoneBook::activateList()
{
    m_listPage->rebuildList(m_currentFilter);
    m_stackedWidget->setCurrentIndex(0); // list = 0, editor = 1, find = 2.
}

void PhoneBook::activateFind()
{
    m_stackedWidget->setCurrentIndex(2); // list = 0, editor = 1, find = 2.
}

void PhoneBook::managerChanged(QContactManager *manager)
{
    m_manager = manager;
    m_editorPage->setCurrentContact(m_manager, 0); // must reset the manager of the editor.
}
