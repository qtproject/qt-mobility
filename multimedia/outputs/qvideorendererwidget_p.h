#ifndef QVIDEORENDERERWIDGET_P_H
#define QVIDEORENDERERWIDGET_P_H

#include "qwidgetmediaoutput.h"

#ifndef QT_NO_VIDEOSURFACE

class QImageMediaOutput;
class QVideoRendererMediaOutput;

class QVideoRendererWidgetPrivate;

class QVideoRendererWidget : public QWidgetMediaOutput
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QVideoRendererWidget)
public:
    QVideoRendererWidget(QWidget *parent = 0);
    ~QVideoRendererWidget();

    QVideoRendererMediaOutput *rendererOutput() const;
    void setRendererOutput(QVideoRendererMediaOutput *output);

    QImageMediaOutput *imageOutput() const;
    void setImageOutput(QImageMediaOutput *output);

public Q_SLOTS:
    void setFullscreen(bool fullscreen);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_imageChanged(const QImage &));
};

#endif

#endif
