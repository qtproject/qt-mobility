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
    m_size   = QSize(320,240);
    converter = 0;
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMinimumHeight(m_size.height());
    setMinimumWidth(m_size.width());
    setBaseSize(m_size.width(),m_size.height());
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

void V4LVideoWidget::setFrameSize(const QSize& s)
{
    m_size = s;
    setMinimumHeight(m_size.height());
    setMinimumWidth(m_size.width());
    setBaseSize(m_size.width(),m_size.height());
    if(converter)
        delete converter;

    converter = CameraFormatConverter::createFormatConverter(m_size.width(),m_size.height());
}

void V4LVideoWidget::paintEvent(QPaintEvent *event)
{
    if(!converter || !m_data) return;

    QImage image;
    image = QImage(converter->convert((unsigned char*)m_data,m_length),m_size.width(),m_size.height(),QImage::Format_RGB16);

    QPainter painter(this);
    if(painter.viewport().isValid()) {
        if(m_size.width() == width())
            painter.drawImage(painter.viewport(),image);
        else {
            int x = (width() - image.width())/2;
            int y = (height() - image.height())/2;
            painter.drawImage(x,y,image);
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
