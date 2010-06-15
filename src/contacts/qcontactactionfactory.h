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


#ifndef QCONTACTACTIONFACTORY_H
#define QCONTACTACTIONFACTORY_H

#include "qtcontactsglobal.h"
#include "qcontactactiondescriptor.h"
#include "qcontactactiontarget.h"

#include <QObject>
#include <QtPlugin>
#include <QString>
#include <QList>
#include <QVariantMap>
#include <QStringList>
#include <QHash>

QTM_BEGIN_NAMESPACE

class QContactAction;
class Q_CONTACTS_EXPORT QContactActionFactory : public QObject
{
    Q_OBJECT

public:
    virtual ~QContactActionFactory() = 0;
    virtual QContactActionDescriptor actionDescriptor() const = 0; // each action factory can create only a single action..
    virtual QContactAction* instance() const = 0;

    virtual QSet<QContactActionTarget> supportedTargets(const QContact& contact) const = 0;
    virtual QContactFilter contactFilter() const = 0;
    virtual QVariant metaData(const QString& key, const QList<QContactActionTarget>& targets, const QVariantMap& parameters = QVariantMap()) const = 0;

    virtual bool supportsContact(const QContact& contact) const; // virtual but not pure virtual; default impl. calls supportedTargets.isEmpty().
    QVariant metaData(const QString& key, const QContactActionTarget& target, const QVariantMap& parameters = QVariantMap()) const
    {
        return metaData(key, QList<QContactActionTarget>() << target, parameters);
    }
    QVariant metaData(const QString& key, const QContact& contact, const QContactDetail& detail = QContactDetail(), const QVariantMap& parameters = QVariantMap()) const
    {
        return metaData(key, QList<QContactActionTarget>() << QContactActionTarget(contact, detail), parameters);
    }
};

QTM_END_NAMESPACE

#endif
