#ifndef QGSTREAMERVIDEOWIDGET_H
#define QGSTREAMERVIDEOWIDGET_H

#include "qvideowidgetcontrol.h"
#include "qvideooutputcontrol.h"

#include <Phonon/VideoWidget>

class QPhononVideoWidget : public QVideoWidgetControl
{
    Q_OBJECT
public:
    QPhononVideoWidget(Phonon::VideoWidget *videoWidget, QObject *parent = 0);
    virtual ~QPhononVideoWidget();

    QWidget *videoWidget() { return m_videoWidget; }

    QVideoWidget::AspectRatio aspectRatio() const;
    QSize customAspectRatio() const;

    void setAspectRatio(QVideoWidget::AspectRatio ratio);
    void setCustomAspectRatio(const QSize &customRatio);

    bool isFullscreen() const;
    void setFullscreen(bool fullscreen);

    int brightness() const;
    void setBrightness(int brightness);

    int contrast() const;
    void setContrast(int contrast);

    int hue() const;
    void setHue(int hue);

    int saturation() const;
    void setSaturation(int saturation);

private:
    Phonon::VideoWidget *m_videoWidget;
    QVideoWidget::AspectRatio m_aspectRatio;
    QSize m_customAspectRatio;
};

class QPhononVideoOutputControl : public QVideoOutputControl
{
    Q_OBJECT
public:

    QPhononVideoOutputControl(QObject *parent = 0)
        :QVideoOutputControl(parent) {}
    ~QPhononVideoOutputControl() {}

    virtual QList<Output> availableOutputs() const
    {
        return QList<Output>() << WidgetOutput;
    }

    Output output() const { return WidgetOutput; }
    void setOutput(Output) {}
};

#endif // QGSTREAMERVIDEOWIDGET_H
