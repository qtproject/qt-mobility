#include "mainwindow.h"
#include "mapbox.h"

#include <qgeocoordinate.h>
#include <qgraphicsgeomap.h>
#include <qgeomaprectangleobject.h>

#include <QGridLayout>
#include <QPushButton>

#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    connect(MapBox::session(), SIGNAL(opened()), this, SLOT(networkSessionOpened()));

    setWindowTitle("MapBox test");

    resize(640, 480);

    m_cleanButton = new QPushButton("Clean cache");

    m_layout = new QGridLayout;
    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(m_layout);
    startTimer(10);

    eventTime.start();
}

MainWindow::~MainWindow()
{

}

void MainWindow::addBox(MapBox * box)
{
    m_boxes << box;

    connect(m_cleanButton, SIGNAL(clicked()), box, SLOT(clearCache()));

    resizeEvent(0);
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    QLayoutItem *child;
    while ((child = m_layout->takeAt(0)) != 0) ;

    int columns = floor(0.5+sqrt(static_cast<qreal>(m_boxes.size())));

    m_layout->addWidget(m_cleanButton, 0, 0, 1, columns);

    int i = 0;
    for (int row = 1; i < m_boxes.size(); ++row) {
        for (int col = 0; col < columns; ++col) {
            if (i+1 >= m_boxes.size()) {
                m_layout->addWidget(m_boxes[i], row, col, 1, columns - i);
                ++i;
                break;
            }
            else {
                m_layout->addWidget(m_boxes[i], row, col);
                ++i;
            }
        }
    }
}

void MainWindow::networkSessionOpened()
{
    for (int i = 0; i < 1; ++i) {
        MapBox * box = MapBox::createOnlineMap(this);
        box->addRectangle(55.1977, 5.87586, 47.5395, 15.2802);
        //box->addRoute(QGeoCoordinate(50, 5.87586), QGeoCoordinate(47.5395, 15.2802));

        addBox(box);
    }
}

void MainWindow::timerEvent(QTimerEvent * event)
{
    const qreal speed = 0.5;
    qreal currentMovement = speed * qMin(300, eventTime.elapsed());

    foreach(MapBox * box, m_boxes) {
        box->map()->pan(currentMovement, 0);
    }
    eventTime.restart();
}
