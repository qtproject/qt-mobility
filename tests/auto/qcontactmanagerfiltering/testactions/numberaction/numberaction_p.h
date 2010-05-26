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

#ifndef QCONTACTTESTNUMBERACTION_P_H
#define QCONTACTTESTNUMBERACTION_P_H

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


class QNumberAction : public DummyAction
{
    Q_OBJECT

public:
    QNumberAction(QObject *parent = 0) : DummyAction(parent) {}
    ~QNumberAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("Number", "NumberCo", 42); }

    QContactFilter contactFilter(const QVariant& value) const
    {
        QContactDetailFilter df;
        // XXX TODO: find some way to pass the defAndFieldNamesForTypeForActions value for Double to this function...
        df.setDetailDefinitionName("DoubleDefinition", "DoubleField");
        df.setValue(value);

        QContactDetailFilter df2;
        // XXX TODO: find some way to pass the defAndFieldNamesForTypeForActions value for Integer to this function...
        df2.setDetailDefinitionName("IntegerDefinition", "IntegerField");
        df2.setValue(value);

        /* We like either doubles or integers */
        return df | df2;
    }

    bool isDetailSupported(const QContactDetail &detail, const QContact &) const
    {
        // XXX TODO: find some way to pass the defAndFieldNamesForTypeForActions value for Double to this function...
        if (detail.definitionName() == QString(QLatin1String("DoubleDefinition")) && detail.hasValue("DoubleField")) {
            return true;
        }

        // XXX TODO: find some way to pass the defAndFieldNamesForTypeForActions value for Integer to this function...
        if (detail.definitionName() == QString(QLatin1String("IntegerDefinition")) && detail.hasValue("IntegerField")) {
            return true;
        }

        return false;
    }
    QList<QContactDetail> supportedDetails(const QContact& contact) const
    {
        // XXX TODO: find some way to pass the defAndFieldNamesForTypeForActions value for Integer to this function...
        QList<QContactDetail> retn = contact.details("IntegerDefinition");
        // XXX TODO: find some way to pass the defAndFieldNamesForTypeForActions value for Double to this function...
        retn.append(contact.details("DoubleDefinition"));
        return retn;
    }
};

class QNumberActionPlugin : public QObject, public QServicePluginInterface
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

        if (descriptor.interfaceName() == "com.nokia.qt.mobility.contacts.qcontactaction.number")
            return new QNumberAction(this);
        else
            return 0;
    }
};
Q_EXPORT_PLUGIN2(contacts_numberaction, QNumberActionPlugin);


#endif
