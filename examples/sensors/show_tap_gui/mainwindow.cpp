/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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
#include "mainwindow.h"

int MainWindow::m_doubleCounter = 0;
int MainWindow::m_singleCounter = 0;

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->singleCheckBox->toggle();
    ui->doubleCheckBox->toggle();

    ui->doubleDirection->setText("");
    ui->singleDirection->setText("");

    ui->doubleTime->setText("");
    ui->singleTime->setText("");

    doublesensor = new QTapSensor();
    doublesensor->setProperty("returnDoubleTapEvents", true);
    filter = new TapSensorFilter();
    doublesensor->addFilter(filter);
    doublesensor->start();
    if (!doublesensor->isActive()) {
        qWarning("Tapsensor (double) didn't start!");
    }
    singlesensor = new QTapSensor();
    singlesensor->setProperty("returnDoubleTapEvents", false);
    singlesensor->addFilter(filter);
    singlesensor->start();
    if (!singlesensor->isActive()) {
        qWarning("Tapsensor (single) didn't start!");
    }

    connect(ui->singleCheckBox, SIGNAL(stateChanged(int)), this, SLOT(changeSingleState(int)));
    connect(ui->doubleCheckBox, SIGNAL(stateChanged(int)), this, SLOT(changeDoubleState(int)));
    connect(filter, SIGNAL(singleHappened(QString)), this, SLOT(singleHappened(QString)));
    connect(filter, SIGNAL(doubleHappened(QString)), this, SLOT(doubleHappened(QString)));

    resize();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete doublesensor;
    delete singlesensor;
    delete filter;
}

void MainWindow::changeSingleState(int state){
    if (state==0) singlesensor->stop();
    else singlesensor->start();
}

void MainWindow::changeDoubleState(int state){
    if (state==0) doublesensor->stop();
    else doublesensor->start();
}

void MainWindow::singleHappened(QString direction){
    ui->singleLCD->display(++m_singleCounter);
    ui->singleDirection->setText(direction);
    ui->singleTime->setText(QTime::currentTime().toString());
}

void MainWindow::doubleHappened(QString direction){
    ui->doubleLCD->display(++m_doubleCounter);
    ui->doubleDirection->setText(direction);
    ui->doubleTime->setText(QTime::currentTime().toString());
}

void MainWindow::resize(){
    QList<QWidget*> widgets;
    widgets.append(ui->doubleDirection);
    widgets.append(ui->singleDirection);
    widgets.append(ui->doubleCheckBox);
    widgets.append(ui->singleCheckBox);
    widgets.append(ui->doubleTime);
    widgets.append(ui->singleTime);
    widgets.append(ui->doublePcs);
    widgets.append(ui->singlePcs);
    widgets.append(ui->lastDouble);
    widgets.append(ui->lastSingle);
    widgets.append(ui->horizontalLayoutWidget);
    widgets.append(ui->horizontalLayoutWidget_2);
    int gain = 50;
    for (int i=0,l=widgets.size();i<l;i++){
        QWidget* w = widgets.at(i);
        int width = w->width();
        int height = w->height();
        w->setFixedHeight(height + gain);
        w->setFixedWidth(width+ gain);
    }
}
