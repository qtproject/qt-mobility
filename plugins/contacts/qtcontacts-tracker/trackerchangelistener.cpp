/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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


#include <QtTracker/ontologies/nco.h>
#include <QDebug>

#include "trackerchangelistener.h"
#include "qcontact.h"
#include "debuglevel.h"

using namespace SopranoLive;
using namespace hcontacts;

TrackerChangeListener::TrackerChangeListener(QObject* parent)
:QObject(parent)
{
    SopranoLive::BackEnds::Tracker::ClassUpdateSignaler *signaler =
            SopranoLive::BackEnds::Tracker::ClassUpdateSignaler::get(
                    nco::PersonContact::iri());
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
        warning() << Q_FUNC_INFO << "unparsed uri to uniqueI:" << contactUrl;
    return id;

}

void TrackerChangeListener::subjectsAdded(const QStringList &subjects)
{
    QList<QContactLocalId> added;
    foreach(const QString &uri, subjects)
    {
        added << url2UniqueId(uri);
    }
    debug() << Q_FUNC_INFO << "added contactids:" << added;
    emit contactsAdded(added);
}

void TrackerChangeListener::subjectsRemoved(const QStringList &subjects)
{
    QList<QContactLocalId> added;
    foreach(const QString &uri, subjects)
    {
        added << url2UniqueId(uri);
    }
    debug() << Q_FUNC_INFO << "removed contactids:" << added;
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
    debug() << Q_FUNC_INFO << "changed contactids:" << added;
    emit contactsChanged(added);
}

void TrackerChangeListener::imAccountChanged(const QStringList& subjects) {
    // leave the debug output for few days as TODO remainder to fix writing to tracker
    debug() << Q_FUNC_INFO << subjects;

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
