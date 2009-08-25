#include "qphononvideowidget.h"

#include <QEvent>
#include <QApplication>
#include <QGridLayout>

QPhononVideoWidget::QPhononVideoWidget(Phonon::VideoWidget *videoWidget, QObject *parent)
    :QVideoWidgetControl(parent), m_videoWidget(videoWidget)
{
    setAspectRatio(AspectRatioAuto);
}

QPhononVideoWidget::~QPhononVideoWidget()
{
}

QVideoWidgetControl::AspectRatio QPhononVideoWidget::aspectRatio() const
{
    return m_aspectRatio;
}

QSize QPhononVideoWidget::customAspectRatio() const
{
    return m_customAspectRatio;
}

void QPhononVideoWidget::setAspectRatio(QVideoWidgetControl::AspectRatio ratio)
{
    m_aspectRatio = ratio;
    switch (ratio) {
        case AspectRatioAuto:
            m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
            break;
        case AspectRatioWidget:
            m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
            break;
        default:
            m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
    }
}

void QPhononVideoWidget::setCustomAspectRatio(const QSize &ratio)
{
    m_customAspectRatio = ratio;
}


bool QPhononVideoWidget::isFullscreen() const
{
    return m_videoWidget->isFullScreen();
}

void QPhononVideoWidget::setFullscreen(bool fullscreen)
{
    if (fullscreen != isFullscreen()) {
        if (fullscreen)
            m_videoWidget->enterFullScreen();
        else
            m_videoWidget->exitFullScreen();

        emit fullscreenChanged(fullscreen);
    }
}

int QPhononVideoWidget::brightness() const
{
    return qRound(100.0*m_videoWidget->brightness());
}

void QPhononVideoWidget::setBrightness(int brightness)
{
    m_videoWidget->setBrightness(brightness/100.0);
    emit brightnessChanged(brightness);
}

int QPhononVideoWidget::contrast() const
{
    return qRound(100.0*m_videoWidget->contrast());
}

void QPhononVideoWidget::setContrast(int contrast)
{
    m_videoWidget->setContrast(contrast/100.0);
    emit contrastChanged(contrast);
}

int QPhononVideoWidget::hue() const
{
    return qRound(100.0*m_videoWidget->hue());
}

void QPhononVideoWidget::setHue(int hue)
{
    m_videoWidget->setHue(hue/100.0);
    emit hueChanged(hue);
}

int QPhononVideoWidget::saturation() const
{
    return qRound(100.0*m_videoWidget->saturation());
}

void QPhononVideoWidget::setSaturation(int saturation)
{
    m_videoWidget->setSaturation(saturation/100.0);
    emit saturationChanged(saturation);
}


/*void QPhononVideoWidget::resizeEvent(QResizeEvent *e)
{
    qDebug() << "QPhononVideoWidget::resizeEvent" << geometry();
    m_videoWidget->setGeometry(0,0,width(),height());
    QMediaWidgetEndpoint::resizeEvent(e);
}*/
