#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <multimedia/qvideowidget.h>

class VideoWidget : public QVideoWidget
{
    Q_OBJECT
public:
    VideoWidget(QMediaObject *object, QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif
