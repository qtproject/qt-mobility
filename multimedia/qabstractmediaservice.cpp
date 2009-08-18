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

#include <QtCore/qtimer.h>

class QAbstractMediaServicePrivate
{
public:
    QAbstractMediaServicePrivate()
        : audioOutput(0)
        , videoOutput(0)
        , dataOutput(0)
        , audioInput(0)
        , videoInput(0)
        , dataInput(0)
    {
    }

    QObject *audioOutput;
    QObject *videoOutput;
    QObject *dataOutput;
    QObject *audioInput;
    QObject *videoInput;
    QObject *dataInput;
};

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
}

QAbstractMediaService::~QAbstractMediaService()
{
    delete d_ptr;
}

/*!
    \property QAbstractMediaService::audioOutput

    The output that a media service renders audio to.
*/

/*!
    Returns the output that a media service renders audio to.
*/
QObject *QAbstractMediaService::audioOutput() const
{
    return d_func()->audioOutput;
}

/*!
    Sets the \a output that a media service renders audio to.

    The output must be a media end point constructed by the createEndpoint() function of the media
    service.

    \sa createEndpoint()
*/
void QAbstractMediaService::setAudioOutput(QObject *output)
{
    d_func()->audioOutput = output;
}

/*!
    \property QAbstractMediaService::videoOutput

    The output that a media service renders video to.
*/

/*!
    Returns the output that a media service renders video to.
*/
QObject *QAbstractMediaService::videoOutput() const
{
    return d_func()->videoOutput;
}

/*!
    Sets the \a output that a media service renders video to.

    The output must be a media end point constructed by the createEndpoint() function of the media
    service.

    \sa createEndpoint()
*/
void QAbstractMediaService::setVideoOutput(QObject *output)
{
    d_func()->videoOutput = output;
}

/*!
    \property QAbstractMediaService::dataOutput

    The output that a media service writes data to.
*/

/*!
    Returns the output that a media service writes data to.
*/
QObject *QAbstractMediaService::dataOutput() const
{
    return d_func()->dataOutput;
}

/*!
    Sets the \a output that a media service writes data to.

    The output must be a media end point constructed by the createEndpoint() function of the media
    service.

    \sa createEndpoint()
*/
void QAbstractMediaService::setDataOutput(QObject *output)
{
    d_func()->dataOutput = output;
}

/*!
    \property QAbstractMediaService::audioInput

    The input that a media service source audio from.
*/

/*!
    Returns the input that a media service sources audio from.
*/
QObject *QAbstractMediaService::audioInput() const
{
    return d_func()->audioInput;
}

/*!
    Sets the \a input that a media service sources audio from.

    The input media be a media end point constructed by the createEndpoint() function of the media
    service.

    \sa createEndpoint()
*/
void QAbstractMediaService::setAudioInput(QObject *input)
{
    d_func()->audioInput = input;
}

/*!
    \property QAbstractMediaService::videoInput

    The input that a media service source video data from.
*/

/*!
    Returns the input that a media service sources video from.
*/
QObject *QAbstractMediaService::videoInput() const
{
    return d_func()->videoInput;
}

/*!
    Sets the \a input that a media service sources video from.

    The input media be a media end point constructed by the createEndpoint() function of the media
    service.

    \sa createEndpoint()
*/
void QAbstractMediaService::setVideoInput(QObject *input)
{
    d_func()->videoInput = input;
}

/*!
    \property QAbstractMediaService::dataInput

    The input that a media service reads data from.
*/

/*!
    Returns the input that a media service reads data from.
*/
QObject *QAbstractMediaService::dataInput() const
{
    return d_func()->dataInput;
}

/*!
    Sets the \a input that a media service reads data from.

    The input media be a media end point constructed by the createEndpoint() function of the media
    service.

    \sa createEndpoint()
*/
void QAbstractMediaService::setDataInput(QObject *input)
{
    d_func()->dataInput = input;
}

/*!
    Returns a list of interfaces for \a direction media end point objects that can be created by a
    media service.

    \sa createEndpoint()
*/
QList<QByteArray> QAbstractMediaService::supportedEndpointInterfaces(
        QMediaEndpointInterface::Direction direction) const
{
    Q_UNUSED(direction);

    return QList<QByteArray>();
}

/*!
    Creates a new media end point object from the given \a interface name.

    An end point object may be an input or output to a media service and can only be used with the
    service object that created it.

    A list of supported end point interfaces can be queried using the supportedEndpointInterfaces()
    function.

    \sa supportedEndpointInterfaces(), setAudioOutput(), setVideoOutput()
*/
QObject *QAbstractMediaService::createEndpoint(const char *interface)
{
    Q_UNUSED(interface);

    return 0;
}


/*!
    \fn QAbstractMediaService::createEndpoint()

    Creates a new media end point object of type \c T.

    \c T must be a pointer to a class that inherits from QObject and has been declared as a media
    endpoint using Q_MEDIA_DECLARE_ENDPOINT.
*/
