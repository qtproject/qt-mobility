#ifndef KEYCONTROLLER_H
#define KEYCONTROLLER_H

#include "timedcontroller.h"
#include <QKeyEvent>

class KeyController : public TimedController
{
    Q_OBJECT

public:
    KeyController();
    virtual ~KeyController();
    virtual void updateCoordinates();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    void handleKeyPress(int code);
    int m_exCode;
    int m_step;

};


#endif // KEYCONTROLLER_H
