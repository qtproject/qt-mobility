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

#include <QIcon>
#include <qmediaobject_p.h>
#include <qaudiocapturesource.h>
#include <qaudiodevicecontrol.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QAudioCaptureSource
    \brief The QAudioCaptureSource class provides an interface to query and select an audio input device.
    \ingroup multimedia

    \preliminary

    QAudioCaptureSource provides access to the audio devices available on your system.

    You can query these devices and select one to use.

    A typical implementation example:
    \code
        QAudioCaptureSource* audiocapturesource = new QAudioCaptureSource;
        QMediaRecorder* capture = new QMediaRecorder(audiocapturesource);
    \endcode

    The audiocapturesource interface is then used to:

    - Get and Set the audio device to use.

    The capture interface is then used to:

    - Set the destination using setOutputLocation()

    - Set the format parameters using setAudioCodec(),

    - Control the recording using record(),stop()

    \sa QMediaRecorder
*/

class QAudioCaptureSourcePrivate : public QMediaObjectPrivate
{
public:
    Q_DECLARE_PUBLIC(QAudioCaptureSource)

    void initControls()
    {
        Q_Q(QAudioCaptureSource);

        if (service != 0)
            audioDeviceControl = qobject_cast<QAudioDeviceControl*>(service->control(QAudioDeviceControl_iid));

        if (audioDeviceControl) {
            q->connect(audioDeviceControl, SIGNAL(selectedDeviceChanged(int)),
                       SIGNAL(selectedDeviceChanged(int)));
            q->connect(audioDeviceControl, SIGNAL(selectedDeviceChanged(QString)),
                       SIGNAL(selectedDeviceChanged(QString)));
            q->connect(audioDeviceControl, SIGNAL(devicesChanged()),
                       SIGNAL(devicesChanged()));
        }
    }

    QAudioCaptureSourcePrivate():provider(0), audioDeviceControl(0) {}
    QMediaServiceProvider *provider;
    QAudioDeviceControl   *audioDeviceControl;


};

/*!
    Construct a QAudioCaptureSource using the QMediaService from \a provider, with \a parent.
*/

QAudioCaptureSource::QAudioCaptureSource(QObject *parent, QMediaServiceProvider *provider):
    QMediaObject(*new QAudioCaptureSourcePrivate, parent, provider->requestService(Q_MEDIASERVICE_AUDIOSOURCE))
{
    Q_D(QAudioCaptureSource);

    d->provider = provider;
    d->initControls();
}

QAudioCaptureSource::QAudioCaptureSource(QMediaObject *mediaObject, QObject *parent)
    :QMediaObject(*new QAudioCaptureSourcePrivate, parent, mediaObject->service())
{
    Q_D(QAudioCaptureSource);

    d->provider = 0;
    d->initControls();
}

/*!
    Destroys the audiocapturesource object.
*/

QAudioCaptureSource::~QAudioCaptureSource()
{
    Q_D(QAudioCaptureSource);

    if (d->provider)
        d->provider->releaseService(d->service);
}

/*!
    Returns true if the audio capture service is available, otherwise returns false.
*/
bool QAudioCaptureSource::isAvailable() const
{
    return d_func()->service != NULL;
}


/*!
    Returns the number of audio input devices available.
*/

int QAudioCaptureSource::deviceCount() const
{
    Q_D(const QAudioCaptureSource);

    if(d->audioDeviceControl)
        return d->audioDeviceControl->deviceCount();

    return 0;
}

/*!
    Returns the name of the audio input device at \a index.
*/

QString QAudioCaptureSource::deviceName(int index) const
{
    Q_D(const QAudioCaptureSource);

    QString text;

    if(d->audioDeviceControl)
        text = d->audioDeviceControl->deviceName(index);

    return text;
}

/*!
    Returns the description of the audio input device at \a index.
*/

QString QAudioCaptureSource::deviceDescription(int index) const
{
    Q_D(const QAudioCaptureSource);

    QString text;

    if(d->audioDeviceControl)
        text = d->audioDeviceControl->deviceDescription(index);

    return text;
}

/*!
    Returns the icon for the audio input device at \a index.
*/

QIcon QAudioCaptureSource::deviceIcon(int index) const
{
    Q_D(const QAudioCaptureSource);

    if(d->audioDeviceControl)
        return d->audioDeviceControl->deviceIcon(index);

    return QIcon();
}

/*!
    Returns the default audio input devices index.
*/

int QAudioCaptureSource::defaultDevice() const
{
    Q_D(const QAudioCaptureSource);

    if(d->audioDeviceControl)
        return d->audioDeviceControl->defaultDevice();

    return 0;
}

/*!
    Returns the index of the currently selected audio input device.
*/

int QAudioCaptureSource::selectedDevice() const
{
    Q_D(const QAudioCaptureSource);

    if(d->audioDeviceControl)
        return d->audioDeviceControl->selectedDevice();

    return 0;
}

/*!
    Sets the audio input device to \a index.
*/

void QAudioCaptureSource::setSelectedDevice(int index)
{
    Q_D(const QAudioCaptureSource);

    if(d->audioDeviceControl)
        d->audioDeviceControl->setSelectedDevice(index);
}

/*!
    \fn QAudioCaptureSource::selectedDeviceChanged(int index)

    Signal emitted when selected audio device changes to \a index.
*/

/*!
    \fn QAudioCaptureSource::selectedDeviceChanged(const QString &deviceName)

    Signal emitted when selected audio device changes to \a deviceName.
*/

/*!
    \fn QAudioCaptureSource::devicesChanged()

    Signal is emitted when the available audio input devices has changed.
*/
#include "moc_qaudiocapturesource.cpp"
QTM_END_NAMESPACE

