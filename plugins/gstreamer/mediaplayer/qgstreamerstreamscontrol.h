#ifndef QGSTREAMERSTREAMSCONTROL_H
#define QGSTREAMERSTREAMSCONTROL_H

#include "qmediastreamscontrol.h"

class QGstreamerPlayerSession;

class QGstreamerStreamsControl : public QMediaStreamsControl
{
public:
    QGstreamerStreamsControl(QGstreamerPlayerSession *session, QObject *parent);
    virtual ~QGstreamerStreamsControl();

    virtual int streamCount();
    virtual int streamType(int streamNumber);
    virtual QMap<QString,QVariant> streamProperties(int streamNumber);
    virtual bool isActive(int streamNumber);
    virtual void setActive(int streamNumber, bool state);

signals:
    void streamsChanged();
    void activeStreamsChanged();

private:
    QGstreamerPlayerSession *m_session;
};

#endif // QGSTREAMERSTREAMSCONTROL_H
