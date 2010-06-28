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

#include <QDebug>
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
    if (descriptor.interfaceName() == QContactActionFactory::InterfaceName
            && descriptor.serviceName() == QString(QLatin1String("tst_qcontactactions:sendemailaction"))
            && descriptor.majorVersion() == 1
            && descriptor.minorVersion() == 1
            && descriptor.customAttribute("ActionName") == QString(QLatin1String("SendEmail"))) {
        return new QContactSendEmailActionFactory();
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(contacts_sendemailaction, QContactSendEmailActionPlugin);

QContactSendEmailActionFactory::QContactSendEmailActionFactory() : QContactActionFactory()
{
    m_sendEmailDescriptor = createDescriptor("SendEmail", "tst_qcontactactions:sendemailaction", 1, QVariantMap(), this);
}

QContactSendEmailActionFactory::~QContactSendEmailActionFactory()
{
}

QList<QContactActionDescriptor> QContactSendEmailActionFactory::actionDescriptors() const
{
    QList<QContactActionDescriptor> retn;
    retn << m_sendEmailDescriptor;
    return retn;
}

QContactAction* QContactSendEmailActionFactory::create(const QContactActionDescriptor& which) const
{
    // note: if the action factory only ever creates one action, this check can be skipped
    if (which == m_sendEmailDescriptor)
        return new QContactSendEmailAction;
    return 0;
}

QSet<QContactActionTarget> QContactSendEmailActionFactory::supportedTargets(const QContact& contact, const QContactActionDescriptor& which) const
{
    QSet<QContactActionTarget> retn;

    // note: if the action factory only ever creates one action, this check can be skipped
    if (which != m_sendEmailDescriptor)
        return retn;

    QList<QContactEmailAddress> emdets = contact.details<QContactEmailAddress>();
    for (int i = 0; i < emdets.size(); ++i) {
        QContactActionTarget curr;
        curr.setContact(contact);
        curr.setDetails(QList<QContactDetail>() << emdets.at(i));
        retn << curr;
    }

    return retn;
}

QContactFilter QContactSendEmailActionFactory::contactFilter(const QContactActionDescriptor& which) const
{
    // note: if the action factory only ever creates one action, this check can be skipped
    if (which != m_sendEmailDescriptor)
        return QContactFilter();

    QContactDetailFilter retn;
    retn.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
    return retn;
}

QVariant QContactSendEmailActionFactory::metaData(const QString& key, const QList<QContactActionTarget>& targets, const QVariantMap& parameters, const QContactActionDescriptor& which) const
{
    Q_UNUSED(key);
    Q_UNUSED(targets);
    Q_UNUSED(parameters);
    Q_UNUSED(which);
    return QVariant();
}

bool QContactSendEmailActionFactory::supportsContact(const QContact& contact, const QContactActionDescriptor& which) const
{
    // note: if the action factory only ever creates one action, this check can be skipped
    if (which != m_sendEmailDescriptor)
        return false;
    return !contact.details<QContactEmailAddress>().isEmpty();
}




QContactSendEmailAction::QContactSendEmailAction()
{
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
