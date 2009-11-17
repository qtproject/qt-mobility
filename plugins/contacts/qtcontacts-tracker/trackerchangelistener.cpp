/* * This file is part of qtcontacts-tracker
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia.
 */


#include <QtTracker/ontologies/nco.h>
#include <QDebug>

#include "trackerchangelistener.h"
#include "qcontact.h"

using namespace SopranoLive;

TrackerChangeListener::TrackerChangeListener(QObject* parent)
:QObject(parent)
{
    SopranoLive::BackEnds::Tracker::ClassUpdateSignaler *signaler =
            SopranoLive::BackEnds::Tracker::ClassUpdateSignaler::get(
                    nco::Contact::iri());
    // Note here that we are not using
    // QAbstractItemModel signals from LiveNodes::model() because
    // node list for which notification comes is fixed. Those are used for
    // async implementation
    if (signaler)
    {
        connect(signaler, SIGNAL(subjectsAdded(const QStringList &)),
                SLOT(subjectsAdded(const QStringList &)));
        connect(signaler,
                SIGNAL(baseRemoveSubjectsd(const QStringList &)),
                SLOT(subjectsRemoved(const QStringList &)));
        connect(signaler,
                SIGNAL(subjectsChanged(const QStringList &)),
                SLOT(subjectsChanged(const QStringList &)));
    }
    //this corresponds with telepathysupport/ TrackerSink::onSimplePresenceChanged
       signaler = SopranoLive::BackEnds::Tracker::ClassUpdateSignaler::get(
                    nfo::IMAccount::iri());
        connect(signaler,
                SIGNAL(subjectsChanged(const QStringList &)),
                SLOT(imAccountChanged(const QStringList &)));

}

TrackerChangeListener::~TrackerChangeListener()
{
}
// TEMPORARY here we'll for now extract ids from tracker contact URI.
// In future need nonblocking async way to get contact ids from tracker contact urls
// let's see which signals will be used from libqttracker
QContactLocalId url2UniqueId(const QString &contactUrl)
{
    QRegExp rx("(\\d+)");
    bool conversion = false;
    QContactLocalId id = 0;
    if( rx.indexIn(contactUrl) != -1 )
    {
        id = rx.cap(1).toUInt(&conversion, 10);
    }
    if( !conversion )
        qWarning() << Q_FUNC_INFO << "unparsed uri to uniqueI:" << contactUrl;
    return id;

}

void TrackerChangeListener::subjectsAdded(const QStringList &subjects)
{
    QList<QContactLocalId> added;
    foreach(const QString &uri, subjects)
    {
        added << url2UniqueId(uri);
    }
    qDebug() << Q_FUNC_INFO << "added contactids:" << added;
    emit contactsAdded(added);
}

void TrackerChangeListener::subjectsRemoved(const QStringList &subjects)
{
    QList<QContactLocalId> added;
    foreach(const QString &uri, subjects)
    {
        added << url2UniqueId(uri);
    }
    qDebug() << Q_FUNC_INFO << "removed contactids:" << added;
    emit contactsRemoved(added);
}

// TODO data changed for full query
void TrackerChangeListener::subjectsChanged(const QStringList &subjects)
{
    QList<QContactLocalId> added;
    foreach(const QString &uri, subjects)
    {
        added << url2UniqueId(uri);
    }
    qDebug() << Q_FUNC_INFO << "changed contactids:" << added;
    emit contactsChanged(added);
}

void TrackerChangeListener::imAccountChanged(const QStringList& subjects) {
    // leave the debug output for few days as TODO remainder to fix writing to tracker
    qDebug() << Q_FUNC_INFO << subjects;

    RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
    // fetch all changed contacts at once
    QSet<QUrl> urls;
    foreach(const QString &str, subjects)
        urls << QUrl(str);
    RDFContact.property<nco::hasIMAccount>().isMemberOf(urls.toList());
    RDFSelect query;
    query.addColumn("contactId", RDFContact.property<nco::contactUID>());

    QSharedPointer<AsyncQuery> request = QSharedPointer<AsyncQuery> (new AsyncQuery(query),
                       &QObject::deleteLater);
    connect(request.data(), SIGNAL(queryReady(AsyncQuery*)), SLOT(imQueryReady(AsyncQuery*)));
    pendingQueries[request.data()] = request;
}

AsyncQuery::AsyncQuery(RDFSelect selectQuery)
{
    nodes = ::tracker()->modelQuery(selectQuery);
    QObject::connect(nodes.model(), SIGNAL(modelUpdated()), this,
            SLOT(queryReady()));
}

void AsyncQuery::queryReady()
{
    emit queryReady(this);
}

void TrackerChangeListener::imQueryReady(AsyncQuery* req)
{
    if( pendingQueries.contains(req))
    {
        QSharedPointer<AsyncQuery> query = pendingQueries.take(req);
        QSet<QContactLocalId> contactsChangedPresence;
        for(int i=0; i<query->nodes->rowCount(); i++) {
            contactsChangedPresence << query->nodes->index(i, 0).data().toUInt();
        }
        if( !contactsChangedPresence.isEmpty() )
            emit contactsChanged(contactsChangedPresence.toList());
    }
}
