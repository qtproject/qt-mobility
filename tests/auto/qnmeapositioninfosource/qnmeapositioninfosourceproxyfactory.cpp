/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qnmeapositioninfosourceproxyfactory.h"
#include "../qlocationtestutils_p.h"

#include <QIODevice>
#include <QTcpServer>
#include <QTcpSocket>


QNmeaPositionInfoSourceProxy::QNmeaPositionInfoSourceProxy(QNmeaPositionInfoSource *source, QIODevice *outDevice)
    : m_source(source),
        m_outDevice(outDevice)
{
}

QNmeaPositionInfoSourceProxy::~QNmeaPositionInfoSourceProxy()
{
    m_outDevice->close();
    delete m_outDevice;
}

QGeoPositionInfoSource *QNmeaPositionInfoSourceProxy::source() const
{
    return m_source;
}

void QNmeaPositionInfoSourceProxy::feedUpdate(const QDateTime &dt)
{
    m_outDevice->write(QLocationTestUtils::createRmcSentence(dt).toAscii());
}

void QNmeaPositionInfoSourceProxy::feedBytes(const QByteArray &bytes)
{
    m_outDevice->write(bytes);
}


QNmeaPositionInfoSourceProxyFactory::QNmeaPositionInfoSourceProxyFactory()
    : m_server(new QTcpServer(this))
{
    bool b = m_server->listen(QHostAddress::LocalHost);
    Q_ASSERT(b);
}

QNmeaPositionInfoSourceProxy *QNmeaPositionInfoSourceProxyFactory::createProxy(QNmeaPositionInfoSource *source)
{
    QTcpSocket *client = new QTcpSocket;
    client->connectToHost(m_server->serverAddress(), m_server->serverPort());
    //qDebug() << "listening on" << m_server->serverAddress() << m_server->serverPort();
    bool b = m_server->waitForNewConnection(5000);
    Q_ASSERT(b);
    b = client->waitForConnected();
    Q_ASSERT(b);

    //QNmeaPositionInfoSource *source = new QNmeaPositionInfoSource(m_mode);
    source->setDevice(m_server->nextPendingConnection());
    Q_ASSERT(source->device() != 0);
    QNmeaPositionInfoSourceProxy *proxy = new QNmeaPositionInfoSourceProxy(source, client);
    proxy->setParent(source);
    return proxy;
}
