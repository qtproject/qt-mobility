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

#include <qcontactdetails.h>
#include "qmlcontact.h"
#include <QDebug>

static QString normalizePropertyName(const QString& name)
{
   if (!name.isEmpty())
     return name.mid(1).prepend(name[0].toLower());
   return QString();
}


QMLContact::QMLContact(QObject *parent)
    :QObject(parent),
    m_contactChanged(false),
    m_contactMap(0)
{

}

void QMLContact::setContact(const QContact& c)
{
    m_contact = c;

    if (m_contactMap) {
        delete m_contactMap;
        m_detailMaps.clear();
    }

    m_contactMap = new QDeclarativePropertyMap(this);
    connect(m_contactMap, SIGNAL(valueChanged(QString,QVariant)), this, SLOT(contactChanged(QString,QVariant)));



    QList<QContactDetail> details = m_contact.details();
    foreach (const QContactDetail& detail, details) {
      QDeclarativePropertyMap* dm = new QDeclarativePropertyMap(m_contactMap);

      connect(dm, SIGNAL(valueChanged(QString,QVariant)), SLOT(detailChanged(QString,QVariant)));

      m_detailMaps.append(dm);;

      QVariantMap values = detail.variantValues();
      foreach (const QString& key, values.keys()) {
          dm->insert(normalizePropertyName(key), values.value(key));
      }
      m_contactMap->insert(normalizePropertyName(detail.definitionName()), QVariant::fromValue(static_cast<QObject*>(dm)));
    }

    m_contactChanged = false;
}

const QContact& QMLContact::contact() const
{
    return m_contact;
}

QVariant QMLContact::contactMap() const
{
    if (m_contactMap)
        return QVariant::fromValue(static_cast<QObject*>(m_contactMap));
    return QVariant();
}

bool QMLContact::isContactChanged() const
{
    return m_contactChanged;
}

void QMLContact::contactChanged(const QString &key, const QVariant &value)
{
    qWarning() << "contactChanged field:"  << key << " value:" << value;
    m_contactChanged = true;
}


void QMLContact::detailChanged(const QString &key, const QVariant &value)
{
    qWarning() << "detailChanged field:"  << key << " value:" << value;
    m_contactChanged = true;
}
