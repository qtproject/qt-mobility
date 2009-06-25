#ifndef QEVRWIDGET_H
#define QEVRWIDGET_H

#include <QtCore/qmetatype.h>
#include <QtGui/qwidget.h>

#include <evr.h>

class QEvrWidgetActivate;

Q_DECLARE_METATYPE(IMFActivate*)

class QEvrWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(IMFActivate* activate READ activate)
public:
    QEvrWidget(QWidget *parent = 0);
    ~QEvrWidget();

    IMFActivate *activate() const;

    bool isFullscreen() const { return m_fullscreen; }

    QSize sizeHint() const;

public Q_SLOTS:
    void setFullscreen(bool fullscreen);

protected:
    void paintEvent(QPaintEvent *event);
    void moveEvent(QMoveEvent *event);
    void resizeEvent(QResizeEvent *event);
    bool event(QEvent *event);

private:
    void setDisplayControl(IMFVideoDisplayControl *control);

    IMFVideoDisplayControl *m_display;
    QEvrWidgetActivate *m_activate;

    bool m_fullscreen;

    friend class QEvrWidgetActivate;
};

#endif