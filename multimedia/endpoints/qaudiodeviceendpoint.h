#ifndef QAUDIODEVICEENDPOINT_H
#define QAUDIODEVICEENDPOINT_H

#include <QtCore/qplugin.h>

class QAudioDeviceEndpointInterface
{
public:
    virtual ~QAudioDeviceEndpointInterface();
};

#define QAudioDeviceEndpointInterface_iid "com.nokia.Qt.QAudioDeviceEndpointInterface/1.0"

Q_DECLARE_INTERFACE(QAudioDeviceEndpointInterface, QAudioDeviceEndpointInterface_iid)

class QAudioDeviceEndpointPrivate;

class QAudioDeviceEndpoint : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Direction directionFilter READ directionFilter WRITE setDirectionFilter)
    Q_PROPERTY(Roles roleFilter READ roleFilter WRITE setRoleFilter)
    Q_PROPERTY(FormFactors formFactorFilter READ formFactorFilter WRITE setFormFactorFilter)
    Q_PROPERTY(int selectedDevice READ selectedDevice WRITE setSelectedDevice NOTIFY selectedDeviceChanged)
    Q_ENUMS(Direction Role FormFactor)
    Q_FLAGS(Roles FormFactors)
    Q_DECLARE_PRIVATE(QAudioDeviceEndpoint)
public:
    enum Direction
    {
        InputDevice = 0x01,
        OutputDevice = 0x02,
        InputOutputDevice = InputDevice | OutputDevice
    };

    enum Role
    {
        ConsoleRole = 0x01,
        MultimediaRole = 0x02,
        CommunicationsRole = 0x04,
        AllRoles = ~0

    };

    Q_DECLARE_FLAGS(Roles, Role);

    enum FormFactor
    {
        UnknownFormFactor = 0x0000,
        RemoteNetworkDevice = 0x0001,
        Speakers = 0x0002,
        LineLevel = 0x0004,
        Headphones = 0x0008,
        Microphone = 0x0010,
        Headset = 0x0020,
        Handset = 0x0040,
        UnknownDigitalPassthrough = 0x0080,
        SPDIF = 0x0100,
        HDMI = 0x0200,
        AllFormFactors = ~0
    };

    Q_DECLARE_FLAGS(FormFactors, FormFactor)

    QAudioDeviceEndpoint(QObject *parent = 0);
    ~QAudioDeviceEndpoint();

    Direction directionFilter() const;
    virtual void setDirectionFilter(Direction direction);

    Roles roleFilter() const;
    virtual void setRoleFilter(Roles roles);

    FormFactors formFactorFilter() const;
    virtual void setFormFactorFilter(FormFactors forms);

    virtual int deviceCount() const = 0;

    virtual int direction(int index) const = 0;
    virtual Roles roles(int index) const = 0;
    virtual FormFactor formFactor(int index) const = 0;

    virtual int defaultInputDevice(Role role) const = 0;
    virtual int defaultOutputDevice(Role role) const = 0;

    int selectedDevice() const;

public Q_SLOTS:
    virtual void setSelectedDevice(int index);

Q_SIGNALS:
    void selectedDeviceChanged(int index);
    void devicesInserted(int index, int count);
    void devicesRemoved(int index, int count);
    void devicesChanged(int index, int count);
};

#endif
