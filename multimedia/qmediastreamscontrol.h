#ifndef QMEDIASTREAMSCONTROL_H
#define QMEDIASTREAMSCONTROL_H

#include "qabstractmediacontrol.h"
#include "qmediastreams.h"

class QMediaStreamsControl : public QAbstractMediaControl
{
Q_OBJECT
public:
    virtual ~QMediaStreamsControl();

    virtual int streamCount() = 0;
    virtual QMediaStreamInfo::StreamType streamType(int streamNumber) = 0;
    virtual QMap<QString,QVariant> streamProperties(int streamNumber) = 0;
    virtual bool isActive(int streamNumber) = 0;
    virtual void setActive(int streamNumber, bool state) = 0;

signals:
    void streamsChanged();
    void activeStreamsChanged();

protected:
    QMediaStreamsControl(QObject *parent = 0);
};

#endif // QMEDIASTREAMSCONTROL_H
