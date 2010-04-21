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

#include "qgeocodingservice_nokia_p.h"
#include "qsearchresponse_nokia_p.h"

QTM_BEGIN_NAMESPACE

QGeocodingServiceNokia::QGeocodingServiceNokia()
        : m_token(""),
        m_referrer(""),
        m_host("dev-a7.bln.gate5.de"),
        m_proxy(QNetworkProxy(QNetworkProxy::NoProxy))
{
}

QGeocodingServiceNokia::~QGeocodingServiceNokia()
{
}

QString QGeocodingServiceNokia::token() const
{
    return m_token;
}

void QGeocodingServiceNokia::setToken(const QString &token)
{
    m_token = token;
}

QString QGeocodingServiceNokia::referrer() const
{
    return m_referrer;
}

void QGeocodingServiceNokia::setReferrer(const QString &referrer)
{
    m_referrer = referrer;
}

QString QGeocodingServiceNokia::host() const
{
    return m_host;
}

void QGeocodingServiceNokia::setHost(const QString &host)
{
    m_host = host;
}

QNetworkProxy QGeocodingServiceNokia::proxy() const
{
    return m_proxy;
}

void QGeocodingServiceNokia::setProxy(const QNetworkProxy &proxy)
{
    m_proxy = proxy;
    m_nam.setProxy(m_proxy);
}

QSearchResponse* QGeocodingServiceNokia::geocode(const QSearchRequest& request)
{
    QString rawRequest = request.searchText;
    QNetworkRequest netRequest = QNetworkRequest(QUrl(rawRequest));
    QNetworkReply* reply = m_nam.get(netRequest);

    QSearchResponse* codingReply = new QSearchResponseNokia(reply);

    connect(codingReply,
            SIGNAL(finished()),
            this,
            SLOT(finishedReply()));
    connect(codingReply,
            SIGNAL(error(QSearchResponse::ErrorCode, QString)),
            this,
            SLOT(errorReply(QSearchResponse::ErrorCode, QString)));

    return codingReply;
}

QSearchResponse* QGeocodingServiceNokia::reverseGeocode(const QSearchRequest& request)
{
    QString rawRequest = request.searchText;
    QNetworkRequest netRequest = QNetworkRequest(QUrl(rawRequest));
    QNetworkReply* reply = m_nam.get(netRequest);

    QSearchResponse* codingReply = new QSearchResponseNokia(reply);

    connect(codingReply,
            SIGNAL(finished()),
            this,
            SLOT(finishedReply()));
    connect(codingReply,
            SIGNAL(error(QSearchResponse::ErrorCode, QString)),
            this,
            SLOT(errorReply(QSearchResponse::ErrorCode, QString)));

    return codingReply;
}

void QGeocodingServiceNokia::finishedReply()
{
    emit finished(static_cast<QSearchResponse *>(this->sender()));
}

void QGeocodingServiceNokia::errorReply(QSearchResponse::ErrorCode errorCode, QString errorString)
{
    emit error(static_cast<QSearchResponse *>(this->sender()), errorCode, errorString);
}

#include "moc_qgeocodingservice_nokia_p.cpp"

QTM_END_NAMESPACE
