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

#ifndef CLIENTADAPTOR_H
#define CLIENTADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

#include "maemo/clientbase.h"
#include "maemo/types.h"

namespace DBus
{
    class ClientAdaptor : public QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Telepathy.Client")
        Q_CLASSINFO("D-Bus Introspection", ""
    "  <interface name=\"org.freedesktop.Telepathy.Client\" >\n"
    "    <property name=\"Interfaces\" type=\"as\" access=\"read\" />\n"
    "  </interface>\n"
            "")

        Q_PROPERTY(QStringList Interfaces READ Interfaces)

    public:
        ClientAdaptor(const QStringList &interfaces, QObject *parent);
        virtual ~ClientAdaptor();

    public: // Properties
        inline QStringList Interfaces() const
        {
            return mInterfaces;
        }

    private:
        QStringList mInterfaces;
    };
} // DBus

Q_DECLARE_METATYPE(DBus::ClientAdaptor*)

#endif //CLIENTADAPTOR_H
