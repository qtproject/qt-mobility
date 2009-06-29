#ifndef QMEDIARECORDERCONTROL_H
#define QMEDIARECORDERCONTROL_H

#include "qabstractmediacontrol.h"

class QMediaSink;

class QMediaRecorderControl : public QAbstractMediaControl
{
    Q_OBJECT
public:
    QMediaRecorderControl(QObject *parent = 0);

    int state() const;
    virtual void setState(int state);

    QMediaSink *sink() const;
    virtual void setSink(QMediaSink *sink);
};

#endif
