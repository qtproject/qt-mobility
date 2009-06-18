
#ifndef QRADIOSERVICE_H
#define QRADIOSERVICE_H

#include <QtCore/qobject.h>

class QRadioService : public QAbstractMediaService, public QRadioServiceInterface
{
    Q_OBJECT

public:
    QRadioService(QObject *parent = 0);
    ~QRadioService();

    virtual QRadioTuner* control() const = 0;
};

#endif
