/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

[sbox-maemo6-i486: ~/w/QMLContacts] > xeyes
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmlcontactsa.h"

#include <QtDebug>
#include <qcontactfetchrequest.h>
#include <qcontactlocalidfilter.h>
#include <qcontactdetails.h>

QTM_USE_NAMESPACE

QTM_BEGIN_NAMESPACE

// ![0]
QMLContactManagerAsync::QMLContactManagerAsync(QObject *parent)
: QObject(parent)
{
    qc = new QContactManager();

}

QMLContactManagerAsync::~QMLContactManagerAsync()
{
    delete qc;
}

QString QMLContactManagerAsync::availableManagers() const
{
    return QContactManager::availableManagers().join(" ");
}

QString QMLContactManagerAsync::manager()
{
    return qc->managerName();
}

void QMLContactManagerAsync::fillContactsIntoMemoryEngine(QContactManager* manager)
{
    QContact c;
    QContactName name;
    QContactOnlineAccount account;
    QContactAvatar avatar;
    
    name.setFirst(QString::fromLatin1("Charles"));
    name.setLast(QString::fromLatin1("Yin"));

    avatar.setAvatar(QString::fromLatin1("C:\\workspace\\qtm-contacts\\examples\\qml-contacts\\contents/pics\\fruit-salad.jpg"));
    avatar.setSubType(QContactAvatar::SubTypeImage);
    account.setAccountUri(QString::fromLatin1("yinyunqiao@hotmail.com"));
    
    c.saveDetail(&name);
    c.saveDetail(&avatar);
    c.saveDetail(&account);
    manager->saveContact(&c);

    name.setFirst(QString::fromLatin1("Michael"));
    name.setLast(QString::fromLatin1("Goddard"));
    account.setAccountUri(QString::fromLatin1("MichaelGoddard@hotmail.com"));
    c.setId(QContactId());
    c.saveDetail(&name);
    c.saveDetail(&avatar);
    c.saveDetail(&account);
    manager->saveContact(&c);

    name.setFirst(QString::fromLatin1("Christopher"));
    name.setLast(QString::fromLatin1("Adams"));
    account.setAccountUri(QString::fromLatin1("ChristopherAdams@hotmail.com"));
    account.setSubTypes(QContactOnlineAccount::SubTypeImpp);
    c.setId(QContactId());
    c.saveDetail(&name);
    c.saveDetail(&avatar);
    c.saveDetail(&account);
    manager->saveContact(&c);


    name.setFirst(QString::fromLatin1("Kevin"));
    name.setLast(QString::fromLatin1("Wu Won"));
    account.setAccountUri(QString::fromLatin1("KevinWuWon@hotmail.com"));
    account.setSubTypes(QContactOnlineAccount::SubTypeImpp);
    c.setId(QContactId());
    c.saveDetail(&name);
    c.saveDetail(&avatar);
    c.saveDetail(&account);
    manager->saveContact(&c);
}

void QMLContactManagerAsync::setManager(QString manager)
{
    delete qc;
    qc = new QContactManager(manager);
    if(!qc)
        qc = new QContactManager();
    connect(qc, SIGNAL(contactsAdded(QList<QContactLocalId>)), this, SIGNAL(contactsAdded(QList<QContactLocalId>)));
    connect(qc, SIGNAL(contactsChanged(QList<QContactLocalId>)), this, SIGNAL(contactsChanged(QList<QContactLocalId>)));
    connect(qc, SIGNAL(contactsRemoved(QList<QContactLocalId>)), this, SIGNAL(contactsRemoved(QList<QContactLocalId>)));
    connect(qc, SIGNAL(relationshipsAdded(QList<QContactLocalId>)), this, SIGNAL(relationshipsAdded(QList<QContactLocalId>)));
    connect(qc, SIGNAL(relationshipsRemoved(QList<QContactLocalId>)), this, SIGNAL(relationshipsRemoved(QList<QContactLocalId>)));
    
    if (manager == "memory" && qc->contacts().isEmpty()) {
        fillContactsIntoMemoryEngine(qc);
    }

    qWarning() << "Changed backend to: " << manager;
}


QString QMLContactManagerAsync::contactListToQString(const QList<QContactLocalId>& contactIds) const
{
    QString list;
    int i;

    for (i = 0; i < contactIds.count(); i++) {           
            list += QString::number(contactIds.at(i)) +  " ";
        }

    return list;
}

QStringList QMLContactManagerAsync::contactListToQString(const QList<QContact>& contact) const
{
    QStringList list;
    int i;

    for (i = 0; i < contact.count(); i++) {
        list += qc->synthesizeDisplayLabel(contact.at(i));
     }

    return list;
}

int QMLContactManagerAsync::numContacts()
{
    QList<QContactLocalId> qlid;

    qlid = qc->contacts();

    return qlid.count();
}

void QMLContactManagerAsync::contacts()
{
    m_contactIds.clear();
    QContactFetchRequest* req = new QContactFetchRequest;
    QContactLocalIdFilter idFil;
    idFil.setIds(qc->contacts());
    req->setFilter(idFil);
    req->setManager(qc);
    connect(req, SIGNAL(progress(QContactFetchRequest*, bool)), this, SLOT(contactProgress(QContactFetchRequest*,bool)));
    req->start();
}

void QMLContactManagerAsync::contactProgress(QContactFetchRequest *request, bool appendOnly)
{
    Q_UNUSED(appendOnly);

    // first, check to make sure that the request is still valid.
    if (qc != request->manager() ||
        request->status() == QContactAbstractRequest::Cancelled) {
        delete request;
        return; // ignore these results.
    }

    if(request->contacts().count() > 0) {
        QContact c;
        foreach(c, request->contacts()) {
            //qWarning() << "Local Id: " << c.localId() << " count: " << m_contactIds.count();
            QmlContact qmlc(c);
            emit contactsLoaded(&qmlc);
        }
    }

    // check to see if the request status is "finished" - clean up.
    if (request->status() == QContactAbstractRequest::Finished) {        
        delete request;
        emit contactsLoadedDone();
    }

}

QString QMLContactManagerAsync::idToName(QString name)
{
    QContact c = qc->contact(name.toInt());
    return qc->synthesizeDisplayLabel(c);
}

// ![0]

#include "moc_qmlcontactsa.cpp"

QTM_END_NAMESPACE
QML_DEFINE_TYPE(QMLContactManagerAsync, 1, 0, QMLContactManagerAsync, QMLContactManagerAsync);
