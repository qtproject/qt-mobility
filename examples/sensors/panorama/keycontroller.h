#ifndef KEYCONTROLLER_H
#define KEYCONTROLLER_H

#include "timedcontroller.h"
#include <QKeyEvent>
#include <QDebug>
#include <QTimer>

class KeyController : public TimedController
{
    Q_OBJECT

public:
    KeyController();
    virtual ~KeyController();
    void handleKeyPress(int code);
    virtual void updateCoordinates();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    int m_exCode;
    int m_step;

};


#endif // KEYCONTROLLER_H
