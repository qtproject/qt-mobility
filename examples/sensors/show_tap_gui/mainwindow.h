#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qtapsensor.h>
#include "tap.h"

QTM_USE_NAMESPACE

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void changeSingleState(int);
    void changeDoubleState(int);
    void singleHappened(QString);
    void doubleHappened(QString);

private:
    Ui::MainWindow *ui;
    QTapSensor* doublesensor;
    QTapSensor* singlesensor;
    TapSensorFilter* filter;
    void resize();
    static int m_singleCounter, m_doubleCounter;

};

#endif // MAINWINDOW_H
