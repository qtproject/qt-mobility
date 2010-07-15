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


#include "qmlcontactdetailfield.h"
#include <QDebug>

QMLContactDetailField::QMLContactDetailField(QObject* parent)
    :QObject(parent),
    m_map(0)
{

}

void QMLContactDetailField::setDetailPropertyMap(QDeclarativePropertyMap* map)
{
    m_map = map;
}

void QMLContactDetailField::setKey(const QString& key)
{
    m_key = key;
}

QString QMLContactDetailField::key() const
{
    return m_key;
}

QVariant QMLContactDetailField::value() const
{
    if (m_map) {
        QVariant v =  m_map->value(m_key);
        if (v.canConvert<QVariantList>()) {
            return v.toStringList().join(",");
        }
        return v;
    }
    return QVariant();
}

void QMLContactDetailField::setValue(const QVariant& value)
{
    if (m_map && m_map->contains(m_key)) {
        (*m_map)[m_key] = value;
    }
}


QString QMLContactDetailField::detailName() const
{
    return m_detailName;
}

void QMLContactDetailField::setDetailName(const QString& name)
{
    m_detailName = name;
}
