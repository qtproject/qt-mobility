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

GroupDetailsDialog::GroupDetailsDialog(QWidget *parent, QContactManager *contactManager, const QContact &contact)
        : QDialog(parent), cm(contactManager)
{
    Q_ASSERT(contactManager);
    setWindowTitle(tr("Group Details"));

    localContact.setGroups(contact.groups());

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
    QList<QUniqueId> grpList = cm->groups();
    for (int index=0; index < grpList.count(); index++){
        QContactGroup grp = cm->group(grpList[index]);
        QListWidgetItem *item = new QListWidgetItem(grp.name(), listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        if (localContact.groups().contains(grpList[index]))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        // store the group ID as Qt::UserRole + 1
        item->setData(Qt::UserRole + 1, grpList[index]);
    }

}

QList<QUniqueId> GroupDetailsDialog::groups()
{
    return localContact.groups();
}

void GroupDetailsDialog::groupItemChanged(QListWidgetItem * item)
{
    QUniqueId id = item->data(Qt::UserRole + 1).toUInt();
    QList<QUniqueId> currentGroups = localContact.groups();
    if (item->checkState() == Qt::Checked){
        if (!currentGroups.contains(id)){
            currentGroups.append(id);
            localContact.setGroups(currentGroups);
        }
    }else{
        if (currentGroups.contains(id)){
            currentGroups.removeOne(id);
            localContact.setGroups(currentGroups);
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
