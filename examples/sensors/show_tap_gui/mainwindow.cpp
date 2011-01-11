#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qlayout.h"
#include <QList>
#include <QTime>

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

    QTime now = QTime::currentTime();
    ui->doubleTime->setText(now.toString());
    ui->singleTime->setText(now.toString());

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
    QList<QWidget*> children;
    children.append(ui->doubleDirection);
    children.append(ui->singleDirection);
    children.append(ui->doubleCheckBox);
    children.append(ui->singleCheckBox);
    children.append(ui->doubleTime);
    children.append(ui->singleTime);
    int gain = 50;
    for (int i=0,l=children.size();i<l;i++){
        QWidget* w = children.at(i);
        int width = w->width();
        int height = w->height();
        w->setFixedHeight(height + gain);
        w->setFixedWidth(width+ gain);
    }
}
