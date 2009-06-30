#ifndef QVIDEORENDERERWIDGET_P_H
#define QVIDEORENDERERWIDGET_P_H

#include "qmediawidgetendpoint.h"

#ifndef QT_NO_VIDEOSURFACE

class QVideoRendererEndpoint;

class QVideoRendererWidgetPrivate;

class QVideoRendererWidget : public QMediaWidgetEndpoint
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QVideoRendererWidget)
public:
    QVideoRendererWidget(QVideoRendererEndpoint *renderer);
    ~QVideoRendererWidget();

    QVideoRendererEndpoint *rendererOutput() const;

public Q_SLOTS:
    void setFullscreen(bool fullscreen);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
};

#endif

#endif
