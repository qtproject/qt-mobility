#ifndef _CONTROL_PANEL_
#define _CONTROL_PANEL_

#include <QApplication>
#include <QFont>
#include <QCheckBox>
#include <QLCDNumber>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include "qgeocoordinate.h"
#include "qgeopositioninfo.h"
#include <iostream>

class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    ControlPanel(QWidget *parent = 0);

private:
    QCheckBox *satCB;
    QCheckBox *nonsatCB;
    bool sat_;    
    bool nonSat_;
    int interval_;
    bool running;

    QPushButton *startButton;
    QPushButton *stopButton;
    QLCDNumber *intervalLCD;
    QLCDNumber *latitudeLCD;
    QLCDNumber *longitudeLCD;
    QLCDNumber *altitudeLCD;

    QString format1(double x) const;
    QString format4(double x) const;

signals:
    void tracking(bool b);
    void settings(bool sat, bool nonsat, int interval);
    void quit();

public slots:
    void satButton(int state);
    void nonSatButton(int state);
    void startButtonCallback();
    void stopButtonCallback();
    void storeInterval(int i);
    void setCoordinate(const QGeoPositionInfo &update);
};

#endif
