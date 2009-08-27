#include "audioinputdevicecontrol.h"

#include <QtGui/QIcon>
#include <QtCore/QDir>
#include <QtCore/QDebug>

#include <QtMultimedia/qaudiodeviceinfo.h>


AudioInputDeviceControl::AudioInputDeviceControl(QObject *parent)
    :QAudioInputDeviceControl(parent)
{
    update();
}

AudioInputDeviceControl::~AudioInputDeviceControl()
{
}

int AudioInputDeviceControl::deviceCount() const
{
    return m_names.size();
}

QString AudioInputDeviceControl::name(int index) const
{
    return m_names[index];
}

QString AudioInputDeviceControl::description(int index) const
{
    return m_descriptions[index];
}

QIcon AudioInputDeviceControl::icon(int index) const
{
    Q_UNUSED(index);
    return QIcon();
}

int AudioInputDeviceControl::defaultDevice() const
{
    return 0;
}

int AudioInputDeviceControl::selectedDevice() const
{
    return m_selectedDevice;
}


void AudioInputDeviceControl::setSelectedDevice(int index)
{
    if (index != m_selectedDevice) {
        m_selectedDevice = index;
        update();
        emit selectedDeviceChanged(index);
        emit selectedDeviceChanged(name(index));
    }
}


void AudioInputDeviceControl::update()
{
    m_names.clear();
    m_descriptions.clear();
    updateDevices();
}

void AudioInputDeviceControl::updateDevices()
{
    m_names.clear();
    m_descriptions.clear();

    QList<QAudioDeviceId> devices = QAudioDeviceInfo::deviceList(QAudio::AudioInput);
    qWarning()<<"devices="<<devices.size();
    for(int i = 0; i < devices.size(); ++i) {
        m_names.append(QAudioDeviceInfo(devices.at(i)).deviceName());
        m_descriptions.append(QAudioDeviceInfo(devices.at(i)).deviceName());
    }
}



