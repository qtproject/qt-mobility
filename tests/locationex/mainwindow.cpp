#include "mainwindow.h"
#include "mapbox.h"

#include <qgeocoordinate.h>
#include <qgraphicsgeomap.h>
#include <qgeomaprectangleobject.h>

#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QGridLayout * layout = new QGridLayout;

    MapBox * box = MapBox::createOnlineMap(this);
    box->addRectangle(55.1977, 5.87586, 47.5395, 15.2802);
    //box->addRoute(QGeoCoordinate(50, 5.87586), QGeoCoordinate(47.5395, 15.2802));

    layout->addWidget(box);

    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(layout);

    setWindowTitle("MapBox test");

    resize(640,480);
}

MainWindow::~MainWindow()
{

}
