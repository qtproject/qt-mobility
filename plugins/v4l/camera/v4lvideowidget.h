#ifndef V4LVIDEOWIDGET_H
#define V4LVIDEOWIDGET_H

#include "qmediawidgetendpoint.h"
#include "v4lcamerasession.h"

class CameraFormatConverter;

class V4LVideoWidget : public QMediaWidgetEndpoint
{
    Q_OBJECT
public:
    V4LVideoWidget(QWidget *parent = 0);
    ~V4LVideoWidget();

    void setLength(qint64 len);
    void setData(char* data);

protected:
    void paintEvent(QPaintEvent *event);

public slots:
    void setFullscreen(bool fullscreen);
    void setAspectRatio(AspectRatio ratio);
    void setCustomAspectRatio(const QSize &customRatio);

private:
    char* m_data;
    qint64 m_length;
    CameraFormatConverter* converter;
};

#endif // V4LVIDEOWIDGET_H
