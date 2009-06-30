#ifndef QWIDGETMEDIAOUTPUT_H
#define QWIDGETMEDIAOUTPUT_H

#include <QtCore/qplugin.h>
#include <QtGui/qwidget.h>

class QMediaWidgetEndpointInterface
{
public:
    virtual ~QMediaWidgetEndpointInterface();
};

#define QMediaWidgetEndpointInterface_iid "com.nokia.Qt.QMediaWidgetEndpointInterface/1.0"

Q_DECLARE_INTERFACE(QMediaWidgetEndpointInterface, QMediaWidgetEndpointInterface_iid)

class QMediaWidgetEndpointPrivate;

class QMediaWidgetEndpoint : public QWidget, public QMediaWidgetEndpointInterface
{
    Q_OBJECT
    Q_INTERFACES(QMediaWidgetEndpointInterface)
    Q_PROPERTY(bool fullscreen READ isFullscreen WRITE setFullscreen)
    Q_DECLARE_PRIVATE(QMediaWidgetEndpoint)
public:
    QMediaWidgetEndpoint(QWidget *parent = 0);
    ~QMediaWidgetEndpoint();

    bool isFullscreen() const;

public Q_SLOTS:
    virtual void setFullscreen(bool fullscreen);

protected:
    QMediaWidgetEndpoint(QMediaWidgetEndpointPrivate &dd, QWidget *parent);
};

#endif
