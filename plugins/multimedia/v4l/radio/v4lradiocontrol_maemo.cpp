/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "v4lradiocontrol.h"
#include "v4lradioservice.h"

#include <QtCore/qdebug.h>

#include <fcntl.h>

#include <sys/ioctl.h>
#include "linux/videodev2.h"

#include <sys/soundcard.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/videodev.h>
#include <linux/soundcard.h>


V4LRadioControl::V4LRadioControl(QObject *parent)
    : QRadioTunerControl(parent)
    , muted(false)
    , stereo(false)
    , m_error(false)
    , sig(0)
    , currentBand(QRadioTuner::FM)
    , step(100000)
    , scanning(false)
    , fd(1)
{
    if (QDBusConnection::systemBus().isConnected()) {
        FMRCEnablerIFace = new QDBusInterface("de.pycage.FMRXEnabler",
                                             "/de/pycage/FMRXEnabler",
                                             "de.pycage.FMRXEnabler",
                                             QDBusConnection::systemBus());
    }

    keepFMRXEnabled();
    createGstPipeline();
    initRadio();
    initHeadPhone();


    /*struct v4l2_control vctrl;
    vctrl.id = V4L2_CID_AUDIO_MUTE;
    vctrl.value = 0;
    int r = ioctl(fd, VIDIOC_S_CTRL, &vctrl);
    if(r<0)
    {
        qDebug() <<"mute failed";
    }
    else
    {
        qDebug() <<"mute succeeded ";
    }*/

    //setVolume(80);

    timer = new QTimer(this);
    timer->setInterval(200);
    connect(timer,SIGNAL(timeout()),this,SLOT(search()));
    timer->start();

    tickTimer = new QTimer(this);
    tickTimer->setInterval(10000);
    connect(tickTimer,SIGNAL(timeout()),this,SLOT(keepFMRXEnabled()));
    tickTimer->start();
}

V4LRadioControl::~V4LRadioControl()
{
    timer->stop();

    if(fd > 0)
        ::close(fd);
}

void V4LRadioControl::keepFMRXEnabled()
{
    if (FMRCEnablerIFace->isValid()) {
        QDBusMessage ret = FMRCEnablerIFace->call("request");
        // TODO:
    }
}

// Workaround to capture sound from the PGA line and play it back
// because N900 doesn't output sound directly to speakers
bool V4LRadioControl::createGstPipeline()
{
    GstElement *source, *sink;

    gst_init (NULL, NULL);
    pipeline = gst_pipeline_new("my-pipeline");

    source = gst_element_factory_make ("pulsesrc", "source");
    sink = gst_element_factory_make ("pulsesink", "sink");

    gst_bin_add_many (GST_BIN (pipeline), source, sink, NULL);

    if (!gst_element_link_many (source, sink, NULL)) {
        return false;
    }

    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    return true;
}

bool V4LRadioControl::isAvailable() const
{
    return available;
}

QtMedia::AvailabilityError V4LRadioControl::availabilityError() const
{
    if (fd > 0)
        return QtMedia::NoError;
    else
        return QtMedia::ResourceError;
}

QRadioTuner::State V4LRadioControl::state() const
{
    return fd > 0 ? QRadioTuner::ActiveState : QRadioTuner::StoppedState;
}

QRadioTuner::Band V4LRadioControl::band() const
{
    return currentBand;
}

bool V4LRadioControl::isBandSupported(QRadioTuner::Band b) const
{
    QRadioTuner::Band bnd = (QRadioTuner::Band)b;
    switch(bnd) {
        case QRadioTuner::FM:
            if(freqMin <= 87500000 && freqMax >= 108000000)
                return true;
            break;
        case QRadioTuner::LW:
            if(freqMin <= 148500 && freqMax >= 283500)
                return true;
        case QRadioTuner::AM:
            if(freqMin <= 520000 && freqMax >= 1610000)
                return true;
        default:
            if(freqMin <= 1711000 && freqMax >= 30000000)
                return true;
    }

    return false;
}

void V4LRadioControl::setBand(QRadioTuner::Band b)
{
    if(freqMin <= 87500000 && freqMax >= 108000000 && b == QRadioTuner::FM) {
        // FM 87.5 to 108.0 MHz, except Japan 76-90 MHz
        currentBand =  (QRadioTuner::Band)b;
        step = 100000; // 100kHz steps
        emit bandChanged(currentBand);

    } else if(freqMin <= 148500 && freqMax >= 283500 && b == QRadioTuner::LW) {
        // LW 148.5 to 283.5 kHz, 9kHz channel spacing (Europe, Africa, Asia)
        currentBand =  (QRadioTuner::Band)b;
        step = 1000; // 1kHz steps
        emit bandChanged(currentBand);

    } else if(freqMin <= 520000 && freqMax >= 1610000 && b == QRadioTuner::AM) {
        // AM 520 to 1610 kHz, 9 or 10kHz channel spacing, extended 1610 to 1710 kHz
        currentBand =  (QRadioTuner::Band)b;
        step = 1000; // 1kHz steps
        emit bandChanged(currentBand);

    } else if(freqMin <= 1711000 && freqMax >= 30000000 && b == QRadioTuner::SW) {
        // SW 1.711 to 30.0 MHz, divided into 15 bands. 5kHz channel spacing
        currentBand =  (QRadioTuner::Band)b;
        step = 500; // 500Hz steps
        emit bandChanged(currentBand);
    }
    //playTime.restart();
}

int V4LRadioControl::frequency() const
{
    return currentFreq;
}

int V4LRadioControl::frequencyStep(QRadioTuner::Band b) const
{
    int step = 0;

    if(b == QRadioTuner::FM)
        step = 100000; // 100kHz steps
    else if(b == QRadioTuner::LW)
        step = 1000; // 1kHz steps
    else if(b == QRadioTuner::AM)
        step = 1000; // 1kHz steps
    else if(b == QRadioTuner::SW)
        step = 500; // 500Hz steps

    return step;
}

QPair<int,int> V4LRadioControl::frequencyRange(QRadioTuner::Band b) const
{
    if(b == QRadioTuner::AM)
        return qMakePair<int,int>(520000,1710000);
    else if(b == QRadioTuner::FM)
        return qMakePair<int,int>(87500000,108000000);
    else if(b == QRadioTuner::SW)
        return qMakePair<int,int>(1711111,30000000);
    else if(b == QRadioTuner::LW)
        return qMakePair<int,int>(148500,283500);

    return qMakePair<int,int>(0,0);
}

void V4LRadioControl::setFrequency(int frequency)
{
    qint64 f = frequency;

    v4l2_frequency freq;

    if(frequency < freqMin)
        f = freqMax;
    if(frequency > freqMax)
        f = freqMin;

    if(fd > 0) {
        memset( &freq, 0, sizeof( freq ) );
        // Use the first tuner
        freq.tuner = 0;
        if ( ::ioctl( fd, VIDIOC_G_FREQUENCY, &freq ) >= 0 ) {
            if(low) {
                // For low, freq in units of 62.5Hz, so convert from Hz to units.
                freq.frequency = (int)(f/62.5);
            } else {
                // For high, freq in units of 62.5kHz, so convert from Hz to units.
                freq.frequency = (int)(f/62500);
            }
            ::ioctl( fd, VIDIOC_S_FREQUENCY, &freq );
            currentFreq = f;
            emit frequencyChanged(currentFreq);
        }
    }
}

bool V4LRadioControl::isStereo() const
{
    return stereo;
}

QRadioTuner::StereoMode V4LRadioControl::stereoMode() const
{
    return QRadioTuner::Auto;
}

void V4LRadioControl::setStereoMode(QRadioTuner::StereoMode mode)
{
    bool stereo = true;

    if(mode == QRadioTuner::ForceMono)
        stereo = false;

    v4l2_tuner tuner;

    memset( &tuner, 0, sizeof( tuner ) );

    if ( ioctl( fd, VIDIOC_G_TUNER, &tuner ) >= 0 ) {
        if(stereo)
            tuner.audmode = V4L2_TUNER_MODE_STEREO;
        else
            tuner.audmode = V4L2_TUNER_MODE_MONO;

        if ( ioctl( fd, VIDIOC_S_TUNER, &tuner ) >= 0 ) {
            emit stereoStatusChanged(stereo);
        }
    }
}

int V4LRadioControl::signalStrength() const
{
    v4l2_tuner tuner;

    // Return the first tuner founds signal strength.
    for ( int index = 0; index < tuners; ++index ) {
        memset( &tuner, 0, sizeof( tuner ) );
        tuner.index = index;
        if ( ioctl( fd, VIDIOC_G_TUNER, &tuner ) < 0 )
            continue;
        if ( tuner.type != V4L2_TUNER_RADIO )
            continue;
        // percentage signal strength
        return tuner.signal*100/65535;
    }

    return 0;
}

int V4LRadioControl::volume() const
{
    /*v4l2_queryctrl queryctrl;

    if(fd > 0) {
        memset( &queryctrl, 0, sizeof( queryctrl ) );
        queryctrl.id = V4L2_CID_AUDIO_VOLUME;
        if ( ioctl( fd, VIDIOC_QUERYCTRL, &queryctrl ) >= 0 ) {
            if(queryctrl.maximum == 0) {
                return vol;
            } else {
                // percentage volume returned
                return queryctrl.default_value*100/queryctrl.maximum;
            }
        }
    }*/

    return 0;
}

void V4LRadioControl::setVolume(int volume)
{
    int vol;
    vol = (volume / 100.0) * 63; // 63 is the headphone max setting

    callAmixer("HP DAC Playback Volume", QString().setNum(vol)+QString(",")+QString().setNum(vol));
    /*v4l2_queryctrl queryctrl;

    if(fd > 0) {
        memset( &queryctrl, 0, sizeof( queryctrl ) );
        queryctrl.id = V4L2_CID_AUDIO_VOLUME;
        if ( ioctl( fd, VIDIOC_QUERYCTRL, &queryctrl ) >= 0 ) {
            v4l2_control control;You have to capture the sound from the PGA line and play it back.
            qDebug() << "Maximum: " << queryctrl.maximum;
            if(queryctrl.maximum > 0) {
                memset( &control, 0, sizeof( control ) );
                control.id = V4L2_CID_AUDIO_VOLUME;
                control.value = volume*queryctrl.maximum/100;
                qDebug () << "Volume: " << volume*queryctrl.maximum/100;
                ioctl( fd, VIDIOC_S_CTRL, &control );
            } else {
                setVol(volume);
            }
            emit volumeChanged(volume);

        }
    }*/
}

bool V4LRadioControl::isMuted() const
{
    return muted;
}

void V4LRadioControl::setMuted(bool muted)
{
    /*v4l2_queryctrl queryctrl;

    if(fd > 0) {
        memset( &queryctrl, 0, sizeof( queryctrl ) );
        queryctrl.id = V4L2_CID_AUDIO_MUTE;
        if ( ioctl( fd, VIDIOC_QUERYCTRL, &queryctrl ) >= 0 ) {
            v4l2_control control;
            memset( &control, 0, sizeof( control ) );
            control.id = V4L2_CID_AUDIO_MUTE;
            control.value = (muted ? queryctrl.maximum : queryctrl.minimum );
            ioctl( fd, VIDIOC_S_CTRL, &control );
            this->muted = muted;
            emit mutedChanged(muted);
        }
    }*/
}

void V4LRadioControl::initHeadPhone()
{
    QMap<QString, QString> settings;

    settings["Jack Function"] = "Headset";
    settings["Left DAC_L1 Mixer HP Switch"] = "off";
    settings["Right DAC_R1 Mixer HP Switch"] = "off";
    settings["Line DAC Playback Switch"] = "on";
    settings["Line DAC Playback Volume"] = "118,118";
    settings["HPCOM DAC Playback Switch"] = "off";
    settings["Left DAC_L1 Mixer HP Switch"] = "off";
    settings["Left DAC_L1 Mixer Line Switch"] = "on";
    settings["Right DAC_R1 Mixer HP Switch"] = "off";
    settings["Right DAC_R1 Mixer Line Switch"] = "on";
    settings["Speaker Function"] = "Off";

    settings["Input Select"] = "ADC";
    settings["Left PGA Mixer Line1L Switch"] = "off";
    settings["Right PGA Mixer Line1L Switch"] = "off";
    settings["Right PGA Mixer Line1R Switch"] = "off";
    settings["PGA Capture Volume"] = "0,0";

    settings["PGA Capture Switch"] = "on";

    settings["Left PGA Mixer Line2L Switch"] = "on";
    settings["Right PGA Mixer Line2R Switch"] = "on";

    QMapIterator<QString, QString> i(settings);
    while (i.hasNext()) {
        i.next();
        callAmixer(i.key(), i.value());
        qDebug() << i.key() << ":" << i.value();
    }
}

void V4LRadioControl::callAmixer(const QString& target, const QString& value)
{
    // not fully implemented
    // check amixer for implementation details
    int err;
    long tmp;
    unsigned int count;
    static snd_ctl_t *handle = NULL;
    QString card("hw:0");
    snd_ctl_elem_info_t *info;
    snd_ctl_elem_id_t *id;
    snd_ctl_elem_value_t *control;
    snd_ctl_elem_type_t type;

    qDebug() << "start control fun" << target << value;

    snd_ctl_elem_info_alloca(&info);
    snd_ctl_elem_id_alloca(&id);
    snd_ctl_elem_value_alloca(&control);
    snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);	/* MIXER */

    // in amixer parse func
    // char target[64];
    // e.g. PGA CAPTure Switch
    snd_ctl_elem_id_set_name(id, target.toAscii());

    if (handle == NULL && (err = snd_ctl_open(&handle, card.toAscii(), 0)) < 0)
    {
        qDebug() << "Control open error" << card << err;
        //return err;
    }

    snd_ctl_elem_info_set_id(info, id);
    if ((err = snd_ctl_elem_info(handle, info)) < 0)
    {
        qDebug() << "Cannot find the given element from control: " << card;
        snd_ctl_close(handle);
        handle = NULL;
        //return err;
    }

    snd_ctl_elem_info_get_id(info, id);	/* FIXME: Remove it when hctl find works ok !!! */
    type = snd_ctl_elem_info_get_type(info);
    count = snd_ctl_elem_info_get_count(info);
    snd_ctl_elem_value_set_id(control, id);

    tmp = 0;
    for (int idx = 0; idx < count && idx < 128; idx++)
    {
        switch (type)
        {
            case SND_CTL_ELEM_TYPE_BOOLEAN:
                qDebug() << "SND_CTL_ELEM_TYPE_BOOLEAN" << SND_CTL_ELEM_TYPE_BOOLEAN;
                if ((value == "on") ||(value == "1"))
                {
                    qDebug() << "Set value On";
                    tmp = 1;
                }
                snd_ctl_elem_value_set_boolean(control, idx, tmp);
                break;
            case SND_CTL_ELEM_TYPE_ENUMERATED:
                qDebug() << "SND_CTL_ELEM_TYPE_ENUMERATED" << SND_CTL_ELEM_TYPE_ENUMERATED;
                tmp = getEnumItemIndex(handle, info, value);
                if (tmp < 0)
                {
                    qDebug() << "Enum value not found, Check control  !!!!!!!!!!!!! " << target << value;

                }
                snd_ctl_elem_value_set_enumerated(control, idx, tmp);
                break;
            case SND_CTL_ELEM_TYPE_INTEGER:
                qDebug() << "SND_CTL_ELEM_TYPE_INTEGER" << SND_CTL_ELEM_TYPE_INTEGER;
                tmp = atoi(value.toAscii());
                qDebug() << "Value to Set" << tmp;
                if (tmp <  snd_ctl_elem_info_get_min(info))
                    tmp = snd_ctl_elem_info_get_min(info);
                else if (tmp > snd_ctl_elem_info_get_max(info))
                    tmp = snd_ctl_elem_info_get_max(info);
                snd_ctl_elem_value_set_integer(control, idx, tmp);
                break;
            default:
                qDebug() << "!!!!!!!!!!!!!!!! Unknown type" << type;
                break;

        }
    }

    if ((err = snd_ctl_elem_write(handle, control)) < 0) {
        qDebug() << "Control element write error" << card ;
        snd_ctl_close(handle);
        handle = NULL;
        //return err;
    }

    snd_ctl_close(handle);
    handle = NULL;
    //return 0;
}


int V4LRadioControl::getEnumItemIndex(snd_ctl_t *handle, snd_ctl_elem_info_t *info,
       const  QString &value)
{
    int items, i, len;

    items = snd_ctl_elem_info_get_items(info);
    if (items <= 0)
        return -1;

    for (i = 0; i < items; i++)
    {
        snd_ctl_elem_info_set_item(info, i);
        if (snd_ctl_elem_info(handle, info) < 0)
            return -1;
        QString name = snd_ctl_elem_info_get_item_name(info);
        qDebug() << " Enum loop: check" << name << " to" << value ;
        if(name == value)
        {
            qDebug() << " Enum found";
            return i;
        }
    }
    return -1;
}

bool V4LRadioControl::isSearching() const
{
    //TODO
    return false;
}

void V4LRadioControl::cancelSearch()
{
    //TODO
}

void V4LRadioControl::searchForward()
{
    // Scan up
    if(scanning) {
        scanning = false;
        return;
    }
    scanning = true;
    forward  = true;
}

void V4LRadioControl::searchBackward()
{
    // Scan down
    if(scanning) {
        scanning = false;
        return;
    }
    scanning = true;
    forward  = false;
    timer->start();
}

void V4LRadioControl::start()
{
}

void V4LRadioControl::stop()
{
}

QRadioTuner::Error V4LRadioControl::error() const
{
    if(m_error)
        return QRadioTuner::OpenError;

    return QRadioTuner::NoError;
}

QString V4LRadioControl::errorString() const
{
    return QString();
}

void V4LRadioControl::search()
{
    int signal = signalStrength();
    if(sig != signal) {
        sig = signal;
        emit signalStrengthChanged(sig);
    }

    if(!scanning) return;

    if(forward) {
        setFrequency(currentFreq+step);
    } else {
        setFrequency(currentFreq-step);
    }
    emit signalStrengthChanged(signalStrength());
}

bool V4LRadioControl::initRadio()
{
    v4l2_tuner tuner;
    v4l2_input input;
    v4l2_frequency freq;
    v4l2_capability cap;

    low = false;
    available = false;
    freqMin = freqMax = currentFreq = 0;

    fd = ::open("/dev/radio1", O_RDWR);

    if(fd != -1) {
        // Capabilites
        memset( &cap, 0, sizeof( cap ) );
        if(::ioctl(fd, VIDIOC_QUERYCAP, &cap ) >= 0) {
            if(((cap.capabilities & V4L2_CAP_RADIO) == 0) && ((cap.capabilities & V4L2_CAP_AUDIO) == 0))
                available = true;
        // TODO:  qDebug() << "Available: " << available;
        }

        tuner.index = 0;

        if ( ioctl( fd, VIDIOC_G_TUNER, &tuner ) < 0 ) {
            // TODO:
        }

        qDebug() << "index:         " << tuner.index;
        qDebug() << "type:          " << tuner.type << ((tuner.type==V4L2_TUNER_RADIO)?"radio":"not radio");
        qDebug() << "name:          " << QString((const char*)tuner.name);
        qDebug() << "capability:    " << QString().setNum(tuner.capability,16);
        qDebug() << "range low:     " << tuner.rangelow;
        qDebug() << "range high:    " << tuner.rangehigh;
        qDebug() << "rxsubchans:    " << tuner.rxsubchans;
        qDebug() << "audmode:       " << tuner.audmode;
        qDebug() << "signal:        " << tuner.signal;
        qDebug() << "afc:           " << tuner.afc;

        if ( tuner.type != V4L2_TUNER_RADIO )
            return false;

        if ( ( tuner.capability & V4L2_TUNER_CAP_LOW ) != 0 ) {
            // Units are 1/16th of a kHz.
            low = true;
        }

        if(low) {
            freqMin = tuner.rangelow * 62.5;
            freqMax = tuner.rangehigh * 62.5;
        } else {
            freqMin = tuner.rangelow * 62500;
            freqMax = tuner.rangehigh * 62500;
        }

        // frequency
        memset( &freq, 0, sizeof( freq ) );

        if(::ioctl(fd, VIDIOC_G_FREQUENCY, &freq ) >= 0) {
            if ( ((int)freq.frequency) != -1 ) {    // -1 means not set.
                if(low)
                    currentFreq = freq.frequency * 62.5;
                else
                    currentFreq = freq.frequency * 62500;
            }
        }

        qWarning()<<"min="<<freqMin<<", max="<<freqMax<<", current="<<currentFreq;

        // stereo
        bool stereo = false;
        memset( &tuner, 0, sizeof( tuner ) );
        if ( ioctl( fd, VIDIOC_G_TUNER, &tuner ) >= 0 ) {
            if((tuner.rxsubchans & V4L2_TUNER_SUB_STEREO) != 0)
                stereo = true;
        }

        vol = getVol();

        return true;
    }

    m_error = true;
    emit error();

    return false;
}

//void V4LRadioControl::setVol(int v)
//{


    /*qDebug() << "setVolume";
    int fd = ::open( "/dev/mixer", O_RDWR, 0 );
    if ( fd < 0 ) {
        qDebug() << "mixer not opened";
        return;
    }
    int volume = v;
    if ( volume < 0 )
        volume = 0;
    else if ( volume > 100 )
        volume = 100;
    vol = volume;
    volume += volume << 8;
    ::ioctl( fd, MIXER_WRITE(SOUND_MIXER_VOLUME), &volume );
    ::close( fd );*/
//}

int V4LRadioControl::getVol()
{
    /*int fd = ::open( "/dev/mixer", O_RDWR, 0 );
    if ( fd >= 0 ) {
        int volume = 0;
        ::ioctl( fd, MIXER_READ(SOUND_MIXER_VOLUME), &volume );
        int left = ( volume & 0xFF );
        int right = ( ( volume >> 8 ) & 0xFF );
        if ( left > right )
            vol = left;
        else
            vol = right;
        ::close( fd );
        return vol;
    }*/

    return 0;
}










