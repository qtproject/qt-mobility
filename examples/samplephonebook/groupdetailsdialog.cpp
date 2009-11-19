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

#include "groupdetailsdialog.h"

#include <QtGui>
#include "qcontact.h"
#include "qcontactmanager.h"

#include "qcontactrelationship.h"
#include "qcontacttype.h"
#include "qcontactdetailfilter.h"

GroupDetailsDialog::GroupDetailsDialog(QWidget *parent, QContactManager *contactManager, const QContact &contact)
        : QDialog(parent), cm(contactManager)
{
    Q_ASSERT(contactManager);
    setWindowTitle(tr("Group Details"));

    QFormLayout *layout = new QFormLayout;

qDebug() << "constructed group details dialog for contact:" << contact.displayLabel().label();
qDebug() << "   id =" << contact.id().managerUri() << ":" << contact.id().localId();

    localContact = contact;
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

    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));
    QList<QContactLocalId> grpList = cm->contacts(groupFilter);
    for (int index=0; index < grpList.count(); index++){
        QContact grp = cm->contact(grpList[index]);
        QListWidgetItem *item = new QListWidgetItem(grp.displayLabel().label(), listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        bool foundInGroup = false;

        QContactId groupId;
        groupId.setManagerUri(cm->managerUri());
        groupId.setLocalId(grpList[index]);
        QList<QContactRelationship> thisGroupsRels = cm->relationships(QContactRelationship::HasMember, groupId, QContactRelationshipFilter::First);
        foreach (const QContactRelationship& currRel, thisGroupsRels) {
            if (currRel.second() == localContact.id()) {
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
        if (currRel.second() == localContactUri) {
            ret.append(currRel.first().localId());
        }
    }
    return ret;
}

void GroupDetailsDialog::groupItemChanged(QListWidgetItem * item)
{
    QContactLocalId id = item->data(Qt::UserRole + 1).toUInt(); // id of the group.
    QContactId localContactUri = localContact.id();
    QContactId groupId;
    groupId.setManagerUri(cm->managerUri());
    groupId.setLocalId(id);
    QList<QContactRelationship> relationships = cm->relationships(QContactRelationship::HasMember, localContactUri, QContactRelationshipFilter::Second);
    QList<QContactLocalId> currentGroups;
    foreach (const QContactRelationship& currRel, relationships) {
        currentGroups.append(currRel.first().localId());
    }

qDebug() << "groups that localcontact is part of:";
foreach (const QContactLocalId& dbgid, currentGroups) qDebug() << dbgid;
    if (item->checkState() == Qt::Checked) {
qDebug() << "item is now checked!";
qDebug() << "already =" << currentGroups.contains(id);
        if (!currentGroups.contains(id)) {
qDebug() << "groupId =" << id;
qDebug() << "localContactId =" << localContactUri.managerUri() << ":" << localContactUri.localId();
            // add it to the group.
            QContactRelationship newRelationship;
            newRelationship.setFirst(groupId);
            newRelationship.setRelationshipType(QContactRelationship::HasMember);
            newRelationship.setSecond(localContactUri);
            bool success = cm->saveRelationship(&newRelationship);
qDebug() << "succeed adding?" << success;
        }
    } else {
qDebug() << "item is now unchecked!";
qDebug() << "already =" << currentGroups.contains(id);
        if (currentGroups.contains(id)) {
            // remove any membership in that group
            QContactRelationship currRel;
            currRel.setFirst(groupId);
            currRel.setRelationshipType(QContactRelationship::HasMember);
            currRel.setSecond(localContactUri);
            bool success = cm->removeRelationship(currRel);
qDebug() << "succeed removing?" << success;
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
