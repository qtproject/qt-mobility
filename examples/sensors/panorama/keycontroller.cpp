
#include "keycontroller.h"
#include "view.h"

KeyController::KeyController(): TimedController(){}

void KeyController::keyPressEvent(QKeyEvent *e)
{
    int code = e->key();
    handleKeyPress(code);
    m_step = m_exCode==code && m_exCode > 0? m_step+2 : 5;
}


void KeyController::updateCoordinates() { handleKeyPress(m_exCode);}


void KeyController::handleKeyPress(int code){
    switch (code){
    case Qt::Key_Right:
        m_x+=m_step;
        break;
    case Qt::Key_Left:
        m_x-=m_step;
        break;
    case Qt::Key_Up:
        m_y-=m_step;
        break;
    case Qt::Key_Down:
        m_y+=m_step;
        break;
    default:
        m_exCode = 0;
        return;
    }
    m_exCode = code;
}


