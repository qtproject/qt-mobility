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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "groupdetailsdialog.h"

#include <QtGui>
#include "qcontact.h"
#include "qcontactmanager.h"

#include "qcontactrelationship.h"
#include "qcontacttype.h"

GroupDetailsDialog::GroupDetailsDialog(QWidget *parent, QContactManager *contactManager, const QContact &contact)
        : QDialog(parent), cm(contactManager)
{
    Q_ASSERT(contactManager);
    setWindowTitle(tr("Group Details"));

    QFormLayout *layout = new QFormLayout;

    contactNameEdit = new QLineEdit(this);
    contactNameEdit->setReadOnly(true);
    contactNameEdit->setText(contact.displayLabel().label());
    listWidget = new QListWidget(this);
    okButton = new QPushButton(tr("&Ok"), this);
    cancelButton = new QPushButton(tr("&Cancel"), this);

    layout->addRow(tr("Contact Name"), contactNameEdit);
    layout->addRow(listWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addRow(buttonLayout);

    setLayout(layout);

    repopulateGroupList();

    connect(okButton, SIGNAL(clicked()),
            this, SLOT(okButtonClicked()));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(cancelButtonClicked()));
    connect(listWidget, SIGNAL(itemChanged(QListWidgetItem *)),
            this, SLOT(groupItemChanged(QListWidgetItem *)));
}


void GroupDetailsDialog::repopulateGroupList()
{
    listWidget->clear();

    QList<QContactLocalId> grpList = cm->contacts(QString(QLatin1String(QContactType::TypeGroup)));
    for (int index=0; index < grpList.count(); index++){
        QContact grp = cm->contact(grpList[index]);
        QListWidgetItem *item = new QListWidgetItem(grp.displayLabel().label(), listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        bool foundInGroup = false;

        QList<QContactRelationship> thisGroupsRels = cm->relationships(grpList[index], QContactRelationship::HasMember);
        QContactId localContactUriOne = localContact.id();
        QContactId localContactUriTwo;
        localContactUriTwo.setLocalId(localContact.id().localId());
        foreach (const QContactRelationship& currRel, thisGroupsRels) {
            if (currRel.destinationContacts().contains(localContactUriOne) || currRel.destinationContacts().contains(localContactUriTwo)) {
                foundInGroup = true;
                break;
            }
        }
        if (foundInGroup)
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        // store the group ID as Qt::UserRole + 1
        item->setData(Qt::UserRole + 1, grpList[index]);
    }

}

QList<QContactLocalId> GroupDetailsDialog::groups()
{
    QContactId localContactUri = localContact.id();
    QList<QContactRelationship> relationships = cm->relationships(QContactRelationship::HasMember, localContactUri);
    QList<QContactLocalId> ret;
    foreach (const QContactRelationship& currRel, relationships) {
        if (currRel.destinationContacts().contains(localContactUri)) {
            ret.append(currRel.sourceContact());
        }
    }
    return ret;
}

void GroupDetailsDialog::groupItemChanged(QListWidgetItem * item)
{
    QContactLocalId id = item->data(Qt::UserRole + 1).toUInt();
    QContactId localContactUri = localContact.id();
    QList<QContactRelationship> relationships = cm->relationships(QContactRelationship::HasMember, localContactUri);
    QList<QContactLocalId> currentGroups;
    foreach (const QContactRelationship& currRel, relationships) {
        if (currRel.destinationContacts().contains(localContactUri)) {
            currentGroups.append(currRel.sourceContact());
        }
    }

    if (item->checkState() == Qt::Checked) {
        if (!currentGroups.contains(id)) {
            // add it to the group.
            QContactRelationship newRelationship = cm->relationship(id, QContactRelationship::HasMember);
            newRelationship.appendDestinationContact(localContactUri);
            cm->saveRelationship(&newRelationship);
        }
    } else {
        if (currentGroups.contains(id)) {
            // remove any membership in that group
            QContactRelationship currRel = cm->relationship(id, QContactRelationship::HasMember);
            currRel.removeDestinationContact(localContactUri);
            cm->saveRelationship(&currRel);
        }
    }
}

void GroupDetailsDialog::show()
{
    repopulateGroupList();
}


void GroupDetailsDialog::okButtonClicked()
{
    accept();
}


void GroupDetailsDialog::cancelButtonClicked()
{
    reject();
}
