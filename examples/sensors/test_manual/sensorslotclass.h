
///////////////////////////////////////////
//head file
///////////////////////////////////////////

#include <QObject>
#include <qsensor.h>
#include <qaccelerometer.h>

QTM_USE_NAMESPACE

class SensorSlotClass:public QObject
{

    Q_OBJECT

public:
    SensorSlotClass();
    virtual ~SensorSlotClass();

    void checkRange(QSensor* sensor, qreal value);
    void checkRate(QSensor* sensor, int &exTimestamp);
    void checkResolution(QSensor* sensor, QAccelerometerReading* reading);


private slots:
    void slotOrientationData();
    void slotAccelerationData();
    void slotRotationData();
    void slotMagnetometerData();
    void slotCompassData();

private:
    void checkDiff(qreal diff, qreal resolution, QString msg);
    int m_accelerometerTimestamp;
    int m_magnetometerTimestamp;
    int m_orientationTimestamp;
    int m_rotationTimestamp;
    int m_compassTimestamp;

    qreal m_x, m_y, m_z;


};


