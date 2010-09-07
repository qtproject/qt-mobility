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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtCore>
#include <QtDBus/QDBusMessage>
#include <qmobilityglobal.h>
#include "maemo/types.h"
#include "maemo/statefulldbusproxy.h"
#include "maemo/interfaces/iconnection.h"
#include "maemo/interfaces/iconnectionrequests.h"
#include "maemo/readyobject.h"
#include "qtelephonycallinfo.h"

QTM_BEGIN_NAMESPACE
     class QTelephonyCallListPrivate;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

namespace DBus
{
    class Connection : public StatefullDBusProxy
                        , public ReadyObject
                        , public ReferenceCounter
    {
        Q_OBJECT
        Q_DISABLE_COPY(Connection)
    public:
        static const Feature FeatureCore;

        Connection(const QDBusConnection busconnection, const QString &busName, const QString &objectPath, QTelephonyCallListPrivate* callList);
        ~Connection();
        void channelStatusChanged(DBus::Channel* channel);

    private Q_SLOTS:
        void onSelfHandleChanged(uint selfHandle);
        void onNewChannel(const QDBusObjectPath& objectPath, const QString& channelType, uint handleType, uint handle, bool suppressHandler);
        void onConnectionError(const QString& error, const QVariantMap& details);
        void onStatusChanged(uint status, uint reason);

        // for IConnectionRequests signals
        void onNewChannels(const DBus::Interfaces::ChannelDetailsList& channels);
        void onChannelClosed(const QDBusObjectPath& removed);

    private: 
        void readCurrentChannels();

        DBus::Interfaces::IConnection* pIConnection;
        DBus::Interfaces::IConnectionRequests* pIConnectionRequests;
        QTelephonyCallListPrivate* ptelephonyCallList;
    };
    typedef SharedPointer<Connection> ConnectionPtr;
}
#endif // CONNECTION_H
