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

#include "qgeocodingmanager_nokia_p.h"

#include "qgeocodexmlparser_p.h"

#include <qgeocodeaddressrequest.h>
#include <qgeocodecoordinaterequest.h>
#include <qgeocodestringrequest.h>

#include "qgeocoderequestwrapper_p.h"

#include <QNetworkProxy>

QGeoCodingManagerEngineNokia::QGeoCodingManagerEngineNokia(QObject *parent)
    : QGeoCodingManagerEngine(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
    m_networkManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,
                                             "172.16.42.137",
                                             8080));
//    m_host = "dev-a7.bln.gate5.de";
    m_host = "loc.desktop.maps.svc.ovi.com";
}

QGeoCodingManagerEngineNokia::~QGeoCodingManagerEngineNokia() {}

void QGeoCodingManagerEngineNokia::requestDestroyed(QGeoCodeAbstractRequest *request)
{
    if (!request)
        return;

    if (!m_requests.contains(request))
        return;

    QNetworkReply *reply = m_requests[request];
    m_requests.remove(request);

    reply->abort();
    reply->deleteLater();
}

bool QGeoCodingManagerEngineNokia::startRequest(QGeoCodeAbstractRequest *request)
{
    if (!request)
        return false;

    if (m_requests.contains(request))
       return false;

    QString rawRequest = "";

    switch (request->type()) {
        case QGeoCodeAbstractRequest::AddressRequest:
            rawRequest = requestString(static_cast<QGeoCodeAddressRequest*>(request));
            break;
        case QGeoCodeAbstractRequest::CoordinateRequest:
            rawRequest = requestString(static_cast<QGeoCodeCoordinateRequest*>(request));
            break;
        case QGeoCodeAbstractRequest::StringRequest:
            rawRequest = requestString(static_cast<QGeoCodeStringRequest*>(request));
            break;
        default:
            break;
    }

    if (rawRequest.isEmpty())
        return false;

    QNetworkRequest networkRequest = QNetworkRequest(QUrl(rawRequest));
    QNetworkReply *reply = m_networkManager->get(networkRequest);
    m_requests.insert(request, reply);

    QGeoCodeRequestWrapper *wrapper
            = new QGeoCodeRequestWrapper(this, request, reply);

    connect(wrapper,
            SIGNAL(finished()),
            this,
            SLOT(networkFinished()));

    connect(wrapper,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(networkError(QNetworkReply::NetworkError)));

    updateRequestStatus(request,
                        QGeoCodeAbstractRequest::ActiveState,
                        QGeoCodingManager::NoError,
                        "");

    return true;
}

void QGeoCodingManagerEngineNokia::networkFinished()
{
    QGeoCodeRequestWrapper *wrapper
            = qobject_cast<QGeoCodeRequestWrapper *>(sender());

    QGeoCodeAbstractRequest *request = wrapper->request();
    QNetworkReply *reply = wrapper->reply();

    m_requests.remove(request);

    if (reply->error() != QNetworkReply::NoError) {
        updateRequestStatus(request,
                            QGeoCodeAbstractRequest::FinishedState,
                            QGeoCodingManager::CommunicationError,
                            reply->errorString());
    } else {
        parseReply(reply, request);
    }

    reply->deleteLater();
}

void QGeoCodingManagerEngineNokia::networkError(QNetworkReply::NetworkError error)
{
    QGeoCodeRequestWrapper *wrapper
            = qobject_cast<QGeoCodeRequestWrapper *>(sender());

    QGeoCodeAbstractRequest *request = wrapper->request();
    QNetworkReply *reply = wrapper->reply();

    m_requests.remove(request);

    updateRequestStatus(request,
                        QGeoCodeAbstractRequest::FinishedState,
                        QGeoCodingManager::CommunicationError,
                        reply->errorString());

    reply->deleteLater();
}

bool QGeoCodingManagerEngineNokia::cancelRequest(QGeoCodeAbstractRequest *request)
{
    if (!request)
        return false;

    if (!m_requests.contains(request))
       return false;

    QNetworkReply *reply = m_requests[request];
    m_requests.remove(request);

    reply->abort();
    reply->deleteLater();

    updateRequestStatus(request,
                        QGeoCodeAbstractRequest::CancelledState,
                        QGeoCodingManager::NoError,
                        "");

    return true;
}

bool QGeoCodingManagerEngineNokia::waitForRequestFinished(QGeoCodeAbstractRequest *request, int msecs)
{
    if (!request)
        return false;

    if (!m_requests.contains(request))
       return false;

    // TODO implement

    /*
    QNetworkReply *reply = m_requests[request];
    m_requests.remove(request);

    reply->deleteLater();
    */

    return true;
}

QString QGeoCodingManagerEngineNokia::requestString(QGeoCodeAddressRequest *request)
{
    QString requestString = "http://";
    requestString += m_host;
    requestString += "/geocoder/gc/1.0?referer=localhost";

    // TODO locale / language handling
    //requestString += "&lg=";
    //requestString += request->locale().language();

    requestString += "&country=";
    requestString += request->address().country();

    if (!request->address().state().isEmpty()) {
        requestString += "&state=";
        requestString += request->address().state();
    }

    if (!request->address().city().isEmpty()) {
        requestString += "&city=";
        requestString += request->address().city();
    }

    if (!request->address().postCode().isEmpty()) {
        requestString += "&zip=";
        requestString += request->address().postCode();
    }

    if (!request->address().thoroughfareName().isEmpty()) {
        requestString += "&street=";
        requestString += request->address().thoroughfareName();
    }

    if (!request->address().thoroughfareNumber().isEmpty()) {
        requestString += "&number=";
        requestString += request->address().thoroughfareNumber();
    }

    return requestString;
}

QString QGeoCodingManagerEngineNokia::requestString(QGeoCodeCoordinateRequest *request)
{
    QString requestString = "http://";
    requestString += m_host;
    requestString += "/geocoder/rgc/1.0?referer=localhost";
    requestString += "&long=";
    requestString += trimDouble(request->coordinate().longitude());
    requestString += "&lat=";
    requestString += trimDouble(request->coordinate().latitude());

    // TODO locale / language handling
    //requestString += "&lg=";
    //requestString += request->locale().language();

    return requestString;
}

QString QGeoCodingManagerEngineNokia::requestString(QGeoCodeStringRequest *request)
{
    QString requestString = "http://";
    requestString += m_host;
    requestString += "/geocoder/gc/1.0?referer=localhost";

    // TODO locale / language handling
    //requestString += "&lg=";
    //requestString += request->locale().language();

    requestString += "&obloc=";
    requestString += request->locationString();

    return requestString;
}

QString QGeoCodingManagerEngineNokia::trimDouble(qreal degree, int decimalDigits)
{
    QString sDegree = QString::number(degree, 'g', decimalDigits);

    int index = sDegree.indexOf('.');

    if (index == -1)
        return sDegree;
    else
        return QString::number(degree, 'g', decimalDigits + index);
}

void QGeoCodingManagerEngineNokia::parseReply(QNetworkReply *reply, QGeoCodeAbstractRequest *request)
{
    QGeoCodeXmlParser parser;

    if (parser.parse(reply)) {
        updateRequestResults(request, parser.results());
        updateRequestStatus(request,
                            QGeoCodeAbstractRequest::FinishedState,
                            QGeoCodingManager::NoError,
                            "");
    } else {
        updateRequestStatus(request,
                            QGeoCodeAbstractRequest::FinishedState,
                            QGeoCodingManager::ParseError,
                            "The response to the requested service was not able to be parsed.");
    }
}

QGeoCodingManagerNokia::QGeoCodingManagerNokia(QObject *parent)
    : QGeoCodingManager(parent)
{
    setEngine(new QGeoCodingManagerEngineNokia(this));
}

QGeoCodingManagerNokia::~QGeoCodingManagerNokia() {}

