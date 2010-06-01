#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <QString>
#include <QWidget>


class InputController : public QObject
{
//    Q_OBJECT
public:

    InputController();
    virtual ~InputController(){};
    virtual void updateCoordinates();

    static int getX(), getY();
    static void setX(int x), setY(int y);

    virtual void keyPressEvent(QKeyEvent *e);

    static const QString QACCELEROMETER,
    QORIENTATIONSENSOR, QROTATIONSENSOR,
    QMAGNETOMETER, QTAPSENSOR, QCOMPASS, QKEYS;


protected:
    //current coordinates
    static int m_x,m_y;




};


#endif // INPUTCONTROLLER_H
