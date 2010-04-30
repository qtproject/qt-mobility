
#include "keycontroller.h"
#include "view.h"
#include <QDebug>

KeyController::KeyController(): TimedController(){
}


void KeyController::keyPressEvent(QKeyEvent *e)
{
//    qDebug()<<"KeyController::keyPressEvent";
    int code = e->key();
    handleKeyPress(code);
    m_step = m_exCode==code ? m_step+2 : 5;
}


void KeyController::updateCoordinates() {
    handleKeyPress(m_exCode);
}


void KeyController::handleKeyPress(int code){
    switch (code){
    case Qt::Key_Escape:
        close();
        break;
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
        m_exCode = code;
        return;
    }
    m_exCode = code;
}


