#include "tennisview.h"

#include <QMouseEvent>

TennisView::TennisView(QWidget *parent) :
    QGraphicsView(parent)
{
    scale_x = scale_y = 1.0;
}

void TennisView::mousePressEvent(QMouseEvent *event)
{
    emit mouseMove(event->x()/scale_x, event->y()/scale_y);
    last_y = event->y();
}

void TennisView::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()) {
        if(10 < abs(last_y - event->y())) {
            emit mouseMove(event->x()/scale_x, event->y()/scale_y);
            last_y = event->y();
        }
    }
}

void TennisView::scale(qreal sx, qreal sy)
{
    scale_x = sx;
    scale_y = sy;
    QGraphicsView::scale(sx, sy);

}
