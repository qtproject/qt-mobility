#ifndef TENNISVIEW_H
#define TENNISVIEW_H

#include <QGraphicsView>

class TennisView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TennisView(QWidget *parent = 0);

    void scale(qreal sx, qreal sy);

signals:
    void mouseMove(int x, int y);

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *ev);

private:
    int last_y;
    qreal scale_x, scale_y;

};

#endif // TENNISVIEW_H
