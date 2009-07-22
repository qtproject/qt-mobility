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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ACTIONFACTORYPLUGINTARGET
#define ACTIONFACTORYPLUGINTARGET contacts_sendemailactionfactory
#endif
#ifndef ACTIONFACTORYPLUGINNAME
#define ACTIONFACTORYPLUGINNAME "SendEmailActionFactory"
#endif

#include "sendemailaction_p.h"

#include "qcontactemailaddress.h"
#include "qcontactfilters.h"

#include <QMessageBox>

QContactSendEmailActionFactory::QContactSendEmailActionFactory()
{
}

QContactSendEmailActionFactory::~QContactSendEmailActionFactory()
{
}

QString QContactSendEmailActionFactory::name() const
{
    return QString(ACTIONFACTORYPLUGINNAME);
}
Q_EXPORT_PLUGIN2(ACTIONFACTORYPLUGINTARGET, QContactSendEmailActionFactory);

QList<QContactActionFactory::ActionDescriptor> QContactSendEmailActionFactory::actionDescriptors() const
{
    return QList<QContactActionFactory::ActionDescriptor>() << QContactActionFactory::ActionDescriptor("SendEmail", "Test", 1);
}

QContactAction* QContactSendEmailActionFactory::instance(const QContactActionFactory::ActionDescriptor&) const
{
    return new QContactSendEmailAction;
}

QContactSendEmailAction::QContactSendEmailAction() : QContactAction()
{
}

QContactSendEmailAction::~QContactSendEmailAction()
{
}

QString QContactSendEmailAction::actionName() const
{
    return QString("SendEmail");
}

QVariantMap QContactSendEmailAction::metadata() const
{
    return QVariantMap();
}

QString QContactSendEmailAction::vendor() const
{
    return QString("Test");
}

int QContactSendEmailAction::implementationVersion() const
{
    return 1;
}

QContactFilter QContactSendEmailAction::contactFilter(const QVariant& value) const
{
    QContactDetailFilter retn;
    retn.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
    retn.setValue(value);
    return retn;
}

bool QContactSendEmailAction::supportsDetail(const QContactDetail& detail) const
{
    return (detail.definitionName() == QContactEmailAddress::DefinitionName);
}
void QContactSendEmailAction::performAction(const QContact& contact, const QContactDetail& detail)
{
    Q_UNUSED(contact);
    Q_UNUSED(detail);
    QMessageBox::information(0, "SendEmail Action", "This example action exists as an example of how the action interface may be implemented; it does not offer the advertised functionality.");
}

