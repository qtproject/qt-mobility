#include <sensormanagerinterface.h>
#include <datatypes/magneticfield.h>
#include <abstractsensor.h>
#include <abstractsensor_i.h>

int main()
{
    SensorManagerInterface* m_remoteSensorManager;
    m_remoteSensorManager = &SensorManagerInterface::instance();
    QList<DataRange> (AbstractSensorChannelInterface::*func)() = &AbstractSensorChannelInterface::getAvailableIntervals;
    return 0;
}

