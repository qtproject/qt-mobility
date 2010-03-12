#include "mybackend.h"
#include <qsensorplugin.h>
#include <qsensormanager.h>

const char *MyBackend::id = "mybackend";

//! [Plugin]
class MyPluginClass : public QObject, QSensorPluginInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QSensorPluginInterface)
public:
    void registerSensors()
    {
        QSensorManager::registerBackend(QAccelerometer::type, MyBackend::id, this);
    }

    QSensorBackend *createBackend(QSensor *sensor)
    {
        if (sensor->identifier() == MyBackend::id)
            return new MyBackend(sensor);
        return 0;
    }
};
//! [Plugin]

//Q_EXPORT_PLUGIN2(libmy_plugin_file_name, MyPluginClass);
#include "plugin.moc"
