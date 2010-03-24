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

#include "qmlcontact.h"
#include <qcontactdetails.h>
#include <QtDebug>
#include <QStringList>

QT_USE_NAMESPACE
QTM_USE_NAMESPACE

QmlContact::QmlContact(QContact& contact, QObject *parent)
    : QObject(parent), m_contact(contact)
{   
}

QmlContact::QmlContact()
{

}

QmlContact::~QmlContact()
{

}

QContact &QmlContact::contact()
{
    return m_contact;
}

void QmlContact::setContact(QContact& contact)
{
    m_contact = contact;
    emit contactChanged(this);
}

QString QmlContact::name()
{
    return m_contact.displayLabel();
}

void QmlContact::setName(QString name)
{
    Q_UNUSED(name);
    qWarning() << "Not implemented yet";
    emit nameChanged(this);
}

QString QmlContact::email()
{
    QList<QContactDetail> allEmails = m_contact.details(QContactEmailAddress::DefinitionName);

    QStringList emails;
    foreach (const QContactDetail& email, allEmails) {
        emails << email.value(QContactEmailAddress::FieldEmailAddress);
    }
    return emails.join(QString::fromLatin1(","));
}

void QmlContact::setEmail(QString email)
{
    Q_UNUSED(email);
    qWarning() << "Not implemented yet";
    emit emailChanged(this);
}


QStringList QmlContact::availableActions()
{
    QList<QContactActionDescriptor> actions =  m_contact.availableActions();
    QStringList names;

    foreach (const QContactActionDescriptor& action, actions) {
        names << action.actionName();
    }
    return names;
}

QStringList QmlContact::details()
{
    QStringList dets;
    QList<QContactDetail> ld = m_contact.details();
    QContactDetail d;
    foreach(d, ld){
        dets += d.definitionName();
    }
    return dets;
}

QStringList QmlContact::contexts()
{
    QStringList dets;
    QList<QContactDetail> ld = m_contact.details();
    QContactDetail d;
    foreach(d, ld){
        dets += d.contexts();
    }
    return dets;
}

QVariantMap QmlContact::values(QString definitionId)
{
    QStringList strlist;
    QContactDetail detail = m_contact.detail(definitionId);

    QVariantMap map = detail.variantValues();
    return map;
}

#include "moc_qmlcontact.cpp"
