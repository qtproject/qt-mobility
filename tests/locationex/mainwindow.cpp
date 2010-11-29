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

    layout->addWidget(box);

    QPen pen(Qt::white);
    pen.setWidth(2);
    pen.setCosmetic(true);
    QColor fill(Qt::black);
    fill.setAlpha(65);
    QGeoMapRectangleObject *rectangle = new QGeoMapRectangleObject(QGeoCoordinate(55.1977, 5.87586), QGeoCoordinate(47.5395, 15.2802));
    rectangle->setPen(pen);
    rectangle->setBrush(QBrush(fill));
    box->map()->addMapObject(rectangle);

    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(layout);

    setWindowTitle("MapBox test");

    resize(640,480);
}

MainWindow::~MainWindow()
{

}
