#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <QString>
#include <QWidget>


class InputController : public QObject
{
//    Q_OBJECT
public:

    InputController();
    void start();
    void stop();
    virtual void startTimer();
    virtual void stopTimer();
    virtual void startSensor();
    virtual void stopSensor();
    int getX(), getY();
    void setX(int x), setY(int y);

    virtual void keyPressEvent(QKeyEvent *e);

    static const QString QACCELEROMETER,
    QORIENTATIONSENSOR, QROTATIONSENSOR,
    QMAGNETOMETER, QTAPSENSOR, QCOMPASS, QKEYS;


protected:
    //current coordinates
    int m_x,m_y;




};


#endif // INPUTCONTROLLER_H
