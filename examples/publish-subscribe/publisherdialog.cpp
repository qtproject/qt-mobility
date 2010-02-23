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

#include "publisherdialog.h"
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include "ui_publisherdialog_hor.h"
#else
#include "ui_publisherdialog.h"
#endif

#include <qvaluespacepublisher.h>

#ifdef QTM_SMALL_SCREEN
#include <QPushButton>
#endif

#include <QDebug>

PublisherDialog::PublisherDialog(QWidget *parent)
:   QDialog(parent), ui(new Ui::PublisherDialog), publisher(0)
{
    ui->setupUi(this);

#ifdef QTM_SMALL_SCREEN
    QPushButton *switchButton =
        ui->buttonBox->addButton(tr("Switch"), QDialogButtonBox::ActionRole);
    connect(switchButton, SIGNAL(clicked()), this, SIGNAL(switchRequested()));
#endif

    //! [1]
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(createNewObject()));
    connect(ui->intValue, SIGNAL(valueChanged(int)), this, SLOT(intValueChanged(int)));
    connect(ui->unsetIntButton, SIGNAL(clicked()), this, SLOT(unsetIntValue()));
    connect(ui->setStringButton, SIGNAL(clicked()), this, SLOT(setStringValue()));
    connect(ui->setByteArrayButton, SIGNAL(clicked()), this, SLOT(setByteArrayValue()));
    //! [1]

    //! [3]
    createNewObject();
    //! [3]
}

PublisherDialog::~PublisherDialog()
{
    delete ui;
    delete publisher;
}

void PublisherDialog::changeEvent(QEvent *e)
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
void PublisherDialog::intValueChanged(int value)
{
    publisher->setValue("intValue", value);
}

void PublisherDialog::unsetIntValue()
{
    publisher->resetValue("intValue");
}

void PublisherDialog::setStringValue()
{
    publisher->setValue("stringValue", ui->stringValue->text());
}

void PublisherDialog::setByteArrayValue()
{
    publisher->setValue("byteArrayValue", ui->byteArrayValue->text().toAscii());
}

//! [0]

//! [2]
void PublisherDialog::createNewObject()
{
    if (publisher)
        delete publisher;

    publisher = new QValueSpacePublisher(ui->basePath->text());
    intValueChanged(ui->intValue->value());
    setStringValue();
    setByteArrayValue();
}
//! [2]
