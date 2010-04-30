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
    setImageWidth(bgPix.width());
    setImageHeight(bgPix.height());
    int h= height();
    int y = qAbs(getImageHeight()-height())/2;
    setSceneRect(0, y, width(), h);

    setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Panorama"));
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setBackgroundBrush(bgPix);
    setCacheMode(QGraphicsView::CacheBackground);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setWindowTitle(tr("Panorama"));



    m_sensors.append(InputController::QACCELEROMETER);
    m_sensors.append(InputController::QORIENTATIONSENSOR);
    m_sensors.append(InputController::QMAGNETOMETER);
    m_sensors.append(InputController::QROTATIONSENSOR);
    m_sensors.append(InputController::QTAPSENSOR);
    m_sensors.append(InputController::QCOMPASS);
    m_sensors.append(InputController::QKEYS);

    createActions();
    handleAction(NULL,InputController::QACCELEROMETER);

    m_timer.setSingleShot(false);
    m_timer.start(m_delay);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
}

View::~View(){
    if (m_controller)
        delete m_controller;
}


void View::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}



void View::handleAction(QString oldSensor, QString newSensor){
    if (oldSensor==newSensor) return;


    QList<QAction*> tmpActions = actions();
    for (int i=0; i<tmpActions.length(); i++){
        if (tmpActions.at(i)->text() ==  oldSensor){
            tmpActions.at(i)->setEnabled(true);
            continue;
        }
        if (tmpActions.at(i)->text() == newSensor){
            tmpActions.at(i)->setEnabled(false);
        }
    }
    setWindowTitle("Panorama: "+ newSensor);
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
        addAction(tmp);
    }
    setContextMenuPolicy(Qt::ActionsContextMenu);

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
    int x = View::getController()->getX();
    int y = checkY(View::getController()->getY());
    qDebug()<<"update called x="<<x<<" y="<<y;
    setSceneRect(x, y, width(), height());
}


InputController* View::getController(){
    return m_controller;
}



void View::setImageWidth(int imageWidth){
    m_imageWidth = imageWidth;
}
int View::getImageWidth(){
    return m_imageWidth;
}


void View::setImageHeight(int imageHeight){
    m_imageHeight = imageHeight;
}

int View::getImageHeight(){
    return m_imageHeight;
}

void View::keyPressEvent(QKeyEvent *e)
{
    if (m_currentSensor!=InputController::QKEYS) return;

    getController()->keyPressEvent(e);

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

    m_controller->start();


}

int View::checkY(int y){
    int limit = View::getImageHeight()-height();
    if (y<0){
        y = limit<0?-limit/2:0;
        m_controller->setY(y);
        return y;
    }

    if (limit<0){
        y=-limit/2;
        m_controller->setY(y);
        return y;
    }


    if (y< limit){
        return y;
    }

    y = limit;
    m_controller->setY(y);
    return y;
}
