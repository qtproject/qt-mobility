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


QContactSendEmailAction::QContactSendEmailAction() : QContactAction()
{
}

QContactSendEmailAction::~QContactSendEmailAction()
{
}

QContactActionDescriptor QContactSendEmailAction::actionDescriptor() const
{
    QContactActionDescriptor ret;
    ret.setActionName("SendEmail");
    ret.setServiceName("tst_qcontactactions:sendemailaction");
    ret.setImplementationVersion(1);
    return ret;
}

QVariantMap QContactSendEmailAction::metaData() const
{
    return QVariantMap();
}

QContactFilter QContactSendEmailAction::contactFilter(const QVariant& value) const
{
    QContactDetailFilter retn;
    retn.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
    retn.setValue(value);
    return retn;
}

bool QContactSendEmailAction::isDetailSupported(const QContactDetail &detail, const QContact &) const
{
    return (detail.definitionName() == QContactEmailAddress::DefinitionName);
}

QList<QContactDetail> QContactSendEmailAction::supportedDetails(const QContact& contact) const
{
    return contact.details(QContactEmailAddress::DefinitionName);
}

bool QContactSendEmailAction::invokeAction(const QContact& contact, const QContactDetail& detail, const QVariantMap& )
{
    Q_UNUSED(contact);
    Q_UNUSED(detail);
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
