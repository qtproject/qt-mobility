#ifndef QGSTREAMERVIDEOWIDGET_H
#define QGSTREAMERVIDEOWIDGET_H

#include "qmediawidgetendpoint.h"
#include "qgstreamerplayersession.h"

class QGstreamerVideoWidget : public QMediaWidgetEndpoint, public QGstreamerVideoRendererInterface
{
    Q_OBJECT
    Q_INTERFACES(QMediaWidgetEndpointInterface QGstreamerVideoRendererInterface)
    Q_PROPERTY(bool fullscreen READ isFullscreen WRITE setFullscreen)
public:
    QGstreamerVideoWidget(QWidget *parent = 0);
    virtual ~QGstreamerVideoWidget();

    GstElement *videoSink();

protected:
    virtual bool event(QEvent*);

private:
    void setOverlay();
    void windowExposed();

    GstElement *m_videoSink;
};

#endif // QGSTREAMERVIDEOWIDGET_H
