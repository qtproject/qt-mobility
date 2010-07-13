/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "dialog.h"

#if defined(Q_WS_MAEMO_5)
# include "dialer_maemo.h"
#else
# ifdef Q_OS_LINUX
#  include "linux/dialer.h"
# endif
# ifdef Q_OS_WIN
#  include "dialer_win.h"
# endif
# ifdef Q_OS_SYMBIAN
#  include "dialer_symbian.h"
# endif
#endif


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    dialer(0)
{
    dialer = new Dialer;

    ui.setupUi(this);
    this->setWindowTitle("Dialer");

    connect(ui.pushButtonDial, SIGNAL(released()), SLOT(dial()));
    connect(ui.pushButton0, SIGNAL(released()), SLOT(button0()));
    connect(ui.pushButton1, SIGNAL(released()), SLOT(button1()));
    connect(ui.pushButton2, SIGNAL(released()), SLOT(button2()));
    connect(ui.pushButton3, SIGNAL(released()), SLOT(button3()));
    connect(ui.pushButton4, SIGNAL(released()), SLOT(button4()));
    connect(ui.pushButton5, SIGNAL(released()), SLOT(button5()));
    connect(ui.pushButton6, SIGNAL(released()), SLOT(button6()));
    connect(ui.pushButton7, SIGNAL(released()), SLOT(button7()));
    connect(ui.pushButton8, SIGNAL(released()), SLOT(button8()));
    connect(ui.pushButton9, SIGNAL(released()), SLOT(button9()));
    setTabOrder(ui.lineEditPhoneNumber, ui.pushButton1);
    setTabOrder(ui.pushButton1, ui.pushButton2);
    setTabOrder(ui.pushButton2, ui.pushButton3);
    setTabOrder(ui.pushButton3, ui.pushButton4);
    setTabOrder(ui.pushButton4, ui.pushButton5);
    setTabOrder(ui.pushButton5, ui.pushButton6);
    setTabOrder(ui.pushButton6, ui.pushButton7);
    setTabOrder(ui.pushButton7, ui.pushButton8);
    setTabOrder(ui.pushButton8, ui.pushButton9);
    setTabOrder(ui.pushButton9, ui.pushButton0);
    setTabOrder(ui.pushButton0, ui.pushButtonDial);
}

Dialog::~Dialog()
{
    if(dialer) delete dialer;
}

void Dialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui.retranslateUi(this);
        break;
    default:
        break;
    }
}



void Dialog::dial()
{
    dialer->dial(ui.lineEditPhoneNumber->text());
    ui.lineEditPhoneNumber->clear();
}

void Dialog::addNumber(int val)
{
    QString txtval = ui.lineEditPhoneNumber->text();
    txtval += QString().setNum(val, 10);
    ui.lineEditPhoneNumber->setText(txtval);
}


