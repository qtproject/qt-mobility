#include "controlpanel.h"

ControlPanel::ControlPanel(QWidget *parent) : QWidget(parent)
{
    QPushButton *quitButton = new QPushButton(tr("Quit"));
    quitButton->setFont(QFont("Times", 15, QFont::Bold));

    startButton = new QPushButton(tr("Start"));
    startButton->setFont(QFont("Times", 15, QFont::Bold));

    stopButton = new QPushButton(tr("Stop"));
    stopButton->setFont(QFont("Times", 15, QFont::Bold));

    satCB = new QCheckBox("Satellite sources");
    nonsatCB = new QCheckBox("Non-Satellite sources");
    satCB->setCheckState(Qt::Checked);
    nonsatCB->setCheckState(Qt::Checked);

    intervalLCD = new QLCDNumber(2);
    intervalLCD->setSegmentStyle(QLCDNumber::Filled);
    intervalLCD->setMinimumSize(80,30);

    latitudeLCD = new QLCDNumber(8);
    latitudeLCD->setSegmentStyle(QLCDNumber::Filled);
    latitudeLCD->setMinimumSize(120,70);
    latitudeLCD->setSmallDecimalPoint(true);

    longitudeLCD = new QLCDNumber(8);
    longitudeLCD->setSegmentStyle(QLCDNumber::Filled);
    longitudeLCD->setMinimumSize(120,70);
    longitudeLCD->setSmallDecimalPoint(true);

    altitudeLCD = new QLCDNumber(7);
    altitudeLCD->setSegmentStyle(QLCDNumber::Filled);
    altitudeLCD->setMinimumSize(120,70);
    altitudeLCD->setSmallDecimalPoint(true);

    QSlider *intervalSlider = new QSlider(Qt::Horizontal,this);
    intervalSlider->setRange(1, 60);

    QLabel *intervalLabel = new QLabel(this);
    intervalLabel->setText("Time Interval:");
    intervalLabel->setAlignment(Qt::AlignLeft);
    QLabel *latLabel = new QLabel(this);
    latLabel->setText("Latitude:");
    latLabel->setAlignment(Qt::AlignLeft);
    QLabel *lonLabel = new QLabel(this);
    lonLabel->setText("Longitude:");
    lonLabel->setAlignment(Qt::AlignLeft);
    QLabel *altLabel = new QLabel(this);
    altLabel->setText("Altitude:");
    altLabel->setAlignment(Qt::AlignLeft);

    
    connect(quitButton, SIGNAL(clicked()),  this, SIGNAL(quit()));
    connect(startButton, SIGNAL(clicked()), this, SLOT(startButtonCallback()));
    connect(stopButton, SIGNAL(clicked()),  this, SLOT(stopButtonCallback()));

    connect(intervalSlider, SIGNAL(valueChanged(int)),
            intervalLCD, SLOT(display(int)));
    connect(intervalSlider, SIGNAL(valueChanged(int)),
            this, SLOT(storeInterval(int)));
    connect(satCB,SIGNAL(stateChanged(int)), this, SLOT(satButton(int)));
    connect(nonsatCB,SIGNAL(stateChanged(int)), this, SLOT(nonSatButton(int)));
    

    QHBoxLayout *layoutSendStop = new QHBoxLayout;
    layoutSendStop->addWidget(startButton);
    layoutSendStop->addWidget(stopButton);
    layoutSendStop->addWidget(quitButton);

    QHBoxLayout *sourcesLayout = new QHBoxLayout;
    sourcesLayout->addWidget(satCB);
    sourcesLayout->addWidget(nonsatCB);

    QVBoxLayout *layout = new QVBoxLayout;
    //layout->addWidget(quitButton);
    layout->addLayout(sourcesLayout);
    layout->addWidget(intervalLabel);
    layout->addWidget(intervalLCD);
    layout->addWidget(intervalSlider);

    layout->addLayout(layoutSendStop);
    layout->addWidget(latLabel);
    layout->addWidget(latitudeLCD);
    layout->addWidget(lonLabel);
    layout->addWidget(longitudeLCD);
    layout->addWidget(altLabel);
    layout->addWidget(altitudeLCD);
    setLayout(layout);

    latitudeLCD->display("100.1");
    longitudeLCD->display("100.1");
    altitudeLCD->display("100.1");
    
    interval_ = 1;
    sat_ = true;
    nonSat_ = true;
    intervalSlider->setValue(1);
    intervalLCD->display("1");
    running = false;
    stopButton->setEnabled(false);

}

void ControlPanel::satButton(int state)
{
    sat_=(state!=0);
    if((nonSat_==false)&&(state==0)){
        nonsatCB->setCheckState(Qt::Checked);
        if(running) startButton->setEnabled(true);
    }
}

void ControlPanel::nonSatButton(int state)
{
    nonSat_=(state!=0);
    if((sat_==false)&&(state==0)){
        satCB->setCheckState(Qt::Checked);
        if(running) startButton->setEnabled(true);
    }
}


QString ControlPanel::format4(double x) const
{
    char buf[16];
    ::snprintf(buf,16,"%.6f",x);
    return QString::fromAscii(buf);
}

QString ControlPanel::format1(double x) const
{
    char buf[16];
    ::snprintf(buf,16,"%.1f",x);
    return QString::fromAscii(buf);
}

void ControlPanel::setCoordinate(const QGeoPositionInfo &update)
{
    QGeoCoordinate coord = update.coordinate();
    latitudeLCD->display(format4(coord.latitude()));
    longitudeLCD->display(format4(coord.longitude()));
    altitudeLCD->display(format1(coord.altitude()));
}

void ControlPanel::storeInterval(int i)
{
    interval_ = i;
    if(running) startButton->setEnabled(true);
}


void ControlPanel::startButtonCallback() 
{
    emit settings(sat_, nonSat_, interval_);
    if(running == false) {
        emit tracking(true);
    }
    running = true;
    startButton->setText(tr("Apply"));
    startButton->setEnabled(false);
    stopButton->setEnabled(true);
}

void ControlPanel::stopButtonCallback() 
{
    emit tracking(false);
    running = false;
    startButton->setText(tr("Start"));
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
}
