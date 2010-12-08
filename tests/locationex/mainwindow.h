#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QTime>

class MapBox;
class QGridLayout;
class QLayout;
class QPushButton;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QList<MapBox *> m_boxes;
    QGridLayout * m_layout;
    QLayout * m_controlLayout;
    void addBox(MapBox * box);
    QTime panTime;
    QPushButton * m_cleanButton;
    QPushButton * m_panToggle;
    QLabel * m_errorLabel;
    QTimer * m_panTimer;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void networkSessionOpened();
    void compareImages();
    void setAutoPanEnabled(bool enabled);
    void panTimerHandler();

protected:
    void resizeEvent(QResizeEvent * event);
};

#endif // MAINWINDOW_H
