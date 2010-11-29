#include "mainwindow.h"
#include "mapbox.h"

#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QGridLayout * layout = new QGridLayout;

    layout->addWidget(MapBox::createOnlineMap(this));

    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(layout);

    setWindowTitle("MapBox test");

    resize(640,480);
}

MainWindow::~MainWindow()
{

}
