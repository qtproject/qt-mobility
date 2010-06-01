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

#ifndef QCONTACTTESTPHONENUMBERACTION_P_H
#define QCONTACTTESTPHONENUMBERACTION_P_H

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

    bool invokeAction(const QContactActionTarget&, const QVariantMap&)
    {
        // Well, do something
        emit stateChanged(QContactAction::FinishedState);
        return true;
    }

    bool invokeAction(const QList<QContactActionTarget>&, const QVariantMap&)
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

/* Static actions for testing matching */
class QPhoneNumberAction : public DummyAction
{
    Q_OBJECT

public:
    QPhoneNumberAction(QObject *parent = 0) : DummyAction(parent) {}
    ~QPhoneNumberAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("PhoneNumber", "PhoneNumberCo", 4); }

    QContactFilter contactFilter(const QVariant& value) const
    {
        QContactDetailFilter df;
        df.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
        df.setValue(value);
        return df;
    }
    bool isTargetSupported(const QContactActionTarget& target) const
    {
        if (target.details().size() != 1 || !target.isValid())
            return false;

        QContactDetail detail = target.details().at(0);
        return detail.definitionName() == QContactPhoneNumber::DefinitionName
                && !detail.variantValue(QContactPhoneNumber::FieldNumber).isNull();
    }
    QList<QContactDetail> supportedDetails(const QContact& contact) const
    {
        return contact.details(QContactPhoneNumber::DefinitionName);
    }
};

class QPhoneNumberActionPlugin : public QObject, public QServicePluginInterface
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

        if (descriptor.interfaceName() == "com.nokia.qt.mobility.contacts.qcontactaction.phonenumber")
            return new QPhoneNumberAction(this);
        else
            return 0;
    }
};
Q_EXPORT_PLUGIN2(contacts_phonenumberaction, QPhoneNumberActionPlugin);

#endif
