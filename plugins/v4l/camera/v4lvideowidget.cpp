#include "v4lvideowidget.h"
#include "cameraformatconverter.h"

#include <QEvent>
#include <QApplication>
#include <QDebug>
#include <QPainter>

V4LVideoWidget::V4LVideoWidget(QWidget *parent)
    :QMediaWidgetEndpoint(parent)
{
    m_data   = 0;
    m_length = 0;
    converter = 0;
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMinimumHeight(240);
    setMinimumWidth(320);
    setBaseSize(320,240);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

V4LVideoWidget::~V4LVideoWidget()
{
}

void V4LVideoWidget::setData(char* data)
{
    m_data = data;
}

void V4LVideoWidget::setLength(qint64 len)
{
    m_length = len;
}

void V4LVideoWidget::paintEvent(QPaintEvent *event)
{
    int w = width();
    int h = height();

    if(w > 320) w = 320;
    if(h > 240) h = 240;

    if(!converter)
        converter = CameraFormatConverter::createFormatConverter(w,h);

    if(m_data) {
        QImage image;
        image = QImage(converter->convert((unsigned char*)m_data,m_length),w,h,QImage::Format_RGB16);
        QPainter painter(this);
        if(painter.viewport().isValid()) {
            painter.drawImage(painter.viewport(),image);
        }
    }
}

void V4LVideoWidget::setFullscreen(bool fullscreen)
{
    QMediaWidgetEndpoint::setFullscreen(fullscreen);
}

void V4LVideoWidget::setAspectRatio(QMediaWidgetEndpoint::AspectRatio ratio)
{
    QMediaWidgetEndpoint::setAspectRatio(ratio);
}

void V4LVideoWidget::setCustomAspectRatio(const QSize &ratio)
{
    QMediaWidgetEndpoint::setCustomAspectRatio(ratio);
}
