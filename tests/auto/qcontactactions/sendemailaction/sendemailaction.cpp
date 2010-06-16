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

#ifndef ACTIONFACTORYPLUGINTARGET
#define ACTIONFACTORYPLUGINTARGET contacts_sendemailactionfactory
#endif
#ifndef ACTIONFACTORYPLUGINNAME
#define ACTIONFACTORYPLUGINNAME SendEmailActionFactory
#endif

#include "sendemailaction_p.h"

#include "qcontactemailaddress.h"
#include "qcontactfilters.h"

#include <QMessageBox>
#include <QTimer>

#define makestr(x) (#x)
#define makename(x) makestr(x)

QObject* QContactSendEmailActionPlugin::createInstance(const QServiceInterfaceDescriptor& descriptor,
                        QServiceContext* context,
                        QAbstractSecuritySession* session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == "com.nokia.qt.mobility.contacts.qcontactaction.sendemail")
        return new QContactSendEmailActionFactory();
    else
        return 0;
}

Q_EXPORT_PLUGIN2(contacts_sendemailaction, QContactSendEmailActionPlugin);

QContactSendEmailActionFactory::QContactSendEmailActionFactory() : QContactActionFactory()
{
}

QContactSendEmailActionFactory::~QContactSendEmailActionFactory()
{
}

QContactAction* QContactSendEmailActionFactory::create() const
{
    return new QContactSendEmailAction;
}

QSet<QContactActionTarget> QContactSendEmailActionFactory::supportedTargets(const QContact& contact) const
{
    QSet<QContactActionTarget> retn;
    QList<QContactEmailAddress> emdets = contact.details<QContactEmailAddress>();
    for (int i = 0; i < emdets.size(); ++i) {
        QContactActionTarget curr;
        curr.setContact(contact);
        curr.setDetails(QList<QContactDetail>() << emdets.at(i));
        retn << curr;
    }

    return retn;
}

QContactFilter QContactSendEmailActionFactory::contactFilter() const
{
    QContactDetailFilter retn;
    retn.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
    return retn;
}

QVariant QContactSendEmailActionFactory::metaData(const QString& key, const QList<QContactActionTarget>& targets, const QVariantMap& parameters) const
{
    Q_UNUSED(key);
    Q_UNUSED(targets);
    Q_UNUSED(parameters);
    return QVariant();
}

bool QContactSendEmailActionFactory::supportsContact(const QContact& contact) const
{
    return !contact.details<QContactEmailAddress>().isEmpty();
}




QContactSendEmailAction::QContactSendEmailAction(QObject* parent)
{
    Q_UNUSED(parent)
}

QContactSendEmailAction::~QContactSendEmailAction()
{
}

bool QContactSendEmailAction::isTargetSupported(const QContactActionTarget &target) const
{
    QList<QContactDetail> dets = target.details();
    if (dets.size() != 1 || !target.isValid())
        return false;
    return (dets.at(0).definitionName() == QContactEmailAddress::DefinitionName);
}

bool QContactSendEmailAction::invokeAction(const QContactActionTarget& target, const QVariantMap& )
{
    if (!isTargetSupported(target))
        return false;

    QTimer::singleShot(1, this, SLOT(performAction()));
    return true;
}

bool QContactSendEmailAction::invokeAction(const QList<QContactActionTarget>& targets, const QVariantMap& )
{
    foreach (const QContactActionTarget& target, targets) {
        if (!isTargetSupported(target)) {
            return false;
        }
    }

    QTimer::singleShot(1, this, SLOT(performAction()));
    return true;
}

QVariantMap QContactSendEmailAction::results() const
{
    return QVariantMap();
}

void QContactSendEmailAction::performAction()
{
    QMessageBox::information(0, "SendEmail Action", "This example action exists as an example of how the action interface may be implemented; it does not offer the advertised functionality.");
    emit stateChanged(QContactAction::FinishedState);
}
