#ifndef QGSTREAMERCAPTURESERVICE_H
#define QGSTREAMERCAPTURESERVICE_H

#include "qabstractmediaservice.h"

#include <gst/gst.h>

class QGstreamerCaptureSession;
class QGstreamerMessage;
class QGstreamerBusHelper;

class QGstreamerCaptureService : public QAbstractMediaService
{
Q_OBJECT
public:
    QGstreamerCaptureService(QObject *parent = 0);
    virtual ~QGstreamerCaptureService();

    QList<QByteArray> supportedEndpointInterfaces(
            QMediaEndpointInterface::Direction direction) const;

    void setAudioInput(QObject *input);

    QObject *createEndpoint(const char *interface);

    QAbstractMediaControl *control(const char *name) const;

private:
    void setAudioPreview(GstElement*);

    QGstreamerCaptureSession *m_captureSession;
};

#endif // QGSTREAMERCAPTURESERVICE_H
