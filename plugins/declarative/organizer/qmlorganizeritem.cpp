/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** item: Nokia Corporation (qt-info@nokia.com)
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

#include <qorganizeritemdetails.h>
#include "qmlorganizeritem.h"
#include "qmlorganizeritemdetail.h"
#include "qmlorganizeritemdetailfield.h"


static QString normalizePropertyName(const QString& name)
{
   if (!name.isEmpty())
     return name.mid(1).prepend(name[0].toLower());
   return QString();
}


QMLOrganizerItem::QMLOrganizerItem(QObject *parent)
    :QAbstractListModel(parent),
    m_itemMap(0)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(DetailNameRole, "detailName");
    roleNames.insert(DetailFieldKeyRole, "key");
    roleNames.insert(DetailFieldValueRole, "value");
    roleNames.insert(DetailFieldRole, "field");
    setRoleNames(roleNames);

    connect(&m_saveRequest, SIGNAL(resultsAvailable()), this, SLOT(onitemSaved()));
}

void QMLOrganizerItem::setManager(QOrganizerItemManager* manager)
{
   m_manager = manager;
}
void QMLOrganizerItem::setItem(const QOrganizerItem& c)
{
    m_item = c;

    if (m_itemMap) {
        delete m_itemMap;
        m_detailMaps.clear();
    }

    foreach (QObject* detail, m_details) {
        delete detail;
    }
    m_details.clear();
    m_detailFields.clear();

    m_itemMap = new QDeclarativePropertyMap(this);


    QList<QOrganizerItemDetail> details = m_item.details();

    foreach (const QOrganizerItemDetail& detail, details) {
      QMLOrganizerItemDetail* qd = new QMLOrganizerItemDetail(this);

      QDeclarativePropertyMap* dm = new QDeclarativePropertyMap(m_itemMap);

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
      m_itemMap->insert(normalizePropertyName(detail.definitionName()), QVariant::fromValue(static_cast<QObject*>(dm)));
    }
}

QOrganizerItem QMLOrganizerItem::item() const
{
    QOrganizerItem c(m_item);
    foreach (QObject* o, m_details) {
        QMLOrganizerItemDetail* d = qobject_cast<QMLOrganizerItemDetail*>(o);
        if (d && d->detailChanged()) {
            QOrganizerItemDetail detail = d->detail();
            c.saveDetail(&detail);
        }
    }

    return c;
}

QList<QObject*> QMLOrganizerItem::detailFields() const
{
    return m_detailFields;
}
QList<QObject*> QMLOrganizerItem::details() const
{
    return m_details;
}

QVariant QMLOrganizerItem::itemMap() const
{
    if (m_itemMap)
        return QVariant::fromValue(static_cast<QObject*>(m_itemMap));
    return QVariant();
}


void QMLOrganizerItem::save()
{
   if (itemChanged()) {
     m_saveRequest.setManager(m_manager);
     m_saveRequest.setItem(item());
     m_saveRequest.start();
   }
}


bool QMLOrganizerItem::itemChanged() const
{
   foreach (QObject* o, m_details) {
     QMLOrganizerItemDetail* detail = qobject_cast<QMLOrganizerItemDetail*>(o);
     if (detail->detailChanged())
       return true;
   }
   return false;
}

void QMLOrganizerItem::onItemSaved()
{
   if (m_saveRequest.isFinished() && m_saveRequest.error() == QOrganizerItemManager::NoError) {
      foreach (QObject* o, m_details) {
        QMLOrganizerItemDetail* detail = qobject_cast<QMLOrganizerItemDetail*>(o);
        detail->setDetailChanged(false);
      }
   }
}



int QMLOrganizerItem::itemId() const
{
    return m_item.localId();
}

int QMLOrganizerItem::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_detailFields.count();
}

QVariant QMLOrganizerItem::data(const QModelIndex &index, int role) const
{
    QMLOrganizerItemDetailField* field = qobject_cast<QMLOrganizerItemDetailField*>(m_detailFields[index.row()]);
    if (field) {
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
