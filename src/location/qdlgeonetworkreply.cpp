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

#include "qdlgeonetworkreply.h"

#include <QNetworkReply>

QTM_BEGIN_NAMESPACE

QDLGeoNetworkReply::QDLGeoNetworkReply(QNetworkReply *reply)
    : m_reply(reply)
{
    connect(m_reply,
            SIGNAL(finished()),
            this,
            SLOT(networkReplyFinished()));
    connect(m_reply,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(networkError(QNetworkReply::NetworkError)));
}

QDLGeoNetworkReply::~QDLGeoNetworkReply()
{
    if (m_reply) {
        m_reply->deleteLater();
    }
}

void QDLGeoNetworkReply::networkReplyFinished()
{
    emit readyToParse(m_reply);
}

void QDLGeoNetworkReply::networkError(QNetworkReply::NetworkError networkError)
{
    // TODO map errors across
    emit error(QDLGeoReply::UnknownError, m_reply->errorString());
}

void QDLGeoNetworkReply::cancel()
{
    if (m_reply) {
        m_reply->abort();
    }
}

#include "moc_qdlgeonetworkreply.cpp"

QTM_END_NAMESPACE

/*
signals:
    void readyToParse(QIODevice *input);
    void error(QDLGeoReply::ErrorCode errorCode, QString errorString = QString());

private:
    QNetworkReply *m_reply;
};
*/
