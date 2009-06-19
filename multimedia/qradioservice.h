
#ifndef QRADIOSERVICE_H
#define QRADIOSERVICE_H

#include "abstractmediaservice.h"

class QRadioService : public QAbstractMediaService
{
    Q_OBJECT

public:
    QRadioService(QObject *parent = 0);
    ~QRadioService();

    virtual QRadioTuner* control() const = 0;
};

#endif
