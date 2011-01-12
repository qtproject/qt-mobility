#include "tennisview.h"

#include <QMouseEvent>

TennisView::TennisView(QWidget *parent) :
    QGraphicsView(parent)
{
}

void TennisView::mousePressEvent(QMouseEvent *event)
{
    emit mouseMove(event->y());
    last_y = event->y();
}

void TennisView::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()) {
        if(10 < abs(last_y - event->y())) {
            emit mouseMove(event->y());
            last_y = event->y();
        }
    }
}
