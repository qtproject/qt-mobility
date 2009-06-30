#ifndef QVIDEORENDERERENDPOINT_H
#define QVIDEORENDERERENDPOINT_H

#ifndef QT_NO_VIDEOSURFACE

#include <QtCore/qplugin.h>
#include <QtGui/qwidget.h>

class QVideoRendererEndpointInterface
{
public:
    virtual ~QVideoRendererEndpointInterface();
};

#define QVideoRendererEndpointInterface_iid "com.nokia.Qt.QVideoRendererEndpointInterface/1.0"

Q_DECLARE_INTERFACE(QVideoRendererEndpointInterface, QVideoRendererEndpointInterface_iid)

class QAbstractVideoSurface;

class QVideoRendererEndpointPrivate;

class QVideoRendererEndpoint : public QObject, public QVideoRendererEndpointInterface
{
    Q_OBJECT
    Q_INTERFACES(QVideoRendererEndpointInterface)
    Q_PROPERTY(QAbstractVideoSurface* surface READ surface WRITE setSurface)
    Q_DECLARE_PRIVATE(QVideoRendererEndpoint)
public:
    QVideoRendererEndpoint(QObject *parent = 0);
    ~QVideoRendererEndpoint();

    QAbstractVideoSurface *surface() const;
    virtual void setSurface(QAbstractVideoSurface *surface);
};

#endif

#endif
