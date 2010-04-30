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

#include "trackerchangelistener.h"
#include "qcontact.h"

using namespace SopranoLive;

TrackerChangeListener::TrackerChangeListener(QContactManagerEngine *eng, QObject* parent) :
    QObject(parent), engine(eng)
{
    signaler_contact = SopranoLive::BackEnds::Tracker::ClassUpdateSignaler::get(nco::Contact::iri());
    if (signaler_contact)
    {
        SopranoLive::BackEnds::Tracker::ClassUpdateSignaler * signaler = signaler_contact;
        connect(signaler, SIGNAL(subjectsAdded(const QStringList &)), SLOT(contactsAdded(const QStringList &)));
        connect(signaler,SIGNAL(subjectsRemoved(const QStringList &)),SLOT(contactsRemoved(const QStringList &)));
        connect(signaler,SIGNAL(subjectsChanged(const QStringList &)),SLOT(contactsChanged(const QStringList &)));
    }

    signaler_imaccount = SopranoLive::BackEnds::Tracker::ClassUpdateSignaler::get(nco::IMAccount::iri());
    if (signaler_imaccount)
    {
        // same for all signals - emit selfContact changed
        SopranoLive::BackEnds::Tracker::ClassUpdateSignaler * signaler = signaler_imaccount;
        connect(signaler, SIGNAL(subjectsAdded(const QStringList &)),SLOT(imAccountsChanged(const QStringList &)));
        connect(signaler,SIGNAL(subjectsRemoved(const QStringList &)),SLOT(imAccountsChanged(const QStringList &)));
        connect(signaler,SIGNAL(subjectsChanged(const QStringList &)),SLOT(imAccountsChanged(const QStringList &)));
    }

    signaler_imaddress = SopranoLive::BackEnds::Tracker::ClassUpdateSignaler::get(nco::IMAddress::iri());
    if (signaler_imaddress)
    {
        // same for all signals - contact changed to be emitted
        SopranoLive::BackEnds::Tracker::ClassUpdateSignaler * signaler = signaler_imaddress;
        connect(signaler, SIGNAL(subjectsAdded(const QStringList &)),SLOT(imAddressesChanged(const QStringList &)));
        connect(signaler,SIGNAL(subjectsRemoved(const QStringList &)),SLOT(imAddressesChanged(const QStringList &)));
        connect(signaler,SIGNAL(subjectsChanged(const QStringList &)),SLOT(imAddressesChanged(const QStringList &)));
    }

}

TrackerChangeListener::~TrackerChangeListener()
{
    if (signaler_imaddress)
        signaler_imaddress->disconnect(this);
    if (signaler_contact)
        signaler_contact->disconnect(this);
    if (signaler_imaccount)
        signaler_imaccount->disconnect(this);
}

QContactLocalId url2UniqueId(const QString &contactUrl)
{
    /* handle conatact:interger URL types comming from
       which are non telepathy url's
    */
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

void TrackerChangeListener::contactsAdded(const QStringList &subjects)
{
    QList<QContactLocalId> added;
    foreach(const QString &uri, subjects)
    {
        added << url2UniqueId(uri);
    }
    emit contactsAdded(added);
}

void TrackerChangeListener::contactsRemoved(const QStringList &subjects)
{
    QList<QContactLocalId> added;
    foreach(const QString &uri, subjects)
    {
        added << url2UniqueId(uri);
    }
    emit contactsRemoved(added);
}


void TrackerChangeListener::contactsChanged(const QStringList &subjects)
{
    QList<QContactLocalId> changed;
    foreach(const QString &uri, subjects) {
        QContactLocalId id = url2UniqueId(uri);
        if (changed.contains(id) == false) {
            changed << id;
        }
    }
    emit contactsChanged(changed);
}

void TrackerChangeListener::imAccountsChanged(const QStringList &subjects)
{
    Q_UNUSED(subjects)
    QContactManager::Error error;
    QContactLocalId selfId = engine->selfContactId(&error);
    if (engine && QContactManager::NoError == error) {
        emit contactsChanged(QList<QContactLocalId>()<<selfId);
    } else {
        qWarning() << __PRETTY_FUNCTION__ << "Signal not propagated:" << engine << error;
    }
}

void TrackerChangeListener::imAddressesChanged(const QStringList &subjects)
{
    Q_UNUSED(subjects)
    // TODO use QHash in engine - mapping IMAddress URIs to contacts
    qWarning() << __PRETTY_FUNCTION__ << "Not implemented";
}
