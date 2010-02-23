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

#include "subscriberdialog.h"
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include "ui_subscriberdialog_hor.h"
#else
#include "ui_subscriberdialog.h"
#endif

#include <qvaluespacesubscriber.h>

#include <QTableWidget>
#include <QListWidget>
#include <QDesktopWidget>

#ifdef QTM_SMALL_SCREEN
#include <QPushButton>
#include <QSizePolicy>
#endif

#include <QDebug>

SubscriberDialog::SubscriberDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SubscriberDialog),
        subscriber(0),
        tableWidget(0), listWidget(0)
{
    ui->setupUi(this);

#ifdef QTM_SMALL_SCREEN
    QPushButton *switchButton =
        ui->buttonBox->addButton(tr("Switch"), QDialogButtonBox::ActionRole);
    connect(switchButton, SIGNAL(clicked()), this, SIGNAL(switchRequested()));
#endif

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    tableWidget = ui->tableWidget;
    QStringList headerLabels;
    headerLabels << tr("Key") << tr("Value") << tr("Type");
    tableWidget->setColumnCount(3);
    tableWidget->setHorizontalHeaderLabels(headerLabels);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setColumnWidth(0, 200);
    tableWidget->setColumnWidth(1, 400);
#else
    QDesktopWidget desktopWidget;
    if (desktopWidget.availableGeometry().width() < 400) {
        // Screen is too small to fit a table widget without scrolling, use a list widget instead.
        listWidget = new QListWidget;
        listWidget->setAlternatingRowColors(true);
        ui->verticalLayout->insertWidget(2, listWidget);
    } else {
        tableWidget = new QTableWidget;
        QStringList headerLabels;
        headerLabels << tr("Key") << tr("Value") << tr("Type");
        tableWidget->setColumnCount(3);
        tableWidget->setHorizontalHeaderLabels(headerLabels);
        tableWidget->horizontalHeader()->setStretchLastSection(true);
        tableWidget->verticalHeader()->setVisible(false);

        ui->verticalLayout->insertWidget(2, tableWidget);
    }
#endif
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(changeSubscriberPath()));
    changeSubscriberPath();
}

SubscriberDialog::~SubscriberDialog()
{
    delete ui;
}

void SubscriberDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

//! [0]
void SubscriberDialog::changeSubscriberPath()
{
    if (listWidget)
        listWidget->clear();
    else if (tableWidget)
        tableWidget->clearContents();

    if (!subscriber)
        subscriber = new QValueSpaceSubscriber(ui->basePath->text(), this);
    else
        subscriber->setPath(ui->basePath->text());

    connect(subscriber, SIGNAL(contentsChanged()), this, SLOT(subscriberChanged()));

    subscriberChanged();
}
//! [0]

//! [1]
void SubscriberDialog::subscriberChanged()
{
    QStringList subPaths = subscriber->subPaths();

    if (listWidget) {
        listWidget->clear();
    } else if (tableWidget) {
        tableWidget->clearContents();
        tableWidget->setRowCount(subPaths.count());
    }

    for (int i = 0; i < subPaths.count(); ++i) {
        QVariant v = subscriber->value(subPaths.at(i));

        if (listWidget) {
            const QString itemTemplate("%1 (%2)\n%3");

            QListWidgetItem *item =
                new QListWidgetItem(itemTemplate.arg(subPaths.at(i), v.typeName(), v.toString()));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            listWidget->addItem(item);
        } else if (tableWidget) {
            QTableWidgetItem *pathItem = new QTableWidgetItem(subPaths.at(i));
            pathItem->setFlags(pathItem->flags() & ~Qt::ItemIsEditable);
            QTableWidgetItem *valueItem = new QTableWidgetItem(v.toString());
            valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
            QTableWidgetItem *typeItem = new QTableWidgetItem(v.typeName());
            typeItem->setFlags(typeItem->flags() & ~Qt::ItemIsEditable);

            tableWidget->setItem(i, 0, pathItem);
            tableWidget->setItem(i, 1, valueItem);
            tableWidget->setItem(i, 2, typeItem);
        }
    }
}
//! [1]
