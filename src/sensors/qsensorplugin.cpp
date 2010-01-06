#include <qsensorplugin.h>

/*!
    \class QSensorPluginInterface
    \ingroup sensors_backend
    \brief The QSensorPluginInterface class is the pure virtual interface to sensor plugins.

    The QSensorPluginInterface class is implemented in sensor plugins to register sensor
    backends with QSensorManager.

    \sa {Creating a sensor plugin}
*/

/*!
    \fn QSensorPluginInterface::registerSensors()

    This function is called when the plugin is loaded. The plugin should register
    sensor backends by calling QSensorManager::registerBackend().

    \sa {Creating a sensor plugin}
*/

