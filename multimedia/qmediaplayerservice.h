#ifndef QMEDIAPLAYERSERVICE_H
#define QMEDIAPLAYERSERVICE_H

#include "qabstractmediaservice.h"


class QMediaPlayerServicePrivate;

class QMediaPlayerService : public QAbstractMediaService
{
    Q_OBJECT

public:
    ~QMediaPlayerService();

    virtual QMediaPlayerControl* control() const = 0;

protected:
    QMediaPlayerService(QObject *parent);
};

#endif  // QMEDIAPLAYERSERVICE_H
