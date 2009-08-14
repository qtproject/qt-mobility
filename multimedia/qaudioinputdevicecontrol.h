#ifndef QAUDIOINPUTDEVICECONTROL_H
#define QAUDIOINPUTDEVICECONTROL_H

#include <qabstractmediacontrol.h>

class QAudioInputDeviceControl : public QAbstractMediaControl
{
    Q_OBJECT
    Q_PROPERTY(int selectedDevice READ selectedDevice WRITE setSelectedDevice NOTIFY selectedDeviceChanged)
public:
    virtual ~QAudioInputDeviceControl();

    virtual int deviceCount() const = 0;

    virtual QString name(int index) const = 0;
    virtual QString description(int index) const = 0;
    virtual QIcon icon(int index) const = 0;

    virtual int defaultDevice() const = 0;
    virtual int selectedDevice() const = 0;

public Q_SLOTS:
    virtual void setSelectedDevice(int index) = 0;

Q_SIGNALS:
    void selectedDeviceChanged(int index);
    void selectedDeviceChanged(const QString &deviceName);
    void devicesChanged();

protected:
    QAudioInputDeviceControl(QObject *parent);
};

#define QAudioInputDeviceControl_iid "com.nokia.qt.AudioInputDeviceControl"
Q_MEDIA_DECLARE_CONTROL(QAudioInputDeviceControl, QAudioInputDeviceControl_iid)


#endif // QAUDIOINPUTDEVICECONTROL_H
