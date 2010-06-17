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
#include "qmlcontactdetail.h"
#include "qmlcontactdetailfield.h"
#include <QDebug>

static QString normalizePropertyName(const QString& name)
{
   if (!name.isEmpty())
     return name.mid(1).prepend(name[0].toLower());
   return QString();
}


QMLContact::QMLContact(QObject *parent)
    :QAbstractListModel(parent),
    m_contactMap(0)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(DetailNameRole, "detailName");
    roleNames.insert(DetailFieldKeyRole, "key");
    roleNames.insert(DetailFieldValueRole, "value");
    roleNames.insert(DetailFieldRole, "field");
    setRoleNames(roleNames);

    connect(&m_saveRequest, SIGNAL(resultAvailable()), this, SLOT(onContactSaved()));
}

void QMLContact::setManager(QContactManager* manager)
{
   m_manager = manager;
}
void QMLContact::setContact(const QContact& c)
{
    m_contact = c;

    if (m_contactMap) {
        delete m_contactMap;
        m_detailMaps.clear();
    }

    foreach (QObject* detail, m_details) {
        delete detail;
    }
    m_details.clear();
    m_detailFields.clear();

    m_contactMap = new QDeclarativePropertyMap(this);


    QList<QContactDetail> details = m_contact.details();

    foreach (const QContactDetail& detail, details) {
      QMLContactDetail* qd = new QMLContactDetail(this);

      QDeclarativePropertyMap* dm = new QDeclarativePropertyMap(m_contactMap);

      connect(dm, SIGNAL(valueChanged(QString,QVariant)), qd, SLOT(detailChanged(QString,QVariant)));


      QVariantMap values = detail.variantValues();
      foreach (const QString& key, values.keys()) {
          dm->insert(normalizePropertyName(key), values.value(key));
      }
      qd->setName(normalizePropertyName(detail.definitionName()));
      m_details.append(qd);
      qd->setDetailPropertyMap(dm);
      m_detailMaps.append(dm);

      m_detailFields << qd->fields();
      m_contactMap->insert(normalizePropertyName(detail.definitionName()), QVariant::fromValue(static_cast<QObject*>(dm)));
    }
}

QContact QMLContact::contact() const
{
    QContact c(m_contact); 
    foreach (QObject* o, m_details) {
        QMLContactDetail* d = qobject_cast<QMLContactDetail*>(o);
        if (d && d->detailChanged()) {
            QContactDetail detail = d->detail();
            c.saveDetail(&detail);
        }
    }

    return c;
}

QList<QObject*> QMLContact::detailFields() const
{
    return m_detailFields;
}
QList<QObject*> QMLContact::details() const
{
    return m_details;
}

QVariant QMLContact::contactMap() const
{
    if (m_contactMap)
        return QVariant::fromValue(static_cast<QObject*>(m_contactMap));
    return QVariant();
}


void QMLContact::save()
{
   if (contactChanged()) {
     m_saveRequest.setManager(m_manager);
     m_saveRequest.setContact(contact());
     m_saveRequest.start();
   }
}


bool QMLContact::contactChanged() const
{
   foreach (QObject* o, m_details) {
     QMLContactDetail* detail = qobject_cast<QMLContactDetail*>(o);
     if (detail->detailChanged())
       return true;
   }
   return false;
}

void QMLContact::onContactSaved()
{
   if (m_saveRequest.isFinished() && m_saveRequest.error() == QContactManager::NoError) {
      foreach (QObject* o, m_details) {
        QMLContactDetail* detail = qobject_cast<QMLContactDetail*>(o);
        detail->setDetailChanged(false);
      }
   }
}



int QMLContact::contactId() const
{
    return m_contact.localId();
}

int QMLContact::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_detailFields.count();
}

QVariant QMLContact::data(const QModelIndex &index, int role) const
{
    qWarning() << "index = " << index.row();
    QMLContactDetailField* field = qobject_cast<QMLContactDetailField*>(m_detailFields[index.row()]);
    if (field) {
        qWarning() << "role = " << role;
        switch(role) {
            case Qt::DisplayRole:
            case DetailFieldKeyRole:
                return field->key();
            case DetailNameRole:
                return field->detailName();
            case DetailFieldValueRole:
                return field->value();
            case DetailFieldRole:
                return QVariant::fromValue((QObject*)field);
        }
    }
    return QVariant();
}
