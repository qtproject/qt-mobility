#ifndef MYBACKEND_H
#define MYBACKEND_H

#include <qaccelerometer.h>
#include <qsensorbackend.h>

QTM_USE_NAMESPACE

class MyBackend : public QSensorBackend
{
public:
    MyBackend(QSensor *sensor) : QSensorBackend(sensor) {}
    void stop() {}
    void start() {}
    void poll() {}

    static const char *id;
};

#endif
