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

#include "batterypublisher.h"
#include "ui_batterypublisher.h"

#include <qvaluespacepublisher.h>

#include <QTimer>

BatteryPublisher::BatteryPublisher(QWidget *parent)
:   QDialog(parent), ui(new Ui::BatteryPublisher), chargeTimer(0)
{
    ui->setupUi(this);

    publisher = new QValueSpacePublisher("/power/battery");

    connect(ui->batteryCharge, SIGNAL(valueChanged(int)),
            this, SLOT(chargeChanged(int)));
    connect(ui->charging, SIGNAL(toggled(bool)),
            this, SLOT(chargingToggled(bool)));

    chargeChanged(ui->batteryCharge->value());
}

BatteryPublisher::~BatteryPublisher()
{
    delete ui;
    delete publisher;
}

void BatteryPublisher::changeEvent(QEvent *e)
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

void BatteryPublisher::timerEvent(QTimerEvent *)
{
    int newCharge = ui->batteryCharge->value() + 1;
    ui->batteryCharge->setValue(newCharge);

    if (newCharge >= 100)
        ui->charging->setChecked(false);
}

void BatteryPublisher::chargeChanged(int newCharge)
{
    publisher->setAttribute("charge", newCharge);
}

void BatteryPublisher::chargingToggled(bool charging)
{
    ui->batteryCharge->setEnabled(!charging);
    publisher->setAttribute("charging", charging);

    if (charging)
        chargeTimer = startTimer(2000);
    else
        killTimer(chargeTimer);
}
