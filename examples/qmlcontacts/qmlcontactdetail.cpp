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

#include "qmlcontactdetail.h"
#include "qmlcontactdetailfield.h"
#include <QDebug>


static QString ToContactDetailName(const QString& name)
{
   if (!name.isEmpty())
     return name.mid(1).prepend(name[0].toUpper());
   return QString();
}


QMLContactDetail::QMLContactDetail(QObject* parent)
    :QObject(parent),
    m_contactChanged(false)
{

}

QDeclarativePropertyMap* QMLContactDetail::propertyMap() const
{
    return m_map;
}
void QMLContactDetail::setDetailPropertyMap(QDeclarativePropertyMap* map)
{
    m_map = map;
    foreach (const QString& key, m_map->keys()) {
        QMLContactDetailField* field = new QMLContactDetailField(this);
        field->setDetailPropertyMap(m_map);
        field->setKey(key);
        m_fields.append(field);
    }
}

QList<QObject*> QMLContactDetail::fields() const
{
    return m_fields;
}

QString QMLContactDetail::name() const
{
    return m_detailName;
}

void QMLContactDetail::setName(const QString& name)
{
    m_detailName = name;
}

bool QMLContactDetail::isDetailChanged() const
{
    return m_contactChanged;
}

QContactDetail QMLContactDetail::detail() const
{
    QContactDetail d(ToContactDetailName(name()));
    foreach (const QString& key, m_map->keys()) {
        d.setValue(ToContactDetailName(key), m_map->value(key));
    }
    return d;
}

void QMLContactDetail::detailChanged(const QString &key, const QVariant &value)
{
    qWarning() << "detailChanged field:"  << key << " value:" << value;
    m_contactChanged = true;
}
