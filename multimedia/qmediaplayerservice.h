#ifndef QMEDIAPLAYERSERVICE_H
#define QMEDIAPLAYERSERVICE_H

#include "qabstractmediaservice.h"

class QMediaPlayerControl;

class QMediaPlayerServicePrivate;

class QMediaPlayerService : public QAbstractMediaService
{
    Q_OBJECT

public:
    ~QMediaPlayerService();

    virtual QAbstractMediaControl* control() const = 0;

protected:
    QMediaPlayerService(QObject *parent);
};

#endif  // QMEDIAPLAYERSERVICE_H
