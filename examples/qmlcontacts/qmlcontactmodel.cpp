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

#include "qmlcontactmodel.h"
#include "qcontactmanager.h"
#include "qcontactdetailfilter.h"
#include "qversitreader.h"
#include "qversitcontactimporter.h"
#include <QColor>
#include <QHash>
#include <QDebug>
#include <QPixmap>
#include <QFile>


QMLContactModel::QMLContactModel(QObject *parent) :
    QAbstractListModel(parent),
    m_manager(0)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(InterestLabelRole, "interestLabel");
    roleNames.insert(InterestRole, "interest");
    roleNames.insert(ContactRole, "contact");
    roleNames.insert(ContactIdRole, "contactId");
    roleNames.insert(AvatarRole, "avatar");
    roleNames.insert(PresenceAvailableRole, "presenceSupported");
    roleNames.insert(PresenceTextRole, "presenceText");
    roleNames.insert(PresenceStateRole, "presenceState");
    roleNames.insert(PresenceMessageRole, "presenceMessage");
    setRoleNames(roleNames);

    m_fetchHint.setOptimizationHints(QContactFetchHint::NoActionPreferences | QContactFetchHint::NoRelationships);
    m_fetchHint.setDetailDefinitionsHint(QStringList()
                                         << QContactPhoneNumber::DefinitionName
                                         << QContactEmailAddress::DefinitionName
                                         << QContactThumbnail::DefinitionName
                                         << QContactAvatar::DefinitionName
                                         << QContactGlobalPresence::DefinitionName
                                         << QContactDisplayLabel::DefinitionName
                                         << QContactOnlineAccount::DefinitionName);
    m_sortOrder.setDetailDefinitionName(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel);
    m_sortOrder.setCaseSensitivity(Qt::CaseSensitive);

    connect(&m_contactsRequest, SIGNAL(resultsAvailable()), this, SLOT(resultsReceived()));
    m_contactsRequest.setFetchHint(m_fetchHint);
    m_contactsRequest.setSorting(m_sortOrder);

    QContactDetailFilter d;
    d.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    d.setValue(QContactType::TypeContact);

    m_contactsRequest.setFilter(d);

    setManager(QString());
}

QStringList QMLContactModel::availableManagers() const
{
    return QContactManager::availableManagers();
}

QString QMLContactModel::manager() const
{
    return m_manager->managerName();
}
QList<QObject*> QMLContactModel::details(int id) const
{
    if (m_contactMap.contains(id))
        return m_contactMap.value(id)->details();
    return QList<QObject*>();
}
void QMLContactModel::exposeContactsToQML()
{
    foreach (const QContact& c, m_contacts) {
        if (!m_contactMap.contains(c.localId())) {
            QMLContact* qc = new QMLContact(this);
            qc->setContact(c);
            m_contactMap.insert(c.localId(), qc);
        } else {
            m_contactMap.value(c.localId())->setContact(c);
        }
    }
}

void QMLContactModel::fillContactsIntoMemoryEngine(QContactManager* manager)
{
    QVersitReader reader;
    QFile file("contents/example.vcf");
    bool ok = file.open(QIODevice::ReadOnly);
    if (ok) {
       reader.setDevice(&file);
       if (reader.startReading() && reader.waitForFinished()) {
           QVersitContactImporter importer;
           importer.importDocuments(reader.results());
           QList<QContact> contacts = importer.contacts();
           manager->saveContacts(&contacts, 0);
       }
    }
}

int QMLContactModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_contacts.count();
}

void QMLContactModel::setManager(const QString& managerName)
{
    if (m_manager)
        delete m_manager;

    foreach (const QContactLocalId& id, m_contactMap.keys()) {
        delete m_contactMap.value(id);
    }
    m_contactMap.clear();

    m_manager = new QContactManager(managerName);

    if (managerName == "memory" && m_manager->contactIds().isEmpty()) {
        fillContactsIntoMemoryEngine(m_manager);
    }

    qWarning() << "Changed backend to: " << managerName;
    m_contactsRequest.setManager(m_manager);
    connect(m_manager, SIGNAL(dataChanged()), this, SLOT(fetchAgain()));
    fetchAgain();
}

void QMLContactModel::resultsReceived()
{
    int oldCount = m_contacts.count();

    int newCount = m_contactsRequest.contacts().count();
    if (newCount > oldCount) {
        // Assuming the order is the same
        beginInsertRows(QModelIndex(), newCount - oldCount, newCount);
        m_contacts = m_contactsRequest.contacts();
        endInsertRows();
    } else {
        // Hmm, shouldn't happen
        reset();
        beginInsertRows(QModelIndex(), 0, m_contactsRequest.contacts().count());
        m_contacts =  m_contactsRequest.contacts();
        endInsertRows();
    }

    exposeContactsToQML();
}

void QMLContactModel::fetchAgain()
{
    m_contacts.clear();
    reset();
    m_contactsRequest.start();
}

QPair<QString, QString> QMLContactModel::interestingDetail(const QContact&c) const
{
    // Try a phone number, then email, then online account
    // This does only check the first detail of each type
    QContactDetail p = c.details<QContactPhoneNumber>().value(0);
    if (!p.isEmpty())
        return qMakePair(tr("Phone"), p.value(QContactPhoneNumber::FieldNumber));

    p = c.details<QContactEmailAddress>().value(0);
    if (!p.isEmpty())
        return qMakePair(tr("Email"), p.value(QContactEmailAddress::FieldEmailAddress));

    p = c.details<QContactOnlineAccount>().value(0);
    if (!p.isEmpty())
        return qMakePair(p.value(QContactOnlineAccount::FieldServiceProvider), p.value(QContactOnlineAccount::FieldAccountUri));

    // Well, don't know.
    return qMakePair(QString(), QString());
}



QVariant QMLContactModel::data(const QModelIndex &index, int role) const
{
    QContact c = m_contacts.value(index.row());
    switch(role) {
        case Qt::DisplayRole:
            return c.displayLabel();
        case InterestLabelRole:
            return interestingDetail(c).first;
        case InterestRole:
            return interestingDetail(c).second;
        case ContactRole:
            if (m_contactMap.contains(c.localId())) {
               return m_contactMap.value(c.localId())->contactMap();
           }
        case ContactIdRole:
            return c.localId();
        case AvatarRole:
            //Just let the imager provider deal with it
            return QString("image://thumbnail/%1.%2").arg(manager()).arg(c.localId());
        case Qt::DecorationRole:
            {
                QContactThumbnail t = c.detail<QContactThumbnail>();
                if (!t.thumbnail().isNull())
                    return QPixmap::fromImage(t.thumbnail());
            }
            return QPixmap();
        case PresenceAvailableRole:
            return !c.detail<QContactGlobalPresence>().isEmpty();
        case PresenceMessageRole:
            return c.detail<QContactGlobalPresence>().customMessage();
        case PresenceTextRole:
            return c.detail<QContactGlobalPresence>().presenceStateText();
        case PresenceStateRole:
            return c.detail<QContactGlobalPresence>().presenceState();
    }
    return QVariant();
}

