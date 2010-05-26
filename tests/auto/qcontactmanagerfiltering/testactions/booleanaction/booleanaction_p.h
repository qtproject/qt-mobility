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

#ifndef QCONTACTTESTBOOLEANACTION_P_H
#define QCONTACTTESTBOOLEANACTION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qtcontacts.h"
#include "qserviceplugininterface.h"
#include "qservicecontext.h"
#include "qserviceinterfacedescriptor.h"

#include <QSharedData>
#include <QString>
#include <QVariantMap>

QTM_USE_NAMESPACE

class DummyAction : public QContactAction
{
public:
    DummyAction(QObject* parent = 0) : QContactAction(parent) {}

    QVariantMap metaData() const {return QVariantMap();}

    bool invokeAction(const QContact&, const QContactDetail&, const QVariantMap&)
    {
        // Well, do something
        emit stateChanged(QContactAction::FinishedState);
        return true;
    }

    QVariantMap results() const
    {
        return QVariantMap();
    }

    State state() const {return QContactAction::FinishedState;}

};


class QBooleanAction : public DummyAction
{
    Q_OBJECT

public:
    QBooleanAction(QObject *parent = 0) : DummyAction(parent) {}
    ~QBooleanAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("Boolean", "BooleanCo", 3); }

    QContactFilter contactFilter(const QVariant& value) const
    {
        if (value.isNull() || (value.type() == QVariant::Bool && value.toBool() == true)) {

            /* This only likes bools that are true */
            QContactDetailFilter df;
            // XXX TODO: find some way to pass the defAndFieldNamesForTypeForActions value for Bool to this function...
            df.setDetailDefinitionName("BooleanDefinition", "BooleanField");
            df.setValue(true);
            return df;
        } else {
            return QContactInvalidFilter();
        }
    }
    bool isDetailSupported(const QContactDetail &detail, const QContact &) const
    {
        // XXX TODO: find some way to pass the defAndFieldNamesForTypeForActions value for Bool to this function...
        return (detail.definitionName() == QString(QLatin1String(("BooleanDefinition"))) && detail.hasValue("BooleanField"));
    }
    QList<QContactDetail> supportedDetails(const QContact& contact) const
    {
        // XXX TODO: find some way to pass the defAndFieldNamesForTypeForActions value for Bool to this function...
        return contact.details("BooleanDefinition");
    }
};

class QBooleanActionPlugin : public QObject, public QServicePluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QServicePluginInterface)

public:
    QObject* createInstance(const QServiceInterfaceDescriptor& descriptor,
                            QServiceContext* context,
                            QAbstractSecuritySession* session)
    {
        Q_UNUSED(context);
        Q_UNUSED(session);

        if (descriptor.interfaceName() == "com.nokia.qt.mobility.contacts.qcontactaction.boolean")
            return new QBooleanAction(this);
        else
            return 0;
    }
};
Q_EXPORT_PLUGIN2(contacts_booleanaction, QBooleanActionPlugin);

#endif
