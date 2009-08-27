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

#include "mpddaemon.h"
#include "mpdmetadata.h"
#include "mpdplayercontrol.h"
#include "mpdplaylistcontrol.h"
#include "mpdplayerservice.h"



MpdPlayerService::MpdPlayerService(QObject *parent):
    QMediaPlayerService(parent)
{
    daemon = new MpdDaemon(this);
    connect(daemon, SIGNAL(disconnected()), SLOT(disconnected()));

    playerControl = new MpdPlayerControl(daemon, this);
    playlistControl = new MpdPlaylistControl(daemon, this);
    metadataControl = new MpdMetadata(daemon, this);
}

MpdPlayerService::~MpdPlayerService()
{
}

bool MpdPlayerService::isEndpointSupported(QAbstractMediaService::MediaEndpoint endpointType)
{
    Q_UNUSED(endpointType);
    return false;
}

void MpdPlayerService::setInputStream(QIODevice* stream)
{
    Q_UNUSED(stream);
}

QIODevice* MpdPlayerService::inputStream() const
{
    return 0;
}

void MpdPlayerService::setOutputStream(QIODevice* stream)
{
    Q_UNUSED(stream);
}

QIODevice* MpdPlayerService::outputStream() const
{
    return 0;
}

QString MpdPlayerService::activeEndpoint(QAbstractMediaService::MediaEndpoint endpointType)
{
    Q_UNUSED(endpointType);
    return QString();
}

void MpdPlayerService::setActiveEndpoint(QAbstractMediaService::MediaEndpoint endpointType, const char *interface)
{
    Q_UNUSED(endpointType);
    Q_UNUSED(interface);
}

QList<QString> MpdPlayerService::supportedEndpoints(QAbstractMediaService::MediaEndpoint endpointType) const
{
    Q_UNUSED(endpointType);
    return QList<QString>();
}

QAbstractMediaControl* MpdPlayerService::control(const char *name) const
{
    if (QLatin1String(name) == QMediaPlayerControl_iid)
        return playerControl;
    else if (QLatin1String(name) == QMediaPlaylistControl_iid)
        return playlistControl;
    else if (QLatin1String(name) == QMetadataProviderControl_iid)
        return metadataControl;

    return 0;
}

void MpdPlayerService::stateChanged(int state)
{
}

void MpdPlayerService::disconnected()
{
}

