#include "qphononvideowidget.h"

#include <QEvent>
#include <QApplication>
#include <QGridLayout>

QPhononVideoWidget::QPhononVideoWidget(Phonon::VideoWidget *videoWidget, QWidget *parent)
    :QMediaWidgetEndpoint(parent), m_videoWidget(videoWidget)
{
    m_videoWidget->setParent(this);
    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(m_videoWidget);

    setLayout(layout);

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
