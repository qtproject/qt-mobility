#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <QKeyEvent>
#include <qsensor.h>

QTM_USE_NAMESPACE

class InputController : public QObject
{

public:

    InputController();
    virtual ~InputController(){};
    virtual void updateCoordinates();
    virtual void keyPressEvent(QKeyEvent *e);

    static int getX(), getY();
    static void setX(int x), setY(int y);


    static const QString QACCELEROMETER,
    QORIENTATIONSENSOR, QROTATIONSENSOR,
    QMAGNETOMETER, QTAPSENSOR, QCOMPASS, QKEYS;

public slots:
    virtual void doStart();
    virtual void doStop();

protected:
    //current coordinates
    static int m_x,m_y;
    void setDataRate(QSensor* sensor);




};


#endif // INPUTCONTROLLER_H
