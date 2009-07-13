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

    currentBackendLabel = new QLabel(tr("Current Backend:"));
    backendCombo = new QComboBox();
    QStringList availableManagers = QContactManager::availableManagers();
    foreach (const QString manager, availableManagers)
        backendCombo->addItem(manager);
    connect(backendCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(backendSelected(QString)));

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
    currentIndexLayout->addWidget(currentBackendLabel);
    currentIndexLayout->addWidget(backendCombo);
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

    // instantiate a new contact manager
    cm = 0;
    backendSelected(backendCombo->currentText());
}

PhoneBook::~PhoneBook()
{
    foreach (const QContactManager* manager, managers.values())
        delete manager;
}

void PhoneBook::backendChanged(const QList<QUniqueId>& changes)
{
    // load all contacts from the updated backend
    QList<QUniqueId> contactIds = cm->contacts();
    contacts.clear();
    foreach (const QUniqueId cid, contactIds)
        contacts.append(cm->contact(cid));

    // if there are no contacts in the backend any more, we add a new, unsaved contact
    // otherwise, display the current one.  Either way, need to repopulate the list.
    populateList(cm->contact(changes.value(0))); // this may fail if the change was a removal.
    if (contacts.isEmpty()) {
        addContact();
    } else {
        displayContact();
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
    connect(cm, SIGNAL(contactsAdded(const QList<QUniqueId>&)), this, SLOT(backendChanged(const QList<QUniqueId>&)));
    connect(cm, SIGNAL(contactsChanged(const QList<QUniqueId>&)), this, SLOT(backendChanged(const QList<QUniqueId>&)));
    connect(cm, SIGNAL(contactsRemoved(const QList<QUniqueId>&)), this, SLOT(backendChanged(const QList<QUniqueId>&)));

    // and trigger an update.
    backendChanged(QList<QUniqueId>());
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
            QContactDisplayLabel cdl = contact.detail(QContactDisplayLabel::DefinitionId);
            QContactDisplayLabel sdl = sortedContact.detail(QContactDisplayLabel::DefinitionId);

            // if none set, ask the backend to synthesise them for us.
            if (cdl.isEmpty())
                cdl.setLabel(cm->synthesiseDisplayLabel(contact));
            if (sdl.isEmpty())
                sdl.setLabel(cm->synthesiseDisplayLabel(sortedContact));

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
        new QListWidgetItem(contact.displayLabel().label(), contactsList);
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
    QContactDisplayLabel cdl;
    cdl.setLabel(nameLine->text());
    c.saveDetail(&cdl);

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
    address.setAttribute(QContactDetail::AttributeSubType, QString(QLatin1String(QContactAddress::AttributeSubTypeDomestic) + "," + QLatin1String(QContactAddress::AttributeSubTypeParcel) + "," + QLatin1String(QContactAddress::AttributeSubTypePostal)));
    c.saveDetail(&address);

    return c;
}

void PhoneBook::displayContact()
{
    QContact c = contacts.at(currentIndex);
    c = cm->contact(c.id()); // this removes any unsaved information.

    // display the name
    nameLine->setText(c.displayLabel().label());


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
    QString avatarFile = c.detail(QContactAvatar::DefinitionId).value(QContactAvatar::FieldAvatar);
    if (avatarFile.isNull() || avatarFile.isEmpty()) {
        avatarButton->setIcon(QIcon());
        avatarButton->setText("No image selected");
    } else {
        avatarButton->setIcon(QIcon(avatarFile));
        avatarButton->setText("");
        avatarButton->setIconSize(avatarButton->size());
    }

    // update the UI depending on the current state.
    QString currentState = contacts.at(currentIndex).id() == 0 ? "Unsaved" : "Saved";
    QString currentIndexLabelString = tr("Contact %1 of %2 (%3)").arg(currentIndex+1).arg(contacts.size()).arg(currentState);
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
        contacts.replace(currentIndex, curr);

        avatarButton->setIcon(QIcon(selected));
        avatarButton->setText("");
        avatarButton->setIconSize(avatarButton->size());
    }
}

void PhoneBook::addContact()
{
    currentIndex = contacts.size();
    contacts.append(QContact());
    displayContact();
}

void PhoneBook::saveContact()
{
    QContact c = buildContact();
    c.setId(contacts.at(currentIndex).id());
    QContactAvatar av = contacts.at(currentIndex).detail(QContactAvatar::DefinitionId);
    c.saveDetail(&av);
    if (!cm->saveContact(&c)) {
        QString errorCode = "Unable to save the contact in the database; error code:" + QString::number(cm->error());
        QMessageBox::information(this, "Save Failed", errorCode);
        return;
    }
}

void PhoneBook::removeContact()
{
    QContact current = contacts.at(currentIndex);
    QContactDisplayLabel cdl = current.detail(QContactDisplayLabel::DefinitionId);
    if (cdl.isEmpty())
        cdl.setLabel(cm->synthesiseDisplayLabel(current));
    QString contactName = cdl.label();
    int button = QMessageBox::question(this,
        tr("Confirm Remove"),
        tr("Are you sure you want to remove \"%1\"?").arg(contactName),
        QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::Yes) {
        cm->removeContact(contacts.at(currentIndex).id());
        QMessageBox::information(this, tr("Remove Successful"),
            tr("\"%1\" has been removed from your phone book.").arg(contactName));
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
            QContact current = contacts.at(i);
            QContactDisplayLabel cdl = current.detail(QContactDisplayLabel::DefinitionId);
            if (cdl.isEmpty())
                cdl.setLabel(cm->synthesiseDisplayLabel(current));
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
    QContactDisplayLabel cdl = current.detail(QContactDisplayLabel::DefinitionId);
    if (cdl.isEmpty())
        cdl.setLabel(cm->synthesiseDisplayLabel(current));
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
