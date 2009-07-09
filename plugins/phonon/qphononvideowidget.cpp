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
}

void QPhononVideoWidget::setCustomAspectRatio(const QSize &ratio)
{
    QMediaWidgetEndpoint::setCustomAspectRatio(ratio);
}
