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

#include "qt7metadata.h"
#include "qt7widget.h"
#include "qt7movie.h"
#include "qt7playercontrol.h"
#include "qt7playerservice.h"



Qt7PlayerService::Qt7PlayerService(QObject *parent):
    QMediaPlayerService(parent)
{
    playerControl = new Qt7PlayerControl(this);
    metadataControl = new Qt7Metadata(this);
}

Qt7PlayerService::~Qt7PlayerService()
{
}

bool Qt7PlayerService::isEndpointSupported(QAbstractMediaService::MediaEndpoint endpointType)
{
    Q_UNUSED(endpointType);
    return false;
}

void Qt7PlayerService::setInputStream(QIODevice* stream)
{
    Q_UNUSED(stream);
}

QIODevice* Qt7PlayerService::inputStream() const
{
    return 0;
}

void Qt7PlayerService::setOutputStream(QIODevice* stream)
{
    Q_UNUSED(stream);
}

QIODevice* Qt7PlayerService::outputStream() const
{
    return 0;
}

QString Qt7PlayerService::activeEndpoint(QAbstractMediaService::MediaEndpoint endpointType)
{
    Q_UNUSED(endpointType);
    return QString();
}

void Qt7PlayerService::setActiveEndpoint(QAbstractMediaService::MediaEndpoint endpointType, const QString& endpoint)
{
    Q_UNUSED(endpointType);
    Q_UNUSED(endpoint);
}

QList<QString> Qt7PlayerService::supportedEndpoints(QAbstractMediaService::MediaEndpoint endpointType) const
{
    Q_UNUSED(endpointType);
    return QList<QString>();
}

void Qt7PlayerService::setVideoOutput(QObject *output)
{
    Q_UNUSED(output);
    /*
    Qt7Widget *vout = qobject_cast<Qt7Widget*>(output);
    if (vout != 0) {
        QMediaPlayerService::setVideoOutput(output);
        playerControl->movie()->setVideoWidget(vout);
    }
    */
}

QList<QByteArray> Qt7PlayerService::supportedEndpointInterfaces(QMediaEndpointInterface::Direction direction) const
{
    QList<QByteArray>   rc;

    if (direction == QMediaEndpointInterface::Output)
        rc << QMediaWidgetEndpoint_iid;

    return rc;
}

QObject* Qt7PlayerService::createEndpoint(const char *interface)
{
    if (QLatin1String(interface) == QMediaWidgetEndpoint_iid)
        return new Qt7Widget;

    return 0;
}

QAbstractMediaControl* Qt7PlayerService::control(const char *name) const
{
    if (QLatin1String(name) == QMediaPlayerControl_iid)
        return playerControl;
    else if (QLatin1String(name) == QMetadataProviderControl_iid)
        return metadataControl;

    return 0;
}

