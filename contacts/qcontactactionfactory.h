/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTACTIONFACTORY_H
#define QCONTACTACTIONFACTORY_H

#include "qtcontactsglobal.h"

#include <QObject>
#include <QtPlugin>
#include <QString>
#include <QList>
#include <QStringList>
#include <QHash>

class QContactAction;

class QTCONTACTS_EXPORT QContactActionFactory : public QObject
{
    Q_OBJECT

public:
    virtual ~QContactActionFactory() = 0;

    struct ActionDescriptor {
        QString actionName;
        QString vendorName;
        int vendorVersion;

        ActionDescriptor(const QString& action, const QString& vendor, int version)
                : actionName(action),
                vendorName(vendor),
                vendorVersion(version)
        {
        }

        /*
        bool operator <(const ActionDescriptor& other) const
        {
            if (actionName < other.actionName)
                return true;
            if (actionName == other.actionName) {
                if (vendorName < other.vendorName)
                    return true;
                else if (vendorName == other.vendorName) {
                    if (vendorVersion < other.vendorVersion)
                        return true;
                    else if (vendorVersion == other.vendorVersion)
                        return this < &other; // equality, try to be stable
                }
            }
            return false;
        }
        */

        bool operator ==(const ActionDescriptor& other) const
        {
            return actionName == other.actionName
                    && vendorName == other.vendorName
                    && vendorVersion == other.vendorVersion;
        }
    };

    virtual QString name() const = 0;
    virtual QList<ActionDescriptor> actionDescriptors() const = 0;
    virtual QContactAction* instance(const ActionDescriptor& descriptor) const = 0;
};

uint qHash(const QContactActionFactory::ActionDescriptor& ad);

#define QT_CONTACTS_ACTION_FACTORY_INTERFACE "com.nokia.qt.mobility.contacts.actionfactory/1.0"
Q_DECLARE_INTERFACE(QContactActionFactory, QT_CONTACTS_ACTION_FACTORY_INTERFACE);

#endif
