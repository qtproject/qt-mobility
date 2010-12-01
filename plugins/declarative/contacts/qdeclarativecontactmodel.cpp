/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
#include <qcontactdetails.h>

#include "qdeclarativecontactmodel_p.h"
#include "qcontactmanager.h"
#include "qcontactdetailfilter.h"
#include "qcontactlocalidfilter.h"
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

/*!
    \qmlclass ContactModel QDeclarativeContactModel
    \brief The ContactModel element provides access to contacts from the contacts store.
    \ingroup qml-contacts

    This element is part of the \bold{QtMobility.contacts 1.1} module.

    ContactModel provides a model of contacts from the contacts store.
    The contents of the model can be specified with \l filter, \l sortOrders and \l fetchHint properties.
    Whether the model is automatically updated when the store or \l contacts changes, can be
    controlled with \l ContactModel::autoUpdate property.

    There are two ways of accessing the contact data: via model by using views and delegates,
    or alternatively via \l contacts list property. Of the two, the model access is preferred.
    Direct list access (i.e. non-model) is not guaranteed to be in order set by \l sortOrder.

    At the moment the model roles provided by ContactModel are display, decoration and \c contact.
    Through the \c contact role can access any data provided by the Contact element.

    \sa RelationshipModel, Contact, {QContactManager}
*/





class QDeclarativeContactModelPrivate
{
public:
    QDeclarativeContactModelPrivate()
        :m_manager(0),
        m_fetchHint(0),
        m_filter(0),
        m_fetchRequest(0),
        m_autoUpdate(true),
        m_updatePending(false),
        m_componentCompleted(false)
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

    QContactFetchRequest* m_fetchRequest;
    QList<QContactLocalId> m_updatedContactIds;
    QVersitReader m_reader;
    QVersitWriter m_writer;
    QStringList m_importProfiles;

    bool m_autoUpdate;
    bool m_updatePending;
    bool m_componentCompleted;
};

QDeclarativeContactModel::QDeclarativeContactModel(QObject *parent) :
    QAbstractListModel(parent),
    d(new QDeclarativeContactModelPrivate)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(ContactRole, "contact");
    setRoleNames(roleNames);

    connect(this, SIGNAL(managerChanged()), SLOT(update()));
    connect(this, SIGNAL(filterChanged()), SLOT(update()));
    connect(this, SIGNAL(fetchHintChanged()), SLOT(update()));
    connect(this, SIGNAL(sortOrdersChanged()), SLOT(update()));
    
    //import vcard
    connect(&d->m_reader, SIGNAL(stateChanged(QVersitReader::State)), this, SLOT(startImport(QVersitReader::State)));
}

/*!
  \qmlproperty string ContactModel::manager

  This property holds the manager uri of the contact backend engine.
  */
QString QDeclarativeContactModel::manager() const
{
    if (d->m_manager)
    	return d->m_manager->managerName();
    return QString();
}
void QDeclarativeContactModel::setManager(const QString& managerName)
{
    if (d->m_manager)
        delete d->m_manager;


    d->m_manager = new QContactManager(managerName);

    connect(d->m_manager, SIGNAL(dataChanged()), this, SLOT(update()));
    connect(d->m_manager, SIGNAL(contactsAdded(QList<QContactLocalId>)), this, SLOT(update()));
    connect(d->m_manager, SIGNAL(contactsRemoved(QList<QContactLocalId>)), this, SLOT(contactsRemoved(QList<QContactLocalId>)));
    connect(d->m_manager, SIGNAL(contactsChanged(QList<QContactLocalId>)), this, SLOT(contactsChanged(QList<QContactLocalId>)));
    emit managerChanged();
}
void QDeclarativeContactModel::componentComplete()
{
    d->m_componentCompleted = true;
    if (!d->m_manager)
        setManager(QString());

    if (d->m_autoUpdate)
        update();
}
/*!
  \qmlproperty bool ContactModel::autoUpdate

  This property indicates whether or not the contact model should be updated automatically, default value is true.
  */
void QDeclarativeContactModel::setAutoUpdate(bool autoUpdate)
{
    if (autoUpdate == d->m_autoUpdate)
        return;
    d->m_autoUpdate = autoUpdate;
    emit autoUpdateChanged();
}

bool QDeclarativeContactModel::autoUpdate() const
{
    return d->m_autoUpdate;
}

void QDeclarativeContactModel::update()
{
    if (!d->m_componentCompleted || d->m_updatePending)
        return;
    d->m_updatePending = true; // Disallow possible duplicate request triggering
    QMetaObject::invokeMethod(this, "fetchAgain", Qt::QueuedConnection);
}

void QDeclarativeContactModel::cancelUpdate()
{
    if (d->m_fetchRequest) {
        d->m_fetchRequest->cancel();
        d->m_fetchRequest->deleteLater();
        d->m_fetchRequest = 0;
        d->m_updatePending = false;
    }
}

/*!
  \qmlproperty string ContactModel::error

  This property holds the latest error code returned by the contact manager.

  This property is read only.
  */
QString QDeclarativeContactModel::error() const
{
    if (!d->m_manager)
        return QLatin1String("Invalid contact manager");
    switch (d->m_manager->error()) {
    case QContactManager::DoesNotExistError:
        return QLatin1String("DoesNotExist");
    case QContactManager::AlreadyExistsError:
        return QLatin1String("AlreadyExists");
    case QContactManager::InvalidDetailError:
        return QLatin1String("InvalidDetail");
    case QContactManager::InvalidRelationshipError:
        return QLatin1String("InvalidRelationship");
    case QContactManager::LockedError:
        return QLatin1String("LockedError");
    case QContactManager::DetailAccessError:
        return QLatin1String("DetailAccessError");
    case QContactManager::PermissionsError:
        return QLatin1String("PermissionsError");
    case QContactManager::OutOfMemoryError:
        return QLatin1String("OutOfMemory");
    case QContactManager::NotSupportedError:
        return QLatin1String("NotSupported");
    case QContactManager::BadArgumentError:
        return QLatin1String("BadArgument");
    case QContactManager::UnspecifiedError:
        return QLatin1String("UnspecifiedError");
    case QContactManager::VersionMismatchError:
        return QLatin1String("VersionMismatch");
    case QContactManager::LimitReachedError:
        return QLatin1String("LimitReached");
    case QContactManager::InvalidContactTypeError:
        return QLatin1String("InvalidContactType");
    default:
        break;
    }
    return QLatin1String("NoError");
}


/*!
  \qmlproperty list<string> ContactModel::availableManagers

  This property holds the list of available manager names.
  This property is read only.
  */
QStringList QDeclarativeContactModel::availableManagers() const
{
    return QContactManager::availableManagers();
}
static QString urlToLocalFileName(const QUrl& url)
{
   if (!url.isValid()) {
      return url.toString();
   } else if (url.scheme() == "qrc") {
      return url.toString().remove(0, 5).prepend(':');
   } else {
      return url.toLocalFile();
   }

}

/*!
  \qmlmethod ContactModel::importContacts(url url, list<string> profiles)

  Import contacts from a vcard by the given \a url and optional \a profiles.
  */
void QDeclarativeContactModel::importContacts(const QUrl& url, const QStringList& profiles)
{
   //qWarning() << "importing contacts from:" << url;
   d->m_importProfiles = profiles;

   //TODO: need to allow download vcard from network
   QFile*  file = new QFile(urlToLocalFileName(url));
   bool ok = file->open(QIODevice::ReadOnly);
   if (ok) {
      d->m_reader.setDevice(file);
      d->m_reader.startReading();
   }
}

/*!
  \qmlmethod ContactModel::exportContacts(url url, list<string> profiles)

  Export contacts into a vcard file to the given \a url by optional \a profiles.
  At the moment only the local file url is supported in export method.
  */
void QDeclarativeContactModel::exportContacts(const QUrl& url, const QStringList& profiles)
{
   //qWarning() << "exporting contacts into:" << url;

   QString profile = profiles.isEmpty()? QString() : profiles.at(0);
    //only one profile string supported now
   QVersitContactExporter exporter(profile);

   QList<QContact> contacts;
   foreach (QDeclarativeContact* dc, d->m_contacts) {
       contacts.append(dc->contact());
   }

   exporter.exportContacts(contacts, QVersitDocument::VCard30Type);
   QList<QVersitDocument> documents = exporter.documents();
   QFile* file = new QFile(urlToLocalFileName(url));
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



/*!
  \qmlproperty Filter ContactModel::filter

  This property holds the filter instance used by the contact model.

  \sa Filter
  */
QDeclarativeContactFilter* QDeclarativeContactModel::filter() const
{
    return d->m_filter;
}

void QDeclarativeContactModel::setFilter(QDeclarativeContactFilter* filter)
{
    d->m_filter = filter;
    if (d->m_filter) {
        connect(d->m_filter, SIGNAL(filterChanged()), SLOT(update()));
        emit filterChanged();
    }
}

/*!
  \qmlproperty FetchHint ContactModel::fetchHint

  This property holds the fetch hint instance used by the contact model.

  \sa FetchHint
  */
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

/*!
  \qmlproperty QDeclarativeListProperty ContactModel::contacts

  This property holds a list of contacts.

  \sa Contact
  */
QDeclarativeListProperty<QDeclarativeContact> QDeclarativeContactModel::contacts()
{
    return QDeclarativeListProperty<QDeclarativeContact>(this,
                                                         0,
                                                         contacts_append,
                                                         contacts_count,
                                                         contacts_at,
                                                         contacts_clear);
}



void QDeclarativeContactModel::contacts_append(QDeclarativeListProperty<QDeclarativeContact>* prop, QDeclarativeContact* contact)
{
    Q_UNUSED(prop);
    Q_UNUSED(contact);
    qWarning() << "ContactModel: appending contacts is not currently supported";
}

int QDeclarativeContactModel::contacts_count(QDeclarativeListProperty<QDeclarativeContact>* prop)
{
    return static_cast<QDeclarativeContactModel*>(prop->object)->d->m_contacts.count();
}

QDeclarativeContact* QDeclarativeContactModel::contacts_at(QDeclarativeListProperty<QDeclarativeContact>* prop, int index)
{
    return static_cast<QDeclarativeContactModel*>(prop->object)->d->m_contacts.at(index);
}

void QDeclarativeContactModel::contacts_clear(QDeclarativeListProperty<QDeclarativeContact>* prop)
{
    QDeclarativeContactModel* model = static_cast<QDeclarativeContactModel*>(prop->object);
    model->clearContacts();
    emit model->contactsChanged();
}


/*!
  \qmlproperty QDeclarativeListProperty ContactModel::sortOrders

  This property holds a list of sort orders used by the organizer model.

  \sa SortOrder
  */
QDeclarativeListProperty<QDeclarativeContactSortOrder> QDeclarativeContactModel::sortOrders()
{
    return QDeclarativeListProperty<QDeclarativeContactSortOrder>(this,
                                                                  0,
                                                                  sortOrder_append,
                                                                  sortOrder_count,
                                                                  sortOrder_at,
                                                                  sortOrder_clear);
}

void QDeclarativeContactModel::startImport(QVersitReader::State state)
{
    if (state == QVersitReader::FinishedState || state == QVersitReader::CanceledState) {
        QVersitContactImporter importer(d->m_importProfiles);
        importer.importDocuments(d->m_reader.results());
        QList<QContact> contacts = importer.contacts();

        delete d->m_reader.device();
        d->m_reader.setDevice(0);

        if (d->m_manager) {
            if (d->m_manager->saveContacts(&contacts))
                qWarning() << "contacts imported.";
                update();
        }
    }
}

/*!
  \qmlmethod ContactModel::fetchContacts(list<int> contactIds)
  Fetch a list of contacts from the contacts store by given \a contactIds.
  */
void QDeclarativeContactModel::fetchContacts(const QList<QContactLocalId>& contactIds)
{
    d->m_updatedContactIds = contactIds;
    d->m_updatePending = true;
    QMetaObject::invokeMethod(this, "fetchAgain", Qt::QueuedConnection);
}
void QDeclarativeContactModel::clearContacts()
{
    d->m_contacts.clear();
    qDeleteAll(d->m_contacts);
    d->m_contactMap.clear();
}

void QDeclarativeContactModel::fetchAgain()
{
    cancelUpdate();
    if (d->m_updatedContactIds.isEmpty()) //fetch all contacts
        clearContacts();

    QList<QContactSortOrder> sortOrders;
    foreach (QDeclarativeContactSortOrder* so, d->m_sortOrders) {
        sortOrders.append(so->sortOrder());
    }
    d->m_fetchRequest = new QContactFetchRequest(this);
    d->m_fetchRequest->setManager(d->m_manager);
    d->m_fetchRequest->setSorting(sortOrders);

    if (!d->m_updatedContactIds.isEmpty()) {
        QContactLocalIdFilter f;
        f.setIds(d->m_updatedContactIds);
        d->m_fetchRequest->setFilter(f);
        d->m_updatedContactIds.clear();
    } else if (d->m_filter){
        d->m_fetchRequest->setFilter(d->m_filter->filter());
    } else {
        d->m_fetchRequest->setFilter(QContactFilter());
    }

    d->m_fetchRequest->setFetchHint(d->m_fetchHint ? d->m_fetchHint->fetchHint() : QContactFetchHint());

    connect(d->m_fetchRequest,SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(requestUpdated()));

    d->m_fetchRequest->start();
}

void QDeclarativeContactModel::requestUpdated()
{
    //Don't use d->m_fetchRequest, this pointer might be invalid if cancelUpdate() was called, use QObject::sender() instead.
    QContactFetchRequest* req = qobject_cast<QContactFetchRequest*>(QObject::sender());
    if (req && req->isFinished()) {
        QList<QContact> contacts = req->contacts();
        if (d->m_contacts.isEmpty()) {
            QList<QDeclarativeContact*> dcs;
            foreach (QContact c, contacts) {
                QDeclarativeContact* dc = new QDeclarativeContact(c, d->m_manager->detailDefinitions(c.type()), this);
                dcs.append(dc);
                d->m_contactMap.insert(c.localId(), dc);
            }

            reset();
            beginInsertRows(QModelIndex(), 0, req->contacts().count());
            d->m_contacts = dcs;
            endInsertRows();
        } else {
            //Partial updating, insert the fetched contacts into the the exist contact list.
            QList<QDeclarativeContact*> dcs;
            foreach (QContact c, contacts) {
                if (d->m_contactMap.contains(c.localId())) {
                    d->m_contactMap.value(c.localId())->setContact(c);
                } else {
                    QDeclarativeContact* dc = new QDeclarativeContact(c, d->m_manager->detailDefinitions(c.type()), this);
                    dcs.append(dc);
                    d->m_contactMap.insert(c.localId(), dc);
                }
            }
            beginInsertRows(QModelIndex(), d->m_contacts.count(), req->contacts().count());
            d->m_contacts.append(dcs);
            endInsertRows();
        }
        emit contactsChanged();
        emit errorChanged();
        req->deleteLater();
        d->m_fetchRequest = 0;
        d->m_updatePending = false;
    }
}

/*!
  \qmlmethod ContactModel::saveContact(Contact contact)
  Save the given \a contact into the contacts store. Once saved successfully, the dirty flags of this contact will be reset.

  \sa Contact::modified
  */
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
        emit errorChanged();
    }
}

/*!
  \qmlmethod ContactModel::removeContact(int contactId)
  Remove the contact from the contacts store by given \a contactId.
  */
void QDeclarativeContactModel::removeContact(QContactLocalId id)
{
    removeContacts(QList<QContactLocalId>() << id);
}

/*!
  \qmlmethod ContactModel::removeContacts(list<int> contactIds)
  Remove the list of contacts from the contacts store by given \a contactIds.
  */

void QDeclarativeContactModel::removeContacts(const QList<QContactLocalId>& ids)
{
    QContactRemoveRequest* req = new QContactRemoveRequest(this);
    req->setManager(d->m_manager);
    req->setContactIds(ids);

    connect(req,SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(contactRemoved()));

    req->start();
}

void QDeclarativeContactModel::contactsRemoved(const QList<QContactLocalId>& ids)
{
    bool emitSignal = false;
    foreach (const QContactLocalId& id, ids) {
        if (d->m_contactMap.contains(id)) {
            int row = 0;
            //TODO:need a fast lookup
            for (; row < d->m_contacts.count(); row++) {
                if (d->m_contacts.at(row)->contactId() == id)
                    break;
            }

            if (row < d->m_contacts.count()) {
                beginRemoveRows(QModelIndex(), row, row);
                d->m_contacts.removeAt(row);
                d->m_contactMap.remove(id);
                endRemoveRows();
                emitSignal = true;
            }
        }
    }
    emit errorChanged();
    if (emitSignal)
        emit contactsChanged();
}

void QDeclarativeContactModel::contactsChanged(const QList<QContactLocalId>& ids)
{
    if (d->m_autoUpdate) {
        QList<QContactLocalId> updatedIds;
        foreach (const QContactLocalId& id, ids) {
            if (d->m_contactMap.contains(id)) {
                updatedIds << id;
            }
        }

        if (updatedIds.count() > 0)
            fetchContacts(updatedIds);
    }
}

void QDeclarativeContactModel::contactsRemoved()
{
    if (d->m_autoUpdate) {
        QContactRemoveRequest* req = qobject_cast<QContactRemoveRequest*>(QObject::sender());


        if (req->isFinished()) {
            QList<QContactLocalId> ids = req->contactIds();
            QList<int> errorIds = req->errorMap().keys();
            QList<QContactLocalId> removedIds;
            for (int i = 0; i < ids.count(); i++) {
                if (!errorIds.contains(i))
                    removedIds << ids.at(i);
            }
            if (!removedIds.isEmpty())
                contactsRemoved(removedIds);
            req->deleteLater();
        }
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


void QDeclarativeContactModel::sortOrder_append(QDeclarativeListProperty<QDeclarativeContactSortOrder> *p, QDeclarativeContactSortOrder *sortOrder)
{
    QDeclarativeContactModel* model = qobject_cast<QDeclarativeContactModel*>(p->object);
    if (model && sortOrder) {
        QObject::connect(sortOrder, SIGNAL(sortOrderChanged()), model, SIGNAL(sortOrdersChanged()));
        model->d->m_sortOrders.append(sortOrder);
        emit model->sortOrdersChanged();
    }
}

int  QDeclarativeContactModel::sortOrder_count(QDeclarativeListProperty<QDeclarativeContactSortOrder> *p)
{
    QDeclarativeContactModel* model = qobject_cast<QDeclarativeContactModel*>(p->object);
    if (model)
        return model->d->m_sortOrders.size();
    return 0;
}
QDeclarativeContactSortOrder * QDeclarativeContactModel::sortOrder_at(QDeclarativeListProperty<QDeclarativeContactSortOrder> *p, int idx)
{
    QDeclarativeContactModel* model = qobject_cast<QDeclarativeContactModel*>(p->object);

    QDeclarativeContactSortOrder* sortOrder = 0;
    if (model) {
        int i = 0;
        foreach (QDeclarativeContactSortOrder* s, model->d->m_sortOrders) {
            if (i == idx) {
                sortOrder = s;
                break;
            } else {
                i++;
            }
        }
    }
    return sortOrder;
}
void  QDeclarativeContactModel::sortOrder_clear(QDeclarativeListProperty<QDeclarativeContactSortOrder> *p)
{
    QDeclarativeContactModel* model = qobject_cast<QDeclarativeContactModel*>(p->object);

    if (model) {
        model->d->m_sortOrders.clear();
        emit model->sortOrdersChanged();
    }
}


