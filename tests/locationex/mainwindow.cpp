#include "mainwindow.h"
#include "mapbox.h"

#include <qgeocoordinate.h>
#include <qgraphicsgeomap.h>
#include <qgeomaprectangleobject.h>

#include <QGridLayout>
#include <QPushButton>
#include <QTimer>
#include <QLabel>

#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    connect(MapBox::session(), SIGNAL(opened()), this, SLOT(networkSessionOpened()));

    setWindowTitle("MapBox test");

    m_cleanButton = new QPushButton("Clean cache");
    m_cleanButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_panToggle = new QPushButton("auto-pan");
    m_panToggle->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_panToggle->setCheckable(true);
    m_errorLabel = new QLabel("err");

    m_controlLayout = new QHBoxLayout;
    m_controlLayout->addWidget(m_cleanButton);
    m_controlLayout->addWidget(m_panToggle);
    m_controlLayout->addWidget(m_errorLabel);

    m_layout = new QGridLayout;
    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(m_layout);
    resize(640, 480);

    m_panTimer = new QTimer;
    m_panTimer->setInterval(0);
    connect(m_panTimer, SIGNAL(timeout()), this, SLOT(panTimerHandler()));

    QTimer * compareImagesTimer = new QTimer(this);
    compareImagesTimer->setInterval(1000);
    connect(compareImagesTimer, SIGNAL(timeout()), this, SLOT(compareImages()));
    compareImagesTimer->start();

    connect(m_panToggle, SIGNAL(toggled(bool)), this, SLOT(setAutoPanEnabled(bool)));
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

    int columns = floor(0.5 + sqrt(static_cast<qreal>(m_boxes.size())));

    m_layout->addLayout(m_controlLayout, 0, 0, 1, columns);

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
    for (int i = 0; i < 2; ++i) {
        MapBox * box = MapBox::createOnlineMap(this);
        box->addRectangle(55.1977, 5.87586, 47.5395, 15.2802);
        box->addRoute(QGeoCoordinate(50, 5.87586), QGeoCoordinate(47.5395, 15.2802));

        addBox(box);
    }
}

void MainWindow::panTimerHandler()
{
    const qreal speed = 0.5;
    qreal currentMovement = speed * qMin(300, panTime.restart());

    foreach(MapBox * box, m_boxes) {
        box->map()->pan(currentMovement, 0);
    }
}

void MainWindow::compareImages()
{
    if (m_boxes.size() < 2) {
        m_errorLabel->hide();
        return;
    }

    QImage firstBoxImage = m_boxes.first()->grab().toImage();

    QStringList results;

    foreach (MapBox * box, m_boxes) {
        if (results.isEmpty()) {
            results << "N/A";
            continue;
        }

        QImage currentImage = box->grab().toImage();
        if (currentImage.size() != firstBoxImage.size()) {
            results << "sizes differ";
            continue;
        }

        int errorCount = MapBox::countErrors(currentImage, firstBoxImage);
        qreal squareError = MapBox::squareError(currentImage, firstBoxImage);

        results << QString("sqerr=%1, errcnt=%2").arg(squareError).arg(errorCount);
    }

    results.removeFirst();
    m_errorLabel->setText(results.join(" | "));
    m_errorLabel->show();
}

void MainWindow::setAutoPanEnabled(bool enabled)
{
    if (enabled != m_panTimer->isActive()) {
        if (enabled) {
            panTime.start();
            m_panTimer->start();
        }
        else {
            m_panTimer->stop();
        }
    }
}
