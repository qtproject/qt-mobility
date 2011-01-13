#ifndef TAP_H
#define TAP_H

#include <qtapsensor.h>

QTM_USE_NAMESPACE

class TapSensorFilter : public QObject, public QTapFilter
{
 Q_OBJECT
 public:
    bool filter(QTapReading *reading);
signals:
    void doubleHappened(QString);
    void singleHappened(QString);
};

#endif // TAP_H
