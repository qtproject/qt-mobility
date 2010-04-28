/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlcontact.h"
#include <qcontactdetails.h>
#include <QtDebug>
#include <QStringList>

#include <QPixmap>
#include <QImage>

QT_USE_NAMESPACE
QTM_USE_NAMESPACE

QmlContact::QmlContact(const QContact& contact, QObject *parent)
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

QString QmlContact::name() const
{
    return m_contact.displayLabel();
}

void QmlContact::setName(QString name)
{
    Q_UNUSED(name);
    qWarning() << "Not implemented yet";
    emit nameChanged(this);
}

QString QmlContact::email() const
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

QString QmlContact::avatar() const
{
    return m_contact.detail<QContactAvatar>().imageUrl().toString();
}

QPixmap QmlContact::thumbnail() const
{
    return QPixmap::fromImage(m_contact.detail<QContactThumbnail>().thumbnail());
}

bool QmlContact::hasThumbnail() const
{
    return !thumbnail().isNull();
}

QString QmlContact::interest() const
{
    // Try a phone number
    QString det = m_contact.detail(QContactPhoneNumber::DefinitionName).value(QContactPhoneNumber::FieldNumber);
    if (!det.isEmpty())
        return det;

    det = m_contact.detail(QContactEmailAddress::DefinitionName).value(QContactEmailAddress::FieldEmailAddress);
    if (!det.isEmpty())
        return det;

    det = m_contact.detail(QContactOnlineAccount::DefinitionName).value(QContactOnlineAccount::FieldAccountUri);
    if (!det.isEmpty())
        return det;

    // Well, don't know.
    return QString();
}

QString QmlContact::interestLabel() const
{
    // Try a phone number
    QString det = m_contact.detail(QContactPhoneNumber::DefinitionName).value(QContactPhoneNumber::FieldNumber);
    if (!det.isEmpty())
        return tr("Phone number:");

    det = m_contact.detail(QContactEmailAddress::DefinitionName).value(QContactEmailAddress::FieldEmailAddress);
    if (!det.isEmpty())
        return tr("Email:");

    det = m_contact.detail(QContactOnlineAccount::DefinitionName).value(QContactOnlineAccount::FieldAccountUri);
    if (!det.isEmpty())
        return QString("%1:").arg(m_contact.detail(QContactOnlineAccount::DefinitionName).value(QContactOnlineAccount::FieldServiceProvider));

    // Well, don't know.
    return QString();
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
