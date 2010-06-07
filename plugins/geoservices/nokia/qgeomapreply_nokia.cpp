/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgeomapreply_nokia_p.h"

QGeoMapReplyNokia::QGeoMapReplyNokia(QNetworkReply *reply, const QGeoTiledMapRequest &request, QObject *parent)
        : QGeoTiledMapReply(request, parent),
        m_reply(reply)
{
    cleanedUp = false;
    connect(m_reply,
            SIGNAL(finished()),
            this,
            SLOT(networkFinished()));

    connect(m_reply,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(networkError(QNetworkReply::NetworkError)));
}

QGeoMapReplyNokia::~QGeoMapReplyNokia()
{
    //m_reply->abort();
    //TODO: possible mem leak -> m_reply->deleteLater() ?
    if (!cleanedUp)
        delete m_reply;
    //m_reply->deleteLater();
}

QNetworkReply* QGeoMapReplyNokia::networkReply() const
{
    return m_reply;
}

void QGeoMapReplyNokia::abort()
{
    m_reply->abort();
    m_reply->deleteLater();
    cleanedUp = true;
}

void QGeoMapReplyNokia::networkFinished()
{
    if (m_reply->error() != QNetworkReply::NoError) {
        return;
    }

    QPixmap tile;

    if (!tile.loadFromData(m_reply->readAll(), "PNG")) {
        // add a qWarning with the actual parser.errorString()
        setError(QGeoTiledMapReply::ParseError, "The response from the service was not in a recognisable format.");
    }

    if (!tile.isNull() && !tile.size().isEmpty()) {
        setMapImage(tile);
        setFinished(true);
    } else {
        // add a qWarning with the actual parser.errorString()
        setError(QGeoTiledMapReply::ParseError, "The map image is empty.");
    }

    m_reply->deleteLater();
    cleanedUp = true;
}

void QGeoMapReplyNokia::networkError(QNetworkReply::NetworkError error)
{
    if (m_reply->error() != QNetworkReply::OperationCanceledError)
        setError(QGeoTiledMapReply::CommunicationError, m_reply->errorString());
    m_reply->deleteLater();
    cleanedUp = true;
}
