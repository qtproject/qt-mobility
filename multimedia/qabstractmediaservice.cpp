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

#include "qabstractmediaservice.h"
#include "qabstractmediaservice_p.h"

#include "qaudiodevicecontrol.h"
#include "qvideodevicecontrol.h"

#include <QtCore/qtimer.h>

/*!
    \class QAbstractMediaService
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

void QAbstractMediaService::setInputStream(QIODevice* stream)
{
    d_ptr->inputStream = stream;
}

QIODevice* QAbstractMediaService::inputStream() const
{
    return d_ptr->inputStream;
}

void QAbstractMediaService::setOutputStream(QIODevice* stream)
{
    d_ptr->outputStream = stream;
}

QIODevice* QAbstractMediaService::outputStream() const
{
    return d_ptr->outputStream;
}

/*!
    Returns the currently active endpoint id for \endpointType.
*/

QString QAbstractMediaService::activeEndpoint(QAbstractMediaService::MediaEndpoint endpointType)
{
    QString ret;
    int idx = 0;

    QAudioDeviceControl *audioControl = control<QAudioDeviceControl *>();
    QVideoDeviceControl *videoControl = control<QVideoDeviceControl *>();

    switch(endpointType) {
        case QAbstractMediaService::AudioInput:
            if(audioControl) {
                idx = audioControl->selectedDevice();
                ret = audioControl->name(idx);
            }
            break;
        case QAbstractMediaService::AudioOutput:
            if(audioControl) {
                idx = audioControl->selectedDevice();
                ret = audioControl->name(idx);
            }
            break;
        case QAbstractMediaService::VideoInput:
            if(videoControl) {
                idx = videoControl->selectedDevice();
                ret = videoControl->name(idx);
            }
            break;
        case QAbstractMediaService::VideoOutput:
            if(videoControl) {
                idx = videoControl->selectedDevice();
                ret = videoControl->name(idx);
            }
            break;
        default:
            ret.clear();
    }

    return ret;
}

/*!
    Sets the active endpoint for \a endpointType to \a endpoint
*/

void QAbstractMediaService::setActiveEndpoint(QAbstractMediaService::MediaEndpoint endpointType, const QString& endpoint)
{
    int numDevices = 0;

    QAudioDeviceControl *audioControl = control<QAudioDeviceControl *>();
    QVideoDeviceControl *videoControl = control<QVideoDeviceControl *>();

    switch(endpointType) {
        case QAbstractMediaService::AudioInput:
            if(audioControl) {
                numDevices = audioControl->deviceCount();
                for(int i=0;i<numDevices;i++) {
                    if(qstrcmp(endpoint.toLocal8Bit().constData(),audioControl->name(i).toLocal8Bit().constData()) == 0) {
                        audioControl->setSelectedDevice(i);
                        break;
                    }
                }
            }
            break;
        case QAbstractMediaService::AudioOutput:
            if(audioControl) {
                numDevices = audioControl->deviceCount();
                for(int i=0;i<numDevices;i++) {
                    if(qstrcmp(endpoint.toLocal8Bit().constData(),audioControl->name(i).toLocal8Bit().constData()) == 0) {
                        audioControl->setSelectedDevice(i);
                        break;
                    }
                }
            }
            break;
        case QAbstractMediaService::VideoInput:
            if(videoControl) {
                numDevices = videoControl->deviceCount();
                for(int i=0;i<numDevices;i++) {
                    if(qstrcmp(endpoint.toLocal8Bit().constData(),videoControl->name(i).toLocal8Bit().constData()) == 0) {
                        videoControl->setSelectedDevice(i);
                        break;
                    }
                }
            }
            break;
        case QAbstractMediaService::VideoOutput:
            if(videoControl) {
                numDevices = videoControl->deviceCount();
                for(int i=0;i<numDevices;i++) {
                    if(qstrcmp(endpoint.toLocal8Bit().constData(),videoControl->name(i).toLocal8Bit().constData()) == 0) {
                        videoControl->setSelectedDevice(i);
                        break;
                    }
                }
            }
            break;
        default:
            return;
    }
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


