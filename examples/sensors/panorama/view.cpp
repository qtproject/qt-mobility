#include "view.h"
#include "inputcontroller.h"
#include "keycontroller.h"
#include "accelerometercontroller.h"
#include "magnetometercontroller.h"
#include "rotationcontroller.h"
#include "orientationcontroller.h"
#include "compasscontroller.h"

int View::m_imageWidth;
int View::m_imageHeight;
QString View::m_currentSensor;

View::View(QGraphicsScene *scene) :
        QGraphicsView(scene), m_timer(this) ,m_delay(30){

    m_controller = 0;

    QPixmap bgPix(":/images/koskipuisto_pieni.jpg");
    m_pix = bgPix;
    m_imageWidth = bgPix.width();
    m_imageHeight = bgPix.height();
    int h= height();
    int y = qAbs(m_imageHeight-h)/2;
    setSceneRect(0, y, width(), h);

    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setBackgroundBrush(bgPix);
    setCacheMode(QGraphicsView::CacheBackground);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setWindowTitle("Panorama");

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
}

View::~View(){ if (m_controller) delete m_controller;}


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


void View::update(){
    if (m_mouseMode){
        m_x = checkX(m_x + m_dx);
        m_y = checkY(m_y+ m_dy);
    }
    else{
        if (m_controller){
            m_x = checkX(m_controller->getX());
            m_y = checkY(m_controller->getY());
        }
    }
    setSceneRect(m_x, m_y, width(), height());
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

    do{
        if (sensor==InputController::QACCELEROMETER){
            m_controller = new AccelerometerController();
            break;
        }
        if (sensor==InputController::QROTATIONSENSOR){
            m_controller = new RotationController();
            break;
        }
        if (sensor==InputController::QMAGNETOMETER){
            m_controller = new MagnetometerController();
            break;
        }
        if (sensor==InputController::QORIENTATIONSENSOR){
            m_controller = new OrientationController();
            break;
        }

        if (sensor==InputController::QTAPSENSOR){
            break;
        }
        if (sensor==InputController::QCOMPASS){
            m_controller = new CompassController();
            break;
        }
        if (sensor==InputController::QKEYS){
            m_controller = new KeyController();
            break;
        }

    }while (true);

    if (m_controller) m_controller->start();

}


int View::checkX(int x){
    return qAbs(x) < m_imageWidth ? x : x % m_imageWidth;

}

int View::checkY(int y){
    int limit = m_imageHeight-height();
    if (y<0){
        y = limit<0?-limit/2:0;
        if (m_controller) m_controller->setY(y);
        return y;
    }

    if (limit<0){
        y=-limit/2;
        if (m_controller) m_controller->setY(y);
        return y;
    }


    if (y< limit){ return y;}

    y = limit;
    if (m_controller) m_controller->setY(y);
    return y;
}



void View::mousePressEvent ( QMouseEvent* ){
    m_mousePressTime = QTime::currentTime();
    m_exController = m_controller;
    m_mouseMode = false;
}



void View::mouseMoveEvent(QMouseEvent* event){

    if (m_mouseMode){

        m_dx = m_eventX - event->x();
        m_dy = m_eventY - event->y();

        update();

        m_eventX = event->x();
        m_eventY = event->y();
        return;
    }

    if (m_mousePressTime.msecsTo(QTime::currentTime())>200){
        m_menu->setVisible(false);
        m_mouseMode = true;
        m_eventX = event->x();
        m_eventY = event->y();
        m_controller = 0;
    }

}



void View::mouseReleaseEvent(QMouseEvent* event){
    m_controller = m_exController;

    if (m_mouseMode){
        if (m_controller){
            m_controller->setX(m_x);
            m_controller->setY(m_y);
        }
    }
    m_mouseMode = false;

    if (m_mousePressTime.msecsTo(QTime::currentTime())>200) return;

    int x = event->x();
    int y = event->y();
    m_menu->move(x, y);
    m_menu->setVisible(true);

}
