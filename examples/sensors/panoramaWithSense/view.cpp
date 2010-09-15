#include "view.h"
#include "accelerometercontroller.h"
#include "magnetometercontroller.h"
#include "rotationcontroller.h"
#include "orientationcontroller.h"
#include "compasscontroller.h"
#include "tapcontroller.h"
#include <QDebug>
#include <iostream>



int View::m_imageWidth;
int View::m_imageHeight;
QString View::m_currentSensor;
bool View::m_isStarted = true;



View::View(QGraphicsScene *scene) :
        QGraphicsView(scene), m_timer(this) ,m_delay(30){

    qDebug()<<"in View now";

    QPixmap bgPix(":/images/koskipuisto_pieni.jpg");
    m_pix = bgPix;

    setupWindow();

    int h = height();
    int y = qAbs(m_imageHeight-h)/2;
    setSceneRect(0, y, width(), h);


    m_sensors.append(InputController::QACCELEROMETER);
    m_sensors.append(InputController::QORIENTATIONSENSOR);
    m_sensors.append(InputController::QMAGNETOMETER);
    m_sensors.append(InputController::QROTATIONSENSOR);
    m_sensors.append(InputController::QTAPSENSOR);
    m_sensors.append(InputController::QCOMPASS);
    m_sensors.append(InputController::QKEYS);


    m_menu = new QMenu(this);
    createActions();
    handleAction(NULL,InputController::QACCELEROMETER);

    m_timer.setSingleShot(false);
    m_timer.start(m_delay);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(this,SIGNAL(sceneRectChanged(QRectF)), this, SLOT(checkSensors(QRectF)));

}


View::~View(){
    m_timer.stop();
    if (m_controller) delete m_controller;
    if (m_exController) delete m_exController;
}


void View::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}



void View::handleAction(QString oldSensor, QString newSensor){

    m_menu->setVisible(false);

    if (oldSensor==newSensor) return;


    QList<QAction*> tmpActions = m_menu->actions();
    for (int i=0; i<tmpActions.length(); i++){
        if (tmpActions.at(i)->text() ==  oldSensor){
            tmpActions.at(i)->setEnabled(true);
            continue;
        }
        if (tmpActions.at(i)->text() == newSensor){
            tmpActions.at(i)->setEnabled(false);
        }
    }
    m_currentSensor = newSensor;
    switchController(m_currentSensor);

}



void View::createActions()
{

    for (int i=0; i<m_sensors.length();i++){
        QAction* tmp = new QAction(m_sensors.at(i), this);

        const QString sensor = m_sensors.at(i);
        do{
            if (sensor==InputController::QACCELEROMETER){
                connect(tmp, SIGNAL(triggered()), this, SLOT(startAccelerometer()));
                break;
            }
            if (sensor==InputController::QORIENTATIONSENSOR){
                connect(tmp, SIGNAL(triggered()), this, SLOT(startOrientationSensor()));
                break;
            }
            if (sensor==InputController::QROTATIONSENSOR){
                connect(tmp, SIGNAL(triggered()), this, SLOT(startRotationSensor()));
                break;
            }
            if (sensor==InputController::QMAGNETOMETER){
                connect(tmp, SIGNAL(triggered()), this, SLOT(startMagnetometer()));
                break;
            }
            if (sensor==InputController::QTAPSENSOR){
                connect(tmp, SIGNAL(triggered()), this, SLOT(startTapSensor()));
                break;
            }
            if (sensor==InputController::QCOMPASS){
                connect(tmp, SIGNAL(triggered()), this, SLOT(startCompass()));
                break;
            }
            if (sensor==InputController::QKEYS){
                connect(tmp, SIGNAL(triggered()), this, SLOT(startKeys()));
                break;
            }
        }while (true);
        m_menu->addAction(tmp);
    }

}

void View::startAccelerometer(){
    handleAction(m_currentSensor, InputController::QACCELEROMETER);
}

void View::startOrientationSensor(){
    handleAction(m_currentSensor, InputController::QORIENTATIONSENSOR);
}

void View::startTapSensor(){
    handleAction(m_currentSensor, InputController::QTAPSENSOR);
}

void View::startMagnetometer(){
    handleAction(m_currentSensor, InputController::QMAGNETOMETER);
}

void View::startRotationSensor(){
    handleAction(m_currentSensor, InputController::QROTATIONSENSOR);
}

void View::startKeys(){
    handleAction(m_currentSensor, InputController::QKEYS);
}

void View::startCompass(){
    handleAction(m_currentSensor, InputController::QCOMPASS);
}




void View::keyPressEvent(QKeyEvent *e)
{
    if (m_currentSensor!=InputController::QKEYS) return;

    if (m_controller) m_controller->keyPressEvent(e);
}


void View::switchController(QString sensor){

    if (m_controller){
        delete m_controller;
        m_controller =0;
    }

    if (sensor==InputController::QACCELEROMETER){
        m_controller = new AccelerometerController();
        return;
    }
    if (sensor==InputController::QROTATIONSENSOR){
        m_controller = new RotationController();
        return;
    }
    if (sensor==InputController::QMAGNETOMETER){
        m_controller = new MagnetometerController();
        return;
    }
    if (sensor==InputController::QORIENTATIONSENSOR){
        m_controller = new OrientationController();
        return;
    }
    if (sensor==InputController::QTAPSENSOR){
        m_controller = new TapController();
        return;
    }
    if (sensor==InputController::QCOMPASS){
        m_controller = new CompassController();
        return;
    }
    if (sensor==InputController::QKEYS){
        m_controller = new KeyController();
        return;
    }



}


int View::checkX(int x){
    int tmpX = qAbs(x) < m_imageWidth ? x : x % m_imageWidth;
    m_mouseMode?m_exController->setX(tmpX):m_controller->setX(tmpX);
    return tmpX;

}

int View::checkY(int y){
    int limit = m_imageHeight-height();
    if (y<0){
        y = limit<0?-limit/2:0;
    }
    else if (limit<0){
        y=-limit/2;
    }
    else if (y > limit){y=limit;}

    m_mouseMode?m_exController->setY(y):m_controller->setY(y);
    return y;
}



void View::mousePressEvent ( QMouseEvent* ){
    m_mousePressTime = QTime::currentTime();
    m_exController = m_controller;
    m_mouseMode = false;
}


void View::mouseMoveEvent(QMouseEvent* event){

    // first time
    if (!m_mouseMode){
        m_mouseMode = true;
        m_eventX = event->x();
        m_eventY = event->y();
        return;
    }

    if (m_menu->isVisible()){
        if (m_mousePressTime.msecsTo(QTime::currentTime())>300)
            m_menu->setVisible(false);
    }

    int cur_x = event->x();
    int cur_y = event->y();

    m_dx = m_eventX - cur_x;
    m_dy = m_eventY - cur_y;

    m_eventX = cur_x;
    m_eventY = cur_y;

    update();

}



void View::mouseReleaseEvent(QMouseEvent* event){
    m_controller = m_exController;
    m_exController = 0;
    m_mouseMode = false;


    if (m_menu->isVisible()){
        m_menu->setVisible(false);
        return;
    }

    // slow press -> move
    if (m_mousePressTime.msecsTo(QTime::currentTime())>300) return;

    // quick press -> show menu
    int x = event->x();
    int y = event->y();
    m_menu->move(x, y);
    m_menu->setVisible(true);

}


void View::update(){

    if (!m_isStarted) return;

    if (!m_mouseMode && m_controller)
        m_controller->updateCoordinates();

    if (m_menu->isVisible()){
        if (m_mousePressTime.msecsTo(QTime::currentTime())>5000)
            m_menu->setVisible(false);
    }


    int x = m_mouseMode?m_exController->getX():m_controller->getX();
    int y = m_mouseMode?m_exController->getY():m_controller->getY();

    x = checkX(m_mouseMode?x+m_dx:x);
    y = checkY(m_mouseMode?y+m_dy:y);

    setSceneRect(x, y, width(), height());
}




void View::setupWindow(){
    int w = m_pix.width();
    int h = m_pix.height();

    if (m_imageWidth>0 && m_imageHeight > 0){
        int x = m_controller? m_controller->getX(): (m_exController?m_exController->getX():0);
        int y = m_controller? m_controller->getY(): (m_exController?m_exController->getY():0);

        checkX(x*w/m_imageWidth);
        checkY(y*h/m_imageHeight);
    }


    m_imageWidth = w;
    m_imageHeight = h;


    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setBackgroundBrush(m_pix);
    setCacheMode(QGraphicsView::CacheBackground);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setWindowTitle("PanoramaWithSense");

}


 void View::focusInEvent(QFocusEvent*) {
    if (m_controller) m_controller->doStart();
    m_isStarted = true;
}

 void View::focusOutEvent(QFocusEvent*) {
    if (m_controller) m_controller->doStop();
    m_isStarted = false;
}


