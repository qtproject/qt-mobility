#include <sensormanagerinterface.h>
#include <datatypes/magneticfield.h>

int main()
{
    SensorManagerInterface* m_remoteSensorManager;
    m_remoteSensorManager = &SensorManagerInterface::instance();
    return 0;
}

