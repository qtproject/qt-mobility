#ifndef QMEDIARECORDERSERVICE_H
#define QMEDIARECORDERSERVICE_H

#include "qabstractmediaservice.h"

class QMediaRecorderService : public QAbstractMediaService
{
    Q_OBJECT
public:
    QMediaRecorderService(QObject *parent = 0);

    virtual QAbstractMediaControl *control() const = 0;
};

#endif
