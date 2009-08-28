#ifndef AUDIOINPUTDEVICECONTROL_H
#define AUDIOINPUTDEVICECONTROL_H

#include <qaudiodevicecontrol.h>
#include <QtCore/qstringlist.h>

class AudioInputDeviceControl : public QAudioDeviceControl
{
Q_OBJECT
public:
    AudioInputDeviceControl(QObject *parent);
    ~AudioInputDeviceControl();

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
    void updateDevices();

    int m_selectedDevice;
    QStringList m_names;
    QStringList m_descriptions;
};

#endif // AUDIOINPUTDEVICECONTROL_H
