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

#include "qdeclarativecontactmodel_p.h"
#include "qcontactmanager.h"
#include "qcontactdetailfilter.h"
#include "qversitreader.h"
#include "qversitwriter.h"
#include "qversitcontactimporter.h"
#include "qversitcontactexporter.h"
#include <QColor>
#include <QHash>
#include <QDebug>
#include <QPixmap>
#include <QFile>
#include <QMap>

#include "qcontactrequests.h"

class QDeclarativeContactModelPrivate
{
public:
    QDeclarativeContactModelPrivate()
        :m_manager(0),
        m_fetchHint(0),
        m_filter(0)
    {
    }
    ~QDeclarativeContactModelPrivate()
    {
        if (m_manager)
            delete m_manager;
    }

    QList<QDeclarativeContact*> m_contacts;
    QMap<QContactLocalId, QDeclarativeContact*> m_contactMap;
    QContactManager* m_manager;
    QDeclarativeContactFetchHint* m_fetchHint;
    QList<QDeclarativeContactSortOrder*> m_sortOrders;
    QDeclarativeContactFilter* m_filter;

    QVersitReader m_reader;
    QVersitWriter m_writer;
};

QDeclarativeContactModel::QDeclarativeContactModel(QObject *parent) :
    QAbstractListModel(parent),
    d(new QDeclarativeContactModelPrivate)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(ContactRole, "contact");
    setRoleNames(roleNames);

    connect(this, SIGNAL(managerChanged()), SLOT(fetchAgain()));
    connect(this, SIGNAL(filterChanged()), SLOT(fetchAgain()));
    connect(this, SIGNAL(fetchHintChanged()), SLOT(fetchAgain()));
    connect(this, SIGNAL(sortOrdersChanged()), SLOT(fetchAgain()));
    
    d->m_manager = new QContactManager();
    //import vcard
    connect(&d->m_reader, SIGNAL(stateChanged(QVersitReader::State)), this, SLOT(startImport(QVersitReader::State)));
}

QString QDeclarativeContactModel::manager() const
{
    if (d->m_manager)
    	return d->m_manager->managerName();
    return QString();
}


QString QDeclarativeContactModel::error() const
{
    switch (d->m_manager->error()) {
    case QContactManager::DoesNotExistError:
        return QLatin1String("Not exist");
    case QContactManager::AlreadyExistsError:
        return QLatin1String("Already exist");
    case QContactManager::InvalidDetailError:
        return QLatin1String("Invalid detail");
    case QContactManager::InvalidRelationshipError:
        return QLatin1String("Invalid relationship");
    case QContactManager::LockedError:
        return QLatin1String("Locked error");
    case QContactManager::DetailAccessError:
        return QLatin1String("Detail access error");
    case QContactManager::PermissionsError:
        return QLatin1String("Permissions error");
    case QContactManager::OutOfMemoryError:
        return QLatin1String("Out of memory");
    case QContactManager::NotSupportedError:
        return QLatin1String("Not supported");
    case QContactManager::BadArgumentError:
        return QLatin1String("Bad argument");
    case QContactManager::UnspecifiedError:
        return QLatin1String("Unspecified error");
    case QContactManager::VersionMismatchError:
        return QLatin1String("Version mismatch");
    case QContactManager::LimitReachedError:
        return QLatin1String("Limit reached");
    case QContactManager::InvalidContactTypeError:
        return QLatin1String("Invalid contact type");
    default:
        break;
    }
    return QLatin1String("Status ok");
}

QStringList QDeclarativeContactModel::availableManagers() const
{
    return QContactManager::availableManagers();
}
static QString urlToLocalFileName(const QString& str)
{
   QUrl url(str);
   if (!url.isValid()) {
      return str;
   } else if (url.scheme() == "qrc") {
      return url.toString().remove(0, 5).prepend(':');
   } else {
      return url.toString(QUrl::RemoveScheme);
   }

}
void QDeclarativeContactModel::importContacts(const QString& fileName)
{
   qWarning() << "importing contacts from:" << fileName;
   QFile*  file = new QFile(urlToLocalFileName(fileName));
   bool ok = file->open(QIODevice::ReadOnly);
   if (ok) {
      d->m_reader.setDevice(file);
      d->m_reader.startReading();
   }
}

void QDeclarativeContactModel::exportContacts(const QString& fileName)
{
   qWarning() << "exporting contacts into:" << fileName;
   QVersitContactExporter exporter;
   QList<QContact> contacts;
   foreach (QDeclarativeContact* dc, d->m_contacts) {
       contacts.append(dc->contact());
   }

   exporter.exportContacts(contacts, QVersitDocument::VCard30Type);
   QList<QVersitDocument> documents = exporter.documents();
   QFile* file = new QFile(urlToLocalFileName(fileName));
   bool ok = file->open(QIODevice::ReadWrite);
   if (ok) {
      d->m_writer.setDevice(file);
      d->m_writer.startWriting(documents);
   }
}

void QDeclarativeContactModel::contactsExported(QVersitWriter::State state)
{
    if (state == QVersitWriter::FinishedState || state == QVersitWriter::CanceledState) {
         delete d->m_writer.device();
         d->m_writer.setDevice(0);
    }
}

int QDeclarativeContactModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->m_contacts.count();
}

void QDeclarativeContactModel::setManager(const QString& managerName)
{
    if (d->m_manager)
        delete d->m_manager;


    d->m_manager = new QContactManager(managerName);

    qWarning() << "Changed backend to: " << managerName;
    connect(d->m_manager, SIGNAL(dataChanged()), this, SLOT(fetchAgain()));
    emit managerChanged();
}

QDeclarativeContactFilter* QDeclarativeContactModel::filter() const
{
    return d->m_filter;
}

void QDeclarativeContactModel::setFilter(QDeclarativeContactFilter* filter)
{
    if (filter && filter != d->m_filter) {
        if (d->m_filter)
            delete d->m_filter;
        d->m_filter = filter;
        emit filterChanged();
    }
}

QDeclarativeContactFetchHint* QDeclarativeContactModel::fetchHint() const
{
    return d->m_fetchHint;
}
void QDeclarativeContactModel::setFetchHint(QDeclarativeContactFetchHint* fetchHint)
{
    if (fetchHint && fetchHint != d->m_fetchHint) {
        if (d->m_fetchHint)
            delete d->m_fetchHint;
        d->m_fetchHint = fetchHint;
        emit fetchHintChanged();
    }
}


QDeclarativeListProperty<QDeclarativeContact> QDeclarativeContactModel::contacts()
{
    return QDeclarativeListProperty<QDeclarativeContact>(this, d->m_contacts);
}

QDeclarativeListProperty<QDeclarativeContactSortOrder> QDeclarativeContactModel::sortOrders()
{
    return QDeclarativeListProperty<QDeclarativeContactSortOrder>(this, d->m_sortOrders);
}

void QDeclarativeContactModel::startImport(QVersitReader::State state)
{
    if (state == QVersitReader::FinishedState || state == QVersitReader::CanceledState) {
        QVersitContactImporter importer;
        importer.importDocuments(d->m_reader.results());
        QList<QContact> contacts = importer.contacts();

        delete d->m_reader.device();
        d->m_reader.setDevice(0);

        if (d->m_manager) {
            if (d->m_manager->saveContacts(&contacts))
                qWarning() << "contacts imported.";
                fetchAgain();
        }
    }
}

void QDeclarativeContactModel::fetchAgain()
{
    d->m_contacts.clear();
    d->m_contactMap.clear();
   // reset();

    QList<QContactSortOrder> sortOrders;
    foreach (QDeclarativeContactSortOrder* so, d->m_sortOrders) {
        sortOrders.append(so->sortOrder());
    }
    QContactFetchRequest* req = new QContactFetchRequest(this);
    req->setManager(d->m_manager);
    req->setSorting(sortOrders);

    req->setFilter(d->m_filter? d->m_filter->filter() : QContactFilter());
    req->setFetchHint(d->m_fetchHint ? d->m_fetchHint->fetchHint() : QContactFetchHint());

    connect(req,SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(requestUpdated()));

    req->start();
    emit contactsChanged();
}

void QDeclarativeContactModel::requestUpdated()
{
    QContactFetchRequest* req = qobject_cast<QContactFetchRequest*>(QObject::sender());
    if (req && req->isFinished()) {
        QList<QContact> contacts = req->contacts();

        QList<QDeclarativeContact*> dcs;
        foreach(QContact c, contacts) {
            QDeclarativeContact* dc = new QDeclarativeContact(c, d->m_manager->detailDefinitions(c.type()), this);
            dcs.append(dc);
            d->m_contactMap.insert(c.localId(), dc);
        }

        reset();
        beginInsertRows(QModelIndex(), 0, req->contacts().count());
        d->m_contacts = dcs;
        endInsertRows();

        req->deleteLater();
    }
}


void QDeclarativeContactModel::saveContact(QDeclarativeContact* dc)
{
    if (dc) {
        QContact c = dc->contact();
        QContactSaveRequest* req = new QContactSaveRequest(this);
        req->setManager(d->m_manager);
        req->setContact(c);

        connect(req,SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(contactsSaved()));

        req->start();
    }
}


void QDeclarativeContactModel::contactsSaved()
{
    QContactSaveRequest* req = qobject_cast<QContactSaveRequest*>(QObject::sender());
    if (req->isFinished()) {
        if (req->error() == QContactManager::NoError) {
            QList<QContact> cs = req->contacts();
            foreach (const QContact& c, cs) {
                if (d->m_contactMap.contains(c.localId())) {
                    d->m_contactMap.value(c.localId())->setContact(c);
                } else {
                    //new saved contact
                    QDeclarativeContact* dc = new QDeclarativeContact(c, d->m_manager->detailDefinitions(c.type()) , this);
                    d->m_contactMap.insert(c.localId(), dc);
                    beginInsertRows(QModelIndex(), d->m_contacts.count(), d->m_contacts.count());
                    d->m_contacts.append(dc);
                    endInsertRows();
                }
            }
        }
        req->deleteLater();
    }
}


void QDeclarativeContactModel::removeContact(QContactLocalId id)
{
    removeContacts(QList<QContactLocalId>() << id);
}

void QDeclarativeContactModel::removeContacts(const QList<QContactLocalId>& ids)
{
    QContactRemoveRequest* req = new QContactRemoveRequest(this);
    req->setManager(d->m_manager);
    req->setContactIds(ids);

    connect(req,SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(contactRemoved()));

    req->start();
}

void QDeclarativeContactModel::contactsRemoved()
{
    QContactRemoveRequest* req = qobject_cast<QContactRemoveRequest*>(QObject::sender());
    if (req->isFinished()) {
        QList<QContactLocalId> ids = req->contactIds();
        QList<int> errorIds = req->errorMap().keys();


        for( int i = 0; i < ids.count(); i++) {
            if (!errorIds.contains(i)) {
                int row = 0;
                QContactLocalId localId = ids.at(i);
                for (; row < d->m_contacts.count(); row++) {
                    if (d->m_contacts.at(row)->contactId() == localId)
                        break;
                }
                if (row < d->m_contacts.count()) {
                    beginRemoveRows(QModelIndex(), row, row);
                    d->m_contacts.removeAt(row);
                    d->m_contactMap.remove(localId);
                    endRemoveRows();
                } else {
                    //impossible?
                    qWarning() << "this contact " << localId << " was already removed!";
                }
            }
        }
        req->deleteLater();
    }
}


QVariant QDeclarativeContactModel::data(const QModelIndex &index, int role) const
{
    QDeclarativeContact* dc = d->m_contacts.value(index.row());
    QContact c = dc->contact();

    switch(role) {
        case Qt::DisplayRole:
            return c.displayLabel();
        case Qt::DecorationRole:
            {
                QContactThumbnail t = c.detail<QContactThumbnail>();
                if (!t.thumbnail().isNull())
                    return QPixmap::fromImage(t.thumbnail());
                return QPixmap();
            }
        case ContactRole:
            return QVariant::fromValue(dc);
    }
    return QVariant();
}

