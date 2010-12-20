#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QGraphicsScene>

class Board : public QObject
{
    Q_OBJECT
public:
    explicit Board(QObject *parent = 0);

    enum Edge {
        Top = 1,
        Bottom,
        Left,
        Right
    };

    enum BoardSize {
        Width = 640,
        Height = 360,
        Paddle = 50
    };

    QGraphicsScene *getScene() {
        return scene;
    }

signals:
    void ballCollision(Board::Edge pos);
    void scored(Board::Edge pos);

public slots:
    void setBall(int x, int y);
    void setLeftPaddle(int y);
    void setRightPaddle(int y);
    void setScore(int l, int r);

    void setStatus(QString text, int opacity_start, int opacity_end);

    void sceneChanged(const QList<QRectF> &region);

private:
    QGraphicsScene *scene;
    QGraphicsRectItem *ball;
    QGraphicsRectItem *topWall;
    QGraphicsRectItem *bottomWall;
    QGraphicsRectItem *leftPaddle;
    QGraphicsRectItem *rightPaddle;

    QGraphicsTextItem *leftScore;
    QGraphicsTextItem *rightScore;

    QGraphicsTextItem *status;

    void checkBall();
};

#endif // BOARD_H
