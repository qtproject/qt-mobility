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

#include "qgeoroutereply_nokia_p.h"

#include "qgeoroutereply.h"
#include "qgeoroutexmlparser_nokia_p.h"

QTM_BEGIN_NAMESPACE

QGeoRouteReplyNokia::QGeoRouteReplyNokia(QNetworkReply *reply)
        : m_reply(reply)
{
    connect(m_reply,
            SIGNAL(finished()),
            this,
            SLOT(parse()));
    connect(m_reply,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(translateError(QNetworkReply::NetworkError)));

    /*
     // not sure if we want this or not
     // it will free some memory once the parse has gone through
     // probably want to do it indirectly and set a flag so that we don't
     // double delete in the constructor
    connect(this,
            SIGNAL(finished()),
            m_reply,
            SLOT(deleteLater()));
    connect(this,
            SIGNAL(error(QDLGeoReply::ErrorCode,QString)),
            m_reply,
            SLOT(deleteLater()));
            */
}

QGeoRouteReplyNokia::~QGeoRouteReplyNokia()
{
    if (m_reply)
        m_reply->abort();
    delete m_reply;
}

QGeoRouteReplyNokia::ResultCode QGeoRouteReplyNokia::resultCode() const
{
    return m_code;
}

void QGeoRouteReplyNokia::setResultCode(QGeoRouteReplyNokia::ResultCode code)
{
    m_code = code;
}

/*!
  emits finished if all went well
  otherwise emits error
*/
void QGeoRouteReplyNokia::parse()
{
    if (m_reply->size() == 0) {
        emit error(QGeoRouteReply::NoContentError, "The reply to the routing request was empty.");
        return;
    }

    QGeoRouteXmlParserNokia parser;
    bool success = parser.parse(m_reply, this);

    if (!success) {
        // emit error based on parser error string
        emit error(QGeoRouteReply::ParsingError, QString("Parsing error: %1").arg(parser.errorString()));
        return;
    }

    if (m_code != QGeoRouteReplyNokia::OK) {
        // emit error based on content of xml
        emit error(QGeoRouteReply::UnknownError, "The reply contained a status code that indicates that the request was not successful.");
        return;
    }

    emit finished();
}

void QGeoRouteReplyNokia::translateError(QNetworkReply::NetworkError errorCode)
{
    // TODO map errors across from errorCode
    emit error(QGeoRouteReply::NetworkError, QString("Network error: %1").arg(m_reply->errorString()));
}

void QGeoRouteReplyNokia::cancel()
{
    m_reply->abort();
}

#include "moc_qgeoroutereply_nokia_p.cpp"

QTM_END_NAMESPACE
