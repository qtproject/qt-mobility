#include "qphononvideowidget.h"

#include <QEvent>
#include <QApplication>
#include <QGridLayout>

QPhononVideoWidget::QPhononVideoWidget(Phonon::VideoWidget *videoWidget, QWidget *parent)
    :QMediaWidgetEndpoint(parent), m_videoWidget(videoWidget)
{
}

QPhononVideoWidget::~QPhononVideoWidget()
{
}

void QPhononVideoWidget::setAspectRatio(QMediaWidgetEndpoint::AspectRatio ratio)
{
    QMediaWidgetEndpoint::setAspectRatio(ratio);
    switch (ratio) {
        case QMediaWidgetEndpoint::AspectRatioAuto:
            m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
            break;
        case QMediaWidgetEndpoint::AspectRatioWidget:
            m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
            break;
        default:
            m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
    }
}

void QPhononVideoWidget::setCustomAspectRatio(const QSize &ratio)
{
    QMediaWidgetEndpoint::setCustomAspectRatio(ratio);
}

void QPhononVideoWidget::resizeEvent(QResizeEvent *e)
{
    qDebug() << "QPhononVideoWidget::resizeEvent" << geometry();
    m_videoWidget->setGeometry(0,0,width(),height());
    QMediaWidgetEndpoint::resizeEvent(e);
}
