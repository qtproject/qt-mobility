#ifndef VIEW_H
#define VIEW_H

#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QTimer>
#include "inputcontroller.h"



class View : public QGraphicsView
{
    Q_OBJECT

public:
//    View(QGraphicsScene *scene, InputController *controller);
    View(QGraphicsScene *scene);
    virtual ~View();
    static void setImageHeight(int imageHeight);
    static int getImageHeight();
    static void setImageWidth(int imageWidth);
    static int getImageWidth();

protected:
    QPixmap m_pix;

    virtual void resizeEvent(QResizeEvent *event);
    virtual void keyPressEvent(QKeyEvent *e);

    InputController* getController();
//    void setController(InputController* controller);


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
    int checkY(int y);
    void switchController(QString sensor);
    void createActions();
    void handleAction(QString oldSensor, QString newSensor);
    static int m_imageWidth;
    static int m_imageHeight;

    static QString m_currentSensor;
    QList<QString> m_sensors;
    QTimer m_timer;
    InputController *m_controller;
    int m_delay;
};


#endif // VIEW_H
