/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Orgnizer: Nokia Corporation (qt-info@nokia.com)
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

#include "qmlorgnizer.h"
#include <qorgnizerdetails.h>
#include <QtDebug>
#include <QStringList>

#include <QPixmap>
#include <QImage>

QT_USE_NAMESPACE
QTM_USE_NAMESPACE

QmlOrgnizer::QmlOrgnizer(const QOrgnizer& orgnizer, QObject *parent)
    : QObject(parent), m_orgnizer(orgnizer)
{   
}

QmlOrgnizer::QmlOrgnizer()
{

}

QmlOrgnizer::~QmlOrgnizer()
{

}

QOrgnizer &QmlOrgnizer::orgnizer()
{
    return m_orgnizer;
}

void QmlOrgnizer::setOrgnizer(QOrgnizer& orgnizer)
{
    m_orgnizer = orgnizer;
    emit orgnizerChanged(this);
}

QString QmlOrgnizer::name() const
{
    return m_orgnizer.displayLabel();
}

void QmlOrgnizer::setName(QString name)
{
    Q_UNUSED(name);
    qWarning() << "Not implemented yet";
    emit nameChanged(this);
}

QString QmlOrgnizer::email() const
{
    QList<QOrgnizerDetail> allEmails = m_orgnizer.details(QOrgnizerEmailAddress::DefinitionName);

    QStringList emails;
    foreach (const QOrgnizerDetail& email, allEmails) {
        emails << email.value(QOrgnizerEmailAddress::FieldEmailAddress);
    }
    return emails.join(QString::fromLatin1(","));
}

void QmlOrgnizer::setEmail(QString email)
{
    Q_UNUSED(email);
    qWarning() << "Not implemented yet";
    emit emailChanged(this);
}

QString QmlOrgnizer::avatar() const
{
    return m_orgnizer.detail<QOrgnizerAvatar>().imageUrl().toString();
}

QPixmap QmlOrgnizer::thumbnail() const
{
    return QPixmap::fromImage(m_orgnizer.detail<QOrgnizerThumbnail>().thumbnail());
}

bool QmlOrgnizer::hasThumbnail() const
{
    return !thumbnail().isNull();
}

QString QmlOrgnizer::interest() const
{
    // Try a phone number
    QString det = m_orgnizer.detail(QOrgnizerPhoneNumber::DefinitionName).value(QOrgnizerPhoneNumber::FieldNumber);
    if (!det.isEmpty())
        return det;

    det = m_orgnizer.detail(QOrgnizerEmailAddress::DefinitionName).value(QOrgnizerEmailAddress::FieldEmailAddress);
    if (!det.isEmpty())
        return det;

    det = m_orgnizer.detail(QOrgnizerOnlineAccount::DefinitionName).value(QOrgnizerOnlineAccount::FieldAccountUri);
    if (!det.isEmpty())
        return det;

    // Well, don't know.
    return QString();
}

QString QmlOrgnizer::interestLabel() const
{
    // Try a phone number
    QString det = m_orgnizer.detail(QOrgnizerPhoneNumber::DefinitionName).value(QOrgnizerPhoneNumber::FieldNumber);
    if (!det.isEmpty())
        return tr("Phone number:");

    det = m_orgnizer.detail(QOrgnizerEmailAddress::DefinitionName).value(QOrgnizerEmailAddress::FieldEmailAddress);
    if (!det.isEmpty())
        return tr("Email:");

    det = m_orgnizer.detail(QOrgnizerOnlineAccount::DefinitionName).value(QOrgnizerOnlineAccount::FieldAccountUri);
    if (!det.isEmpty())
        return QString("%1:").arg(m_orgnizer.detail(QOrgnizerOnlineAccount::DefinitionName).value(QOrgnizerOnlineAccount::FieldServiceProvider));

    // Well, don't know.
    return QString();
}

QStringList QmlOrgnizer::details()
{
    QStringList dets;
    QList<QOrgnizerDetail> ld = m_orgnizer.details();
    QOrgnizerDetail d;
    foreach(d, ld){
        dets += d.definitionName();
    }
    return dets;
}

QStringList QmlOrgnizer::contexts()
{
    QStringList dets;
    QList<QOrgnizerDetail> ld = m_orgnizer.details();
    QOrgnizerDetail d;
    foreach(d, ld){
        dets += d.contexts();
    }
    return dets;
}

QVariantMap QmlOrgnizer::values(QString definitionId)
{
    QStringList strlist;
    QOrgnizerDetail detail = m_orgnizer.detail(definitionId);

    QVariantMap map = detail.variantValues();
    return map;
}

#include "moc_qmlorgnizer.cpp"
