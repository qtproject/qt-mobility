#ifndef QGSTREAMERVIDEOWIDGET_H
#define QGSTREAMERVIDEOWIDGET_H

#include "qmediawidgetendpoint.h"
#include "qgstreamerplayersession.h"

class QGstreamerVideoWidget : public QMediaWidgetEndpoint, public QGstreamerVideoRendererInterface
{
    Q_OBJECT
    Q_INTERFACES(QGstreamerVideoRendererInterface)
    Q_PROPERTY(bool fullscreen READ isFullscreen WRITE setFullscreen)
public:
    QGstreamerVideoWidget(QWidget *parent = 0);
    virtual ~QGstreamerVideoWidget();

    GstElement *videoSink();
    void precessNewStream() { setOverlay(); }

protected slots:
    virtual void setAspectRatio(QMediaWidgetEndpoint::AspectRatio ratio);
    virtual void setCustomAspectRatio(const QSize &ratio);

    void setOverlay();

protected:
    virtual bool event(QEvent*);

private:
    void windowExposed();

    GstElement *m_videoSink;
};

#endif // QGSTREAMERVIDEOWIDGET_H
