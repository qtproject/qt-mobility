#ifndef QGSTREAMERBUSHELPER_H
#define QGSTREAMERBUSHELPER_H

#include <QObject>

#include <qgstreamermessage.h>
#include <gst/gst.h>


class QGstreamerBusHelperPrivate;

class QGstreamerBusHelper : public QObject
{
    Q_OBJECT
    friend class QGstreamerBusHelperPrivate;

public:
    QGstreamerBusHelper(GstBus* bus, QObject* parent = 0);
    ~QGstreamerBusHelper();

signals:
    void message(QGstreamerMessage const& message);

private:
    QGstreamerBusHelperPrivate*   d;
};

#endif
