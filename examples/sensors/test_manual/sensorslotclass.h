
///////////////////////////////////////////
//head file
///////////////////////////////////////////

#include <QObject>

class SensorSlotClass:public QObject
{

    Q_OBJECT

public:
    SensorSlotClass();
    virtual ~SensorSlotClass();

public slots:
    void slotOrientationData();
    void slotAccelerationData();
    void slotRotationData();
    void slotMagnetometerData();
    void slotCompassData();

};


