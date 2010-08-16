#ifndef VIEW_H
#define VIEW_H

#include <QPixmap>
#include <QGraphicsView>
#include <QMenu>
#include <QTimer>
#include <QTime>
#include "inputcontroller.h"




class View : public QGraphicsView
{
    Q_OBJECT

public:
    View(QGraphicsScene *scene);
    virtual ~View();
    static int m_imageWidth;


protected:

    virtual void resizeEvent(QResizeEvent *event);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

    virtual void focusInEvent(QFocusEvent *);
    virtual void focusOutEvent(QFocusEvent *);




private slots:
    void startAccelerometer();
    void startTapSensor();
    void startMagnetometer();
    void startRotationSensor();
    void startOrientationSensor();
    void startCompass();
    void startKeys();
    void update();

private:
    int checkX(int x);
    int checkY(int y);
    void createActions();
    void handleAction(QString oldSensor, QString newSensor);
    void setupWindow();
    void switchController(QString sensor);

    static int m_imageHeight;

    static QString m_currentSensor;
    QList<QString> m_sensors;
    QTimer m_timer;
    InputController *m_controller;
    InputController *m_exController;
    int m_delay;
    QTime m_mousePressTime;
    QMenu* m_menu;
    bool m_mouseMode;
    int m_eventX, m_eventY;
    int m_dx, m_dy;
    QTime m_zoomTime;
    QPixmap m_pix;
    static bool m_isStarted;


};


#endif // VIEW_H
