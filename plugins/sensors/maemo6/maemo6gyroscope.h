#ifndef MAEMO6GYROSCOPE_H
#define MAEMO6GYROSCOPE_H

#include "maemo6sensorbase.h"
#include <qgyroscope.h>
#include <gyroscopesensor_i.h>
#include <datatypes/xyz.h>


QTM_USE_NAMESPACE

class maemo6gyroscope : public maemo6sensorbase
{
    Q_OBJECT

public:
    static char const * const id;
    maemo6gyroscope(QSensor *sensor);

private:
    QGyroscopeReading m_reading;
    static bool m_initDone;
    static const float MILLI;
private slots:
    void slotDataAvailable(const XYZ& data);
};


#endif // MAEMO6GYROSCOPE_H
