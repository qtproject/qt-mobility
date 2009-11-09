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

#include "consumerdialog.h"
#include "ui_consumerdialog.h"

#include <qvaluespacesubscriber.h>

#include <QDebug>

ConsumerDialog::ConsumerDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ConsumerDialog),
        subscriber(0)
{
    ui->setupUi(this);

    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(changeSubscriberPath()));
    changeSubscriberPath();
}

ConsumerDialog::~ConsumerDialog()
{
    delete ui;
}

void ConsumerDialog::changeEvent(QEvent *e)
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
void ConsumerDialog::changeSubscriberPath()
{
    ui->values->clearContents();

    if (!subscriber)
        subscriber = new QValueSpaceSubscriber(ui->basePath->text(), this);
    else
        subscriber->setPath(ui->basePath->text());

    connect(subscriber, SIGNAL(contentsChanged()), this, SLOT(subscriberChanged()));

    subscriberChanged();
}
//! [0]

//! [1]
void ConsumerDialog::subscriberChanged()
{
    ui->values->clearContents();

    QStringList subPaths = subscriber->subPaths();
    ui->values->setRowCount(subPaths.count());

    for (int i = 0; i < subPaths.count(); ++i) {
        QVariant v = subscriber->value(subPaths.at(i));

        QTableWidgetItem *pathItem = new QTableWidgetItem(subPaths.at(i));
        QTableWidgetItem *valueItem = new QTableWidgetItem(v.toString());
        QTableWidgetItem *typeItem = new QTableWidgetItem(v.typeName());
        ui->values->setItem(i, 0, pathItem);
        ui->values->setItem(i, 1, valueItem);
        ui->values->setItem(i, 2, typeItem);
    }
}
//! [1]
