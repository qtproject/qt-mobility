#include "qgstreameraudioinputdevicecontrol.h"

#include <QtGui/QIcon>
#include <QtCore/QDir>
#include <QtCore/QDebug>

#ifdef HAVE_ALSA
#include <alsa/asoundlib.h>
#endif

QGstreamerAudioInputDeviceControl::QGstreamerAudioInputDeviceControl(QObject *parent)
    :QAudioDeviceControl(parent)
{
    update();
}

QGstreamerAudioInputDeviceControl::~QGstreamerAudioInputDeviceControl()
{
}

int QGstreamerAudioInputDeviceControl::deviceCount() const
{
    return m_names.size();
}

QString QGstreamerAudioInputDeviceControl::name(int index) const
{
    return m_names[index];
}

QString QGstreamerAudioInputDeviceControl::description(int index) const
{
    return m_descriptions[index];
}

QIcon QGstreamerAudioInputDeviceControl::icon(int index) const
{
    Q_UNUSED(index);
    return QIcon();
}

int QGstreamerAudioInputDeviceControl::defaultDevice() const
{
    return 0;
}

int QGstreamerAudioInputDeviceControl::selectedDevice() const
{
    return m_selectedDevice;
}


void QGstreamerAudioInputDeviceControl::setSelectedDevice(int index)
{
    if (index != m_selectedDevice) {
        m_selectedDevice = index;
        emit selectedDeviceChanged(index);
        emit selectedDeviceChanged(name(index));
    }
}


void QGstreamerAudioInputDeviceControl::update()
{
    m_names.clear();
    m_descriptions.clear();
    updateAlsaDevices();
    updateOssDevices();
}

void QGstreamerAudioInputDeviceControl::updateAlsaDevices()
{
#ifdef HAVE_ALSA
    void **hints, **n;
    if (snd_device_name_hint(-1, "pcm", &hints) < 0) {
        qWarning()<<"no alsa devices available";
        return;
    }
    n = hints;

    while (*n != NULL) {
        char *name = snd_device_name_get_hint(*n, "NAME");
        char *descr = snd_device_name_get_hint(*n, "DESC");
        char *io = snd_device_name_get_hint(*n, "IOID");

        if ((name != NULL) && (descr != NULL)) {
            if ( io == NULL || qstrcmp(io,"Input") == 0 ) {
                m_names.append(QLatin1String("alsa:")+QString::fromUtf8(name));
                m_descriptions.append(QString::fromUtf8(descr));
            }
        }

        if (name != NULL)
            free(name);
        if (descr != NULL)
            free(descr);
        if (io != NULL)
            free(io);
        n++;
    }
    snd_device_name_free_hint(hints);
#endif
}

void QGstreamerAudioInputDeviceControl::updateOssDevices()
{
    QDir devDir("/dev");

    QStringList entries = QString("dsp dsp0 dsp1 dsp2 dsp3 dsp4 dsp5 dsp6 dsp7 dsp8 dsp9").split(' ');

    foreach (const QString &entry, entries) {
        if (devDir.exists(entry)) {
            QString device = devDir.absoluteFilePath(entry);

            m_names.append(QLatin1String("oss:")+device);
            m_descriptions.append(QString("OSS device %1").arg(device));
        }
    }
}

