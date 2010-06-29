/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DBUSADAPTOR_H_1275967717
#define DBUSADAPTOR_H_1275967717

#include "qmobilityglobal.h"
#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "message.h"

QT_BEGIN_HEADER

class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

QTM_BEGIN_NAMESPACE
/*
 * Adaptor class for interface org.freedesktop.Telepathy.Connection.Interface.Requests
 */
class TelepathyAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Telepathy.Connection.Interface.Requests")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.freedesktop.Telepathy.Connection.Interface.Requests\" >\n"
"    <signal name=\"NewChannels\" >\n"
"      <arg direction=\"out\" type=\"a(i)\" name=\"channelsarray\" />\n"
"      <annotation value=\"ChannelsArray\" name=\"com.trolltech.QtDBus.QtTypeName.In0\" />\n"
"    </signal>\n"
"    <method name=\"createNewChannels\" >\n"
"      <arg direction=\"in\" type=\"a(i)\" name=\"channelsarray\" />\n"
"      <annotation value=\"ChannelsArray\" name=\"com.trolltech.QtDBus.QtTypeName.In0\" />\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    TelepathyAdaptor(QObject *parent);
    virtual ~TelepathyAdaptor();

public Q_SLOTS: // METHODS
    void createNewChannels(const ChannelsArray& channelsarray);
   
Q_SIGNALS: // SIGNALS
    void NewChannels(const ChannelsArray& channelsarray);
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif //DBUSADAPTOR_H_1275967717
