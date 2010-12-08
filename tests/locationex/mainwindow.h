#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QTime>

class MapBox;
class QGridLayout;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QList<MapBox *> m_boxes;
    QGridLayout * m_layout;
    void addBox(MapBox * box);
    QTime eventTime;
    QPushButton * m_cleanButton;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void networkSessionOpened();

protected:
    void timerEvent(QTimerEvent * event);
    void resizeEvent(QResizeEvent * event);
};

#endif // MAINWINDOW_H
