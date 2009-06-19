#ifndef QVIDEORENDERERMEDIAOUTPUT_H
#define QVIDEORENDERERMEDIAOUTPUT_H

#ifndef QT_NO_VIDEOSURFACE

#include <QtCore/qplugin.h>
#include <QtGui/qwidget.h>

class QVideoRendererMediaOutputInterface
{
public:
    virtual ~QVideoRendererMediaOutputInterface();
};

#define QVideoRendererMediaOutputInterface_iid "com.nokia.Qt.QVideoRendererMediaOutputInterface/1.0"

Q_DECLARE_INTERFACE(QVideoRendererMediaOutputInterface, QVideoRendererMediaOutputInterface_iid)

class QAbstractVideoSurface;

class QVideoRendererMediaOutputPrivate;

class QVideoRendererMediaOutput : public QObject, public QVideoRendererMediaOutputInterface
{
    Q_OBJECT
    Q_INTERFACES(QVideoRendererMediaOutputInterface)
    Q_PROPERTY(QAbstractVideoSurface* surface READ surface WRITE setSurface)
    Q_DECLARE_PRIVATE(QVideoRendererMediaOutput)
public:
    QVideoRendererMediaOutput(QObject *parent = 0);
    ~QVideoRendererMediaOutput();

    QAbstractVideoSurface *surface() const;
    virtual void setSurface(QAbstractVideoSurface *surface);
};

#endif

#endif
