#ifndef QMEDIAPLAYERSERVICE_H
#define QMEDIAPLAYERSERVICE_H

#include "qabstractmediaservice.h"


class QMediaPlayerServicePrivate;

class QMediaPlayerService : public QAbstractMediaService
{
public:
    ~QMediaPlayerService();

    virtual QMediaPlayerControl* control() const = 0;

protected:
    QMediaPlayerService(QObject *parent);

Q_SLOTS:
    void propertyWatch();
};

#endif  // QMEDIAPLAYERSERVICE_H
