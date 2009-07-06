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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "phonebook.h"
#include "serialiser.h"

#include <QtGui>

PhoneBook::PhoneBook(QWidget *parent)
    : QWidget(parent)
{
    // instantiate a new contact manager - memory backend
    cm = new QContactManager();

    contactsList = new QListWidget();
    contactsList->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));

    QLabel *nameLabel = new QLabel(tr("Name:"));
    nameLine = new QLineEdit;

    QLabel *avatarLabel = new QLabel(tr("Avatar:"));
    avatarButton = new QPushButton("No image selected");
    QSizePolicy avatarPolicy;
    avatarPolicy.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
    avatarPolicy.setVerticalPolicy(QSizePolicy::MinimumExpanding);
    avatarButton->setSizePolicy(avatarPolicy);
    avatarButton->installEventFilter(this);

    QLabel *emailLabel = new QLabel(tr("Email:"));
    emailLine = new QLineEdit;

    QLabel *homePhoneLabel = new QLabel(tr("Home Phone:"));
    homePhoneLine = new QLineEdit;

    QLabel *workPhoneLabel = new QLabel(tr("Work Phone:"));
    workPhoneLine = new QLineEdit;

    QLabel *mobilePhoneLabel = new QLabel(tr("Mobile Phone:"));
    mobilePhoneLine = new QLineEdit;

    QLabel *addressLabel = new QLabel(tr("Address:"));
    addressText = new QTextEdit;

    addButton = new QPushButton(tr("&Add"));
    addButton->setEnabled(false);
    removeButton = new QPushButton(tr("&Remove"));
    removeButton->setEnabled(false);
    findButton = new QPushButton(tr("&Find"));
    findButton->setEnabled(false);
    saveButton = new QPushButton(tr("&Save"));
    saveButton->setEnabled(true);

    currentIndexLabel = new QLabel();
    currentIndexLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    importButton = new QPushButton(tr("&Import"));
    importButton->setToolTip(tr("Import contact from vCard file"));
    exportButton = new QPushButton(tr("&Export"));
    exportButton->setToolTip(tr("Export contact as vCard file"));
    exportButton->setEnabled(false);

    quitButton = new QPushButton(tr("&Quit"));

    dialog = new FindDialog;

    connect(avatarButton, SIGNAL(clicked()), this, SLOT(selectAvatar()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(addContact()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveContact()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeContact()));
    connect(findButton, SIGNAL(clicked()), this, SLOT(findContact()));
    connect(importButton, SIGNAL(clicked()), this, SLOT(importFromVCard()));
    connect(exportButton, SIGNAL(clicked()), this, SLOT(exportAsVCard()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(contactsList, SIGNAL(currentRowChanged(int)), this, SLOT(contactSelected(int)));

    QVBoxLayout *buttonLayout1 = new QVBoxLayout;
    buttonLayout1->addStretch();
    buttonLayout1->addWidget(addButton);
    buttonLayout1->addWidget(saveButton);
    buttonLayout1->addWidget(removeButton);
    buttonLayout1->addWidget(findButton);
    buttonLayout1->addWidget(importButton);
    buttonLayout1->addWidget(exportButton);
    buttonLayout1->addWidget(quitButton);
    buttonLayout1->addStretch();

    QHBoxLayout *currentIndexLayout = new QHBoxLayout;
    currentIndexLayout->addStretch();
    currentIndexLayout->addWidget(currentIndexLabel);

    QGridLayout *inputLayout = new QGridLayout;
    inputLayout->addWidget(nameLabel, 0, 0, Qt::AlignRight);
    inputLayout->addWidget(nameLine, 0, 1);
    inputLayout->addWidget(avatarLabel, 1, 0, Qt::AlignRight);
    inputLayout->addWidget(avatarButton, 1, 1);
    inputLayout->addWidget(emailLabel, 2, 0, Qt::AlignRight);
    inputLayout->addWidget(emailLine, 2, 1);
    inputLayout->addWidget(homePhoneLabel, 3, 0,Qt::AlignRight);
    inputLayout->addWidget(homePhoneLine, 3, 1);
    inputLayout->addWidget(workPhoneLabel, 4, 0, Qt::AlignRight);
    inputLayout->addWidget(workPhoneLine, 4, 1);
    inputLayout->addWidget(mobilePhoneLabel, 5, 0, Qt::AlignRight);
    inputLayout->addWidget(mobilePhoneLine, 5, 1);
    inputLayout->addWidget(addressLabel, 6, 0, Qt::AlignRight);
    inputLayout->addWidget(addressText, 6, 1);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(contactsList, 0, 0, 3, 1);
    mainLayout->addLayout(inputLayout, 0, 1, 2, 2);
    mainLayout->addLayout(buttonLayout1, 0, 3, 2, 1);
    mainLayout->addLayout(currentIndexLayout, 2, 2);

    setLayout(mainLayout);
    setWindowTitle(tr("Sample Phone Book"));

    // add a new, blank contact and kick off the display
    addContact();
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
            if (contact.name().displayName().toLower() < sorted.at(i).name().displayName().toLower()) {
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
        new QListWidgetItem(contact.name().displayName(), contactsList);
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
    c.name().setDisplayName(nameLine->text());

    QContactEmailAddress emailAddress;
    emailAddress.setEmailAddress(emailLine->text());
    emailAddress.setAttribute(QContactDetail::AttributeSubType, QContactEmailAddress::AttributeSubTypeInternet);
    c.saveDetail(&emailAddress);

    QContactPhoneNumber homePhone;
    homePhone.setNumber(homePhoneLine->text());
    homePhone.setAttribute(QContactDetail::AttributeContext, QContactDetail::AttributeContextHome);
    homePhone.setAttribute(QContactDetail::AttributeSubType, QContactPhoneNumber::AttributeSubTypeLandline);
    c.saveDetail(&homePhone);

    QContactPhoneNumber workPhone;
    workPhone.setNumber(workPhoneLine->text());
    workPhone.setAttribute(QContactDetail::AttributeContext, QContactDetail::AttributeContextWork);
    workPhone.setAttribute(QContactDetail::AttributeSubType, QContactPhoneNumber::AttributeSubTypeLandline);
    c.saveDetail(&workPhone);

    QContactPhoneNumber mobilePhone;
    mobilePhone.setNumber(mobilePhoneLine->text());
    mobilePhone.setAttribute(QContactDetail::AttributeSubType, QContactPhoneNumber::AttributeSubTypeMobile);
    c.saveDetail(&mobilePhone);

    QContactAddress address;
    address.setDisplayLabel(addressText->toPlainText());
    address.setAttribute(QContactDetail::AttributeSubType, QString(QContactAddress::AttributeSubTypeDomestic + "," + QContactAddress::AttributeSubTypeParcel + "," + QContactAddress::AttributeSubTypePostal));
    c.saveDetail(&address);

    QContactAvatar avatar;
    avatar.setAvatar(contactAvatars.at(currentIndex));
    avatar.setAttribute(QContactDetail::AttributeSubType, QContactAvatar::AttributeSubTypeImage);
    c.saveDetail(&avatar);

    return c;
}

void PhoneBook::displayContact()
{
    QContact c = contacts.at(currentIndex);

    // display the name
    nameLine->setText(c.name().displayName());

    // display the email address
    emailLine->setText(c.detail(QContactEmailAddress::DefinitionId).value(QContactEmailAddress::FieldEmailAddress));

    // display the phone numbers
    QList<QContactDetail> phns = c.details(QContactPhoneNumber::DefinitionId);
    bool foundHomePhone = false;
    bool foundWorkPhone = false;
    bool foundMobilePhone = false;
    for (int i = 0; i < phns.size(); i++) {
        QContactPhoneNumber current = phns.at(i);
        if (current.attribute(QContactDetail::AttributeSubType).contains(QContactPhoneNumber::AttributeSubTypeMobile)) {
            mobilePhoneLine->setText(current.number());
            foundMobilePhone = true;
        } else if (current.attribute(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextWork)) {
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

    // display the address
    addressText->setText((QContactAddress(c.detail(QContactAddress::DefinitionId))).displayLabel());

    // and build the avatar filename and display it if it exists.
    QString avatarFile = contactAvatars.at(currentIndex);
    if (avatarFile.isNull() || avatarFile.isEmpty()) {
        avatarButton->setIcon(QIcon());
        avatarButton->setText("No image selected");
    } else {
        avatarButton->setIcon(QIcon(avatarFile));
        avatarButton->setText("");
        avatarButton->setIconSize(avatarButton->size());
    }

    // update the UI depending on the current state.
    QString currentIndexLabelString = tr("Contact %1 of %2 (%3)").arg(currentIndex+1).arg(contacts.size()).arg((contacts.at(currentIndex).id() == 0) ? tr("Unsaved") : tr("Saved"));
    currentIndexLabel->setText(currentIndexLabelString);

    // update the buttons
    if (contacts.at(currentIndex).id() == 0) {
        addButton->setEnabled(false);
        exportButton->setEnabled(false);
        removeButton->setEnabled(false);
    } else {
        addButton->setEnabled(true);
        findButton->setEnabled(true);
        exportButton->setEnabled(true);
        removeButton->setEnabled(true);
    }
}

void PhoneBook::selectAvatar()
{
    QString selected = QFileDialog::getOpenFileName(this, "Select avatar image file", ".");
    if (!selected.isNull()) {
        QContact curr = contacts.at(currentIndex);
        QContactAvatar av = curr.detail(QContactAvatar::DefinitionId);
        av.setAvatar(selected);
        curr.saveDetail(&av);

        contactAvatars.replace(currentIndex, selected);
        contacts.replace(currentIndex, curr);
    }
    displayContact();
}

void PhoneBook::addContact()
{
    currentIndex = contacts.size();
    contacts.append(QContact());
    contactAvatars.append("");
    displayContact();
}

void PhoneBook::saveContact()
{
    QContact c = buildContact();
    c.setId(contacts.at(currentIndex).id());
    if (c.id() != 0) {
        // we want to check that the contact has actually changed since last save
        if (c == contacts.at(currentIndex)) {
            // no change.
            return;
        }
    }

    if (!cm->saveContact(&c)) {
        QString errorCode = "Unable to save the contact in the database; error code:" + QString::number(cm->error());
        QMessageBox::information(this, "Save Failed", errorCode);
        return;
    }

    contacts.replace(currentIndex, c);
    populateList(c);
    displayContact();
}

void PhoneBook::removeContact()
{
    QString contactName = contacts.at(currentIndex).name().displayName();
    int button = QMessageBox::question(this,
        tr("Confirm Remove"),
        tr("Are you sure you want to remove \"%1\"?").arg(contactName),
        QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::Yes) {
        // remove the contact, and adjust our current position.
        if (contacts.at(currentIndex).id() != 0)
            cm->removeContact(contacts.at(currentIndex).id());
        contacts.removeAt(currentIndex);
        contactAvatars.removeAt(currentIndex);
        while (contacts.size() <= currentIndex)
            currentIndex -= 1;

        // if all contacts have been removed from the list, add a new (unsaved) contact.
        if (currentIndex < 0) {
            addContact();
        }

        QMessageBox::information(this, tr("Remove Successful"),
            tr("\"%1\" has been removed from your phone book.").arg(contactName));
        QContact currentContact = contacts.at(currentIndex);
        populateList(currentContact);
        displayContact();
    }
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
    if (contacts.at(currentIndex).id() == 0) {
        contacts.removeAt(currentIndex);
        contactAvatars.removeAt(currentIndex);
    }

    // we should display the previous contact
    currentIndex -= 1;
    displayContact();
}

void PhoneBook::findContact()
{
    dialog->show();

    if (dialog->exec() == 1) {
        bool found = false;
        QString contactName = dialog->getFindText();
        // XXX TODO: use QContactManager::contactsWithDetail
        for (int i = 0; i < contacts.size(); i++) {
            if (contacts.at(i).name().displayName() == contactName) {
                contactsList->setCurrentRow(i);
                contactSelected(i);
                found = true;
                break;
            }
        }

        if (!found) {
            QMessageBox::information(this, tr("Contact Not Found"),
                tr("Sorry, \"%1\" is not in your address book.").arg(contactName));
            return;
        }
    }
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
    if (contacts.at(currentIndex).id() == 0) {
        contacts.replace(currentIndex, importedContact);
        contactAvatars.replace(currentIndex, importedContact.detail(QContactAvatar::DefinitionId).value(QContactAvatar::FieldAvatar));
    } else {
        contacts.append(importedContact);
        contactAvatars.append(importedContact.detail(QContactAvatar::DefinitionId).value(QContactAvatar::FieldAvatar));
    }

    // update the display
    populateList(importedContact);
    displayContact();
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
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(this, tr("Unable to export"),
            tr("Unable to export contact \"%1\"!").arg(contacts.at(currentIndex).name().displayName()));
        return;
    }

    QTextStream out(&file);
    foreach (const QString& line, vcardLines)
        out << line << "\n";
    file.close();

    QMessageBox::information(this, tr("Contact Exported"),
        tr("Successfully exported contact \"%1\" as \"%2\"!").arg(contacts.at(currentIndex).name().displayName()).arg(newName));
}
