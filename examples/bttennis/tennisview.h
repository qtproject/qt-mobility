#ifndef TENNISVIEW_H
#define TENNISVIEW_H

#include <QGraphicsView>

class TennisView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TennisView(QWidget *parent = 0);

signals:
    void mouseMove(int delta);

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *ev);

private:
    int last_y;

};

#endif // TENNISVIEW_H
