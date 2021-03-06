/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "publishsubscribemainwindow.h"
#include "ui_publishsubscribemainwindow.h"

#include <qvaluespacesubscriber.h>

PublishSubscribeMainWindow::PublishSubscribeMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PublishSubscribeMainWindow)
{
    ui->setupUi(this);
    batterySubscriber = new QValueSpaceSubscriber("/resources/battery", this);
    connect(batterySubscriber, SIGNAL(contentsChanged()), this, SLOT(subscriberChanged()));

    chargingSubscriber = new QValueSpaceSubscriber("/resources/charging", this);
    connect(chargingSubscriber, SIGNAL(contentsChanged()), this, SLOT(subscriberChanged()));

    profileSubscriber = new QValueSpaceSubscriber("/profile", this);
    connect(profileSubscriber, SIGNAL(contentsChanged()), this, SLOT(subscriberChanged()));

    subscriberChanged();
}

PublishSubscribeMainWindow::~PublishSubscribeMainWindow()
{
    delete ui;
}

void PublishSubscribeMainWindow::subscriberChanged()
{
    QVariant batteryLevel = batterySubscriber->value("level");
    if (!batteryLevel.isNull()) {
        ui->labelBatteryLevel->setText(batteryLevel.toString());
    } else {
        ui->labelBatteryLevel->setText(tr("value not found"));
    }

    QVariant batteryStatus = batterySubscriber->value("status");
    if (!batteryStatus.isNull()) {
        switch (batteryStatus.toInt()) {
        case 0: ui->labelBatteryStatus->setText(tr("ok")); break;
        case 1: ui->labelBatteryStatus->setText(tr("low")); break;
        case 2: ui->labelBatteryStatus->setText(tr("empty")); break;
        default:
            ui->labelBatteryStatus->setText(tr("unknown"));
            break;
        }
    } else {
        ui->labelBatteryStatus->setText(tr("value not found"));
    }

    QVariant chargingStatus = chargingSubscriber->value("status");
    if (!chargingStatus.isNull()) {
        switch (chargingSubscriber->value("status").toInt()) {
        case -1: ui->labelChargingStatus->setText(tr("error")); break;
        case 0: ui->labelChargingStatus->setText(tr("not connected")); break;
        case 1: ui->labelChargingStatus->setText(tr("charging")); break;
        case 2: ui->labelChargingStatus->setText(tr("not charging")); break;
        case 3: ui->labelChargingStatus->setText(tr("almost complete")); break;
        case 4: ui->labelChargingStatus->setText(tr("charging complete")); break;
        case 5: ui->labelChargingStatus->setText(tr("charging continued")); break;
        default:
            ui->labelChargingStatus->setText(tr("unknown"));
            break;
        }
    } else {
        ui->labelChargingStatus->setText(tr("value not found"));
    }

    QVariant profileId = profileSubscriber->value("id");
    if (!profileId.isNull()) {
        switch (profileId.toInt()) {
        case 0: ui->labelCurrentProfile->setText(tr("General profile")); break;
        case 1: ui->labelCurrentProfile->setText(tr("Silent profile")); break;
        case 2: ui->labelCurrentProfile->setText(tr("Meeting profile")); break;
        case 3: ui->labelCurrentProfile->setText(tr("Outdoor profile")); break;
        case 4: ui->labelCurrentProfile->setText(tr("Pager profile")); break;
        case 5: ui->labelCurrentProfile->setText(tr("Off-line profile")); break;
        case 6: ui->labelCurrentProfile->setText(tr("Drive profile")); break;
        default:
            if (profileId.toInt() >= 30 && profileId.toInt() <= 49) {
                ui->labelCurrentProfile->setText(tr("User-created profile"));
            } else {
                ui->labelCurrentProfile->setText(tr("unknown"));
            }
            break;
        }
    } else {
        ui->labelCurrentProfile->setText(tr("value not found"));
    }
}
