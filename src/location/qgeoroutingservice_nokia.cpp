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

#include "qgeoroutingservice_nokia_p.h"
#include "qgeoroutereply_nokia_p.h"

QTM_BEGIN_NAMESPACE

QGeoRoutingServiceNokia::QGeoRoutingServiceNokia()
        : m_token(""),
        m_referrer(""),
        m_host("172.24.32.155"),
        m_proxy(QNetworkProxy(QNetworkProxy::NoProxy))
{
}

QGeoRoutingServiceNokia::~QGeoRoutingServiceNokia()
{
}

QString QGeoRoutingServiceNokia::token() const
{
    return m_token;
}

void QGeoRoutingServiceNokia::setToken(const QString &token)
{
    m_token = token;
}

QString QGeoRoutingServiceNokia::referrer() const
{
    return m_referrer;
}

void QGeoRoutingServiceNokia::setReferrer(const QString &referrer)
{
    m_referrer = referrer;
}

QString QGeoRoutingServiceNokia::host() const
{
    return m_host;
}

void QGeoRoutingServiceNokia::setHost(const QString &host)
{
    m_host = host;
}

QNetworkProxy QGeoRoutingServiceNokia::proxy() const
{
    return m_proxy;
}

void QGeoRoutingServiceNokia::setProxy(const QNetworkProxy &proxy)
{
    m_proxy = proxy;
    m_nam.setProxy(m_proxy);
}

QGeoRouteReply* QGeoRoutingServiceNokia::getRoute(const QGeoRouteRequest& request)
{
    QString rawRequest = request.requestString(m_host);
    QNetworkRequest netRequest = QNetworkRequest(QUrl(rawRequest));
    QNetworkReply* netReply = m_nam.get(netRequest);

    QGeoRouteReply* routeReply = new QGeoRouteReplyNokia(netReply);

    connect(routeReply,
            SIGNAL(finished()),
            this,
            SLOT(finishedReply()));
    connect(routeReply,
            SIGNAL(error(QGeoRouteReply::ErrorCode, QString)),
            this,
            SLOT(errorReply(QGeoRouteReply::ErrorCode, QString)));

    return routeReply;
}

void QGeoRoutingServiceNokia::finishedReply()
{
    emit finished(static_cast<QGeoRouteReply *>(this->sender()));
}

void QGeoRoutingServiceNokia::errorReply(QGeoRouteReply::ErrorCode errorCode, QString errorString)
{
    emit error(static_cast<QGeoRouteReply *>(this->sender()), errorCode, errorString);
}

#include "moc_qgeoroutingservice_nokia_p.cpp"

QTM_END_NAMESPACE
