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
#include "qmlcontactsa.h"

#include <QtDebug>
#include <QFile>
#include <QIODevice>
#include <qcontactfetchrequest.h>
#include <qcontactlocalidfilter.h>
#include <qcontactdetails.h>
#include <qversitreader.h>
#include <qversitcontactimporter.h>

QT_USE_NAMESPACE
QTM_USE_NAMESPACE

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
    QVersitReader reader;
    QFile file(":/contents/example.vcf");
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

void QMLContactManagerAsync::setManager(QString manager)
{
    delete qc;
    qc = new QContactManager(manager);
    connect(qc, SIGNAL(contactsAdded(QList<QContactLocalId>)), this, SIGNAL(contactsAdded(QList<QContactLocalId>)));
    connect(qc, SIGNAL(contactsChanged(QList<QContactLocalId>)), this, SIGNAL(contactsChanged(QList<QContactLocalId>)));
    connect(qc, SIGNAL(contactsRemoved(QList<QContactLocalId>)), this, SIGNAL(contactsRemoved(QList<QContactLocalId>)));
    connect(qc, SIGNAL(relationshipsAdded(QList<QContactLocalId>)), this, SIGNAL(relationshipsAdded(QList<QContactLocalId>)));
    connect(qc, SIGNAL(relationshipsRemoved(QList<QContactLocalId>)), this, SIGNAL(relationshipsRemoved(QList<QContactLocalId>)));
    
    if (manager == "memory" && qc->contactIds().isEmpty()) {
        fillContactsIntoMemoryEngine(qc);
    }

    qWarning() << "Changed backend to: " << manager;
}


QString QMLContactManagerAsync::contactListToQString(const QList<QContactLocalId>& contactIds) const
{
    QString list;

    for (int i = 0; i < contactIds.count(); i++) {
        list += QString::number(contactIds.at(i)) +  ' ';
    }

    return list;
}

QStringList QMLContactManagerAsync::contactListToQString(const QList<QContact>& contact) const
{
    QStringList list;

    for (int i = 0; i < contact.count(); i++) {
        list += qc->synthesizedDisplayLabel(contact.at(i));
     }

    return list;
}

int QMLContactManagerAsync::numContacts()
{
    return qc->contactIds().count();
}

void QMLContactManagerAsync::contacts()
{
    m_contactIds.clear();
    QContactFetchRequest* req = new QContactFetchRequest;
    QContactLocalIdFilter idFil;
    idFil.setIds(qc->contactIds());
    req->setFilter(idFil);
    req->setManager(qc);
    connect(req, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(contactProgress(QContactAbstractRequest::State)));
    req->start();
}

void QMLContactManagerAsync::contactProgress(QContactAbstractRequest::State newState)
{
    QContactFetchRequest *request = qobject_cast<QContactFetchRequest*>(sender());

    // first, check to make sure that the request is still valid.
    if (qc != request->manager() ||
        newState == QContactAbstractRequest::CanceledState) {
        request->deleteLater();
        return; // ignore these results.
    }

    if(request->contacts().count() > 0) {
        foreach(const QContact& c, request->contacts()) {
            QmlContact qmlc(c);
            emit contactsLoaded(&qmlc);
        }
    }

    // check to see if the request status is "finished" - clean up.
    if (newState == QContactAbstractRequest::FinishedState) {
        request->deleteLater();
        emit contactsLoadedDone();
    }
}

QString QMLContactManagerAsync::idToName(QString name)
{
    QContact c = qc->contact(name.toInt());
    return qc->synthesizedDisplayLabel(c);
}

// ![0]

#include "moc_qmlcontactsa.cpp"
