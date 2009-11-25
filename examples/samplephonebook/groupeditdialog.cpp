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

#include "groupeditdialog.h"

#include <QtGui>
#include "qcontact.h"
#include "qcontactmanager.h"
#include "qcontactdetailfilter.h"
#include "qcontactname.h"

GroupEditDialog::GroupEditDialog(QWidget *parent, QContactManager *contactManager)
        : QDialog(parent), cm(contactManager)
{
    Q_ASSERT(contactManager);
    setWindowTitle(tr("Groups Edit"));

    QFormLayout *layout = new QFormLayout;

    groupNameEdit = new QLineEdit(this);
    listWidget = new QListWidget(this);
    addButton = new QPushButton(tr("Add"), this);
    saveButton = new QPushButton(tr("Save"), this);
    deleteButton = new QPushButton(tr("Delete"), this);
    doneButton = new QPushButton(tr("Done"), this);


    layout->addRow(tr("Group Name"), groupNameEdit);
    layout->addRow(listWidget);

    QHBoxLayout *row1Layout = new QHBoxLayout;
    row1Layout->addWidget(addButton);
    row1Layout->addWidget(saveButton);
    row1Layout->addWidget(deleteButton);
    layout->addRow(row1Layout);

    QHBoxLayout *row2Layout = new QHBoxLayout;
    row2Layout->addStretch(1);
    row2Layout->addWidget(doneButton);
    layout->addRow(row2Layout);

    setLayout(layout);

    repopulateGroupList();

    connect(addButton, SIGNAL(clicked()),
            this, SLOT(addButtonClicked()));
    connect(deleteButton, SIGNAL(clicked()),
            this, SLOT(deleteButtonClicked()));
    connect(saveButton, SIGNAL(clicked()),
            this, SLOT(saveButtonClicked()));
    connect(doneButton, SIGNAL(clicked()),
            this, SLOT(doneButtonClicked()));
    connect(listWidget, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(groupItemActivated(QListWidgetItem *)));
    connect(listWidget, SIGNAL(currentItemChanged (QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(groupItemChanged (QListWidgetItem *, QListWidgetItem *)));
}


void GroupEditDialog::repopulateGroupList()
{
    listWidget->clear();
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));
    QList<QContactLocalId> grpList = cm->contacts(groupFilter);
    for (int index=0; index < grpList.count(); index++){
        QContact grp = cm->contact(grpList[index]);
        QListWidgetItem *item = new QListWidgetItem(grp.displayLabel(), listWidget);
        // store the group ID as Qt::UserRole + 1
        item->setData(Qt::UserRole + 1, grpList[index]);
    }

}

void GroupEditDialog::show()
{
    repopulateGroupList();
}

void GroupEditDialog::addButtonClicked()
{
    if (!groupNameEdit->text().isEmpty()){
        QContact grp;
        grp.setType(QContactType::TypeGroup);
        bool result;
        QContactName groupName;
        groupName.setCustomLabel(groupNameEdit->text());
        grp.saveDetail(&groupName);
        result = cm->saveContact(&grp);

        if (!result){
            QMessageBox::information(this, tr("Group Add"),
                tr("Failed to add Group '%1'.").arg(grp.displayLabel()));
        }else{
            repopulateGroupList();
            // select the added group name
            for (int index = 0; index < listWidget->count();index++){
                if (listWidget->item(index)->data(Qt::UserRole + 1).toUInt() == grp.id().localId()){
                    listWidget->setCurrentItem(listWidget->item(index));
                    break;
                }
            }
        }

    }else{
        QMessageBox::information(this, tr("Empty Field"),
                tr("Please enter a name."));
        groupNameEdit->setFocus();
    }
}

void GroupEditDialog::saveButtonClicked()
{
    if (listWidget->currentItem()) {
        QContactLocalId grpID = listWidget->currentItem()->data(Qt::UserRole + 1).toUInt();
        QContact grp = cm->contact(grpID);
        bool result = false;
        if (!grp.isEmpty()) {
            QContactName groupName = grp.detail<QContactName>();
            groupName.setCustomLabel(groupNameEdit->text());
            grp.saveDetail(&groupName);
            result = cm->saveContact(&grp);
        }
        if (!result) {
            QMessageBox::information(this, tr("Save Group"),
                    tr("Failed to save Group '%1'.").arg(listWidget->currentItem()->text()));
        }else {
            repopulateGroupList();
            // select the group name again
            for (int index = 0; index < listWidget->count();index++){
                if (listWidget->item(index)->data(Qt::UserRole + 1).toUInt() == grp.id().localId()){
                    listWidget->setCurrentItem(listWidget->item(index));
                    break;
                }
            }
        }
    }
}

void GroupEditDialog::deleteButtonClicked()
{
    if (listWidget->currentItem()){
        int lastCurrentRow = listWidget->currentRow();
        QContactLocalId grpID = listWidget->currentItem()->data(Qt::UserRole + 1).toUInt();
        if (!cm->removeContact(grpID)){
            QMessageBox::information(this, tr("Remove Group"),
                tr("Failed to removed Group '%1'.").arg(listWidget->currentItem()->text()));
        }else{
            repopulateGroupList();
            if (lastCurrentRow < listWidget->count()){
                listWidget->setCurrentRow(lastCurrentRow);
            }else {
                if (listWidget->count())
                    listWidget->setCurrentRow(listWidget->count() - 1);
            }
        }
    }
}

void GroupEditDialog::doneButtonClicked()
{
    accept();
}

void GroupEditDialog::groupItemActivated(QListWidgetItem * item)
{
    if (item)
        groupNameEdit->setText(listWidget->currentItem()->text());
    else
        groupNameEdit->clear();
}

void GroupEditDialog::groupItemChanged ( QListWidgetItem * current, QListWidgetItem * /*previous*/ )
{
    groupItemActivated(current);
}
