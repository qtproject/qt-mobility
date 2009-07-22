#include "qgstreamerstreamscontrol.h"
#include "qgstreamerplayersession.h"

QGstreamerStreamsControl::QGstreamerStreamsControl(QGstreamerPlayerSession *session, QObject *parent)
    :QMediaStreamsControl(parent), m_session(session)
{
    connect(m_session, SIGNAL(streamsChanged()), SIGNAL(streamsChanged()));
}

QGstreamerStreamsControl::~QGstreamerStreamsControl()
{
}

int QGstreamerStreamsControl::streamCount()
{
    return m_session->streamCount();
}

int QGstreamerStreamsControl::streamType(int streamNumber)
{
    return m_session->streamProperties(streamNumber).value("Type").toInt();
}

QMap<QString,QVariant> QGstreamerStreamsControl::streamProperties(int streamNumber)
{
    return m_session->streamProperties(streamNumber);
}

bool QGstreamerStreamsControl::isActive(int streamNumber)
{
    return false;
}

void QGstreamerStreamsControl::setActive(int streamNumber, bool state)
{
}
