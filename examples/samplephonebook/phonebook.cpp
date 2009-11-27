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
#include "serialiser.h"
#include "contactdetailsform.h"
#include "maindialogform_640_480.h"
#include "maindialogform_240_320.h"
#include "groupeditdialog.h"
#include "groupdetailsdialog.h"
#include <QDesktopWidget>

#include <QtGui>

PhoneBook::PhoneBook(QWidget *parent)
    : QWidget(parent), dialog(0), addingContact(false), editingContact(false)
{

    QVBoxLayout *layout = new QVBoxLayout;

    QDesktopWidget screenWidget;

    if (QApplication::desktop()->width() > 480){
		smallScreenSize = false;
        mainDialogForm640By480 = new MainDialogForm640By480(this);
        mainForm = mainDialogForm640By480;
        detailsForm = mainForm;
        layout->addWidget(mainForm);
        setLayout(layout);
    }else{
		smallScreenSize = true;
        mainDialogForm240By320 = new MainDialogForm240By320(this);
        contactDetailsForm = new ContactDetailsForm(mainDialogForm240By320);
        mainForm = mainDialogForm240By320;
	    detailsForm = contactDetailsForm;
		layout->addWidget(mainForm);
		setLayout(layout);
		showMaximized();
    }


    addButton = qFindChild<QPushButton*>(mainForm, "addButton");
    openButton = qFindChild<QPushButton*>(mainForm, "openButton");
    removeButton = qFindChild<QPushButton*>(mainForm, "deleteButton");
    findButton = qFindChild<QPushButton*>(mainForm, "findButton");
    importButton = qFindChild<QPushButton*>(mainForm, "importButton");
    exportButton = qFindChild<QPushButton*>(mainForm, "exportButton");
    avatarButton = qFindChild<QPushButton*>(mainForm, "avatarButton");
    quitButton = qFindChild<QPushButton*>(mainForm, "quitButton");
    if(!avatarButton)
        avatarButton = qFindChild<QPushButton*>(detailsForm, "avatarButton");
    saveButton = qFindChild<QPushButton*>(mainForm, "saveButton");
    if(!saveButton)
        saveButton = qFindChild<QPushButton*>(detailsForm, "saveButton");
    cancelButton = qFindChild<QPushButton*>(detailsForm, "cancelButton");
    groupsButton = qFindChild<QPushButton*>(detailsForm, "groupsButton");

    avatarButton->installEventFilter(this);

    removeButton->setEnabled(false);
    findButton->setEnabled(false);
    saveButton->setEnabled(true);
    exportButton->setEnabled(false);

    nameLine = qFindChild<QLineEdit*>(detailsForm, "nameEdit");
    avatarPixmapLabel = qFindChild<QLabel*>(detailsForm, "avatarPixmapLabel");
    emailLine = qFindChild<QLineEdit*>(detailsForm, "emailEdit");
    homePhoneLine = qFindChild<QLineEdit*>(detailsForm, "homePhoneEdit");
    workPhoneLine = qFindChild<QLineEdit*>(detailsForm, "workPhoneEdit");
    mobilePhoneLine = qFindChild<QLineEdit*>(detailsForm, "mobilePhoneEdit");
    addressText = qFindChild<QPlainTextEdit*>(detailsForm, "addressEdit");
    contactsList = qFindChild<QListWidget*>(mainForm, "contactListWidget");
    currentIndexLabel = qFindChild<QLabel*>(mainForm, "contactStatusLabel");
    backendCombo = qFindChild<QComboBox*>(mainForm, "contactEngineComboBox");

    QStringList availableManagers = QContactManager::availableManagers();
    foreach (const QString manager, availableManagers)
        backendCombo->addItem(manager);
    connect(backendCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(backendSelected(QString)));

    if(cancelButton)
        connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelContact()));
    if(openButton){
        connect(openButton, SIGNAL(clicked()), this, SLOT(openContact()));
        openButton->setEnabled(false);
    }
    connect(avatarButton, SIGNAL(clicked()), this, SLOT(selectAvatar()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(addContact()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveContact()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeContact()));
    connect(findButton, SIGNAL(clicked()), this, SLOT(findContact()));
    connect(importButton, SIGNAL(clicked()), this, SLOT(importFromVCard()));
    connect(exportButton, SIGNAL(clicked()), this, SLOT(exportAsVCard()));
    if(quitButton)
        connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(groupsButton, SIGNAL(clicked()), this, SLOT(editGroupDetails()));
    connect(contactsList, SIGNAL(currentRowChanged(int)), this, SLOT(contactSelected(int)));
    if (smallScreenSize){
	connect(detailsForm, SIGNAL(accepted()), this, SLOT(saveContact()));
	connect(detailsForm, SIGNAL(rejected()), this, SLOT(cancelContact()));
    }
    setWindowTitle(tr("Sample Phone Book"));
    // instantiate a new contact manager
    cm = 0;
    backendSelected(backendCombo->currentText());

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

    // and build the avatar filename and display it if it exists.
    QString avatarFile = c.detail(QContactAvatar::DefinitionName).value(QContactAvatar::FieldAvatar);
    if (avatarFile.isNull() || avatarFile.isEmpty()) {
        avatarPixmapLabel->clear();
    } else {
        QPixmap avatarPix(avatarFile);
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
        curr.saveDetail(&av);
        contacts.replace(currentIndex, curr);

	QPixmap avatarPix;
	if (avatarPix.load(selected)){
	    avatarPixmapLabel->setPixmap(avatarPix.scaled(avatarPixmapLabel->size()));
	}else{
	    qWarning() << "Unable to load avatar" << selected;
	    qWarning() << "Supported image formats are " << supportedImageFormats << " ;"  <<  QImageReader::supportedImageFormats();
	    avatarPixmapLabel->clear();
	}
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

void PhoneBook::updateButtons()
{
    QString currentState = "Unsaved";
    if (!contacts.count() || (contacts.at(currentIndex).id() == QContactId())) {
        addButton->setEnabled(true);
        findButton->setEnabled(false);
        exportButton->setEnabled(false);
        removeButton->setEnabled(false);
        saveButton->setEnabled(addingContact || editingContact);
        if(openButton)
            openButton->setEnabled(false);
        if (!smallScreenSize){
            nameLine->setEnabled(addingContact);
            emailLine->setEnabled(addingContact);
            homePhoneLine->setEnabled(addingContact);
            workPhoneLine->setEnabled(addingContact);
            mobilePhoneLine->setEnabled(addingContact);
            addressText->setEnabled(addingContact);
            avatarButton->setEnabled(addingContact);
        }
    } else {
        addButton->setEnabled(!(addingContact || editingContact));
        findButton->setEnabled(true);
        exportButton->setEnabled(true);
        removeButton->setEnabled(true);
        saveButton->setEnabled((mainForm == detailsForm) || addingContact || editingContact);
        if(openButton)
            openButton->setEnabled(true);
        if (!smallScreenSize){
            nameLine->setEnabled(true);
            emailLine->setEnabled(true);
            homePhoneLine->setEnabled(true);
            workPhoneLine->setEnabled(true);
            mobilePhoneLine->setEnabled(true);
            addressText->setEnabled(true);
            avatarButton->setEnabled(true);
        }
        currentState = "Saved";
    }
    importButton->setEnabled(!(addingContact || editingContact));

    // update the UI depending on the current state.
    int contactNumber = (contacts.isEmpty() ? 0 : currentIndex + 1);
    QString currentIndexLabelString = tr("Contact %1 of %2 (%3)").arg(contactNumber).arg(contacts.size()).arg(currentState);
    currentIndexLabel->setText(currentIndexLabelString);
}

void PhoneBook::removeContact()
{
    if (currentIndex < 0)
        return;
    QContact current = contacts.at(currentIndex);
    QContactDisplayLabel cdl = current.detail(QContactDisplayLabel::DefinitionName);
    QString contactName = cdl.label();
    int button = QMessageBox::question(this,
        tr("Confirm Remove"),
        tr("Are you sure you want to remove \"%1\"?").arg(contactName),
        QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::Yes) {
        cm->removeContact(contacts.at(currentIndex).id().localId());
        QMessageBox::information(this, tr("Remove Successful"),
            tr("\"%1\" has been removed from your phone book.").arg(contactName));
    }
    updateButtons();
}

void PhoneBook::next()
{
    // we should display the next contact
    currentIndex += 1;
    displayContact();
}

void PhoneBook::previous()
{
    // first, check to see if the current index is saved.
    // if not, we delete it.
    if (contacts.at(currentIndex).id() == QContactId()) {
        contacts.removeAt(currentIndex);
    }

    // we should display the previous contact
    currentIndex -= 1;
    displayContact();
}

void PhoneBook::findContact()
{

    dialog = new FindDialog(this);

    if (smallScreenSize)
	dialog->showMaximized();

    if (dialog->exec() == QDialog::Accepted) {
        bool found = false;
        if (dialog->isSimpleFilterEnabled()){
            QString contactName = dialog->getFindText();
            // XXX TODO: use QContactManager::contactsWithDetail
            for (int i = 0; i < contacts.size(); i++) {
                QContact current = contacts.at(i);
                QContactDisplayLabel cdl = current.detail(QContactDisplayLabel::DefinitionName);
                if (cdl.label() == contactName) {
                    contactsList->setCurrentRow(i);
                    contactSelected(i);
                    found = true;
                    break;
                }
            }
            if (!found) {
                QMessageBox::information(this, tr("Contact Not Found"),
                        tr("Sorry, \"%1\" is not in your address book.").arg(contactName));
            }
        }else{
            QList<QContactLocalId> matchedContacts = cm->contacts(dialog->getFindFilter());
            if (matchedContacts.count()){
                QStringList matchedContactNames;
                QContact matchedContact;
                contactsList->clearSelection();
                // find the names of contacts that match
                for (int index = 0; index < matchedContacts.count(); index++){
                    matchedContact = cm->contact(matchedContacts[index]);
                    if (!matchedContact.isEmpty())
                        matchedContactNames.append("\"" + matchedContact.displayLabel() + "\"");
                }
                QMessageBox::information(this, tr("Contact(s) Found"), tr("Matched contact(s): %1").arg(matchedContactNames.join(",")));
            }else{
                QMessageBox::information(this, tr("Contact not Found"), tr("No contacts in your addressbook match filter"));
            }
        }
    }
    delete dialog;
    dialog = 0;
    if (smallScreenSize)
	showMaximized();
}

void PhoneBook::openContact()
{
    editingContact = true;
    displayContact();
    if (smallScreenSize){
        nameLine->setFocus();
	if (contactDetailsForm->exec() == QDialog::Accepted)
	    saveContact();
    }else{
	detailsForm->show();
    }
}

void PhoneBook::cancelContact()
{
    if (smallScreenSize && !(addingContact || editingContact))
	return;

    addingContact = false;
    editingContact = false;
    if (smallScreenSize){
	contactDetailsForm->reject();
	showMaximized();
    }
    currentIndex = lastIndex;
    updateButtons();
}

void PhoneBook::importFromVCard()
{
    QString importFile = QFileDialog::getOpenFileName(this, "Select vCard file to import", ".", "*.vcf");
    if (importFile.isNull())
        return;

    // import the vcard
    QContact importedContact;

    // read in the file that we are importing
    QFile file(importFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(this, tr("Import Failed"),
                tr("Sorry, unable to import \"%1\".").arg(importFile));
        return;
    }

    QTextStream in(&file);
    QStringList vcardLines;
    while (!in.atEnd()) {
        vcardLines << in.readLine();
    }

    if (vcardLines.isEmpty()) {
        QMessageBox::information(this, tr("Import Failed"),
                tr("Sorry, unable to import \"%1\".").arg(importFile));
        return;
    }
    file.close();

    // convert it to a contact - probably need to write function to check that it's a valid vcard...
    importedContact = Serialiser::convertVcard(vcardLines);

    // if the current contact is newly added (and not saved), we overwrite it
    cm->saveContact(&importedContact);
}

void PhoneBook::exportAsVCard()
{
    // convert the display name to a meaningful file name
    QString newName = QFileDialog::getSaveFileName(this, "Export contact as...", ".", "*.vcf");
    if (newName.isNull()) {
        // they clicked cancel.
        return;
    }
    if (!newName.endsWith(".vcf")) {
        newName += ".vcf";
    }

    QContact currentContact;
    if (currentIndex >= contacts.size())
        currentContact = buildContact();
    else
        currentContact = contacts.at(currentIndex);
    QStringList vcardLines = Serialiser::convertContact(currentContact);

    QFile file(newName);
    QContact current = contacts.at(currentIndex);
    QContactDisplayLabel cdl = current.detail(QContactDisplayLabel::DefinitionName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(this, tr("Unable to export"),
            tr("Unable to export contact \"%1\"!").arg(cdl.label()));
        return;
    }

    QTextStream out(&file);
    foreach (const QString& line, vcardLines)
        out << line << "\n";
    file.close();

    QMessageBox::information(this, tr("Contact Exported"),
        tr("Successfully exported contact \"%1\" as \"%2\"!").arg(cdl.label()).arg(newName));
}

void PhoneBook::editGroupDetails()
{
    QContact c = buildContact();
    c.setId(contacts.value(currentIndex).id());
    GroupDetailsDialog dlg(detailsForm, cm, c);
    if (smallScreenSize)
	dlg.showMaximized();

    if (dlg.exec() == QDialog::Accepted)
        contactGroups = dlg.groups();

    if (currentIndex < contacts.size())
        contacts.replace(currentIndex, c);
}
