#ifndef QVIDEORENDERERWIDGET_P_H
#define QVIDEORENDERERWIDGET_P_H

#include "qwidgetmediaoutput.h"

#ifndef QT_NO_VIDEOSURFACE

class QVideoRendererMediaOutput;

class QVideoRendererWidgetPrivate;

class QVideoRendererWidget : public QWidgetMediaOutput
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QVideoRendererWidget)
public:
    QVideoRendererWidget(QWidget *parent = 0);
    ~QVideoRendererWidget();

    QVideoRendererMediaOutput *renderer() const;
    void setRenderer(QVideoRendererMediaOutput *renderer);

public Q_SLOTS:
    void setFullscreen(bool fullscreen);
};

#endif

#endif
