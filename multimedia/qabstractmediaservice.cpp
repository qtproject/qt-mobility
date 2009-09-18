/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <multimedia/qabstractmediaservice.h>
#include <multimedia/qabstractmediaservice_p.h>

#include <multimedia/qaudiodevicecontrol.h>
#include <multimedia/qvideodevicecontrol.h>

#include <QtCore/qtimer.h>

/*!
    \class QAbstractMediaService
    \brief The QAbstractMediaService class provides a common base class for media service
    implementations.
    \preliminary
*/

/*!
    \enum QAbstractMediaService::MediaEndpoint

    Enumerates the possible end points a media service may have.

    \value VideoInput A video input end point.
    \value VideoOutput A video output end point.
    \value AudioInput An audio input end point.
    \value AudioOutput An audio output end point.
    \value StreamInput A data stream input end point
    \value StreamOutput A data stream output end point.
*/

/*!
    Construct a QAbstractMediaService with \a parent. This class is meant as a
    base class for Multimedia services so this constructor is protected.
*/

QAbstractMediaService::QAbstractMediaService(QObject *parent)
    : QObject(parent)
    , d_ptr(new QAbstractMediaServicePrivate)
{
    d_ptr->q_ptr = this;
}

/*!
    \internal
*/
QAbstractMediaService::QAbstractMediaService(QAbstractMediaServicePrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

/*!
    Destroys a media service.
*/

QAbstractMediaService::~QAbstractMediaService()
{
    delete d_ptr;
}

/*!
    Return true if \a endpointType is available.
*/

bool QAbstractMediaService::isEndpointSupported(QAbstractMediaService::MediaEndpoint endpointType)
{
    QAudioDeviceControl *audioControl = control<QAudioDeviceControl *>();
    QVideoDeviceControl *videoControl = control<QVideoDeviceControl *>();

    switch(endpointType) {
        case QAbstractMediaService::AudioInput:
        case QAbstractMediaService::AudioOutput:
            if(audioControl) return true;
            break;
        case QAbstractMediaService::VideoInput:
        case QAbstractMediaService::VideoOutput:
            if(videoControl) return true;
            break;
        case QAbstractMediaService::StreamInput:
        case QAbstractMediaService::StreamOutput:
            return true;
        default:
            return false;
    }
    return false;
}

/*!
    Sets the data input \a stream of a media service.
*/
void QAbstractMediaService::setInputStream(QIODevice* stream)
{
    d_ptr->inputStream = stream;
}

/*!
    Returns the data input stream a media service.
*/
QIODevice* QAbstractMediaService::inputStream() const
{
    return d_ptr->inputStream;
}

/*!
    Sets the data output \a stream of a media service.
*/
void QAbstractMediaService::setOutputStream(QIODevice* stream)
{
    d_ptr->outputStream = stream;
}

/*!
    Returns the data output stream of a media service.
*/
QIODevice* QAbstractMediaService::outputStream() const
{
    return d_ptr->outputStream;
}

/*!
    Returns a list of currently active endpoints for \a endpointType.
*/

QList<QString> QAbstractMediaService::activeEndpoints(QAbstractMediaService::MediaEndpoint endpointType)
{
    QList<QString> ret;
    int idx = 0;

    QAudioDeviceControl *audioControl = control<QAudioDeviceControl *>();
    QVideoDeviceControl *videoControl = control<QVideoDeviceControl *>();

    switch(endpointType) {
        case QAbstractMediaService::AudioInput:
            if(audioControl) {
                idx = audioControl->selectedDevice();
                ret << audioControl->name(idx);
            }
            break;
        case QAbstractMediaService::AudioOutput:
            if(audioControl) {
                idx = audioControl->selectedDevice();
                ret << audioControl->name(idx);
            }
            break;
        case QAbstractMediaService::VideoInput:
            if(videoControl) {
                idx = videoControl->selectedDevice();
                ret << videoControl->name(idx);
            }
            break;
        case QAbstractMediaService::VideoOutput:
            if(videoControl) {
                idx = videoControl->selectedDevice();
                ret << videoControl->name(idx);
            }
            break;
        default:
            ret.clear();
    }

    return ret;
}

/*!
    Returns true if set of the active endpoint for \a endpointType to \a endpoint succeeds.
*/

bool QAbstractMediaService::setActiveEndpoint(QAbstractMediaService::MediaEndpoint endpointType, const QString& endpoint)
{
    int numDevices = 0;

    QAudioDeviceControl *audioControl = control<QAudioDeviceControl *>();
    QVideoDeviceControl *videoControl = control<QVideoDeviceControl *>();

    switch(endpointType) {
        case QAbstractMediaService::AudioInput:
        case QAbstractMediaService::AudioOutput:
            if(audioControl) {
                numDevices = audioControl->deviceCount();
                for(int i=0;i<numDevices;i++) {
                    if (endpoint == audioControl->name(i)) {
                        audioControl->setSelectedDevice(i);
                        return true;
                    }
                }
            }
            break;
        case QAbstractMediaService::VideoInput:
        case QAbstractMediaService::VideoOutput:
            if(videoControl) {
                numDevices = videoControl->deviceCount();
                for(int i=0;i<numDevices;i++) {
                    if (endpoint == videoControl->name(i)) {
                        videoControl->setSelectedDevice(i);
                        return true;
                    }
                }
            }
            break;
        default:
            return false;
    }
    return false;
}

/*!
    Returns the description of the \a endpoint for the \a endpointType.
*/

QString QAbstractMediaService::endpointDescription(QAbstractMediaService::MediaEndpoint endpointType, const QString& endpoint)
{
    QString desc;
    //TODO
    return desc;
}

/*!
    Returns a list of endpoints available for the \a endpointType.
*/

QList<QString> QAbstractMediaService::supportedEndpoints(QAbstractMediaService::MediaEndpoint endpointType) const
{
    int numDevices = 0;
    QList<QString> list;

    QAudioDeviceControl *audioControl = control<QAudioDeviceControl *>();
    QVideoDeviceControl *videoControl = control<QVideoDeviceControl *>();

    if(endpointType == QAbstractMediaService::AudioInput && audioControl) {
        numDevices = audioControl->deviceCount();
        for(int i=0;i<numDevices;i++)
            list.append(audioControl->name(i));

    } else if(endpointType == QAbstractMediaService::VideoInput && videoControl) {
        numDevices = videoControl->deviceCount();
        for(int i=0;i<numDevices;i++)
            list.append(videoControl->name(i));

    } else if(endpointType == QAbstractMediaService::AudioOutput && audioControl) {
        numDevices = audioControl->deviceCount();
        for(int i=0;i<numDevices;i++)
            list.append(audioControl->name(i));

    } else if(endpointType == QAbstractMediaService::VideoOutput && videoControl) {
        numDevices = videoControl->deviceCount();
        for(int i=0;i<numDevices;i++)
            list.append(videoControl->name(i));
    }

    return list;
}

/*!
    \fn QAbstractMediaService::control(const char *interface) const

    Returns a pointer to the media control implementing \a interface.

    If the service does not implement the control a null pointer is returned instead.
*/

/*!
    \fn QAbstractMediaService::control() const

    Returns a pointer to the media control of type T implemented by a media service.

    If the service does not implment the control a null pointer is returned instead.
*/
