#ifndef QGSTREAMERAUDIOINPUTDEVICECONTROL_H
#define QGSTREAMERAUDIOINPUTDEVICECONTROL_H

#include <qaudiodevicecontrol.h>
#include <QtCore/qstringlist.h>

class QGstreamerAudioInputDeviceControl : public QAudioDeviceControl
{
Q_OBJECT
public:
    QGstreamerAudioInputDeviceControl(QObject *parent);
    ~QGstreamerAudioInputDeviceControl();

    int deviceCount() const;

    QString name(int index) const;
    QString description(int index) const;
    QIcon icon(int index) const;

    int defaultDevice() const;
    int selectedDevice() const;

public Q_SLOTS:
    void setSelectedDevice(int index);

private:
    void update();
    void updateAlsaDevices();
    void updateOssDevices();

    int m_selectedDevice;
    QStringList m_names;
    QStringList m_descriptions;
};

#endif // QGSTREAMERAUDIOINPUTDEVICECONTROL_H
