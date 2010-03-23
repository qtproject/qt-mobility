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
        list += QString::number(contactIds.at(i)) +  " ";
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
        QContact c;
        foreach(c, request->contacts()) {
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
