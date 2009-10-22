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

#include "finddialog.h"
#include "../incomingcalls/filterdialog.h"
#include "../incomingcalls/filterdialog.cpp"
#include <QtGui>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent), simpleFilter(true)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    filterDialog = new FilterDialog(this);

    findLabel = new QLabel(tr("Contact name:"));
    lineEdit = new QLineEdit;

    findButton = new QPushButton(tr("&Find"));
    findText = "";

    filterButton = new QPushButton(tr("Show Filter Details"));

    QHBoxLayout *simpleFindLayout = new QHBoxLayout;
    simpleFindLayout->addWidget(findLabel);
    simpleFindLayout->addWidget(lineEdit);
    simpleFindLayout->addStretch(1);
    simpleFindLayout->addWidget(findButton);
    simpleFindLayout->addStretch(1);


    QHBoxLayout *simpleFilterLayout = new QHBoxLayout;
    simpleFilterLayout->addStretch(1);
    simpleFilterLayout->addWidget(filterButton);
    simpleFilterLayout->addStretch(1);
    layout->addLayout(simpleFindLayout);
    layout->addLayout(simpleFilterLayout);
    layout->addWidget(filterDialog);

    setLayout(layout);
    setWindowTitle(tr("Find a Contact"));
    connect(findButton, SIGNAL(clicked()), this, SLOT(findClicked()));
    connect(findButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(filterButton, SIGNAL(clicked()), this, SLOT(filterClicked()));
    connect(filterDialog, SIGNAL(hidden()), this, SLOT(filterHidden()));

    lineEdit->setFocus();
}

void FindDialog::findClicked()
{
    QString text = lineEdit->text();

    if (text.isEmpty()) {
        QMessageBox::information(this, tr("Empty Field"),
            tr("Please enter a name."));
        return;
    } else {
        findText = text;
        lineEdit->clear();
        hide();
    }
}

void FindDialog::filterClicked()
{
    simpleFilter = false;
    lineEdit->hide();
    findButton->hide();
    findLabel->hide();
    filterButton->hide();
	filterDialog->showDialog();
}

void FindDialog::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);
    simpleFilter = true;
    filterButton->show();
    filterDialog->hide();
    lineEdit->show();
    findButton->show();
    findLabel->show();
    lineEdit->setFocus();
}

void FindDialog::filterHidden()
{
    // was the dialog accepted?
    if (filterDialog->status() == 1) {
        accept();
    }else{
        reject();
    }
}

bool FindDialog::isSimpleFilterEnabled() const
{
    return simpleFilter;
}

QString FindDialog::getFindText() const
{
    return findText;
}

QContactFilter FindDialog::getFindFilter() const
{
    return filterDialog->filter();
}
