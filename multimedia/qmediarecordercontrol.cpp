#include "qmediarecordercontrol.h"

QMediaRecorderControl::QMediaRecorderControl(QObject *parent)
    : QAbstractMediaControl(parent)
{
}

int QMediaRecorderControl::state() const
{
    return property("state").toInt();
}

void QMediaRecorderControl::setState(int state)
{
    changePropertyValue("state", state);
}

QMediaSink *QMediaRecorderControl::sink() const
{
    return 0;
}

void QMediaRecorderControl::setSink(QMediaSink *sink)
{
}
