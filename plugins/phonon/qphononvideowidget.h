#ifndef QGSTREAMERVIDEOWIDGET_H
#define QGSTREAMERVIDEOWIDGET_H

#include "qmediawidgetendpoint.h"
#include <phonon>

class QPhononVideoWidget : public QMediaWidgetEndpoint
{
    Q_OBJECT    
    Q_PROPERTY(bool fullscreen READ isFullscreen WRITE setFullscreen)
public:
    QPhononVideoWidget(Phonon::VideoWidget *videoWidget, QWidget *parent = 0);
    virtual ~QPhononVideoWidget();

    Phonon::VideoWidget *videoWidget() { return m_videoWidget; }

protected slots:
    virtual void setAspectRatio(QMediaWidgetEndpoint::AspectRatio ratio);
    virtual void setCustomAspectRatio(const QSize &ratio);
private:
    Phonon::VideoWidget *m_videoWidget;
};

#endif // QGSTREAMERVIDEOWIDGET_H
